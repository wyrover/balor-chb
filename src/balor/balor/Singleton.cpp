#include "Singleton.hpp"

#include <typeinfo>
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_THREAD_NO_LIB
#include <boost/thread/recursive_mutex.hpp>

#include <balor/system/Module.hpp>
#include <balor/test/verify.hpp>
#include <../balor_singleton/balor/singleton/getSingletonInstance.hpp>


namespace balor {
	namespace detail {


using boost::recursive_mutex;
using namespace balor::system;


namespace {
class BalorSingletonModule {
public:
	BalorSingletonModule() : module(L"balor_singleton.dll") {
		if (module) {
			function = module.getFunction<decltype(balor::singleton::getSingletonInstance)>("getSingletonInstance");
			assert(function);
		}
	}

	bool empty() const { return !module; }
	void* getSingletonInstance(const std::type_info& info, void* (*createInstanceFunction)()) {
		return (*function)(info, createInstanceFunction);
	}

	Module module;
	decltype(balor::singleton::getSingletonInstance)* function;
};


recursive_mutex& getSingletonMutex() { // mutex 를 전역 변수 초기화에서도 사용할 수 있도록 함수에서 얻도록 한다.
	static recursive_mutex singletonMutex; // 이 인스턴스는 DLL 마다 만들어진다. 초기화 처리 중에서 다른 형의 Singleton 을 만들 수 있도록 recursive_mutex로 핸둔다
	return singletonMutex;
}


recursive_mutex& singletonMutex = getSingletonMutex(); // 이 전역 변수로 멀티스레드에서 접근했을 때 mutex 초기화가 보증된다.
} // namespace



void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)()) {
	recursive_mutex::scoped_lock lock(getSingletonMutex()); // mutex のインスタンスは DLL ごとに作成されるので唯一ではなく DLL ごとにロックされてしまう。
	static BalorSingletonModule module; // このインスタンスは DLL ごとに作成されるので DLL ごとのロックで問題ない。LoadLibrary は単に DLL ごとにカウンタを増やすだけ。
	if (module.empty()) {
		return (*createInstanceFunction)(); // balor_singletone.dll が無い場合は DLL を使わないとみなす。よってロックは唯一であり、この関数を超えてメモリ最適化は起きないので呼び出し元でメモリバリアは必要ない。
	} else {
		return module.getSingletonInstance(info, createInstanceFunction); // この関数処理は呼び出し先の DLL 内であらためてロックされる。
	}
}



	}
}