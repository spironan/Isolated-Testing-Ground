#pragma once
#include <cassert>
#include <atomic>
#include <cstdint>
#include <thread>

class ReentrantLock32
{
    std::atomic<std::size_t> m_atomic;
    std::int32_t m_refCount;

public:
    ReentrantLock32()
        : m_atomic(0)
        , m_refCount(0)
    { }

    void Acquire()
    {
        std::hash<std::thread::id> hasher;
        std::size_t tid = hasher(std::this_thread::get_id());
        // if this thread doesn't already hold the lock...
        if (m_atomic.load(std::memory_order_relaxed) != tid)
        {
            // ... spin wait until we do hold it
            std::size_t unlockValue = 0;
            while (!m_atomic.compare_exchange_weak(
                unlockValue,
                tid,
                std::memory_order_relaxed, // fence below!
                std::memory_order_relaxed))
            {
                unlockValue = 0;
                std::this_thread::yield();
            }
        }
        // increment reference count so we can verify that
        // Acquire() and Release() are called in pairs
        ++m_refCount;
        // use an acquire fence to ensure all subsequent
        // reads by this thread will be valid
        std::atomic_thread_fence(std::memory_order_acquire);
    }

    void Release()
    {
        // use release semantics to ensure that all prior
        // writes have been fully committed before we unlock
        std::atomic_thread_fence(std::memory_order_release);
        std::hash<std::thread::id> hasher;
        std::size_t tid = hasher(std::this_thread::get_id());
        std::size_t actual = m_atomic.load(std::memory_order_relaxed);
        assert(actual == tid);
        --m_refCount;
        if (m_refCount == 0)
        {
            // release lock, which is safe because we own it
            m_atomic.store(0, std::memory_order_relaxed);
        }
    }

    bool TryAcquire()
    {
        std::hash<std::thread::id> hasher;
        std::size_t tid = hasher(std::this_thread::get_id());
        bool acquired = false;
        if (m_atomic.load(std::memory_order_relaxed) == tid)
        {
            acquired = true;
        }
        else
        {
            std::size_t unlockValue = 0;
            acquired = m_atomic.compare_exchange_strong(
                unlockValue,
                tid,
                std::memory_order_relaxed, // fence below!
                std::memory_order_relaxed);
        }

        if (acquired)
        {
            ++m_refCount;
            std::atomic_thread_fence(
                std::memory_order_acquire);
        }

        return acquired;
    }
};