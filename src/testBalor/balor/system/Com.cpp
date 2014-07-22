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



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	test::UnitTest::ignoreHandleLeakCheck();
	Com::initialize();
	testAssert(comCheck()); // 初期化済み

	Com::oleInitialize();
	testAssert(oleCheck()); // 初期化済み
	Com::oleUninitialize();

	Com::uninitialize();
}


testCase(initializeAndInitializedAndUninitialize) {
	// STA で初期化
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 未初期化
	testNoThrow(Com::initialize());
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み

	// MTA への変更不可
	testAssertionFailed(Com::initialize(false));

	{// 他スレッドは未初期化状態
		thread work([&] () {
			// initialized 呼び出し不可
			testAssertionFailed(Com::initialized());
			testAssert(!comCheck()); // 未初期化

			// STA で初期化
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(!comCheck()); // 未初期化

			// MTA で初期化
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(!comCheck()); // 未初期化

		});
		work.join();
		testAssert(Com::initialized());
		testAssert(comCheck()); // 初期化済み
	}

	// 初期化の重複
	testNoThrow(Com::initialize());
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み

	// 段階的な終了処理
	Com::uninitialize();
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 未初期化


	// MTA で初期化
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 未初期化
	testNoThrow(Com::initialize(false));
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み

	// STA への変更不可
	testAssertionFailed(Com::initialize(true));

	{// 他スレッドは MTA 初期化状態
		thread work([&] () {
			// initialized 呼び出し不可
			testAssertionFailed(Com::initialized());
			testAssert(comCheck()); // 初期化済み

			// STA で初期化
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み

			// MTA で初期化
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み

		});
		work.join();
		testAssert(Com::initialized());
		testAssert(comCheck()); // 初期化済み
	}

	// 初期化の重複
	testNoThrow(Com::initialize(false));
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み

	// 段階的な終了処理
	Com::uninitialize();
	testAssert(Com::initialized());
	testAssert(comCheck()); // 初期化済み
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!comCheck()); // 未初期化


	//// OLE で初期済み
	//testNoThrow(Com::oleInitialize());
	//testAssert(Com::initialized());
	//testAssert(comCheck()); // 初期化済み

	//// 初期化の重複
	//testNoThrow(Com::initialize());
	//testAssert(Com::initialized());
	//testAssert(comCheck()); // 初期化済み

	//// 段階的な終了処理
	//Com::uninitialize();
	//testAssert(Com::initialized());
	//testAssert(comCheck()); // 初期化済み
	//Com::oleUninitialize();
	//testAssert(!Com::initialized());
	//testAssert(!comCheck()); // 未初期化



	{// 未初期化状態で他スレッド
		thread work([&] () {
			// initialized 呼び出し不可
			testAssertionFailed(Com::initialized());
			testAssert(!comCheck()); // 未初期化

			// STA で初期化
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize());
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(!comCheck()); // 未初期化

			// MTA で初期化
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			testNoThrow(Com::initialize(false));
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(comCheck()); // 初期化済み
			Com::uninitialize();
			testAssert(!comCheck()); // 未初期化

		});
		work.join();
		testAssert(!Com::initialized());
		testAssert(!comCheck()); // 未初期化
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
	// COM STA 初期化状態から
	testNoThrow(Com::initialize());
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 未初期化 ※一度でも OLE を初期化するとあとは COM が初期化されているかどうかだけで判断されるようだ
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 初期化済み

	// 段階的な終了処理
	Com::oleUninitialize();
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 未初期化
	testAssert(comCheck()); // COM 初期化済み
	Com::uninitialize();
	testAssert(!Com::initialized());
	testAssert(!oleCheck()); // 未初期化
	testAssert(!comCheck()); // COM 未初期化


	// COM MTA 初期化状態から
	testNoThrow(Com::initialize(false));
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 未初期化
	testAssertionFailed(Com::oleInitialize()); // MTA からは初期化できない
	testAssert(!Com::oleInitialized());
	//testAssert(!oleCheck()); // 未初期化
	Com::uninitialize();
	testAssert(!oleCheck()); // 未初期化


	// COM 未初期化状態から
	testAssert(!Com::oleInitialized());
	testAssert(!oleCheck()); // 未初期化
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 初期化済み

	// MTA への変更不可
	testAssertionFailed(Com::initialize(false));

	{// 他スレッドは未初期化状態
		thread work([&] () {
			// initialized 呼び出し不可
			testAssertionFailed(Com::oleInitialized());
			testAssert(!oleCheck()); // 未初期化

			// OLE 初期化
			testNoThrow(Com::oleInitialize());
			testAssert(oleCheck()); // 初期化済み
			testNoThrow(Com::oleInitialize());
			testAssert(oleCheck()); // 初期化済み
			Com::oleUninitialize();
			testAssert(oleCheck()); // 初期化済み
			Com::oleUninitialize();
			testAssert(!oleCheck()); // 未初期化
		});
		work.join();
		testAssert(Com::oleInitialized());
		testAssert(oleCheck()); // 初期化済み
	}

	// 初期化の重複
	testNoThrow(Com::oleInitialize());
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 初期化済み

	// 段階的な終了処理
	Com::oleUninitialize();
	testAssert(Com::oleInitialized());
	testAssert(oleCheck()); // 初期化済み
	Com::oleUninitialize();
	testAssert(!Com::oleInitialized());
	testAssert(!oleCheck()); // 未初期化
}



		}
	}
}