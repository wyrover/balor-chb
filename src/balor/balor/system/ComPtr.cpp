#include "ComPtr.hpp"

#include <cassert>
#include <ObjBase.h>

#include <balor/system/Com.hpp>


namespace balor {
	namespace system {
		namespace detail {



void coCreateInstance(REFCLSID rclsid, IUnknown* unknown, REFIID riid, void** pointer) {
	coCreateInstance(rclsid, unknown, riid, pointer, CLSCTX_ALL);
}


void coCreateInstance(REFCLSID rclsid, IUnknown* unknown, REFIID riid, void** pointer, DWORD context) {
	HRESULT result = CoCreateInstance(rclsid, unknown, context, riid, pointer);
	if (result == CO_E_NOTINITIALIZED) {
		if (Com::isMainThread()) {
			Com::initialize();
			result = CoCreateInstance(rclsid, unknown, context, riid, pointer);
		} else {
			throw Com::UninitializedException();
		}
	}
	if (result == REGDB_E_CLASSNOTREG || result == E_NOINTERFACE) {
		throw Com::InterfaceNotFoundException();
	}
	assert(SUCCEEDED(result));
}


#pragma warning(push)
#pragma warning(disable : 4189) // 'result' : ローカル変数が初期化されましたが、参照されていません
void queryInterface(IUnknown* from, const _GUID& riid, void** pointer) {
	HRESULT result = from->QueryInterface(riid, pointer);
	if (result == E_NOINTERFACE) {
		throw Com::InterfaceNotFoundException();
	}
	assert(SUCCEEDED(result));
}
#pragma warning(pop)



		}
	}
}
