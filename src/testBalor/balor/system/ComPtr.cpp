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
	balor::test::UnitTest::ignoreHandleLeakCheck(); // CLSID_CMLangConvertCharset 라는 새로운 CLSID 에 의해서 핸들 할당이 증가하고 있다
	{
		ComPtr<IMultiLanguage> ptr;
		testAssert(!ptr);
	}

	{// COM 을 미 초기화
		testAssert(!Com::initialized());
		testNoThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage));
		testAssert(Com::initialized());
	}

	{// 비 메인 스레드로 COM 을 미 초기화
		thread work([&] () {
			testThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage), Com::UninitializedException);
		});
		work.join();
	}

	{// 클래스를 등록하지 않았다
		testThrow(ComPtr<IMultiLanguage2> ptr(CLSID_CMLangConvertCharset), Com::InterfaceNotFoundException);
	}
	{// 정상 케이스 
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

	// 찾지 못함
	ComPtr<IMLangConvertCharset> charset;
	testThrow(source.queryInterface(charset), Com::InterfaceNotFoundException);

	// 정상 케이스 
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
	{// operator-> 이미 사용을 보여었다
	}
}


testCase(cleanup) {
	Com::uninitialize();
}



		}
	}
}