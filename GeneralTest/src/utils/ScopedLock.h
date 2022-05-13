#pragma once

template<typename Lock>
class ScopedLock
{
    Lock * m_pLock;

public:
    explicit ScopedLock(Lock& lock)
        : m_pLock(&lock)
    {
        m_pLock->Acquire();
    }

    ~ScopedLock()
    {
        m_pLock->Release();
    }
};