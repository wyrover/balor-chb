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
	// 無効なパラメータ
	testAssertionFailed(Module(L""));

	{// デフォルトコンストラクト
		Module module;
		testAssert(!module);
	}

	{// 見つからないファイル名
		Module module(L"balor_utility_testModule_390afjldagad09udfalohnfad.dll");
		testAssert(!module);
	}

	{// DLLのロードとロードしたDLLへのアクセステスト
		Module module(L"testBalorDll.dll");
		testAssert(module);

		{// 関数の取得と呼び出し
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
		{// find フルパスで
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

	{// アンロードされたDLLの検索
		Module module = Module::find(L"testBalorDll.dll");
		testAssert(!module);
	}
}


//testCase(find) { // testCase(construct)にてテスト済み
//}


testCase(getFunction) {
	// 無効なパラメータ
	testAssertionFailed(Module().getFunction<void*>("function"));

	// その他のケースはtestCase(construct)にてテスト済み
}


testCase(path) {
	// その他のケースはtestCase(construct)にてテスト済み
}



		}
	}
}