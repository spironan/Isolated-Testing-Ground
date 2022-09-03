#include "ThreadPool.h"

#include <iostream>

//std::uint32_t ThreadPool::TotalThreadCount = std::thread::hardware_concurrency();
std::uint32_t ThreadPool::TotalWorkerThreads = std::thread::hardware_concurrency(); // TotalThreadCount - 1;
std::atomic_uint32_t TotalInitializedThreads = 0u;

static thread_local uint32_t LocalThreadId = 0u;
static bool g_workerTheadActive = false;
static std::uint32_t g_allocatedTask = 0u;
static std::uint32_t g_totalActiveTask = 0u;

Thread::Thread()
{
    m_thread = std::thread
    {
        [&]()
        {
            LocalThreadId = TotalInitializedThreads++;
            
            // wait!
            while (TotalInitializedThreads != ThreadPool::TotalWorkerThreads); 

            std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Started\n");

            while (g_workerTheadActive || !m_queue.empty())
            {
                while (!m_queue.empty())
                {
                    Task* task = m_queue.front();
                    task->ExecuteTask(m_queue.front());
                    std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Executing Task\n");
                    m_queue.pop();
                    --g_totalActiveTask;
                }
                std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Has nothing to do!\n");
            }
            
            std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Terminated\n");
        }
    };
}

bool Thread::IsInactive() const
{
    return m_queue.empty();
}

void Thread::Wait()
{
    m_thread.join();
}

void Thread::Push(Task* newtask)
{
    m_queue.push(newtask);
}

ThreadPool::ThreadPool()
{
    std::cout << "Starting Thread Pool\n";

    // Initialize here
    Threads.resize(TotalWorkerThreads);
    while (TotalInitializedThreads != TotalWorkerThreads);
    
    // Ensure all threads have completed their queue before beginning
    g_workerTheadActive = true;
}

void ThreadPool::Shutdown()
{
    g_workerTheadActive = false;

    // wait for all threads to finish last bit of work
    for (auto& thread : Threads)
    {
        thread.Wait();
    }

    std::cout << "Terminating Thread Pool\n";
}

Task* ThreadPool::CreateTask(Task::func function)
{
    Task* task = AllocateTask();
    task->Functor = function;
    return task;
}

void ThreadPool::SubmitTask(Task* task)
{
    // make sure all worker threads are active
    while (g_workerTheadActive == false);
    
    while (g_totalActiveTask > s_MaxTask);

    Threads[std::rand() % TotalWorkerThreads].Push(task);
}

Task* ThreadPool::AllocateTask()
{
    ++g_totalActiveTask;
    const uint32_t index = ++g_allocatedTask;
    //return &(g_jobAllocator[(index-1) % MAX_JOBS]); //this line is same as below
    return &(m_taskList[(index - 1u) & (s_MaxTask - 1u)]);   // only works when MAX_JOBS is a power of 2
}
