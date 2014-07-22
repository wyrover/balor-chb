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
		//throw; // 긢긚긣깋긏?딳벍궻댴궸긌긿긞?궬궚궥귡
	}

	return 0;
}
