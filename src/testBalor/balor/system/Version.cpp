#include <balor/system/Version.hpp>

#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace system {
		namespace testVersion {



testCase(operators) {
	// operator <
	testAssert(Version(1) < Version(2));
	testAssert(!(Version(1, 1) < Version(1, 1)));
	testAssert(Version(1, 1) < Version(1, 2));
	testAssert(!(Version(1, 1, 1) < Version(1, 1, 1)));
	testAssert(Version(1, 1, 1) < Version(1, 1, 2));
	testAssert(!(Version(1, 1, 1, 1) < Version(1, 1, 1, 1)));
	testAssert(Version(1, 1, 1, 1) < Version(1, 1, 1, 2));

	// operator <=
	testAssert(!(Version(2) <= Version(1)));
	testAssert(Version(1, 1) <= Version(1, 1));

	// operator ==
	testAssert(Version(1, 2, 3, 4) == Version(1, 2, 3, 4));
	testAssert(!(Version(1, 2, 3, 4) == Version(0, 2, 3, 4)));
	testAssert(!(Version(1, 2, 3, 4) == Version(1, 0, 3, 4)));
	testAssert(!(Version(1, 2, 3, 4) == Version(1, 2, 0, 4)));
	testAssert(!(Version(1, 2, 3, 4) == Version(1, 2, 3, 0)));

	// operator !=
	testAssert(!(Version(1, 2, 3, 4) != Version(1, 2, 3, 4)));
	testAssert(Version(1, 2, 3, 4) != Version(0, 2, 3, 4));
}


testCase(properties) {
	Version version(5, 1, 10, 0x10c80);
	testAssert(version.build() == 10);
	testAssert(version.major() == 5);
	testAssert(version.minor() == 1);
	testAssert(version.revision() == 0x10c80);
}



		}
	}
}