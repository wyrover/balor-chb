#include "GdiplusInitializer.hpp"

#include <utility>
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_THREAD_NO_LIB
#include <boost/thread/mutex.hpp>

#include <balor/test/verify.hpp>
#define ENABLE_WIN32_LEAN_AND_MEAN
#define ENABLE_MINMAX // gdiplus.hが中でmin, maxマクロを使っている！
#include <balor/system/windows.hpp>
#include <balor/Singleton.hpp>

#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


namespace balor {
	namespace graphics {


using boost::mutex;


namespace {
class Global {
	friend Singleton<Global>;

	Global() {}
	~Global() {
		if (initialized) {
			GdiplusInitializer::uninitialize();
		}
	}

public:
	mutex initializeMutex;
	bool initialized;
	Gdiplus::GdiplusStartupInput startupInput;
	ULONG_PTR token;
};
} // namespace



void GdiplusInitializer::initialize() {
	Global& global = Singleton<Global>::get();
	mutex::scoped_lock lock(global.initializeMutex);
	if (!global.initialized) {
		verify(Gdiplus::GdiplusStartup(&global.token, &global.startupInput, nullptr) == Gdiplus::Ok);
		global.initialized = true;
	}
}


bool GdiplusInitializer::initialized() {
	return Singleton<Global>::get().initialized; // 例えこの関数を同期したとしても関数から帰った後にいつ変化するとも限らない
}


void GdiplusInitializer::uninitialize() {
	Global& global = Singleton<Global>::get();
	mutex::scoped_lock lock(global.initializeMutex);
	if (global.initialized) {
		Gdiplus::GdiplusShutdown(global.token);
		global.initialized = false;
	}
}



	}
}