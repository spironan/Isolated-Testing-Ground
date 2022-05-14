#pragma once

#include <xmmintrin.h>
#include <iostream>
#include <cassert>

#define GLM_FORCE_PURE
#define GLM_FORCE_SSE42 //GLM_FORCE_SSE2 or GLM_FORCE_SSE42 if your processor supports it
//#define GLM_FORCE_SIMD_AVX2
#define GLM_FORCE_ALIGNED
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/matrix.hpp>

namespace simd
{
    __m128 g_reg;

    void test128f()
    {
    }

    __m128 Set128f(float x, float y, float z, float w)
    {
        return _mm_set_ps(w, z, y ,x);
    }

    __m128 Mult128f(__m128 first, __m128 second)
    {
        return _mm_mul_ps(first, second);
    }

    void TestAddSSE()
    {
        alignas(16) float A[4];
        alignas(16) float B[4] = { 2.0f, 4.0f, 6.0f, 8.0f };
        // set a = (1, 2, 3, 4) from literal values, and
        // load b = (2, 4, 6, 8) from a floating-point array
        // just to illustrate the two ways of doing this
        // (remember that _mm_set_ps() is backwards!)
        __m128 a = _mm_set_ps(4.0f, 3.0f, 2.0f, 1.0f);
        __m128 b = _mm_load_ps(&B[0]);
        // add the vectors
        __m128 r = _mm_add_ps(a, b);
        // store '__m128 a' into a float array for printing
        _mm_store_ps(&A[0], a);
        // store result into a float array for printing
        alignas(16) float R[4];
        _mm_store_ps(&R[0], r);
        // inspect results
        printf("a = %.1f %.1f %.1f %.1f\n",
            A[0], A[1], A[2], A[3]);
        printf("b = %.1f %.1f %.1f %.1f\n",
            B[0], B[1], B[2], B[3]);
        printf("r = %.1f %.1f %.1f %.1f\n",
            R[0], R[1], R[2], R[3]);
    }
    
    void Standard_Loop(int count,
        float* results,
        const float* dataA,
        const float* dataB)
    {
        for (int i = 0; i < count; ++i)
        {
            results[i] = dataA[i] + dataB[i];
        }
    }

    void Vectorized_Loop(int count,
        float* results,
        const float* dataA,
        const float* dataB)
    {
        // NOTE: the caller needs to ensure that the size of
        // all 3 arrays are equal, and a multiple of four!
        assert(count % 4 == 0);
        for (int i = 0; i < count; i += 4)
        {
            __m128 a = _mm_load_ps(&dataA[i]);
            __m128 b = _mm_load_ps(&dataB[i]);
            __m128 r = _mm_add_ps(a, b);
            _mm_store_ps(&results[i], r);
        }
    }

    void DotArrays_ref(int count,
        float r[],
        const float a[],
        const float b[])
    {
        for (int i = 0; i < count; ++i)
        {
            // treat each block of four floats as a
            // single four-element vector
            const int j = i * 4;
            r[i] = a[j + 0] * b[j + 0] // ax*bx
                + a[j + 1] * b[j + 1] // ay*by
                + a[j + 2] * b[j + 2] // az*bz
                + a[j + 3] * b[j + 3]; // aw*bw
        }
    }

    // DONT DO THIS! its actually slower than the ref one
    void DotArrays_sse_horizontal(int count,
        float r[],
        const float a[],
        const float b[])
    {
        for (int i = 0; i < count; ++i)
        {
            // treat each block of four floats as a
            // single four-element vector
            const int j = i * 4;
            __m128 va = _mm_load_ps(&a[j]); // ax,ay,az,aw
            __m128 vb = _mm_load_ps(&b[j]); // bx,by,bz,bw
            __m128 v0 = _mm_mul_ps(va, vb);
            // add across the register...
            //__m128 v1 = _mm_hadd_ps(v0, v0);
            // (v0w+v0z, v0y+v0x, v0w+v0z, v0y+v0x)
            //__m128 vr = _mm_hadd_ps(v1, v1);
            // (v0w+v0z+v0y+v0x, v0w+v0z+v0y+v0x,
            // v0w+v0z+v0y+v0x, v0w+v0z+v0y+v0x)
            //_mm_store_ss(&r[i], vr); // extract vr.x as a float
        }
    }


    void DotArrays_sse(int count,
        float r[],
        const float a[],
        const float b[])
    {
        for (int i = 0; i < count; i += 4)
        {
            __m128 vaX = _mm_load_ps(&a[(i + 0) * 4]); // a[0,4,8,12]
            __m128 vaY = _mm_load_ps(&a[(i + 1) * 4]); // a[1,5,9,13]
            __m128 vaZ = _mm_load_ps(&a[(i + 2) * 4]); // a[2,6,10,14]
            __m128 vaW = _mm_load_ps(&a[(i + 3) * 4]); // a[3,7,11,15]
            __m128 vbX = _mm_load_ps(&b[(i + 0) * 4]); // b[0,4,8,12]
            __m128 vbY = _mm_load_ps(&b[(i + 1) * 4]); // b[1,5,9,13]
            __m128 vbZ = _mm_load_ps(&b[(i + 2) * 4]); // b[2,6,10,14]
            __m128 vbW = _mm_load_ps(&b[(i + 3) * 4]); // b[3,7,11,15]
            __m128 result;
            result = _mm_mul_ps(vaX, vbX);
            result = _mm_add_ps(result, _mm_mul_ps(vaY, vbY));
            result = _mm_add_ps(result, _mm_mul_ps(vaZ, vbZ));
            result = _mm_add_ps(result, _mm_mul_ps(vaW, vbW));
            _mm_store_ps(&r[i], result);
        }
    }



    // CORRECT SSE WITH TRANSPOSING DONE
    void DotArrays_sse_transpose(int count,
        float r[],
        const float a[],
        const float b[])
    {
        assert(count % 4 == 0);

        for (int i = 0; i < count; i += 4)
        {
            __m128 vaX = _mm_load_ps(&a[(i + 0) * 4]); // a[0,1,2,3]
            __m128 vaY = _mm_load_ps(&a[(i + 1) * 4]); // a[4,5,6,7]
            __m128 vaZ = _mm_load_ps(&a[(i + 2) * 4]); // a[8,9,10,11]
            __m128 vaW = _mm_load_ps(&a[(i + 3) * 4]); // a[12,13,14,15]
            __m128 vbX = _mm_load_ps(&b[(i + 0) * 4]); // b[0,1,2,3]
            __m128 vbY = _mm_load_ps(&b[(i + 1) * 4]); // b[4,5,6,7]
            __m128 vbZ = _mm_load_ps(&b[(i + 2) * 4]); // b[8,9,10,11]
            __m128 vbW = _mm_load_ps(&b[(i + 3) * 4]); // b[12,13,14,15]

            _MM_TRANSPOSE4_PS(vaX, vaY, vaZ, vaW);
            // vaX = a[0,4,8,12]
            // vaY = a[1,5,9,13]
            // ...
            _MM_TRANSPOSE4_PS(vbX, vbY, vbZ, vbW);
            // vbX = b[0,4,8,12]
            // vbY = b[1,5,9,13]
            // ...
            __m128 result;
            result = _mm_mul_ps(vaX, vbX);
            result = _mm_add_ps(result, _mm_mul_ps(vaY, vbY));
            result = _mm_add_ps(result, _mm_mul_ps(vaZ, vbZ));
            result = _mm_add_ps(result, _mm_mul_ps(vaW, vbW));
            _mm_store_ps(&r[i], result);
        }
    }

    void GLMMatrixMultiply(size_t dividable_by_four_size)
    {
        //assert(dividable_by_four_size % 4 == 0);
        //assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
        glm::mat4 testMatA = glm::mat4{ 1.0f, 2.0f, 3.0f, 4.0f, 10.0f, 20.0f, 30.0f, 40.0f, 100.0f, 200.0f, 300.0f, 400.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f };
        glm::mat4 testMatB = glm::mat4{ 1.0f, 2.0f, 3.0f, 4.0f, 10.0f, 20.0f, 30.0f, 40.0f, 100.0f, 200.0f, 300.0f, 400.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f };
        glm::mat4 resMat = glm::mat4{ 1.f };
        for (std::size_t i = 1; i <= dividable_by_four_size; ++i)
        {
            resMat = testMatA * testMatB;
            testMatA *= i;
            testMatB *= i;
            //testMat *= testMat;
        }
        
    }

    void GLMDotProduct(size_t dividable_by_four_size)
    {
        //assert(dividable_by_four_size % 4 == 0);
        //assert(dividable_by_four_size <= std::numeric_limits<std::size_t>::max());
        
        glm::vec4 testA{ 1,2,3,4 }, testB{ 2,4,6,8 };
        float res{ 0.f };
        for (std::size_t i = 1; i <= dividable_by_four_size; ++i)
        {
            res = glm::dot(testA, testB);
            testA *= i;
            testB *= i;
            //testMat *= testMat;
        }
        //std::cout << res << std::endl;
    }
}