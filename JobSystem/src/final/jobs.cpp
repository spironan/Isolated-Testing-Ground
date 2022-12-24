#include "jobs.h"

#include <future>
#include <functional>
#include <latch>
#include <string>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <barrier>
#include "../containers/threadsafe_queue.h"

namespace jobsystem
{
    static std::uint32_t global_worker_thread_count = std::thread::hardware_concurrency() - 1; //std::clamp(global_worker_thread_count, 2u, std::max(2u, std::thread::hardware_concurrency() - 2));
    static std::vector<std::jthread> worker_threads{};

    static ts::threadsafe_queue<packaged_t> work_queue;
    static std::condition_variable_any cv_any;

    static std::mutex write_mutex;
    static std::mutex work_mutex;

    // used to synchronize initialization of threads.
    static std::latch init_latch = std::latch(global_worker_thread_count + 1); // number of workers we desire and + 1 to account for main.

    static std::stop_source shutdown_source;

    auto threadsafe_logging = [&](std::string prefix, std::string msg)
    {
        if constexpr (enable_logging == false)
            return;

        std::scoped_lock lk(write_mutex);
        auto tid = std::this_thread::get_id();
        std::cout << "[" << prefix << "-" << tid << "]:\t" << msg << "\n";
    };

    auto worker_thread_function = [&](std::stop_token stop_token)
    {
        init_latch.arrive_and_wait();

        // we only reach here once every thread is loaded. workers and main.

        threadsafe_logging("worker", "worker thread is activated!");

        // while there's work to be done or system is not told to shutdown
        while (!work_queue.empty() || !stop_token.stop_requested())
        {
            //an attempt to avoid busy wait.
            {
                std::unique_lock worker_lk(work_mutex);
                while (work_queue.empty())
                {
                    // check if cancellation of worker threads was requested.
                    if (cv_any.wait(worker_lk, stop_token, [&]() { return !work_queue.empty(); }) == false)
                    {
                        threadsafe_logging("worker", "worker thread terminating...");
                        return;
                    }
                }
            }

            // attempt to grab some work and perform them if there is.
            {
                //threadsafe_logging("worker", "work detected!");

                //auto work = work_queue.wait_and_pop();
                auto work = work_queue.try_pop();
                if (work)
                {
                    //threadsafe_logging("worker", "doing some work!");
                    std::invoke(*work);
                }
                else
                    std::this_thread::yield();
            }
        }
    };

    auto main_thread_work = [&](std::stop_token stop_token)
    {
        // while there's work to be done or system is not told to shutdown

        while (!work_queue.empty())
        {
            // attempt to grab some work and perform them if there is.
            {
                //threadsafe_logging("worker", "work detected!");

                auto work = work_queue.try_pop();
                if (work)
                {
                    //threadsafe_logging("worker", "doing some work!");
                    //std::invoke(work->fnc);
                    std::invoke(*work);
                }
                else
                    std::this_thread::yield();
            }
        }
    };

    auto future_is_ready = [](job& job)
    {
        for (auto& fu : job.tasks)
        {
            auto we_wait = fu.wait_for(std::chrono::nanoseconds(10));
            if (we_wait == std::future_status::timeout)
                return false;
        }

        // all futures are completed or corrupted.
        return true;
    };
    auto main_thread_work_with_jobs = [&](std::stop_token stop_token, job& job)
    {
        // while there's work to be done or system is not told to shutdown

        while (!future_is_ready(job))
        {
            // attempt to grab some work and perform them if there is.
            {
                //threadsafe_logging("worker", "work detected!");

                auto work = work_queue.try_pop();
                if (work)
                {
                    //threadsafe_logging("worker", "doing some work!");
                    //std::invoke(work->fnc);
                    std::invoke(*work);
                }
                else
                    std::this_thread::yield();
            }
        }
    };

    void initialize()
    {
        threadsafe_logging("main", "jobsystem attempting initialization....");

        // initialize all worker threads.
        for (std::size_t i = 0; i < global_worker_thread_count; i++)
        {
            std::jthread t{ worker_thread_function, shutdown_source.get_token() };
            worker_threads.emplace_back(std::move(t));
        }

        threadsafe_logging("main", std::to_string(worker_threads.size()) + " worker threads initialized....");

        threadsafe_logging("main", "jobsystem main thread initialized!");
        init_latch.count_down();

        threadsafe_logging("main", "jobsystem successfully initialized!");
    }

    void shutdown()
    {
        threadsafe_logging("main", "jobsystem attempting shutdown...");

        // request all other threads to shut down!
        shutdown_source.request_stop();

        // Gather all worker threads
        for (auto& worker : worker_threads)
        {
            worker.join();
        }

        threadsafe_logging("main", "jobsystem successfully shutdown!!");
    }


    return_t submit(functor_t functor)
    {
        std::packaged_task<void(void)> new_work{ functor };
        return_t future = new_work.get_future();
        work_queue.push(std::move(new_work));

        cv_any.notify_one();

        return future;
    }

    void submit(job& job, functor_t functor)
    {
        job.tasks.emplace_back(submit(functor));
    }

    void wait()
    {
        // busy wait. [TODO] Change to std::future instead.
        // blocking call that stalls this thread and waits for queue to be empty!
        //while(!work_queue.empty());
        std::invoke(main_thread_work, shutdown_source.get_token());
    }
    

    void wait(job& job)
    {
        // we do this so our thread still does work instead of afking while waiting for futures to be done.
        std::invoke(main_thread_work_with_jobs, shutdown_source.get_token(), job);
        // get rid of all the jobs we've done.
        job.tasks.clear();

    }
}
