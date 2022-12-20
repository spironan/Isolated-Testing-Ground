//#pragma once
//
//#include <thread>
//#include <queue>
//#include <functional>
//#include <array>
//#include <latch>
//
//// A unit of work
//struct Task
//{
//    using params = Task*;
//    using func = std::function<void(params)>;
//
//    void ExecuteTask(params parameter)
//    {
//        Functor(parameter);
//    }
//
//    func Functor = nullptr;
//    std::atomic_uint32_t UnfinishedTask = 0;
//    void* data[4]{};
//};
//
//static constexpr std::uint32_t TaskSize = sizeof(TaskSize);
//
//struct Thread
//{
//    Thread();
//
//    bool IsInactive() const;
//    void Wait();
//    void Push(Task* newtask);
//private:
//    // thread associated with this
//    std::thread m_thread;
//    // Queue of Task to run
//    std::queue<Task*> m_queue = {};
//
//    void ExecuteTask(Task* task);
//};
//
//// Function to check if x is power of 2
//constexpr bool IsPowerOfTwo(const std::uint32_t n)
//{
//    return (n && !(n & (n - 1)));
//}
//
//struct ThreadPool
//{
//public:
//    static ThreadPool& default_pool()
//    {
//        static ThreadPool tp{};
//        return tp;
//    }
//
//    // Each task should be of considerable size. Too small and we lose performance.
//    static constexpr std::uint32_t s_MaxTask = 64;
//    
//    // needs to be power of 2
//    static_assert(IsPowerOfTwo(s_MaxTask));
//
//    ThreadPool(std::uint32_t thread_count = std::thread::hardware_concurrency());
//    ~ThreadPool();
//
//    inline std::size_t count() const { return m_threadCount; }
//
//    Task* CreateTask(Task::func function);
//    void SubmitTask(Task* task);
//    void WaitForTask(Task const* task);
//
//    std::uint32_t TotalWorkerThreads;
//
//private:
//    //std::thread MasterThread;
//    //std::vector<Thread> Threads;
//    
//    std::latch m_startFlag = std::latch{ 1 };
//    std::atomic_bool m_terminate = false;
//    std::size_t m_threadCount = 1;
//    std::vector<std::thread> m_workerThreads;
//    std::array<Task, s_MaxTask> m_taskList;
//    //static std::atomic_bool m_terminate = true;
//    //static std::uint32_t TotalThreadCount;
//
//private:
//    Task* AllocateTask();
//    bool HasTaskCompleted(Task const* task);
//
//    void WorkerThreadFunc();
//    
//    void Init();
//    void Shutdown();
//};
//
//// potential ideas:
//// - threadsafe queue for global, threadsafe stack for thread local
//// - threadsafe map
//// - 
