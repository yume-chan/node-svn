#include <node.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace coverage
{
	TEST_CLASS(coverage)
	{
	public:

		TEST_METHOD(run)
		{
            const int argc = 1;
            char* argv[argc] = {
                "../node_modules/mocha/bin/_mocha"
            };
            node::Start(argc, argv);
		}

	};
}
