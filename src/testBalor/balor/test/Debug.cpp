#include <balor/test/Debug.hpp>

#include <utility>

#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/UnhandledException.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace test {
		namespace testDebug {


using std::move;
using namespace balor::io;


namespace {
const wchar_t testDirectoryName[] = L"testBalor_system_diagnostics_Debug_ekhjiu879u0adfjalj0gjdftdsa";
File getTestDirectory() {
	File dir(File::getSpecial(File::Special::temporary), testDirectoryName);
	if (dir.exists()) {
		dir.remove(true);
	}
	dir.createDirectory();
	return dir;
}


void removeTestDirectory() {
	File(File::getSpecial(File::Special::temporary), testDirectoryName).remove(true);
}


bool raiseExceptionAndCreateDumpFile(const wchar_t* fileName) {
	bool result = false;
	__try {
		RaiseException(0, 0, 0, nullptr);
	} __except(result = Debug::createDumpFile(GetExceptionInformation(), fileName), EXCEPTION_EXECUTE_HANDLER) {
	}
	return result;
}
} // namespace



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	balor::test::UnitTest::ignoreHandleLeakCheck();

	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	Debug::createDumpFile(nullptr, File(dir, L"file.dmp")); // MiniDumpWriteDump 함수는 처음 호출에서 핸들을 복수 열고 유지한다
}


//testCase(crashDumpFilePath) { // testCase(enableCrashDumpHandler) 에서 테스트
//}


testCase(createDumpFile) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	{// 디렉토리를 찾지 못해서 실패
		File file0(dir, L"not_exist_dir\\file0.dmp");
		testAssert(!Debug::createDumpFile(nullptr, file0.path()));
		testAssert(!file0.exists());
	}
	{// 예외 지정 없음으로 작성
		File file1(dir, L"file1.dmp");
		testAssert(Debug::createDumpFile(nullptr, file1.path()));
		testAssert(file1.exists());
		testAssert(0 < file1.openRead().length());
	}
	{// 예외 지정 있음으로 작성
		File file2(dir, L"file2.dmp");
		testAssert(raiseExceptionAndCreateDumpFile(file2.path()));
		testAssert(file2.exists());
		testAssert(0 < file2.openRead().length());
	}

	// Caution: 덤프 파일이 VC 에서 정상으로 열리는 것을 확인 
}


testCase(enableCrashDumpHandler) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	File file0(dir, L"not_exist_dir\\file0.dmp");
	Debug::enableCrashDumpHandler(file0.path(), false, L"message", L"failedMessage");

	if (IsDebuggerPresent() == 0) { // 디버거를 사용하는한 핸들러는 실행되지 않는다
		{// 디렉토리를 찾지 못해서 실패
			try {
				throw 0;
			} catch (UnhandledException& ) {
			}
			testAssert(!file0.exists());
		}
		{// 크래쉬 덤프 출력(c++ 예외)
			File file1(dir, L"file1.dmp");
			Debug::enableCrashDumpHandler(file1.path(), false, L"message", L"failedMessage");
			bool destructed = false;
			try {
				scopeExit([&] () {
					destructed = true;
				});
				throw 0;
			} catch (UnhandledException& ) {
			}
			testAssert(file1.exists());
			testAssert(0 < file1.openRead().length());
			testAssert(destructed);
		}
		{// 크래쉬덤프 출력(구조화 예외)
			File file2(dir, L"file2.dmp");
			Debug::enableCrashDumpHandler(file2.path(), false, L"message", L"failedMessage");
			bool destructed = false;
			try {
				scopeExit([&] () {
					destructed = true;
				});
				RaiseException(0, 0, 0, nullptr);
			} catch (UnhandledException& ) {
			}
			testAssert(file2.exists());
			testAssert(0 < file2.openRead().length());
			testAssert(destructed);
		}
	} else { // 테스트 건수를 맞춘다
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
	}

	Debug::enableCrashDumpHandler();

	// Caution: 메시지 박스가 표시 되는 패턴으로 성공/실패 메시지를 확인
}


testCase(enableMemoryLeakCheck) {
	testNoThrow(Debug::enableMemoryLeakCheck());

	// Caution: 실제 릭이 보고되었지만 아닌 것을 직접 확인
}


testCase(stackTrace) {
	String stackTrace = Debug::stackTrace();
	testAssert(0 < stackTrace.length());
	testAssert(stackTrace.startsWith(L"testBalor!balor::test::testDebug::stackTrace"));
	testAssert(stackTrace.contains(L"testBalor!balor::test::UnitTest::run"));
}


testCase(write) {
	testNoThrow(Debug::write(""));
	testNoThrow(Debug::write("test Debug::write"));
	testNoThrow(Debug::write(L""));
	testNoThrow(Debug::write(L"test Debug::write"));

	// Caution: 실제 디버그가 출력되지만 아닌 것을 직접 확인 
}


testCase(writeLine) {
	testNoThrow(Debug::writeLine(""));
	testNoThrow(Debug::writeLine("test Debug::writeLine"));
	testNoThrow(Debug::writeLine(L""));
	testNoThrow(Debug::writeLine(L"test Debug::writeLine"));

	// Caution: 실제 디버그가 출력되지만 아닌 것을 직접 확인
}



		}
	}
}