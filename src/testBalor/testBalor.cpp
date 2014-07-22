#include <functional>

#include <balor/gui/MessageBox.hpp>
#include <balor/io/File.hpp>
#include <balor/io/FileStream.hpp>
#include <balor/system/Console.hpp>
#include <balor/system/Module.hpp>
#include <balor/test/Debug.hpp>
#include <balor/test/UnhandledException.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>

#include <tchar.h>
#include <windows.h>


using namespace balor::io;
using namespace balor::system;
using namespace balor::test;
using namespace balor;


void se_translator_function(unsigned int /*code*/, struct _EXCEPTION_POINTERS* ep)
{
	Debug::createDumpFile(ep);
	throw UnhandledException(); //標準C++の例外を発生させる
}


int debugReportHook(int /*reportType*/, char* message, int* /*returnValue*/) {
	// メモリリークが報告されてここに来るときにはグローバル変数は既にデストラクトされている可能性が高いのでグローバル変数は使えない。
	File(Module::current().directory(), L"testBalorLog.txt").openAppend().write(message, 0, String::getLength(message));
	return FALSE;
}


int APIENTRY _tWinMain(HINSTANCE //instance
					  ,HINSTANCE //prevInstance
					  ,LPTSTR    //commandLine
					  ,int       //showState
					  ) {
	Debug::enableMemoryLeakCheck();
	Debug::enableCrashDumpHandler(L"", false);
	_set_se_translator(&se_translator_function); // 構造化例外を C++ 例外に変換してデストラクタを起動する。
	_CrtSetReportHook(debugReportHook); // enableMemoryLeakCheck の結果を拾えるようにレポート関数をフックする。

	Console::open();

	try {
		auto stream = File(Module::current().directory(), L"testBalorLog.txt").create();
		Debug::onWrite() = [&] (ByteStringRange message) {
			stream.write(message.c_str(), 0, message.length());
			Console::write(message);
		};

		UnitTest::run();
		//UnitTest::run("listener");
	} catch (UnhandledException& ) {
	}

	gui::MsgBox::show(L"テスト終了");

	return 0;
}
