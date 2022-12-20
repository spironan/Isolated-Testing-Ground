#include "src/final/jobs.h"
#include <thread>

#include <string>
#include <iostream>
#include <latch>

static std::uint32_t total_work_count = 0;

int main()
{

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    jobsystem::work some_work;
    some_work.fnc = []() {  std::cout << "[task " << ++total_work_count << "] doing some fake work! \n";  };
    jobsystem::work some_work_2;
    some_work_2.fnc = []() { std::cout << "[task " << ++total_work_count << "] doing some fake work! \n"; };
    jobsystem::submit(std::move(some_work));
    for (int i = 0; i < 500; ++i)
        jobsystem::submit(some_work_2);

    jobsystem::initialize();
    
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    
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
