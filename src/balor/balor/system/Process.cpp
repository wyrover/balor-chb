#include "Process.hpp"

#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>


namespace balor {
	namespace system {


//using std::move;
//using namespace balor::utility;



Process::Process() : _handle(GetCurrentProcess()) {
}


Process::~Process() {
}


int Process::gdiHandleCount() const {
	return GetGuiResources(_handle, GR_GDIOBJECTS);
}


int Process::userHandleCount() const {
	return GetGuiResources(_handle, GR_USEROBJECTS);
}



	}
}