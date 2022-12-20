#include "jobs.h"

#include <future>
#include <functional>
#include <latch>
#include <string>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include "../containers/threadsafe_queue.h"

namespace jobsystem
{
    static std::uint32_t globalWorkerThreadCount = std::clamp(globalWorkerThreadCount, 2u, std::max(2u, std::thread::hardware_concurrency() - 3));
    static std::vector<std::thread> workers;
    std::thread deliveryThread;

    static ts::threadsafe_queue<work> work_queue;
    static std::condition_variable cv;
    //static std::unique_lock<std::mutex> work_lock;

    static std::mutex write_mutex;
    static std::mutex work_mutex;

    static std::latch initialization_latch = std::latch(globalWorkerThreadCount + 1 + 1); // whatever number of workers we desire, we must + 1 to account for submit and another for main.
    static std::once_flag initialization_submit;
    
    static std::atomic_bool shutdown_system{ false };
    static std::atomic_bool shutdown_delivery{ false };

    static std::binary_semaphore queue_is_empty{ true };
    void initialize()
    {
        std::cout << "jobsystem attempting initialization.... \n";

        //std::latch shutdown_latch = std::latch{ globalWorkerThreadCount };

        auto worker_thread_logging = [&](std::string msg)
        {
            std::lock_guard lk(write_mutex);
            auto tid = std::this_thread::get_id();
            std::cout << tid << "[worker]:" << msg << "\n";
        };

        auto worker_thread_function = [&]()
        {
            initialization_latch.arrive_and_wait();
            
            // we only reach here once we submitted at least one task.

            worker_thread_logging("thread is activated!");

            // while there's work to be done or system is not told to shutdown
            while (work_queue.size() > 0 || shutdown_system == false)
            {
                //grab mutex and wait
                //std::unique_lock worker_lk(work_mutex);
                //cv.wait(worker_lk, [&]() { return work_queue.size() > 0 || shutdown_system == true; });

                if (work_queue.size() > 0)
                {
                    //worker_thread_logging("work detected!");

                    auto work = work_queue.try_pop();
                    if (work)
                    {
                        //worker_thread_logging("doing some work!");
                        std::invoke(work->fnc);
                    }
                    std::this_thread::yield();
                }
                
                std::this_thread::yield();
            }

            worker_thread_logging("Worker thread terminating!");
        };

        for (std::size_t i = 0; i < globalWorkerThreadCount; i++)
        {
            std::thread t{ worker_thread_function };
            workers.emplace_back(std::move(t));
            //workers.emplace_back(worker_thread_function);
        }

        auto delivery_thread_function = [&]() 
        {
            initialization_latch.arrive_and_wait();

            while (shutdown_delivery == false)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1/60));
                
                //std::unique_lock<std::mutex> lk(work_mutex);

                if(work_queue.size() > 0)
                    cv.notify_all();

                if (work_queue.size() == 0)
                    queue_is_empty.release();
                
                std::this_thread::yield();
            }
        };
        deliveryThread = std::thread{ delivery_thread_function };

        std::cout << "jobsystem successfully initialized. \n";
    }

    void shutdown()
    {
        if (shutdown_system == true)
        {
            std::cout << "jobsystem is not yet initialized! do not call shutdown! \n";
            return;
        }
        
        internal::do_once();

        std::cout << "jobsystem attempting shutdown.... \n";
        shutdown_system = true;

        for (auto& worker : workers)
        {
            worker.join();
        }

        shutdown_delivery = true;
        deliveryThread.join();

        std::cout << "jobsystem successfully shutdown. \n";
    }

    void submit(work work)
    {
        std::unique_lock<std::mutex> lk(work_mutex);

        work_queue.push(work);

        internal::do_once();

        cv.notify_all();
    }

    void wait()
    {
        internal::do_once();
        // blocking call that stalls this thread and waits for queue to be empty!
        queue_is_empty.acquire();
    }

    namespace internal
    {
        void do_once()
        {
            std::call_once(initialization_submit, [&]()
                {
                    // we do this to ensure until you submit the first task, every worker just waits.
                    initialization_latch.count_down();

                    std::lock_guard lk(write_mutex);
                    auto tid = std::this_thread::get_id();
                    std::cout << tid << "[main]: jobsystem do once decrement!" << "\n";
                });
        }
    }
}
