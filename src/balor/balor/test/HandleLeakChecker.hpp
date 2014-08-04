#pragma once

#include <balor/system/PerformanceCounter.hpp>
#include <balor/system/Process.hpp>
#include <balor/StringRange.hpp>


namespace balor {
	namespace test {



/// ファイルなどのシステムのハンドル、ＧＤＩのハンドル、ウインドウなどのユーザハンドルの３種類のハンドルの数を監視する。
class HandleLeakChecker {
public:
	HandleLeakChecker(StringRange exeFileName = L"");

public:
	int getGdiHandleChange();
	int getSystemHandleChange();
	int getUserHandleChange();

private:
	::balor::system::PerformanceCounter counter;
	::balor::system::Process process;
	int gdiHandleCount;
	int systemHandleCount;
	int userHandleCount;
};



	}
}