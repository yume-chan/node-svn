#include "CppUnitTest.h"

#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#include <node.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// clang-format off

namespace tests
{
    TEST_CLASS(UnitTest1)
    {
    public:
        TEST_METHOD(TestMethod1)
        {
            //auto code = node::Start(2, new char*[2]{ "-e", "process.exit()" });
            auto code = node::Start(1, new char*[1]{ "C:\\Users\\Simon\\Desktop\\git\\node-svn\\tests\\index.js" });
        }
    };
}
