#include "JobSystem.h"
#include <vector>
#include <thread>

#include <random>

uint32_t JobSystem::g_workerThreadCount = 0;    // will be updated with hardware_concurrency

using container = WorkStealingQueue<Job>;
static std::vector< container* > g_workStealingQueue;
static std::vector<std::thread> workers;
static bool g_workerTheadActive = false;

static thread_local uint32_t localThreadID = 0;
static thread_local Job g_jobAllocator[JobSystem::MAX_JOBS];
static thread_local uint32_t g_allocatedJobs = 0u;

container* JobSystem::GetWorkerThreadQueue()
{
    return g_workStealingQueue[localThreadID];
}

static std::atomic<uint32_t> g_threadsInit;

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <cassert>

#include "utils/timer.h"

#include <string>
#include <mutex>
static std::mutex g_writeMutex;

void JobSystem::WorkerThreadFunction(uint32_t ID)
{
    {
        std::scoped_lock<std::mutex> lock(g_writeMutex);

        Profiler WorkerThread{ "Worker Thread Init " + std::to_string(ID)};
    
        // create a queue
        //std::unique_ptr<WorkStealingQueue<Job>> myQueue = std::make_unique<WorkStealingQueue<Job>>();
        container* myQueue = new container();

        //add it to global list
        g_workStealingQueue[ID] = myQueue;
        localThreadID = ID;
        ++g_threadsInit;

        {
            // Do Windows-specific thread setup:
            HANDLE handle = (HANDLE)workers.back().native_handle();

            // Put each thread on to dedicated core:
            DWORD_PTR affinityMask = 1ull << localThreadID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            assert(affinity_result > 0);

            //// Increase thread priority:
            //BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
            //assert(priority_result != 0);

            // Name the thread:
            std::wstring wthreadname = L"wi::jobsystem_" + std::to_wstring(localThreadID);
            HRESULT hr = SetThreadDescription(handle, wthreadname.c_str());
            assert(SUCCEEDED(hr));
        }

    }

    while (g_threadsInit.load(std::memory_order_acq_rel) != g_workerThreadCount); /*wait for all to finish loading*/
    
    // main function of each worker thread
    while (g_workerTheadActive)
    {
        Job* job = GetJob();
        if (job)
        {
            Execute(job);
        }
    }

}

void JobSystem::Init()
{
    g_workerThreadCount = std::max(1u, (std::thread::hardware_concurrency() / 4));

    g_workStealingQueue.resize(g_workerThreadCount, nullptr);
    for (size_t i = 0; i < g_workerThreadCount; i++)
    {
        workers.emplace_back(&JobSystem::WorkerThreadFunction, i);
    }

    // ensure all threads have completed their queue before beginning
    while (g_threadsInit.load(std::memory_order_acquire) != g_workerThreadCount); /*wait for begin*/
    
    g_workerTheadActive = true;
}

void JobSystem::Shutdown()
{
    g_workerTheadActive = false;

    for (auto& worker : workers)
    {
        worker.join();
    }

    for (size_t i = 0; i < g_workerThreadCount; i++)
    {
        delete g_workStealingQueue[i];
    }
}

Job* JobSystem::AllocateJob()
{
    const uint32_t index = ++g_allocatedJobs;
    //return &(g_jobAllocator[(index-1) % MAX_JOBS]); //this line is same as below
    return &(g_jobAllocator[(index - 1u) & (MAX_JOBS - 1u)]);   // only works when MAX_JOBS is a power of 2
}

Job* JobSystem::CreateJob(JobFunction function, char* data)
{
    Job* job = AllocateJob();
    job->function = function;
    job->parent = nullptr;
    job->unfinishedJobs = 1;

    return job;
}

Job* JobSystem::CreateJobAsChild(Job* parent, JobFunction function, char* data)
{
    parent->unfinishedJobs.fetch_add(1, std::memory_order_acq_rel); //atomic increment

    Job* job = AllocateJob();
    //job->data = reinterpret_cast<char*>(data);
    job->function = function;
    job->parent = parent;
    job->unfinishedJobs = 1;

    return job;
}

void JobSystem::AddDependancy(Job* ancestor, Job* dependancy)
{
    int32_t const count = ancestor->subdependanciesCnt.fetch_add(1, std::memory_order_acq_rel);
    constexpr static uint32_t maxdependancies = sizeof(Job::subdependancies) / sizeof(Job*);

    if (count > maxdependancies)
    {
        //we are full, add to last dependancy
        AddDependancy(ancestor->subdependancies[maxdependancies - 1], dependancy);
        //re decrement the dependancies
        ancestor->subdependanciesCnt.fetch_sub(1, std::memory_order_acq_rel);
    }
    else
    {
        if (count == maxdependancies)
        {
            // if we are on the last dependancie we should spin a new job for more tasks
            // the empty job here acts as an instant
            Job* nJob = CreateJob([&](Job* job, const void* ptr) { EmptyJob(job, ptr); });

            ancestor->subdependancies[count - 1] = nJob;
            AddDependancy(ancestor->subdependancies[maxdependancies - 1], dependancy);
        }
        else
        {
            ancestor->subdependancies[count - 1] = dependancy;
        }

    }
}

void JobSystem::Submit(Job* job)
{
    container* queue = GetWorkerThreadQueue();
    queue->Push(job);
}

void JobSystem::Execute(Job* job)
{
    (job->function)(job, job->data);
    Finish(job);
}

void JobSystem::Finish(Job* job)
{
    const int32_t unfinishedJobs = job->unfinishedJobs.fetch_sub(1, std::memory_order_acq_rel);
    if (unfinishedJobs == 0)
    {
        // decrementing int -1 signifies that we are done
        job->unfinishedJobs.fetch_sub(1, std::memory_order_acq_rel);

        if ((unfinishedJobs == 0) && (job->parent))
        {
            Finish(job->parent);
        }

        // run follow-up jobs
        int32_t const count = job->subdependanciesCnt.load();
        for (int32_t i = 0; i < count; ++i)
        {
            Submit(job->subdependancies[i]);
        }

    }
}

void JobSystem::Wait(const Job* job)
{
    // wait until the job has completed. in the meantime, work on any other job.
    while (!HasJobCompleted(job))
    {
        Job* nextJob = GetJob();
        if (nextJob)
        {
            Execute(nextJob);
        }
    }
}

void JobSystem::EmptyJob(Job*, const void*)
{
}

void JobSystem::ThreadYield()
{
    _mm_pause();
    //std::this_thread::yield();
}


bool JobSystem::IsEmptyJob(const Job* job)
{
    return job == nullptr;
}

bool JobSystem::HasJobCompleted(const Job* job)
{
    return job->unfinishedJobs.load(std::memory_order_acquire) == -1;
}

uint32_t JobSystem::GenerateRandomNumber(uint32_t from, uint32_t to)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> distrib(from, to);
    return distrib(gen);
}

Job* JobSystem::GetJob()
{
    container* queue = GetWorkerThreadQueue();

    Job* job = queue->Pop();
    if (IsEmptyJob(job))
    {
        // this is not a valid job because our own queue is empty, so try stealing from some other queue
        uint32_t const randomIndex = GenerateRandomNumber(0, g_workerThreadCount - 1);
        container* stealQueue = g_workStealingQueue[randomIndex];
        if (stealQueue == queue)
        {
            // don't try to steal from ourselves
            ThreadYield();
            return nullptr;
        }

        Job* stolenJob = stealQueue->Steal();
        if (IsEmptyJob(stolenJob))
        {
            // we couldn't steal a job from the other queue either, so we just yield our time slice for now
            ThreadYield();
            return nullptr;
        }

        return stolenJob;
    }

    return job;
}
