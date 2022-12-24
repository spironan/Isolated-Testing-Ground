#include "src/final/jobs.h"
#include <thread>

#include <string>
#include <iostream>
#include <latch>

static std::uint32_t total_work_count = 0;

int main()
{
    jobsystem::initialize();

    {
        // Choice 1 : simpliest
        auto test_fnc = []() {  std::cout << "[task " << ++total_work_count << "] doing some fake work! \n";  };
        /*jobsystem::work some_work;
        some_work.fnc = */
        jobsystem::submit(test_fnc);
        jobsystem::wait();
    }


    {
        // Choice 2 : handle futures yourself.
        std::vector<std::future<void>> result;
        auto task_2 = []() { std::cout << "[task " << ++total_work_count << "] doing some fake work! \n"; };
        for (int i = 0; i < 1000; ++i)
            result.emplace_back(jobsystem::submit(task_2));

        for (auto& fu : result)
            fu.get();
    }

    {
        // Choice 3 : use the job to handle for you
        jobsystem::job job{};
    
        auto task_3 = []() 
        { 
            std::cout << "[task " << ++total_work_count << "] doing some fake work! \n"; 
            static thread_local int sub_task_count = 0;

            auto mini_task = []() { std::cout << "[" << std::this_thread::get_id()  << "]\t doing sub-task " << ++sub_task_count << "!\n"; };
            
            jobsystem::job sub_job{};
            
            for(int i = 0; i < 10; ++i)
                jobsystem::submit(mini_task);
            
            jobsystem::wait(sub_job);
        };

        for (int i = 0; i < 10; ++i)
            jobsystem::submit(job, task_3);
    
        jobsystem::wait(job);
    }

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    jobsystem::shutdown();

    //struct job {
    //    const std::string name;
    //    std::string product{ "not worked" };
    //    std::thread action{};
    //} jobs[] = { {"annika"}, {"buru"}, {"chuck"} };

    //std::latch work_done{ std::size(jobs) };
    //std::latch start_clean_up{ 1 };

    //auto work = [&](job& my_job) {
    //    my_job.product = my_job.name + " worked";
    //    work_done.count_down();
    //    start_clean_up.wait();
    //    my_job.product = my_job.name + " cleaned";
    //};

    //std::cout << "Work starting... ";
    //for (auto& job : jobs) {
    //    job.action = std::thread{ work, std::ref(job) };
    //}
    //work_done.wait();
    //std::cout << "done:\n";
    //for (auto const& job : jobs) {
    //    std::cout << "  " << job.product << '\n';
    //}

    //std::cout << "Workers cleaning up... ";
    //start_clean_up.count_down();
    //for (auto& job : jobs) {
    //    job.action.join();
    //}
    //std::cout << "done:\n";
    //for (auto const& job : jobs) {
    //    std::cout << "  " << job.product << '\n';
    //}
}
