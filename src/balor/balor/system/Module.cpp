#include "Module.hpp"

#include <utility>

#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {

using std::move;
using std::swap;
using namespace balor::io;


Module::Module() : _handle(nullptr), _owned(false) {
}


Module::Module(HMODULE handle, bool owned) : _handle(handle), _owned(owned) {
}


Module::Module(StringRange fileName) : _handle(nullptr), _owned(true) {
	assert("Empty fileName" && !fileName.empty());
	_handle = LoadLibraryW(fileName.c_str());
}


Module::Module(Module&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
}


Module::~Module() {
	if (_handle && _owned) {
		verify(FreeLibrary(_handle));
	}
	//_handle = nullptr;
}


Module& Module::operator=(Module&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Module Module::current() {
	return Module();
}


File Module::directory() const {
	return file().parent();
}


File Module::file() const {
	File file;
	verify(GetModuleFileNameW(_handle, const_cast<wchar_t*>(file.path()), File::maxPath));
	file.resetPathLength();
	return file;
}


Module Module::find(StringRange fileName) {
	Module module;
	module._handle = GetModuleHandleW(fileName.c_str());
	return module;
}


bool Module::owned() const {
	return _owned;
}


void Module::owned(bool value) {
	_owned = value;
}


void* Module::_getVoidFunction(ByteStringRange functionName) {
	assert("Null module _handle" && *this);

	return GetProcAddress(_handle, functionName.c_str());
}



	}
}