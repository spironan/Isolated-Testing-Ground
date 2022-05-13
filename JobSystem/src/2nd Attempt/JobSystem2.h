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

    template< class T >
    class SinglyLinkedList
    {
        struct Node
        {
            T m_data;
            Node* m_pNext;
        };

        std::atomic<Node*> m_head { nullptr };

    public:

        void push_front(T data)
        {
            // prep the transaction locally
            auto pNode = new Node();
            pNode->m_data = data;
            pNode->m_pNext = m_head;
            // commit the transaction atomically
            // (retrying until it succeeds)
            while (!m_head.compare_exchange_weak(pNode->m_pNext, pNode));
        }
    };
}