#include "src/utils/Timer.h"
//#include "src/JobSystem.h"

//#include "src/2nd Attempt/JobSystem2.h"
//#include <string>
//#include <thread>

#include "src/ThreadPool/ThreadPool.h"

//void foo(std::size_t N)
//{
//    long double x{ 1.234e5 };
//    for (std::size_t k = 0; k < N; k++)
//    {
//        x += std::sqrt(x);
//    }
//}

//void fn(Job* job, const void* data)
//{
//    uint32_t a = JobSystem::GenerateRandomNumber(0, 100);
//    uint32_t b = JobSystem::GenerateRandomNumber(0, 100);
//    a += b;
//    (void)job;
//    (void)data;
//}

void testFnc(Task* task_ptr)
{
    (void*)task_ptr;

    double a = 5.0;
    double b = 7.0;

    for (size_t i = 0; i < 100000000; ++i)
    {
        a *= std::sqrtl(b);
    }

    long double x{ 1.234e5 };
    for (std::size_t k = 0; k < 100000000; k++)
    {
        x += std::sqrt(x);
    }
}

int main()
{
    // Memory Leak Checker in Debug builds
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // Uncomment to cause a break on allocation for debugging
    //_CrtSetBreakAlloc(/*Allocation Number here*/);
#endif
    ThreadPool m_threadpool;
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }

    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    {
        Task* task0 = m_threadpool.CreateTask(testFnc);
        Task* task1 = m_threadpool.CreateTask(testFnc);
        Task* task2 = m_threadpool.CreateTask(testFnc);
        Task* task3 = m_threadpool.CreateTask(testFnc);
        Task* task4 = m_threadpool.CreateTask(testFnc);
        Task* task5 = m_threadpool.CreateTask(testFnc);
        m_threadpool.SubmitTask(task0);
        m_threadpool.SubmitTask(task1);
        m_threadpool.SubmitTask(task2);
        m_threadpool.SubmitTask(task3);
        m_threadpool.SubmitTask(task4);
        m_threadpool.SubmitTask(task5);
    }
    while (true);

    //std::vector<std::thread> threads;
    //char test = 'a';
    //long value = 0;
    //for (size_t i = 0; i < 10; ++i)
    //{
    //    //threads.emplace_back(JobSystem::SpinLockXCHG, &test);
    //    threads.emplace_back(JobSystem::AtomicIncrementCAS, &value);
    //}

    //for (auto& thread : threads)
    //    thread.join();

    /*{
        Profiler prof{ "Compiler Barrier Producer Consumer" };

        std::thread producer(JobSystem::ProducerThreadCompilerBarrier);
        std::thread consumer(JobSystem::ConsumerThreadCompilerBarrier);
        producer.join();
        consumer.join();
    }


    {
        Profiler prof{ "Memory Fence Producer Consumer" };

        std::thread producer(JobSystem::ProducerThreadMemoryFence);
        std::thread consumer(JobSystem::ConsumerThreadMemoryFence);
        producer.join();
        consumer.join();
    }*/

    //JobSystem::SpinLockXCHG(&test);

    //{
    //    Profiler prof{"Overall Duration"};

    //    {
    //        Profiler init{ "Init Duration" };
    //        JobSystem::Init();
    //    }

    //    while (true);// { std::this_thread::yield(); };
    //    //{
    //    //    Profiler Jobs{ "Jobs Duration" };
    //    //    Job* root = JobSystem::CreateJob(JobSystem::EmptyJob);
    //    //    
    //    //    {
    //    //        Profiler Submission{ " Jobs Submission" };
    //    //        for (size_t i = 0; i < 10000; i++)
    //    //        {
    //    //            //Job* job = JobSystem::CreateJob(JobSystem::EmptyJob); 
    //    //            Job* job = JobSystem::CreateJobAsChild(root, fn);
    //    //            JobSystem::Submit(job);
    //    //        }

    //    //    }

    //    //    JobSystem::Submit(root);
    //    //    JobSystem::Wait(root);
    //    //}

    //    {
    //        Profiler shutdown{ "Shutdown Duration" };
    //        JobSystem::Shutdown();
    //    }
    //}
}
