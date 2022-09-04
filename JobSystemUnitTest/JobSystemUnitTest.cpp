#include "pch.h"
#include "CppUnitTest.h"

#include <JobSystem/src/ThreadPool/ThreadPoolTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace JobSystemUnitTest
{
    TEST_CLASS(JobSystemUnitTest)
    {
    public:
        
        TEST_METHOD(TestMethod1a)
        {
            Test::Test1a();
        }

        TEST_METHOD(TestMethod1b)
        {
            Test::Test1b();
        }

        TEST_METHOD(TestMethod2a)
        {
            Test::Test2a();
        }

        TEST_METHOD(TestMethod2b)
        {
            Test::Test2b();
        }
        TEST_METHOD(TestMethod3)
        {
            Test::Test3();
        }

        TEST_METHOD(TestMethod4)
        {
            Test::Test4();
        }
    };
}
