#include <thread>
#include <new>
#include <atomic>

#include "utils/Timer.h"

using namespace std;

constexpr size_t CL_SIZE = hardware_destructive_interference_size;

void FalseSharing()
{
#if defined(FALSE_SHARING)
    constexpr size_t SECOND_ALIGN = sizeof(int);
#else
    constexpr size_t SECOND_ALIGN = CL_SIZE;
#endif

    using atomic_type = atomic<int>;

    struct shared_or_not
    {
        atomic_type alignas(CL_SIZE) a;
        atomic_type alignas(SECOND_ALIGN) b;
    };

    shared_or_not sharedOrNot;
    auto theThread = [](atomic_type* atomicValue)
    {
        for (size_t r = 100'000'000; r--; )
            ++* atomicValue;
    };
    jthread
        threadA(theThread, &sharedOrNot.a),
        threadB(theThread, &sharedOrNot.b);
}

int main()
{
    {
        Profiler profile("False Sharing Enabled");

        #define FALSE_SHARING
        FalseSharing();
    }

    {
        Profiler profile("False Sharing Disabled");
        #undef FALSE_SHARING
        FalseSharing();
    }
}