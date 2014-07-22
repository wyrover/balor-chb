#include "Com.hpp"

#include <ObjBase.h>

#include <balor/test/verify.hpp>
#include <balor/Singleton.hpp>


namespace balor {
	namespace system {


namespace {
class Global {
	friend Singleton<Global>;

	Global() {
		initializedCount = 0;
		oleInitializedCount = 0;
		nonMainInitializedCount = 0;
		nonMainOleInitializedCount = 0;
		mainThreadId = GetCurrentThreadId();
	}
	~Global() {
		if (oleInitializedCount) {
			Com::oleUninitialize();
		}
		if (initializedCount) {
			Com::uninitialize();
		}
		assert("COM unbalance initialize/uninitialize" && !initializedCount);
		assert("OLE unbalance oleInitialize/oleUninitialize" && !oleInitializedCount);
		assert("COM unbalance initialize/uninitialize in non main thread" && !nonMainInitializedCount);
		assert("OLE unbalance oleInitialize/oleUninitialize in non main thread" && !nonMainOleInitializedCount);
	}

public:
	bool isMainThread() const {
		return mainThreadId == GetCurrentThreadId();
	}

public:
	long initializedCount;
	long oleInitializedCount;
	long nonMainInitializedCount;
	long nonMainOleInitializedCount;

private:
	DWORD mainThreadId;

};


Global& initializeGlobal = Singleton<Global>::get(); // メインスレッドで Global のコンストラクタを呼んでおく。
} // namespace



#pragma warning (push)
#pragma warning (disable : 4189) //  'result' : ローカル変数が初期化されましたが、参照されていません
void Com::initialize(bool isSTA) {
	auto result = CoInitializeEx(nullptr, isSTA ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED);
	assert("Can't change STA to MTA or MTA to STA" && result != RPC_E_CHANGED_MODE);
	assert("Failed to CoInitializeEx" && SUCCEEDED(result));
	Global& global = Singleton<Global>::get();
	if (global.isMainThread()) {
		InterlockedIncrement(&global.initializedCount);
	} else {
		InterlockedIncrement(&global.nonMainInitializedCount);
	}
}
#pragma warning (pop)


bool Com::initialized() {
	assert("Can't call initialized from non main thread" && isMainThread());
	Global& global = Singleton<Global>::get();
	return global.initializedCount || global.oleInitializedCount;
}


bool Com::isMainThread() {
	return Singleton<Global>::get().isMainThread();
}


#pragma warning (push)
#pragma warning (disable : 4189) //  'result' : ローカル変数が初期化されましたが、参照されていません
void Com::oleInitialize() {
	auto result = OleInitialize(nullptr);
	assert("Already MTA initialized. so can't initiazlize OLE." && result != RPC_E_CHANGED_MODE);
	assert("Failed to OleInitialize" && SUCCEEDED(result));
	Global& global = Singleton<Global>::get();
	if (global.isMainThread()) {
		InterlockedIncrement(&global.oleInitializedCount);
	} else {
		InterlockedIncrement(&global.nonMainOleInitializedCount);
	}
}
#pragma warning (pop)


bool Com::oleInitialized() {
	assert("Can't call oleInitialized from non main thread" && isMainThread());
	return 0 < Singleton<Global>::get().oleInitializedCount;
}


void Com::oleUninitialize() {
	OleUninitialize();
	Global& global = Singleton<Global>::get();
	if (global.isMainThread()) {
		InterlockedDecrement(&global.oleInitializedCount);
	} else {
		InterlockedDecrement(&global.nonMainOleInitializedCount);
	}
}


void Com::uninitialize() {
	CoUninitialize();
	Global& global = Singleton<Global>::get();
	if (global.isMainThread()) {
		InterlockedDecrement(&global.initializedCount);
	} else {
		InterlockedDecrement(&global.nonMainInitializedCount);
	}
}



	}
}