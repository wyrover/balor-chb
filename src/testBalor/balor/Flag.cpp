#include <balor/Flag.hpp>

#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testFlag {



testCase(construct) {
	Flag<int> defConst;
	Flag<int> valueConst(5);
	testAssert(int(valueConst) == 5);
	Flag<int> copyConst(valueConst);
	testAssert(int(copyConst) == 5);
}


testCase(operators) {
	Flag<int> flag;
	flag = 4;
	testAssert(int(flag) == 4);
	flag = Flag<int>(6);
	testAssert(int(flag) == 6);
}


testCase(maskTest) {
	Flag<int> flag(0x103);
	testAssert(flag[0x002] == true);
	testAssert(flag[0x100] == true);
	testAssert(flag[0x004] == false);
	testAssert(flag[0x00f] == false);
}


testCase(set) {
	Flag<int> flag(0x01);
	flag.set(0x02);
	testAssert(int(flag) == 0x03);
	flag.set(0x01, false);
	testAssert(int(flag) == 0x02);
	flag.set(0xf0, true);
	testAssert(int(flag) == 0xf2);
}


testCase(flagFunction) {
	Flag<int> value;
	value = toFlag(5);
	testAssert(int(value) == 5);
}



	}
}