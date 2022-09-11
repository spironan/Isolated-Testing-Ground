#include "src/utils/Timer.h"
//#include "src/JobSystem.h"

#include "src/OpenMP/OpenMpTest.h"

//#include "src/2nd Attempt/JobSystem2.h"
//#include <string>
//#include <thread>

#include "src/ThreadPool/ThreadPool.h"
#include "src/ThreadPool/ThreadPoolTest.h"

#include "src/containers/threadsafe_queue.h"

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


struct A
{
    int i = -1;
    char c = 'z';
    double d = -9.99;

    friend std::ostream& operator<<(std::ostream& oss, A const& a)
    {
        oss << a.i << a.c << a.d;
        return oss;
    }
};


int main()
{
    // Memory Leak Checker in Debug builds
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // Uncomment to cause a break on allocation for debugging
    //_CrtSetBreakAlloc(/*Allocation Number here*/);
#endif

    OpenMpTest a;
    //a.fn();
    
    //a.calculatePi_Serial();
    a.calculatePi_Ex2();
    a.calculatePi_Ex3();
    a.calculatePi_Ex4();

    //Test::Test1a();
    //
    //ts::threadsafe_queue<A> m_queue;
    //
    //A test;
    //test.i = 10;
    //test.c = 'c';
    //test.d = 0.1;

    //std::thread a([&]() { m_queue.push(A()); });
    //std::thread b([&]() { m_queue.push(test); });
    //std::thread c([&]() { m_queue.push(test); });

    ////if (m_queue.empty())
    ////{
    ////    m_queue.push(A());
    ////    
    ////    m_queue.push(test);
    ////    m_queue.push(std::move(test));
    ////    //m_queue.push(test.i, test.c, test.d);
    ////}
    ////else
    ////{
    //    A val;
    //    m_queue.try_pop(val);
    //    std::cout << val << std::endl;
    //    std::shared_ptr<A> second = m_queue.try_pop();
    //    if(second)
    //        std::cout << *second << std::endl;
    ////}

    //std::cout << m_queue.size() << std::endl;

    //m_queue.clear();

    //while (!m_queue.empty())
    //{
    //    std::cout << *m_queue.wait_and_pop() << std::endl;
    //}

    //a.join(), b.join(), c.join();

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
