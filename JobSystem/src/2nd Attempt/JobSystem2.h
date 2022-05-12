#pragma once
#include <cstdint>
#include <functional>

#include <mutex>
#include <condition_variable>
#include <iostream>
#include <cassert>

namespace JobSystem
{
    //using Function = std::function<void(uintptr_t params)>;
    //enum class Priority
    //{
    //    LOW, NORMAL, HIGH, CRITICAL,
    //};

    //struct Counter;
    //Counter* AllocCounter();
    //void FreeCounter(Counter* pCounter);

    //struct Job
    //{
    //    Function            m_function{ nullptr };
    //    uintptr_t           m_params{ };
    //    Priority            m_priority{ NORMAL };
    //    //Counter* m_counter;
    //    std::atomic_int32_t m_counter{ 0 };
    //};
    //static constexpr size_t jobSize = sizeof(Job);

    //// Create Jobs for Job System
    //void CreateJob(Job const& job);

    //// Wait for Job to Terminate (for its counter to become zero)
    //void WaitForCounter(Counter* counter);


    //std::mutex g_mutex;
    //std::condition_variable cv;
    //bool g_ready = false;
    //void JobWorkerThread(void*)
    //{
    //    // keep on running jobs forever...
    //    while (true)
    //    {
    //        Job job;
    //        // wait for a job to become available
    //        std::unique_lock<std::mutex> lock(g_mutex);
    //        cv.wait(lock, [] { return !g_ready; });

    //        // copy the JobDeclaration locally and
    //        // release our mutex lock
    //        job = GetNextJobFromQueue();
    //        lock.unlock();
    //        // run the job
    //        job.m_function(job.m_params);
    //        // job is done! rinse and repeat...
    //    }
    //}

    // ATOMICITY

    void SpinLockTAS(long* lock)
    {
        while (_interlockedbittestandset(lock, 0))
        {
            std::this_thread::yield();
        }

        // when we get here, we know that we successfully
        // stored a value of true into *pLock AND that it
        // previously contained false, so no one else has
        // the lock -- we're done

    }

    void SpinLockXCHG(char* pLock)
    {
        volatile char old = '\0';
        while (true)
        {
            // emit the xchg instruction for 8-bit words
            _InterlockedExchange8(&old, *pLock);
            if (old == 'a')
            {
                // if we get back false,
                // then the lock succeeded
                break;
            }
            _mm_pause();
            //std::this_thread::yield();


        }
    }

    void SpinLockCAS(long* pValue)
    {
        const int initialValue = *pValue;
        const int newValue = 0xFFFFFFFF;
        while (!_InterlockedCompareExchange(pValue, newValue, initialValue))
        {
            // must be locked by someone else -- retry
            _mm_pause();
        }
        std::cout << *pValue << std::endl;
    }

    void AtomicIncrementCAS(long* pValue)
    {
        const int initialValue = *pValue;
        const int newValue = initialValue + 1;
        while (!_InterlockedCompareExchange(pValue, newValue, initialValue))
        {
            // must be locked by someone else -- retry
            _mm_pause();
        }
        std::cout << *pValue << std::endl;

        std::atomic_bool test;
    }



    // COMPILER BARRIER 
    // PROS : prevent compiler from reordering, 
    // CONS : CPU's out of order execution logic from reordering instructions at RUNTIME still possible

    int32_t g_dataCB = 0;
    int32_t g_readyCB = 0;

    void ProducerThreadCompilerBarrier()
    {
        // produce some data
        g_dataCB = 42;
        // dear compiler: please don't reorder
        // instructions across this barrier!
        //asm volatile("" ::: "memory") GCC
        _ReadWriteBarrier();
        // inform the consumer
        g_readyCB = 1;
    }

    void ConsumerThreadCompilerBarrier()
    {
        // wait for the data to be ready
        while (!g_readyCB)
            _mm_pause();
        // dear compiler: please don't reorder
        // instructions across this barrier!
        //asm volatile("" ::: "memory") GCC
        _ReadWriteBarrier();

        // consume the data
        assert(g_dataCB == 42);
    }


    // TRUE MEMORY FENCES
    // PROS : Serve as an insutrction reordering barriers for both compiler and CPU
    // Also prevents memory reordering bugs
    // Technically all we need is memory fences and atomic instruction to write lock-free algos
    
    // Producer Consumer 2.0

    int32_t g_dataMF = 0;
    int32_t g_readyMF = 0;
    
    void ProducerThreadMemoryFence() // running on Core 1
    {
        g_dataMF = 42;
        // make the write to g_ready into a write-release
        // by placing a release fence *before* it
        //MemoryBarrier();
        std::atomic_thread_fence(std::memory_order_release);
        //RELEASE_FENCE()
        g_readyMF = 1;
    }

    void ConsumerThreadMemoryFence() // running on Core 2
    {
        // make the read of g_ready into a read-acquire
        // by placing an acquire fence *after* it
        while (!g_readyMF)
            _mm_pause();

        std::atomic_thread_fence(std::memory_order_acquire);
        //ACQUIRE_FENCE();
        // we can now read g_data safely...
        assert(g_dataMF == 42);
    }


    class SpinLock
    {
        std::atomic_flag m_atomic;

    public:
        SpinLock() 
            : m_atomic() 
        {}

        bool TryAcquire()
        {
            // use an acquire fence to ensure all subsequent
            // reads by this thread will be valid
            bool alreadyLocked = m_atomic.test_and_set(std::memory_order_acquire);
            return !alreadyLocked;
        }

        void Acquire()
        {
            // spin until successful acquire
            while (!TryAcquire())
            {
                // reduce power consumption on Intel CPUs
                // (can substitute with std::this_thread::yield()
                // on platforms that don't support CPU pause, if
                // thread contention is expected to be high)
                std::this_thread::yield();
            }
        }

        void Release()
        {
            // use release semantics to ensure that all prior
            // writes have been fully committed before we unlock
            m_atomic.clear(std::memory_order_release);
        }

    };

    template<class LOCK>
    class ScopedLock
    {
        using lock_t = LOCK;
        lock_t* m_pLock;

    public:
        explicit ScopedLock(lock_t& lock) : m_pLock(&lock)
        {
            m_pLock->Acquire();
        }

        ~ScopedLock()
        {
            m_pLock->Release();
        }
    };

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
}