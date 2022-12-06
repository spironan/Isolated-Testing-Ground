#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <future>

/*
* Base Example : Create a thread that just does some work and display result

    void Factorial(int N)
    {
        int res = 1;
        for (int i = N; i > 1; --i)
            res *= i;

        std::cout << " Result is " << res << std::endl;
    }

    void main()
    {
        std::thread t1(Factorial, 4);
        t1.join();
    }

*/

/*
* Example Modified : What if we want to get returned result?
*
* Can see it is very messy just to do something so simple.

    std::mutex global_mutex;
    std::condition_variable global_cv;

    void Factorial(int N, int& x)
    {
        int res = 1;
        for (int i = N; i > 1; --i)
            res *= i;

        std::cout << " Result is " << res << std::endl;
        x = res;
        // needs mutex to make sure this is finished
        // needs cv to tell it to join later on.
    }

    void main()
    {
        int final_result{};
        std::thread t1{ Factorial, 4 , std::ref(final_result) };
        t1.join();
    }
*/

/*
* Example Using Async.
*/

int Factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; --i)
        res *= i;

    std::cout << "Result is " << res << std::endl;
    return res;
}

int FactorialWithPromise(std::future<int>& f)
{
    int res = 1;
    int N = f.get();    // expects the promise to come true. 
                        // Otherwise we get std::future_errc::broken_promise as exception.
    for (int i = N; i > 1; --i)
        res *= i;

    std::cout << "Result is " << res << std::endl;
    return res;
}

// take note shared future should be passed by copy!
int FactorialWithSharedFuture(std::shared_future<int> f)
{
    int res = 1;
    int N = f.get();    // expects the promise to come true. 
                        // Otherwise we get std::future_errc::broken_promise as exception.
    for (int i = N; i > 1; --i)
        res *= i;

    std::cout << "Result is " << res << std::endl;
    return res;
}

void EntryPoint()
{
    // Futures can only be moved and cannot be copied.
    {
        int x;

        // The first parameter decides whether or not a separate thread actually launches.
        // the default value for this parameter is std::launch::async | std::launch::deferred <- implementation defined.
        // std::launch::deferred will not create a separate thread and wait till when get is called and launch the function there.
        // std::launch::async will be guaranteed to create a separate thread
        std::future<int> fu = std::async(std::launch::async | std::launch::deferred, Factorial, 4);
        // the above is equivalent to this : std::future<int> fu = std::async(Factorial, 4);

        x = fu.get();
        // calling get again crashes.
        // x = fu.get();
        std::cout << "X retrieved value is : " << x << std::endl;
    }

    // Promises Just like Futures CANNOT be copied. Only Moved
    {
        int x;
        // default initialize promise, value is not set yet.
        std::promise<int> p;
        // future awaits for once the promise has been set
        std::future<int> f = p.get_future();
        // future is passed into function and thus function also waits until promise is valid
        std::future<int> fu = std::async(std::launch::async, FactorialWithPromise, std::ref(f));

        // do something else : simulating by sleeping 100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // finally we set the promise which should than trigger the others to continue.
        p.set_value(10);
        // we retrieve the value from the async function
        // Note we have to manage where we get and the ordering otherwise it'll deadlock
        x = fu.get();

        std::cout << "X retrieved value is : " << x << std::endl;
    }

    // what if we want to make run many async functions? We all have a Shared future
    // a shared future can be passed by value!!
    {

        int x;
        // default initialize promise, value is not set yet.
        std::promise<int> p;
        // future awaits for once the promise has been set
        std::future<int> f = p.get_future();
        // create a shared future to be used
        std::shared_future<int> sf = f.share();
        // future is passed into function and thus function also waits until promise is valid
        std::future<int> fu = std::async(std::launch::async, FactorialWithSharedFuture, sf);
        std::future<int> fu2 = std::async(std::launch::async, FactorialWithSharedFuture, sf);
        std::future<int> fu3 = std::async(std::launch::async, FactorialWithSharedFuture, sf);
        // imagine if there were more threads running

        // do something else : simulating by sleeping 100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // finally we set the promise which should than trigger the others to continue.
        p.set_value(5);
        // we retrieve the value from the async function
        // Note we have to manage where we get and the ordering otherwise it'll deadlock
        x = fu.get();

        std::cout << "X retrieved value is : " << x << std::endl;
    }

}