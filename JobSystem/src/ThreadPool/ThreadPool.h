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

    func Functor;
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
};

// Function to check if x is power of 2
constexpr bool IsPowerOfTwo(const std::uint32_t n)
{
    return (n && !(n & (n - 1)));
}

struct ThreadPool
{
public:
    // Each task should be of considerable size. Too small and we lose performance.
    static constexpr std::uint32_t s_MaxTask = 128;
    
    // needs to be power of 2
    static_assert(IsPowerOfTwo(s_MaxTask));

    ThreadPool();
    ~ThreadPool() { Shutdown(); }

    Task* CreateTask(Task::func function);
    void SubmitTask(Task* task);

    static std::uint32_t TotalWorkerThreads;
private:
    //std::thread MasterThread;
    std::vector<Thread> Threads;
    std::array<Task, s_MaxTask> m_taskList;

    //static std::uint32_t TotalThreadCount;

    Task* AllocateTask();
    void Shutdown();
};
