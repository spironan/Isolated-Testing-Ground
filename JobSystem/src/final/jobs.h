#pragma once
#include <functional>

namespace jobsystem
{
    // there should be no jobs limit

    void initialize();
    void shutdown();

    struct work
    {
        std::function<void(void)> fnc;
    };

    void submit(work work);
    void wait();
    //void submit(work const& work);

    //// job is not done immediately, you are expected to wait.
    //template<typename T>
    //std::future<void> submitwork(std::function<T> func)
    //{
    //    
    //}

    namespace internal
    {
        void do_once();
    }

}
