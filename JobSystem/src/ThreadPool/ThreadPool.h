#pragma once

#include <thread>
#include <queue>
#include <functional>
#include <array>

// A unit of work
struct Task
{
    using params = Task*;
    using func = std::function<void(params)>;

    void ExecuteTask(params parameter)
    {
        Functor(parameter);
    }

    func Functor = nullptr;
    std::atomic_uint32_t UnfinishedTask = 0;
    void* data[4]{};
};

static constexpr std::uint32_t TaskSize = sizeof(TaskSize);

struct Thread
{
    Thread();

    bool IsInactive() const;
    void Wait();
    void Push(Task* newtask);
private:
    // thread associated with this
    std::thread m_thread;
    // Queue of Task to run
    std::queue<Task*> m_queue = {};

    void ExecuteTask(Task* task);
};

// Function to check if x is power of 2
constexpr bool IsPowerOfTwo(const std::uint32_t n)
{
    return (n && !(n & (n - 1)));
}

struct ThreadPool
{
public:
    ThreadPool() = delete;

    // Each task should be of considerable size. Too small and we lose performance.
    static constexpr std::uint32_t s_MaxTask = 64;
    
    // needs to be power of 2
    static_assert(IsPowerOfTwo(s_MaxTask));

    static void Init();
    static void Shutdown();

    /*ThreadPool();
    ~ThreadPool() { Shutdown(); }*/

    static Task* CreateTask(Task::func function);
    static void SubmitTask(Task* task);
    static void WaitForTask(Task const* task);

    static std::uint32_t TotalWorkerThreads;
private:
    //std::thread MasterThread;
    static std::vector<Thread> Threads;
    static std::array<Task, s_MaxTask> m_taskList;

    //static std::uint32_t TotalThreadCount;

    static Task* AllocateTask();
    static bool HasTaskCompleted(Task const* task);
};
