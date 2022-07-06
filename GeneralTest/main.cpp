#include <thread>
#include <new>
#include <atomic>

#include "src/utils/Timer.h"

#include "src/SIMD.h"

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
    /*{
        Profiler profile("False Sharing Enabled");

        #define FALSE_SHARING
        FalseSharing();
    }

    {
        Profiler profile("False Sharing Disabled");
        #undef FALSE_SHARING
        FalseSharing();
    }

    {
        simd::TestAddSSE();
    }*/

    /*static constexpr std::size_t dividable_by_four_size = 4096 * 4096 * 6;
    static_assert(dividable_by_four_size % 4 == 0);
    static_assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
    float* arrA, *arrB, *res;
    arrA = new float[dividable_by_four_size];
    arrB = new float[dividable_by_four_size];
    res = new float[dividable_by_four_size];

    for (std::size_t i = 0; i < dividable_by_four_size; ++i)
    {
        arrA[i] = i;
        arrB[i] = i * 2.0f;
    }

    {
        Profiler profile("Standard Loop");
        simd::Standard_Loop(dividable_by_four_size, res, arrA, arrB);
    }

    {
        Profiler profile("Vectorized Loop");
        simd::Vectorized_Loop(dividable_by_four_size, res, arrA, arrB);
    }

    delete[] arrA;
    delete[] arrB;
    delete[] res;*/

    Profiler profile("programme total lifetime");
    
    std::cout << "SIMD" << std::endl;

    {
#ifdef _DEBUG
        static constexpr std::size_t dividable_by_four_size = 4096 * 512;
#elif _RELEASE
        static constexpr std::size_t dividable_by_four_size = 4096 * 4096 * 64;
#endif
        static_assert(dividable_by_four_size % 4 == 0);
        static_assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
        Profiler profile("glm dot products for " + std::to_string(dividable_by_four_size));
        simd::GLMDotProduct(dividable_by_four_size);
    }

    {
#ifdef _DEBUG
        static constexpr std::size_t dividable_by_four_size = 4096 * 256;
#elif _RELEASE
        static constexpr std::size_t dividable_by_four_size = 4096 * 1024;
#endif
        static_assert(dividable_by_four_size % 4 == 0);
        static_assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
        Profiler profile("glm matrix multiply for " + std::to_string(dividable_by_four_size));
        simd::GLMMatrixMultiply(dividable_by_four_size);
    }
    std::cout << "========================================================" << std::endl;

//    #undef GLM_FORCE_PURE
//    #undef GLM_FORCE_SSE42 //GLM_FORCE_SSE2 or GLM_FORCE_SSE42 if your processor supports it
//    //#define GLM_FORCE_SIMD_AVX2
//    #undef GLM_FORCE_ALIGNED
//
//    std::cout << "STANDARD" << std::endl;
//
//    {
//#ifdef _DEBUG
//        static constexpr std::size_t dividable_by_four_size = 4096 * 512;
//#elif _RELEASE
//        static constexpr std::size_t dividable_by_four_size = 4096 * 4096 * 64;
//#endif
//        static_assert(dividable_by_four_size % 4 == 0);
//        static_assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
//        Profiler profile("glm dot products for " + std::to_string(dividable_by_four_size));
//        simd::GLMDotProduct(dividable_by_four_size);
//    }
//
//    {
//#ifdef _DEBUG
//        static constexpr std::size_t dividable_by_four_size = 4096 * 256;
//#elif _RELEASE
//        static constexpr std::size_t dividable_by_four_size = 4096 * 1024;
//#endif
//        static_assert(dividable_by_four_size % 4 == 0);
//        static_assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
//        Profiler profile("glm matrix multiply for " + std::to_string(dividable_by_four_size));
//        simd::GLMMatrixMultiply(dividable_by_four_size);
//    }
//    
//    std::cout << "========================================================" << std::endl;

}