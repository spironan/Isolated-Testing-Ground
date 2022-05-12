#pragma once

//#include <deque>
//#include <mutex>
#include <array>

template<typename T, size_t size = 4096>
class WorkStealingQueue
{
public:
    void Push(T* item);
    T* Pop();
    T* Steal();

private:
    //std::deque<T> m_queue;
    //std::mutex m_mutex;
    std::array<T*, size> m_queue;
    long m_top, m_bottom;
};

#include "WorkStealingQueue.hpp"

