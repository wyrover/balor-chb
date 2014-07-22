#include "Resource.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Cursor.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace io {

using std::move;
using std::wstring;
using namespace balor::graphics;
using namespace balor::locale;

namespace {
StringRange idToName(int id) {
	assert("Invalid resource id" && 0 <= id && id <= USHRT_MAX);
	return MAKEINTRESOURCEW(id);
}


bool isId(const StringRange& name) {
	return (reinterpret_cast<DWORD>(name.c_str()) & 0xffff0000) == 0;
}


struct getLocalName {
	getLocalName(StringRange name, const wchar_t* localeName) {
		StringBuffer buffer(localName);
		buffer += String::refer(name);
		buffer += L'.';
		buffer += localeName;
		length = buffer.length();
	}

	operator const wchar_t*() { return localName; }

	wchar_t localName[2048];
	int length;
};
} // namespace



Resource::Resource(HMODULE module) : _module(module) {
	if (!_module) {
		_module = GetModuleHandleW(nullptr);
		assert(_module);
	}
}


Resource::Resource(const Locale& locale, HMODULE module) : _locale(locale), _module(module) {
	if (!_module) {
		_module = GetModuleHandleW(nullptr);
		assert(_module);
	}
	StringBuffer buffer(_localeName);
	locale.nameToBuffer(buffer);
}


Bitmap Resource::getBitmap(int id) {
	return getBitmap(idToName(id));
}


Bitmap Resource::getBitmap(StringRange name) {
	if (locale() != Locale::invariant() && !isId(name)) {
		auto handle = static_cast<HBITMAP>(LoadImageW(_module, getLocalName(name, _localeName), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)); // まずは BITMAP リソースから探す
		if (handle) {
			return Bitmap(handle, true);
		}
	}
	auto handle = static_cast<HBITMAP>(LoadImageW(_module, name.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)); // まずは BITMAP リソースから探す
	if (!handle) {
		const DWORD errorCode = GetLastError();
		switch (errorCode) {
			case ERROR_NOT_ENOUGH_MEMORY       : throw OutOfMemoryException();
			case ERROR_RESOURCE_TYPE_NOT_FOUND :
			case ERROR_RESOURCE_NAME_NOT_FOUND : { // 無ければ RCDATA リソースから
				auto stream = getRawData(name);
				return Bitmap(stream);
			} break;
			default                            : assert("Failed to LoadImageW" && false); break;
		}
	}
	return Bitmap(handle, true);
}


Cursor Resource::getCursor(int id) {
	return getCursor(idToName(id));
}


Cursor Resource::getCursor(StringRange name) {
	if (locale() != Locale::invariant() && !isId(name)) {
		auto handle = static_cast<HCURSOR>(LoadImageW(_module, getLocalName(name, _localeName), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
		if (handle) {
			return Cursor(handle);
		}
	}
	auto handle = static_cast<HCURSOR>(LoadImageW(_module, name.c_str(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	if (!handle) {
		const DWORD errorCode = GetLastError();
		switch (errorCode) {
			case ERROR_NOT_ENOUGH_MEMORY       : throw OutOfMemoryException();
			case ERROR_RESOURCE_TYPE_NOT_FOUND :
			case ERROR_RESOURCE_NAME_NOT_FOUND : throw NotFoundException();
			default                            : assert(L"Failed to LoadImageW" && false); break;
		}
	}
	return Cursor(handle);
}


Icon Resource::getIcon(int id) {
	return getIcon(idToName(id));
}


Icon Resource::getIcon(int id, const Size& size) {
	return getIcon(idToName(id), size);
}


Icon Resource::getIcon(int id, int width, int height) {
	return getIcon(idToName(id), width, height);
}


Icon Resource::getIcon(StringRange name) {
	return getIcon(name, Icon::largeSize());
}


Icon Resource::getIcon(StringRange name, const Size& size) {
	if (locale() != Locale::invariant() && !isId(name)) {
		auto handle = static_cast<HICON>(LoadImageW(_module,  getLocalName(name, _localeName), IMAGE_ICON, size.width, size.height, LR_SHARED));
		if (handle) {
			return Icon(handle);
		}
	}
	auto handle = static_cast<HICON>(LoadImageW(_module, name.c_str(), IMAGE_ICON, size.width, size.height, LR_SHARED));
	if (!handle) {
		const DWORD errorCode = GetLastError();
		switch (errorCode) {
			case ERROR_NOT_ENOUGH_MEMORY       : throw OutOfMemoryException();
			case ERROR_RESOURCE_TYPE_NOT_FOUND :
			case ERROR_RESOURCE_NAME_NOT_FOUND : throw NotFoundException();
			default                            : assert(L"Failed to LoadImageW" && false); break;
		}
	}
	return Icon(handle);
}


Icon Resource::getIcon(StringRange name, int width, int height) {
	return getIcon(name, Size(width, height));
}


MemoryStream Resource::getRawData(int id) {
	return getRawData(idToName(id));
}


MemoryStream Resource::getRawData(StringRange name) {
	HRSRC resource = nullptr;
	if (locale() != Locale::invariant() && !isId(name)) {
		resource = FindResourceW(_module, getLocalName(name, _localeName), reinterpret_cast<LPWSTR>(RT_RCDATA));
	}
	if (!resource) {
		resource = FindResourceW(_module, name.c_str(), reinterpret_cast<LPWSTR>(RT_RCDATA));
	}
	if (!resource) {
		throw NotFoundException();
	}
	HGLOBAL const global = LoadResource(_module, resource);
	assert("Failed to LoadResource" && global);
	void* const buffer = LockResource(global);
	assert("Failed to LockResource" && buffer);
	const DWORD size = SizeofResource(_module, resource);
	assert(size <= INT_MAX);
	return MemoryStream(buffer, 0, size, false);
}


String Resource::getString(int id) {
	StringBuffer buffer;
	getStringToBuffer(buffer, id);
	return move(buffer);
}


String Resource::getString(StringRange name) {
	StringBuffer buffer;
	getStringToBuffer(buffer, name);
	return move(buffer);
}


void Resource::getStringToBuffer(StringBuffer& buffer, int id) {
	assert("Invalid resource id" && 0 <= id && id <= USHRT_MAX);
	wchar_t array[2048];
	int length = LoadStringW(_module, id, array, sizeof(array) / sizeof(array[0]));
	if (!length) {
		throw NotFoundException();
	}
	buffer += String::refer(array, length);
}


void Resource::getStringToBuffer(StringBuffer& buffer, StringRange name) {
	if (_namedStringTable.empty()) {
		auto memory = getRawData(String::literal(L"namedStringTable"));
		auto end = static_cast<wchar_t*>(memory.buffer()) + memory.length() / sizeof(wchar_t);
		const wchar_t* last = static_cast<wchar_t*>(memory.buffer());
		if (*last == 0xfeff) {
			++last;
		}
		const wchar_t* nameBegin = last;
		const wchar_t* valueBegin = last;
		String name;
		bool escaped = false;
		bool needReplace = false;
		const wchar_t* i = last;
		for (; i < end; ++i) {
			if (*i == L'"') {
				if (escaped) {
					++i;
					if (i == end) {
						break;
					}
					if (*i == L'"') {
						needReplace = true;
						continue;
					}
					escaped = false;
					if (*i == L',') {
						name = String(nameBegin + 1, i - nameBegin - 2);
						if (needReplace) {
							name = name.replace(String::literal(L"\"\""), String::literal(L"\""));
							needReplace = false;
						}
						valueBegin = i + 1;
					} else {
						String value = String(valueBegin + 1, i - valueBegin - 2);
						if (needReplace) {
							value = value.replace(String::literal(L"\"\""), String::literal(L"\""));
							needReplace = false;
						}
						_namedStringTable[move(name)] = move(value);
						nameBegin = i + 1;
						if (*i == L'\r' && i + 1 < end && *(i + 1) == L'\n') {
							++i;
							++nameBegin;
						}
					}
				} else {
					escaped = true;
				}
			} else if (!escaped) {
				if (*i == L',') {
					name = String(nameBegin, i - nameBegin);
					valueBegin = i + 1;
				} else if (*i == L'\r') {
					++i;
					if (i == end) {
						break;
					}
					if (*i == L'\n') {
						_namedStringTable[move(name)] = String(valueBegin, i - valueBegin - 1);
						nameBegin = i + 1;
					}
				} else if (*i == L'\n') {
					_namedStringTable[move(name)] = String(valueBegin, i - valueBegin);
					nameBegin = i + 1;
				}
			}
		}
	}
	if (locale() != Locale::invariant() && !isId(name)) {
		const auto localName = getLocalName(name, _localeName);
		auto i = _namedStringTable.find(String::refer(localName.localName, localName.length));
		if (i != _namedStringTable.end()) {
			buffer += i->second;
			return;
		}
	}
	auto i = _namedStringTable.find(String::refer(name));
	if (i == _namedStringTable.end()) {
		throw NotFoundException();
	}
	buffer += i->second;
}


Locale Resource::locale() const {
	return _locale;
}



	}
}