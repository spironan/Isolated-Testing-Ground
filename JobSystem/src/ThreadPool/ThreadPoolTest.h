#pragma once
#include "ThreadPool.h"

struct Test
{
public:
    static inline void testFnc(Task* task_ptr)
    {
        (void*)task_ptr;

        long double x{ 1.234e5 };
        for (std::size_t k = 0; k < 1000000; k++)
        {
            x += std::sqrt(x);
        }
    }

    static inline void testFncIter(Task* task_ptr)
    {
        std::size_t* startId = static_cast<std::size_t*>(task_ptr->data[0]);
        std::size_t* endId = static_cast<std::size_t*>(task_ptr->data[1]);
        for (std::size_t i = *startId; i < *endId; ++i)
        {
            testFnc(task_ptr);
        }
    }

    static inline void Test1a()
    {
        ThreadPool::Init();
        Task* task0 = ThreadPool::CreateTask(testFnc);
        ThreadPool::SubmitTask(task0);
        ThreadPool::WaitForTask(task0);
        ThreadPool::Shutdown();
    }

    static inline void Test1b()
    {
        Task* t = nullptr;
        (void*)t;
        testFnc(t);
    }

    static inline void Test2a()
    {
        ThreadPool::Init();
        Task* task = nullptr;
        for (std::size_t i = 0; i < ThreadPool::s_MaxTask; ++i)
        {
            task = ThreadPool::CreateTask(testFnc);
            ThreadPool::SubmitTask(task);
        }
        ThreadPool::WaitForTask(task);
        ThreadPool::Shutdown();
    }

    static inline void Test2b()
    {
        for (std::size_t i = 0; i < ThreadPool::s_MaxTask; ++i)
        {
            Task* t = nullptr;
            (void*)t;
            testFnc(t);
        }
    }

    static inline void Test3()
    {
        ThreadPool::Init();
        for (std::size_t i = 0; i < ThreadPool::s_MaxTask * 2; ++i)
        {
            Task* task0 = ThreadPool::CreateTask(testFnc);
            ThreadPool::SubmitTask(task0);
            ThreadPool::WaitForTask(task0);
        }
        ThreadPool::Shutdown();
    }

    static inline void Test4()
    {
        ThreadPool::Init();
        Task* task = nullptr;
        static const std::size_t task_per_thread = ThreadPool::s_MaxTask / ThreadPool::TotalWorkerThreads;
        for (std::size_t i = 0; i < ThreadPool::TotalWorkerThreads; ++i)
        {
            task = ThreadPool::CreateTask(testFncIter);

            std::size_t startId = i * task_per_thread, endId = ((i + 1) * ThreadPool::s_MaxTask / ThreadPool::TotalWorkerThreads);
            std::size_t* startIdPtr = &i;
            std::size_t* endIdPtr = &endId;

            task->data[0] = startIdPtr;
            task->data[1] = endIdPtr;

            ThreadPool::SubmitTask(task);
        }
        ThreadPool::WaitForTask(task);
        ThreadPool::Shutdown();
    }
};
