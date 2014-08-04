#pragma once

#include <balor/test/noMacroAssert.hpp>
#include <balor/Exception.hpp>

struct _GUID;
struct IUnknown;

namespace balor {
	namespace system {



namespace detail {
	void coCreateInstance(const ::_GUID& rclsid, ::IUnknown* unknown, const ::_GUID& riid, void** pointer);
	void coCreateInstance(const ::_GUID& rclsid, ::IUnknown* unknown, const ::_GUID& riid, void** pointer, unsigned long context);
	void queryInterface(::IUnknown* from, const ::_GUID& riid, void** pointer);
}


/**
 * COM インターフェースの作成、開放の支援をするスマートポインター。
 */
#pragma warning (push)
#pragma warning (disable : 4189) //  'result' : ローカル変数が初期化されましたが、参照されていません
template<typename T>
class ComPtr {
public:
	/// ヌルポインタで初期化。
	ComPtr() : _ptr(nullptr) {}
	ComPtr(const ComPtr& value) : _ptr(value._ptr) {
		_ptr->AddRef();
	}
	/// REFCLSID から COM インターフェースを作成。
	/// COM が未初期化でなおかつメインスレッドであれば Com::Initialize(true) で初期化を試みる。メインスレッドでなければ Com::UninitializedException を投げる。
	/// COM インターフェースが未登録または未実装の場合は Com::InterfaceNotFoundException を投げる。
	explicit ComPtr(const ::_GUID& rclsid, ::IUnknown* unknown = nullptr) : _ptr(nullptr) {
		::balor::system::detail::coCreateInstance(rclsid, unknown, __uuidof(T), reinterpret_cast<void**>(&_ptr));
	}
	ComPtr(const ::_GUID& rclsid, ::IUnknown* unknown, unsigned long context) : _ptr(nullptr) {
		::balor::system::detail::coCreateInstance(rclsid, unknown, __uuidof(T), reinterpret_cast<void**>(&_ptr), context);
	}

	ComPtr(ComPtr&& value) : _ptr(value._ptr) {
		value._ptr = nullptr;
	}
	~ComPtr() {
		if (_ptr) {
			_ptr->Release();
			//_ptr = nullptr;
		}
	}

	ComPtr& operator=(ComPtr&& value) {
		if (&_ptr != &value._ptr) {
			auto backup = _ptr;
			_ptr = value._ptr;
			value._ptr = backup;
		}
		return *this;
	}
	ComPtr& operator=(const ComPtr& value) {
		if (this != &value) {
			if (_ptr) {
				_ptr->Release();
			}
			_ptr = value._ptr;
			_ptr->AddRef();
		}
		return *this;
	}

public:
	/// newPtr の型に QueryInterface を行う。
	/// COM インターフェースが未登録または未実装の場合は Com::InterfaceNotFoundException を投げる。
	template<typename T2>
	void queryInterface(ComPtr<T2>& newPtr) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(newPtr == nullptr);
#endif
		::balor::system::detail::queryInterface(*this, __uuidof(T2), reinterpret_cast<void**>(&newPtr));
	}

public:
	/// COM インターフェースポインタの取得。
	operator T*() const { return _ptr; }
	/// COM インターフェースポインタへのアドレスの取得。CoCreateInstance での使用を想定しているのでポインタは nullptr でなければならない。
	T** operator&() {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(_ptr == nullptr);
#endif
		return &_ptr;
	}
	/// COM インターフェースメンバへのアクセス。
	T* operator->() const { return _ptr; }

private:
	T* _ptr;
};
#pragma warning (pop)



	}
}