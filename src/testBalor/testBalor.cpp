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
	throw UnhandledException(); // 표준 C++ 예외를 발생시킨다
}


int debugReportHook(int /*reportType*/, char* message, int* /*returnValue*/) {
	// 메모리 릭이 보고되어서 여기에 올 때에는 전역 변수는 이미 소멸 되어 있을 가능성이 높으므로 전역 변수는 사용하지 않는다
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
	_set_se_translator(&se_translator_function); // 구조화 예외를 C++ 예외로 변환하여 소멸자를 실행한다
	_CrtSetReportHook(debugReportHook); // enableMemoryLeakCheck의 결과를 줍도록 리포트 함수를 훅한다

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

	gui::MsgBox::show(L"테스트 종료");

	return 0;
}
