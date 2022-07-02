#include "src/utils/Timer.h"
//#include "src/JobSystem.h"

#include "src/2nd Attempt/JobSystem2.h"
#include <string>
#include <thread>

void foo(std::size_t N)
{
    long double x{ 1.234e5 };
    for (std::size_t k = 0; k < N; k++)
    {
        x += std::sqrt(x);
    }
}

//void fn(Job* job, const void* data)
//{
//    uint32_t a = JobSystem::GenerateRandomNumber(0, 100);
//    uint32_t b = JobSystem::GenerateRandomNumber(0, 100);
//    a += b;
//    (void)job;
//    (void)data;
//}

int main()
{
    // Memory Leak Checker in Debug builds
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    // Uncomment to cause a break on allocation for debugging
    //_CrtSetBreakAlloc(/*Allocation Number here*/);
#endif

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

    {
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
    }

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
