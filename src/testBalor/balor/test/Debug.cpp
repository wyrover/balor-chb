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



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	Debug::createDumpFile(nullptr, File(dir, L"file.dmp")); // MiniDumpWriteDump 関数は最初の呼び出しでだけハンドルを複数開いて保持する
}


//testCase(crashDumpFilePath) { // testCase(enableCrashDumpHandler) にてテスト
//}


testCase(createDumpFile) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	{// ディレクトリがみつからず失敗
		File file0(dir, L"not_exist_dir\\file0.dmp");
		testAssert(!Debug::createDumpFile(nullptr, file0.path()));
		testAssert(!file0.exists());
	}
	{// 例外指定無しで作成
		File file1(dir, L"file1.dmp");
		testAssert(Debug::createDumpFile(nullptr, file1.path()));
		testAssert(file1.exists());
		testAssert(0 < file1.openRead().length());
	}
	{// 例外指定ありで作成
		File file2(dir, L"file2.dmp");
		testAssert(raiseExceptionAndCreateDumpFile(file2.path()));
		testAssert(file2.exists());
		testAssert(0 < file2.openRead().length());
	}

	// Caution: ダンプファイルがVCで正常に開けることを目視で確認
}


testCase(enableCrashDumpHandler) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	File file0(dir, L"not_exist_dir\\file0.dmp");
	Debug::enableCrashDumpHandler(file0.path(), false, L"message", L"failedMessage");

	if (IsDebuggerPresent() == 0) { // デバッガを使う限りハンドラは起動されない
		{// ディレクトリがみつからず失敗
			try {
				throw 0;
			} catch (UnhandledException& ) {
			}
			testAssert(!file0.exists());
		}
		{// クラッシュダンプ出力（Ｃ＋＋例外）
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
		{// クラッシュダンプ出力（構造化例外）
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
	} else { // テスト件数は合わせる
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
		testAssert(true);
	}

	Debug::enableCrashDumpHandler();

	// Caution: メッセージボックスが表示されるパターンで成功／失敗メッセージを目視で確認
}


testCase(enableMemoryLeakCheck) {
	testNoThrow(Debug::enableMemoryLeakCheck());

	// Caution: 実際にリークが報告されるか否か目視で確認
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

	// Caution: 実際にデバッグ出力されるか否か目視で確認
}


testCase(writeLine) {
	testNoThrow(Debug::writeLine(""));
	testNoThrow(Debug::writeLine("test Debug::writeLine"));
	testNoThrow(Debug::writeLine(L""));
	testNoThrow(Debug::writeLine(L"test Debug::writeLine"));

	// Caution: 実際にデバッグ出力されるか否か目視で確認
}



		}
	}
}