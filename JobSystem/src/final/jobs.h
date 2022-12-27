#pragma once
#include <functional>
#include <future>

namespace jobsystem
{
    static constexpr const bool enable_logging = true;

    // individual unit of work.
    using functor_t = std::function<void(void)>;
    using packaged_t = std::packaged_task<void(void)>;
    using return_t = std::future<std::invoke_result_t<functor_t>>;

    // a collective expression of meaningful work.
    struct job
    {
        std::vector<return_t> tasks;
    };

    return_t submit(functor_t functor);
    void submit(job& job, functor_t functor);

    void launch(job& job);

    void wait();
    void wait(job& job);

    return_t submit_and_launch(functor_t functor);
    void submit_and_launch(job& job, functor_t functor);

    void launch_and_wait(job& job);

    void initialize();
    void shutdown();
}
