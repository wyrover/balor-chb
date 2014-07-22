#include <balor/Singleton.hpp>

#include <balor/testSingleton.hpp> // testBalorDll

#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace testSingleton {


using namespace balor::io;
using namespace balor::system;


testCase(get) {
	// singleton dllの存在確認
	testAssert(File::exists(String::refer(Module::current().directory()) + L"\\balor_singleton.dll"));

	// ※残念ながら balor_singleton.dll 無しのケースはテストできていない。複数のプロセスが必要だろう。目視では確認済み。

	
	int& exeInt = Singleton<int>::get();
	exeInt = 5;
	short& exeShort = Singleton<short>::get();
	exeShort = 6;
	testAssert(Singleton<int>::get() == 5);
	testAssert(Singleton<short>::get() == 6);

	Module module(L"testBalorDll.dll");
	auto getDllInt = module.getFunction<decltype(getSingletonInt)>("getSingletonInt");
	auto getDllShort = module.getFunction<decltype(getSingletonShort)>("getSingletonShort");

	int& dllInt = (*getDllInt)();
	short& dllShort = (*getDllShort)();
	testAssert(&exeInt == &dllInt);
	testAssert(&exeShort == &dllShort);
	testAssert(dllInt == 5);
	testAssert(dllShort == 6);
}



	}
}