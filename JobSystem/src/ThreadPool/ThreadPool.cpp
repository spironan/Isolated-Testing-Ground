//#include "ThreadPool.h"
//
//#include <iostream>
//
////std::uint32_t ThreadPool::TotalThreadCount = std::thread::hardware_concurrency();
////std::uint32_t ThreadPool::TotalWorkerThreads = std::thread::hardware_concurrency(); // TotalThreadCount - 1;
//std::atomic_uint32_t TotalInitializedThreads = 0u;
//
//std::vector<Thread> ThreadPool::Threads;
//std::array<Task, ThreadPool::s_MaxTask> ThreadPool::m_taskList;
//
//
//static thread_local uint32_t LocalThreadId = 0u;
//static std::atomic_bool g_workerTheadActive = true;
//static std::atomic_uint32_t g_allocatedTask = 0u;      // potential data race here.
//static std::atomic_uint32_t g_totalActiveTask = 0u;    // potential data race here.
//
////#define PRINT
//#ifdef PRINT
//#define PRINT_MSG(...) std::cout << __VA_ARGS__
//#else
//#define PRINT_MSG(...)
//#endif
//
//Thread::Thread()
//{
//    // could have a try catch here.
//
//    m_thread = std::thread
//    {
//        [&]()
//        {
//            LocalThreadId = TotalInitializedThreads++;
//            
//            // try to use std barrier or latch
//            // using latch to wait
//
//            // wait!
//            m_start_flag.wait();
//            //while (TotalInitializedThreads != ThreadPool::TotalWorkerThreads); 
//
//            PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Started\n"));
//            //std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Started\n");
//
//            while (g_workerTheadActive || !m_queue.empty())
//            {
//                while (!m_queue.empty())
//                {
//                    Task* task = m_queue.front();
//                    PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Executing Task\n"));
//                    ExecuteTask(task);
//                    m_queue.pop();
//                    --g_totalActiveTask;
//                }
//                PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Has nothing to do!\n"));
//                std::this_thread::yield();
//            }
//            
//            PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Terminated\n"));
//        }
//    };
//}
//
//bool Thread::IsInactive() const
//{
//    return m_queue.empty();
//}
//
//void Thread::Wait()
//{
//    m_thread.join();
//}
//
//void Thread::Push(Task* newtask)
//{
//    m_queue.push(newtask);
//}
//
//void Thread::ExecuteTask(Task* task)
//{
//    task->ExecuteTask(task);
//    //while (task->UnfinishedTask != 1);
//    task->UnfinishedTask.fetch_sub(1, std::memory_order_seq_cst);
//    if (task->UnfinishedTask == 0)
//    {
//        PRINT_MSG("Finished Task\n");
//    }
//}
//
////void ThreadPool::Init()
////{
////    PRINT_MSG("Starting Thread Pool\n");
////
////    // Initialize here
////    Threads.resize(TotalWorkerThreads);
////    for (auto& thread : Threads)
////    {
////        //thread.data = 
////        std::thread(&ThreadPool::ThreadFunc, this);
////    }
////    //while (TotalInitializedThreads != TotalWorkerThreads);
////
////    m_start_flag.count_down();
////
////    //// Ensure all threads have completed their queue before beginning
////    //g_workerTheadActive = true; // this could be too slow.
////}
////
////void ThreadPool::Shutdown()
////{
////    /*while (g_workerTheadActive == false)
////        std::this_thread::yield();*/
////
////    g_workerTheadActive = false;
////
////    // wait for all threads to finish last bit of work
////    for (auto& thread : Threads)
////    {
////        thread.Wait();
////    }
////
////    PRINT_MSG("Terminating Thread Pool\n");
////}
//
//ThreadPool::ThreadPool(std::uint32_t thread_count)
//    : m_threadCount {std::max(thread_count, 1u)}
//    , m_startFlag { 1 }
//    , m_terminate { false }
//{
//    try 
//    {
//        PRINT_MSG("Starting Thread Pool\n");
//
//        // Create the worker threads.
//        for (size_t i = 0; i < thread_count; ++i) 
//        {
//            //local_task_queues_.push_back(std::make_shared<threadsafe_stack<task>>());
//            std::thread worker_thread{ &ThreadPool::WorkerThreadFunc, this };
//            //worker_index_lookup_.insert(worker_thread.get_id(), i);
//            m_workerThreads.emplace_back(std::move(worker_thread));
//        }
//
//        // Create the local task queues BEFORE starting the threads so that the threads do not try to pop a non-existent queue.
//        m_startFlag.count_down();
//    }
//    catch (...) 
//    {
//        // Set end_flag_ = true before decreasing the counter on latch, so that the worker threads do not enter the while loop.
//        m_terminate = true;
//        m_startFlag.count_down();
//        throw;
//    }
//}
//
//ThreadPool::~ThreadPool()
//{
//    m_terminate = true;
//    
//    for (std::thread& worker_thread : m_workerThreads)
//    {
//        worker_thread.join();
//    }
//    
//    PRINT_MSG("Terminating Thread Pool\n");
//}
//
//Task* ThreadPool::CreateTask(Task::func function)
//{
//    Task* task = AllocateTask();
//    task->Functor = function;
//    task->UnfinishedTask.fetch_add(1, std::memory_order_seq_cst);
//    //::_InterlockedIncrement64(&task->UnfinishedTask);
//    //_interlockedincrement64();
//    return task;
//}
//
//
//void ThreadPool::SubmitTask(Task* task)
//{
//    // make sure all worker threads are active
//    while (g_workerTheadActive == false);
//    
//    if (g_totalActiveTask > s_MaxTask)
//        return;
//
//    Threads[std::rand() % TotalWorkerThreads].Push(task);
//}
//
//void ThreadPool::WaitForTask(Task const* task)
//{
//    // wait until the job has completed. in the meantime, work on any other job.
//    while (!HasTaskCompleted(task));
//}
//
//bool ThreadPool::HasTaskCompleted(Task const* task)
//{
//    return task->UnfinishedTask.load(std::memory_order_seq_cst) == 0;
//}
//
//void ThreadPool::WorkerThreadFunc()
//{
//    LocalThreadId = TotalInitializedThreads++;
//
//    // try to use std barrier or latch
//    // using latch to wait
//
//    // wait!
//    m_startFlag.wait();
//    //while (TotalInitializedThreads != ThreadPool::TotalWorkerThreads); 
//
//    PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Started\n"));
//    //std::cout << std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Started\n");
//
//    while (g_workerTheadActive || !m_queue.empty())
//    {
//        while (!m_queue.empty())
//        {
//            Task* task = m_queue.front();
//            PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Executing Task\n"));
//            ExecuteTask(task);
//            m_queue.pop();
//            --g_totalActiveTask;
//        }
//        PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Has nothing to do!\n"));
//        std::this_thread::yield();
//    }
//
//    PRINT_MSG(std::string("Thread [") + std::to_string(LocalThreadId) + std::string("] Terminated\n"));
//}
//
//Task* ThreadPool::AllocateTask()
//{
//    ++g_totalActiveTask;
//    const uint32_t index = ++g_allocatedTask;
//    //return &(g_jobAllocator[(index-1) % MAX_JOBS]); //this line is same as below
//    return &(m_taskList[(index - 1u) & (s_MaxTask - 1u)]);   // only works when MAX_JOBS is a power of 2
//}
