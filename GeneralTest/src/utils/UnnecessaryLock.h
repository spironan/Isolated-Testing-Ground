#pragma once
#include <cassert>

class UnnecessaryLock
{
    volatile bool m_locked;
public:
    void Acquire()
    {
        // assert no one already has the lock
        assert(!m_locked);
        // now lock (so we can detect overlapping
        // critical operations if they happen)
        m_locked = true;
    }
    void Release()
    {
        // assert correct usage (that Release()
        // is only called after Acquire())
        assert(m_locked);
        // unlock
        m_locked = false;
    }
};
    
class UnnecessaryScopedLock
{
    UnnecessaryLock m_lock;

public:
    explicit UnnecessaryScopedLock()
    {
        m_lock.Acquire();
    }

    ~UnnecessaryScopedLock() { m_lock.Release(); }
};

#if ASSERTIONS_ENABLED
    #define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L) (L).Acquire()
    #define END_ASSERT_LOCK_NOT_NECESSARY(L) (L).Release()
#else
    #define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L)
    #define END_ASSERT_LOCK_NOT_NECESSARY(L)
#endif

#if ASSERTIONS_ENABLED
    #define ASSERT_LOCK_NOT_NECESSARY(J,L) \
    UnnecessaryLockJanitor J(L)
#else
    #define ASSERT_LOCK_NOT_NECESSARY(J,L)
#endif