#include <balor/system/System.hpp>

#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/Version.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {
		namespace testSystem {



testCase(startup) { // 
	balor::test::UnitTest::ignoreHandleLeakCheck();

	System::userName(); // GetUserNameW 함수는 핸들을 처음 호출에서 학보하고 내부에서 계속 유지한다 
}


testCase(commandLineArgCount) {
	testAssert(System::commandLineArgCount() == 1);
}


testCase(computerName) {
	testAssert(0 < System::computerName().length());

	// Caution: 각 머신에서 눈으로 확인 
}


testCase(getCommandLineArg) {
	testAssert(System::getCommandLineArg(0) == Module::current().file());
}


testCase(is64BitOs) {
	testNoThrow(System::is64BitOs());

	// Caution: 각 OS에서 눈으로 확인 
}


testCase(is64BitProcess) {
	testNoThrow(System::is64BitProcess());

	// Caution: 각 OS에서 눈으로 확인
}


testCase(osKind) {
	testAssert(System::osKind() != System::OsKind::unknown);

	// Caution: 각 OS에서 눈으로 확인
}


testCase(osServicePack) {
	testAssert(0 < System::osServicePack().length());

	// Caution: 각 OS에서 눈으로 확인
}


testCase(osVersion) {
	testAssert(Version(5, 1) <= System::osVersion()); // XP 이후

	// Caution: 각 OS에서 눈으로 확인
}


testCase(processorCount) {
	testAssert(0 < System::processorCount());

	// Caution: 각 OS에서 눈으로 확인
}


testCase(userName) {
	testAssert(0 < System::userName().length());

	// Caution: 각 OS에서 눈으로 확인
}



		}
	}
}