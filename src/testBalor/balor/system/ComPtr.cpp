#include <balor/system/ComPtr.hpp>

#include <utility>
#include <boost/thread.hpp>
#include <MLang.h>

#include <balor/system/Com.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace system {
		namespace testComPtr {

using std::move;
using boost::thread;


testCase(construct) {
	balor::test::UnitTest::ignoreHandleLeakCheck(); // CLSID_CMLangConvertCharset という新たな CLSID によってハンドル割り当てが増加している
	{
		ComPtr<IMultiLanguage> ptr;
		testAssert(!ptr);
	}

	{// COM が未初期化
		testAssert(!Com::initialized());
		testNoThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage));
		testAssert(Com::initialized());
	}

	{// 非メインスレッドで COM が未初期化
		thread work([&] () {
			testThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage), Com::UninitializedException);
		});
		work.join();
	}

	{// クラスが登録されていない
		testThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMLangConvertCharset), Com::InterfaceNotFoundException);
	}
	{// 正常ケース
		ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage);
		testAssert(ptr != nullptr);
		MIMECPINFO info;
		testAssert(SUCCEEDED(ptr->GetCodePageInfo(932, GetUserDefaultUILanguage(), &info)));
		testAssert(info.uiCodePage == 932);
	}
}


testCase(copyConstruct) {
	ComPtr<IMultiLanguage2> source(CLSID_CMultiLanguage);
	ComPtr<IMultiLanguage2> ptr = source;
	testAssert(ptr != nullptr);
	testAssert(source == ptr);
}


testCase(rvalueConstruct) {
	ComPtr<IMultiLanguage2> source(CLSID_CMultiLanguage);
	ComPtr<IMultiLanguage2> ptr = move(source);
	testAssert(!source);
	testAssert(ptr != nullptr);
}


//testCase(destruct) { // ハンドル的ななにかでリークを検出できぬものか？
//	ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage);
//	testAssert(ptr != nullptr);
//	ptr.~ComPtr<IMultiLanguage2>();
//	testAssert(!ptr);
//}


testCase(assignment) {
	ComPtr<IMultiLanguage2> source(CLSID_CMultiLanguage);
	ComPtr<IMultiLanguage2> ptr;
	ptr = source;
	testAssert(ptr != nullptr);
	testAssert(ptr == source);

	ptr = ptr;
	testAssert(ptr != nullptr);
}


testCase(rvalueAssignment) {
	ComPtr<IMultiLanguage2> source(CLSID_CMultiLanguage);
	ComPtr<IMultiLanguage2> ptr;
	ptr = move(source);
	testAssert(!source);
	testAssert(ptr != nullptr);

	ptr = move(ptr);
	testAssert(ptr != nullptr);
}


testCase(queryInterface) {
	ComPtr<IMultiLanguage> source(CLSID_CMultiLanguage);

	// 見つからない
	ComPtr<IMLangConvertCharset> charset;
	testThrow(source.queryInterface(charset), Com::InterfaceNotFoundException);

	// 正常ケース
	ComPtr<IMultiLanguage2> ptr;
	source.queryInterface(ptr);
	testAssert(ptr != nullptr);
}


testCase(operators) {
	{// operator T*
		ComPtr<IMultiLanguage2> ptr;
		IMultiLanguage2* rawPtr = ptr;
		testAssert(rawPtr == nullptr);
	}
	{
		ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage);
		IMultiLanguage2* rawPtr = ptr;
		testAssert(rawPtr != nullptr);
	}
	{// oprator&
		ComPtr<IMultiLanguage> source(CLSID_CMultiLanguage);
		ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage);
		testAssertionFailed(&ptr);
		ptr = ComPtr<IMultiLanguage2>();
		testAssert(SUCCEEDED(source->QueryInterface(__uuidof(IMultiLanguage2), reinterpret_cast<void**>(&ptr))));
		testAssert(ptr != nullptr);
	}
	{// operator-> 既に使って見せた
	}
}


testCase(cleanup) {
	Com::uninitialize();
}



		}
	}
}