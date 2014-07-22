#include "StdAfx.h"
#include "Sample.hpp"

#include <balor/test/Debug.hpp>
#include <balor/test/UnhandledException.hpp>

#include <tchar.h>
#include <windows.h>


int WINAPI WinMain(HINSTANCE //instance
					  ,HINSTANCE //prevInstance
					  ,LPSTR     //commandLine
					  ,int       //showState
					  ) {
	balor::test::Debug::enableMemoryLeakCheck();
	balor::test::Debug::enableCrashDumpHandler();

	//_crtBreakAlloc = 381;

	try {
		sample::Sample sample;
		sample.main();
	} catch (balor::test::UnhandledException& ) {
		//throw; // 소멸자가 실행되는 것을 위해서 캐치
	}

	return 0;
}
