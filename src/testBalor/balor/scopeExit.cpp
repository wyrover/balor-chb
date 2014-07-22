#include <balor/scopeExit.hpp>

#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testScopeExit {



testCase(scopeExitFunction) {
	int count = 0;
	{
		scopeExit([&] () {
			++count;
		});
		testAssert(count == 0);
	}
	testAssert(count == 1);
}


	}
}