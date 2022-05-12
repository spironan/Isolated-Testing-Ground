#pragma once

#include "WorkStealingQueue.h"

#include <stdint.h>
#include <atomic>
#include <functional>

struct Job;
using JobFunction = std::function<void(Job*, void const*)>;

struct Job
{
    JobFunction function{ nullptr };
    Job* parent{ nullptr };
    std::atomic<int32_t> unfinishedJobs{ 0 }; // atomic
    void* data[2]{};
    std::atomic<int32_t> subdependanciesCnt{ 0 }; // atomic
    Job* subdependancies[3]{};
}; // sizeof(Job) == 128 bytes

static constexpr int jobSize = sizeof(Job);

//static_assert(sizeof(Job) == 128);

class JobSystem
{
public:
    static constexpr uint32_t MAX_JOBS = 4096;
    
    static uint32_t g_workerThreadCount; // will be initialized to std::thread::hardware_concurrency

    static void Init();
    static void Shutdown();
    static Job* CreateJob(JobFunction function, char* data = nullptr);
    static Job* CreateJobAsChild(Job* parent, JobFunction function, char* data = nullptr);
    static void Submit(Job* job);
    static void Wait(const Job* job);
    static void AddDependancy(Job* ancestor, Job* dependancy);
    static void EmptyJob(Job*, const void*);

    static uint32_t GenerateRandomNumber(uint32_t from, uint32_t to);
private:

    static WorkStealingQueue<Job>* GetWorkerThreadQueue();
    static void WorkerThreadFunction(uint32_t ID);
    static bool IsEmptyJob(const Job* job);
    static bool HasJobCompleted(const Job* job);
    static Job* GetJob();
    static Job* AllocateJob();
    static void Execute(Job* job);
    static void Finish(Job* job);
    static void ThreadYield();
};