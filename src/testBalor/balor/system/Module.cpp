#include <balor/system/Module.hpp>

#include <utility>
#include <balor/system/testModule.hpp> // testBalorDll

#include <balor/io/File.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {
		namespace testModule {


using std::move;


testCase(construct) {
	// 무효한 파라미터
	testAssertionFailed(Module(L""));

	{// 기본 생성자
		Module module;
		testAssert(!module);
	}

	{// 찾지 못한 파일 이름 
		Module module(L"balor_utility_testModule_390afjldagad09udfalohnfad.dll");
		testAssert(!module);
	}

	{// DLL 로드와 로드한 DLL로의 접근 테스트 
		Module module(L"testBalorDll.dll");
		testAssert(module);

		{// 함수 취득과 테스트 
			auto function0 = module.getFunction<decltype(return100)>("return100");
			testAssert(function0 != nullptr);
			testAssert((*function0)() == 100);
			auto function1 = module.getFunction<decltype(return255)>("return255");
			testAssert(function1 != nullptr);
			testAssert((*function1)() == 255);
			auto function2 = module.getFunction<decltype(return255)>("return256");
			testAssert(function2 == nullptr);
		}
		{// find
			Module finded = Module::find(L"testBalorDll.dll");
			testAssert(finded);
			testAssert(finded.file() == module.file());
		}
		{// find 풀 패스로 
			Module finded = Module::find(module.file());
			testAssert(finded);
			testAssert(finded.file() == module.file());
		}
		{// move
			Module moved = move(module);
			testAssert(moved);
			testAssert(!module);

			module = move(moved);
			testAssert(!moved);
			testAssert(module);
		}
	}

	{// 언로드된 DLL 검색
		Module module = Module::find(L"testBalorDll.dll");
		testAssert(!module);
	}
}


//testCase(find) { // testCase(construct)
//}


testCase(getFunction) {
	// 무효한 파라미터
	testAssertionFailed(Module().getFunction<void*>("function"));

	
}


testCase(path) {
	
}



		}
	}
}