#pragma once

#include <mutex>
#include <iostream>
#include <fstream>

// Unique Lock provides more flexibility than Lock guard.
// the flexibility of a unique lock is NOT FREE.
// if performance is of concern and flexibility of unique lock is not required
// use lock guard instead

std::mutex global_mutex;
std::ofstream f;
std::once_flag global_once_flag;

void EntryPoint()
{
    // a way to initialize and call something once, both efficient and threadsafe!!!
    std::call_once(global_once_flag, [&]() { f.open("log.txt"); });

    //std::lock_guard<std::mutex> locker(global_mutex);

    // more heavy weight than lock_guard but provides more flexibility.
    std::unique_lock<std::mutex> ulocker(global_mutex);
    // we can defer locking if we like with the second parameter.
    //std::unique_lock<std::mutex> ulocker(global_mutex, std::defer_lock);
    // ulocker.lock();
    
    // Critical Code
    f << "Lets " <<  "log " << "some " << "critical " << "code ";

    // we can also choose to unlock a unique lock 
    ulocker.unlock();
    // do otherthings that need not be sequential [threadsafe]
}
