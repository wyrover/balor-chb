#pragma once

#include <cassert>
#include <ObjBase.h>


namespace balor {
	namespace system {



/// IUnknown の COM インターフェース実装を手助けする基底クラス。
template<typename T>
class ComBase : public T {
protected:
	ComBase() : _count(1) {
	}
	virtual ~ComBase() {
		assert(!_count);
	}

private: // 複製の禁止。
	ComBase(const ComBase& );
	ComBase& operator=(const ComBase& );

public:
	virtual ULONG STDMETHODCALLTYPE AddRef() {
		return ::InterlockedIncrement(&_count);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void** object) {
		if (IsEqualIID(id, IID_IUnknown) || IsEqualIID(id, __uuidof(T))) {
			*object = this;
			AddRef();
			return S_OK;
		} else {
			*object = NULL;
			return E_NOINTERFACE;
		}
	}

	virtual ULONG STDMETHODCALLTYPE Release() {
		auto newCount = ::InterlockedDecrement(&_count);
		if (!newCount) {
			delete this;
		}
		return newCount;
	}

protected:
	LONG _count;
};



	}
}