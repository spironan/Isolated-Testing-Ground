#pragma once
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <future>

void factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; --i)
        res *= i;

    std::cout << " Result is " << res << std::endl;
}

int Factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; --i)
        res *= i;

    std::cout << " Result is " << res << std::endl;
    return res;
}

void EntryPoint()
{
    /* thread */
    std::thread t1(factorial, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    std::this_thread::sleep_until(tp);

    /* mutex */
    std::mutex mu;
    std::lock_guard<std::mutex> locker(mu);
    std::unique_lock<std::mutex> ulocker(mu);
    ulocker.try_lock();
    ulocker.try_lock_for(std::chrono::microseconds(500));
    ulocker.try_lock_until(tp);

    /* Condition Variable */
    std::condition_variable cond;
    cond.wait(ulocker);
    cond.wait_for(ulocker, std::chrono::microseconds(2));
    cond.wait_until(ulocker, tp);

    /* Future and Promises */
    std::promise<int> p;
    std::future<int> f = p.get_future();
    f.get();    // if data is not available it'll call wait internally.
    f.wait();
    f.wait_for(std::chrono::nanoseconds(100));
    f.wait_until(tp);

    /* Async */
    std::future<int> fu = std::async(std::launch::async, Factorial, 6);

    /* Packaged Task */
    std::packaged_task<int(int)> t(factorial);
    std::future<int> fu2 = t.get_future();
    t(6);

}
