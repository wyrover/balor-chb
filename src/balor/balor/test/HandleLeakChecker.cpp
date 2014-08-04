#include "HandleLeakChecker.hpp"

#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace test {


using namespace balor::io;
using namespace balor::system;



HandleLeakChecker::HandleLeakChecker(StringRange exeFileName) : counter(L"Process", L"Handle count"
	, exeFileName.empty() ? Module::current().file().nameWithoutExtension() : exeFileName) {
	gdiHandleCount    = process.gdiHandleCount();
	systemHandleCount = counter.nextValue<int>();
	userHandleCount   = process.userHandleCount();
}


int HandleLeakChecker::getGdiHandleChange() {
	int newGdiHandleCount     = process.gdiHandleCount();
	return newGdiHandleCount - gdiHandleCount;
}


int HandleLeakChecker::getSystemHandleChange() {
	int newSystemHandleCount     = counter.nextValue<int>();
	return newSystemHandleCount - systemHandleCount;
}


int HandleLeakChecker::getUserHandleChange() {
	int newUserHandleCount     = process.userHandleCount();
	return newUserHandleCount - userHandleCount;
}



	}
}