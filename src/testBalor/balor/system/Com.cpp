#include <balor/system/Com.hpp>

#include <boost/thread.hpp>
#include <MLang.h>

#include <balor/gui/Frame.hpp>
#include <balor/system/ComBase.hpp>
#include <balor/system/ComPtr.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace system {
		namespace testCom {

using boost::thread;
using namespace balor::gui;


namespace {
bool comCheck() {
	IMultiLanguage2* ptr;
	auto result = CoCreateInstance(CLSID_CMultiLanguage, nullptr, CLSCTX_ALL, __uuidof(IMultiLanguage2), reinterpret_cast<void**>(&ptr));
	if (result == CO_E_NOTINITIALIZED) {
		return false;
	}
	assert(SUCCEEDED(result));
	verify(SUCCEEDED(ptr->Release()));
	return true;
}

struct DropTarget : public IDropTarget {
	DropTarget() : count(1) {}
	virtual ~DropTarget() {}
	virtual ULONG STDMETHODCALLTYPE AddRef() { return ++count; }
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void** ptr) {
		if (IsEqualIID(id, IID_IUnknown) || IsEqualIID(id, __uuidof(IDropTarget))) {
			*ptr = this;
			AddRef();
			return S_OK;
		} else {
			*ptr = NULL;
			return E_NOINTERFACE;
		}
	}
	virtual ULONG STDMETHODCALLTYPE Release() { return --count; }
	virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* , DWORD , POINTL , DWORD* ) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD , POINTL , DWORD* ) { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DragLeave() { return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* , DWORD , POINTL , DWORD* ) { return S_OK; }
	long count;
};

bool oleCheck() {
	DropTarget target;
	Frame frame(L"");
	auto result = RegisterDragDrop(frame, &target);
	if (result == CO_E_NOTINITIALIZED || result == E_OUTOFMEMORY) {
		return false;
	}
	assert(SUCCEEDED(result));
	verify(SUCCEEDED(RevokeDragDrop(frame)));
	return true;
}
} // namespace



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	test::UnitTest::ignoreHandleLeakCheck();
	Com::initialize();
	testAssert(comCheck()); // 초기화 끝남

	Com::oleInitialize();
	testAssert(oleCheck()); // 초기화 끝남
	Com::oleUninitialize();

	Com::uninitialize();
}


testCase(initializeAndInitializedAndUninitialize) {
	// STA 로 초기화 
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 미 초기화
	testNoThrow(Com::initialize());
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남

	// MTA 로 변경 가능
	testAssertionFailed(Com::initialize(false));

	{// 다른 스레드는 미 초기화 상태 
		thread work([&] () {
			// initialized 호출 불가
			testAssertionFailed(Com::initialized());
			testAssert(!comCheck()); // 미 초기화

			// STA 로 초기화 
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(!comCheck()); // 미 초기화

			// MTA 로 초기화
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(!comCheck()); // 미 초기화

		});
		work.join();
		testAssert(Com::initialized());
		testAssert(comCheck()); // 초기화 끝남
	}

	// 초기화 중복
	testNoThrow(Com::initialize());
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남

	// 단계적 종료 처리
	Com::uninitialize();
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 미 초기화


	// MTA 로 초기화
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 미 초기화
	testNoThrow(Com::initialize(false));
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남

	// STA 로 변경 불가
	testAssertionFailed(Com::initialize(true));

	{// 다른 스레드는 MTA 초기화 상태
		thread work([&] () {
			// initialized 호출 불가 
			testAssertionFailed(Com::initialized());
			testAssert(comCheck()); // 초기화 끝남

			// STA 로 초기화
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남

			// MTA 로 초기화
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남

		});
		work.join();
		testAssert(Com::initialized());
		testAssert(comCheck()); // 초기화 끝남
	}

	// 초기화 중복
	testNoThrow(Com::initialize(false));
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남

	// 단계적 종료 처리 
	Com::uninitialize();
	testAssert(Com::initialized());
	testAssert(comCheck()); // 초기화 끝남
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 미 초기화


	


	{// 미 초기화 상태로 다른 스레드
		thread work([&] () {
			// initialized 호출 불가
			testAssertionFailed(Com::initialized());
			testAssert(!comCheck()); // 미 초기화

			// STA 로 초기화
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(!comCheck()); // 미 초기화

			// MTA 로 초기화
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(comCheck()); // 초기화 끝남
			Com::uninitialize();
			testAssert(!comCheck()); // 미 초기화

		});
		work.join();
		testAssert(!Com::initialized());
		testAssert(!comCheck()); // 미 초기화
	}
}


testCase(isMainThread) {
	testAssert(Com::isMainThread());
	thread work([&] () {
		testAssert(!Com::isMainThread());
	});
	work.join();
}


testCase(oleInitializeAndOleInitializedAndOleUninitialize) {
	// COM STA 초기화 상태에서
	testNoThrow(Com::initialize());
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 미 초기화. 한번이라도 OLE 을 초기화 한 후는 COM 이 초기화 되었는지 어떤지만을 판된 하도록
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 초기화 끝남

	// 단계적 종료 처리
	Com::oleUninitialize();
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 미 초기화
	testAssert(comCheck()); // COM 초기화 끝남
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!oleCheck()); // 미 초기화
	testAssert(!comCheck()); // COM 미 초기화


	// COM MTA 초기화 상태에서
	testNoThrow(Com::initialize(false));
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 미 초기화
	testAssertionFailed(Com::oleInitialize()); // MTA 부터는 초기화 할 수 없음
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 미 초기화
	Com::uninitialize();
	testAssert(!oleCheck()); // 미 초기화


	// COM 미 초기화 상태에서
	testAssert(!Com::oleInitialized());
	testAssert(!oleCheck()); // 미 초기화
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 초기화 끝남

	// MTA 로 변경 불가
	testAssertionFailed(Com::initialize(false));

	{// 다른 스레드 미 초기화 상태 
		thread work([&] () {
			// initialized 호출 불가
			testAssertionFailed(Com::oleInitialized());
			testAssert(!oleCheck()); // 미 초기화

			// OLE 초기화
			testNoThrow(Com::oleInitialize());
			testAssert(oleCheck()); // 초기화 끝남
			testNoThrow(Com::oleInitialize());
			testAssert(oleCheck()); // 초기화 끝남
			Com::oleUninitialize();
			testAssert(oleCheck()); // 초기화 끝남
			Com::oleUninitialize();
			testAssert(!oleCheck()); // 미 초기화
		});
		work.join();
		testAssert(Com::oleInitialized());
		testAssert(oleCheck()); // 초기화 끝남
	}

	// 초기화 중복 
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 초기화 끝남

	// 단계적 종료 처리 
	Com::oleUninitialize();
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 초기화 끝남
	Com::oleUninitialize();
	testAssert(!Com::oleInitialized());
	testAssert(!oleCheck()); // 미 초기화
}



		}
	}
}