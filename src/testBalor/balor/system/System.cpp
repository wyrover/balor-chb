#include <balor/system/System.hpp>

#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/Version.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {
		namespace testSystem {



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	System::userName(); // GetUserNameW 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
}


testCase(commandLineArgCount) {
	testAssert(System::commandLineArgCount() == 1);
}


testCase(computerName) {
	testAssert(0 < System::computerName().length());

	// Caution: 各マシンで目視で確認すること
}


testCase(getCommandLineArg) {
	testAssert(System::getCommandLineArg(0) == Module::current().file());
}


testCase(is64BitOs) {
	testNoThrow(System::is64BitOs());

	// Caution: 各ＯＳで目視で確認すること
}


testCase(is64BitProcess) {
	testNoThrow(System::is64BitProcess());

	// Caution: 各ＯＳで目視で確認すること
}


testCase(osKind) {
	testAssert(System::osKind() != System::OsKind::unknown);

	// Caution: 各ＯＳで目視で確認すること
}


testCase(osServicePack) {
	testAssert(0 < System::osServicePack().length());

	// Caution: 各ＯＳで目視で確認すること
}


testCase(osVersion) {
	testAssert(Version(5, 1) <= System::osVersion()); // XP以降

	// Caution: 各ＯＳで目視で確認すること
}


testCase(processorCount) {
	testAssert(0 < System::processorCount());

	// Caution: 各マシンで目視で確認すること
}


testCase(userName) {
	testAssert(0 < System::userName().length());

	// Caution: 各マシンで目視で確認すること
}



		}
	}
}