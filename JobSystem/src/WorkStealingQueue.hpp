#pragma once
//#include <mutex>
#include <atomic>

static constexpr unsigned int NUMBER_OF_JOBS = 4096u;
static constexpr unsigned int MASK = NUMBER_OF_JOBS - 1u;

template<typename T, size_t size>
void WorkStealingQueue<T, size>::Push(T* item)
{
    //std::scoped_lock<decltype(m_mutex)> lock(m_mutex);

    const int32_t b = m_bottom;
    m_queue[b & MASK] = item;

    // ensure the job is written before b+1 is published to other threads.
    // on x86/64, a compiler barrier is enough.
    std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);

    m_bottom = b + 1;
}

template<typename T, size_t size>
T* WorkStealingQueue<T, size>::Pop()
{
    //std::scoped_lock<decltype(m_mutex)>  lock(m_mutex);
    
    int32_t const b = m_bottom - 1;
    m_bottom = b;

    _InterlockedExchange(static_cast<long*>(&m_bottom), b);

    int32_t t = m_top;
    if (t <= b)
    {
        // non-empty queue
        T* item = m_queue[b & MASK];
        if (t != b)
        {
            // there's still more than one item left
            return item;
        }

        // this is the last item in the queue
        if (_InterlockedCompareExchange(static_cast<long*>(&m_top), t + 1, t) != t)
        {
            // failed race against steal operation
            item = nullptr;
        }

        m_bottom = t + 1;
        return item;
    }
    else
    {
        // deque was already empty
        m_bottom = t;
        return nullptr;
    }

    //const int32_t jobCount = m_bottom - m_top;
    //if (jobCount <= 0)
    //{
    //    // no job left in the queue
    //    return nullptr;
    //}

    //--m_bottom;
    //return m_queue[m_bottom & MASK];
}

template<typename T, size_t size>
T* WorkStealingQueue<T, size>::Steal()
{
    //std::scoped_lock<decltype(m_mutex)> lock(m_mutex);
    
    const int32_t t = m_top;

    // ensure that top is always read before bottom.
    // loads will not be reordered with other loads on x86, so a compiler barrier is enough.
    std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);

    const int32_t b = m_bottom;
    if (t < b)
    {
        // non - empty queue
        T* item = m_queue[t & MASK];

        if (_InterlockedCompareExchange(static_cast<long*>(&m_top), t + 1, t) != t)
        {
            // a concurrent steal or pop operation removed an element from the deque in the meantime
            return nullptr;
        }

        return item;
    }
    else
    {
        // empty queue
        return nullptr;
    }
}