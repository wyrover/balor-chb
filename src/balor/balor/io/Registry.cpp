#include "Registry.hpp"

#include <type_traits>
#include <vector>

#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace io {

using std::move;
using std::swap;
using std::vector;
using namespace balor::system;


namespace {
static_assert(std::is_same<BYTE, unsigned char>::value, "Invalid typedef");
static_assert(std::is_same<DWORD, unsigned long>::value, "Invalid typedef");

static_assert(Registry::ValueKind::string       == REG_SZ       , "Invalid enum value");
static_assert(Registry::ValueKind::expandString == REG_EXPAND_SZ, "Invalid enum value");
static_assert(Registry::ValueKind::binary       == REG_BINARY   , "Invalid enum value");
static_assert(Registry::ValueKind::dword        == REG_DWORD    , "Invalid enum value");
static_assert(Registry::ValueKind::multiString  == REG_MULTI_SZ , "Invalid enum value");
static_assert(Registry::ValueKind::qword        == REG_QWORD    , "Invalid enum value");


void checkResult(const LONG result) {
	if (result != ERROR_SUCCESS) {
		switch (result) {
			case ERROR_ACCESS_DENIED      : throw Registry::AccessDeniedException();
			case ERROR_FILE_NOT_FOUND     : break; // ó·äOÇìäÇ∞Ç»Ç¢èÍçáÇÃÇŸÇ§Ç™ëΩÇ¢
			case ERROR_INVALID_PARAMETER  : throw Registry::InvalidParameterException();
			case ERROR_KEY_DELETED        : throw Registry::KeyDeletedException();
			default                       : assert("failed to Registry function" && false); // Ç»ÇÒÇÁÇ©ÇÃÉGÉâÅ[
		}
	}
}
} // namespace



bool Registry::ValueKind::_validate(ValueKind value) {
	switch (value) {
		case string       :
		case expandString :
		case binary       :
		case dword        :
		case multiString  :
		case qword        : return true;
		default           : return false;
	}
}



Registry::NamesIterator::NamesIterator(HKEY handle, bool isKeyName) : handle(handle), isKeyName(isKeyName), count(static_cast<unsigned long>(-1)) {
	assert("Null handle" && handle);

	name[0] = L'\0';
	++*this;
}


void Registry::NamesIterator::refresh() {
	assert("Iterator out of range" && *this);
	--count;
	++*this;
}


const wchar_t* Registry::NamesIterator::operator*() const {
	assert("Iterator out of range" && *this);
	return name;
}


Registry::NamesIterator& Registry::NamesIterator::operator++() {
	assert("Iterator out of range" && *this);
	DWORD size = sizeof(name) / sizeof(name[0]);
	LONG result;
	if (isKeyName) {
		FILETIME time;
		result = RegEnumKeyExW(handle, ++count, name, &size, nullptr, nullptr, nullptr, &time);
	} else {
		result = RegEnumValueW(handle, ++count, name, &size, nullptr, nullptr, nullptr, nullptr);
	}
	if (result == ERROR_NO_MORE_ITEMS) {
		handle = nullptr;
	} else {
		checkResult(result);
	}
	return *this;
}


Registry::NamesIterator::operator bool() const {
	return handle != nullptr;
}



Registry::Registry() : _handle(nullptr) {
}


Registry::Registry(Registry&& value) : _handle(value._handle) {
	value._handle = nullptr;
}


Registry::Registry(StringRange path, bool writable) : _handle(nullptr) {
	assert("Empty path" && !path.empty());
	int rootKeyNameEnd = String::refer(path).indexOf(L'\\');
	StringRange subKeyName = L"";
	if (rootKeyNameEnd == -1) {
		rootKeyNameEnd = path.length();
	} else {
		subKeyName  = StringRange(path.c_str() + rootKeyNameEnd + 1);
	}
	if (!rootKeyNameEnd) {
		assert("Invalid Registry root key name" && false);
	} else if (!String::compare(path, 0, L"HKEY_CLASSES_ROOT", 0, rootKeyNameEnd)) {
		*this = Registry::classesRoot();
	} else if (!String::compare(path, 0, L"HKEY_CURRENT_CONFIG", 0, rootKeyNameEnd)) {
		*this = Registry::currentConfig();
	} else if (!String::compare(path, 0, L"HKEY_CURRENT_USER", 0, rootKeyNameEnd)) {
		*this = Registry::currentUser();
	} else if (!String::compare(path, 0, L"HKEY_LOCAL_MACHINE", 0, rootKeyNameEnd)) {
		*this = Registry::localMachine();
	} else if (!String::compare(path, 0, L"HKEY_USERS", 0, rootKeyNameEnd)) {
		*this = Registry::users();
	} else {
		assert("Invalid Registry root key name" && false);
	}
	if (!subKeyName.empty()) {
		*this = openKey(subKeyName, writable);
	}
}


Registry::~Registry() {
	if (_handle) {
		verify(RegCloseKey(_handle) == ERROR_SUCCESS);
		//_handle = nullptr;
	}
}


Registry& Registry::operator=(Registry&& value) {
	swap(_handle, value._handle);
	return *this;
}


Registry Registry::classesRoot() {
	Registry registry;
	registry._handle = HKEY_CLASSES_ROOT;
	return registry;
}


Registry Registry::createKey(StringRange keyName, bool writable) {
	assert("Empty Registry" && *this);
	assert("Empty keyName" && !keyName.empty());

	Registry key;
	checkResult(RegCreateKeyExW(_handle, keyName.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE
		, writable ? KEY_ALL_ACCESS : KEY_READ, nullptr, &key._handle, nullptr));
	return key;
}


Registry Registry::currentConfig() {
	Registry registry;
	registry._handle = HKEY_CURRENT_CONFIG;
	return registry;
}


Registry Registry::currentUser() {
	Registry registry;
	registry._handle = HKEY_CURRENT_USER;
	return registry;
}


void Registry::flush() {
	assert("Empty Registry" && *this);
	verify(RegFlushKey(_handle) == ERROR_SUCCESS);
}


vector<unsigned char> Registry::getBinary(StringRange valueName) const {
	assert("Empty Registry" && *this);

	DWORD type = REG_NONE;
	DWORD size = 0;
	LONG result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, nullptr, &size);
	vector<unsigned char> buffer;
	if (result == ERROR_SUCCESS && 0 < size) {
		buffer.resize(size);
		result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, buffer.data(), &size);
	}
	if (result == ERROR_FILE_NOT_FOUND) {
		throw ValueNotFoundException();
	}
	checkResult(result);
	return buffer;
}


unsigned long Registry::getDword(StringRange valueName) const {
	assert("Empty Registry" && *this);

	DWORD type = REG_NONE;
	DWORD size = 0;
	LONG result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, nullptr, &size);
	unsigned long value = 0;
	if (result == ERROR_SUCCESS) {
		if (type != REG_DWORD && size != sizeof(value)) {
			throw ValueKindMismatchException();
		}
		result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
	}
	if (result == ERROR_FILE_NOT_FOUND) {
		throw ValueNotFoundException();
	}
	checkResult(result);
	return value;
}


unsigned __int64 Registry::getQword(StringRange valueName) const {
	assert("Empty Registry" && *this);

	DWORD type = REG_NONE;
	DWORD size = 0;
	LONG result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, nullptr, &size);
	unsigned __int64 value = 0;
	if (result == ERROR_SUCCESS) {
		if (type != REG_DWORD && size != sizeof(value)) {
			throw ValueKindMismatchException();
		}
		result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
	}
	if (result == ERROR_FILE_NOT_FOUND) {
		throw ValueNotFoundException();
	}
	checkResult(result);
	return value;
}


String Registry::getString(StringRange valueName) const {
	assert("Empty Registry" && *this);
	StringBuffer buffer;
	getStringToBuffer(buffer, valueName);
	return move(buffer);
}


void Registry::getStringToBuffer(StringBuffer& buffer, StringRange valueName) const {
	assert("Empty Registry" && *this);

	DWORD type = REG_NONE;
	DWORD size = 0;
	LONG result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, nullptr, &size);
	if (result == ERROR_SUCCESS && 0 < size) {
		if (type != REG_SZ
		 && type != REG_EXPAND_SZ
		 && type != REG_MULTI_SZ) {
			throw ValueKindMismatchException();
		}
		int length = size / sizeof(wchar_t);
		buffer.reserveAdditionally(length);
		result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, &type, reinterpret_cast<LPBYTE>(buffer.end()), &size);
		buffer.expandLength(length - 1);
	}
	if (result == ERROR_FILE_NOT_FOUND) {
		throw ValueNotFoundException();
	}
	checkResult(result);
}


Registry::ValueKind Registry::getValueKind(StringRange valueName) const {
	assert("Empty Registry" && *this);

	ValueKind kind;
	const LONG result = RegQueryValueExW(_handle, valueName.c_str(), nullptr, reinterpret_cast<LPDWORD>(&kind), nullptr, nullptr);
	if (result == ERROR_FILE_NOT_FOUND) {
		return ValueKind::notFound;
	} else {
		checkResult(result);
		if (!ValueKind::_validate(kind)) {
			return ValueKind::unknown;
		}
	}
	return kind;
}


int Registry::keyCount() const {
	assert("Empty Registry" && *this);

	DWORD count = 0;
	checkResult(RegQueryInfoKeyW(_handle, nullptr, nullptr, nullptr, &count, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr));
	return count;
}


Registry::NamesIterator Registry::keyNamesIterator() const {
	return NamesIterator(_handle, true);
}


Registry Registry::localMachine() {
	Registry registry;
	registry._handle = HKEY_LOCAL_MACHINE;
	return registry;
}


Registry Registry::openKey(StringRange keyName, bool writable) const {
	assert("Empty Registry" && *this);
	assert("Empty kKeyName" && !keyName.empty());

	Registry key;
	checkResult(RegOpenKeyExW(_handle, keyName.c_str(), 0
		, writable ? KEY_ALL_ACCESS : KEY_READ, &key._handle));
	return key;
}


void Registry::removeKey(StringRange keyName, bool recursive) {
	assert("Empty Registry" && *this);
	assert("Empty keyName" && !keyName.empty());

	if (recursive) {
		Registry key = openKey(keyName, true);
		if (key) {
			auto i = NamesIterator(key, true);
			while (i) {
				key.removeKey(*i, true);
				i.refresh();
			}
		}
	}
	checkResult(RegDeleteKeyW(_handle, keyName.c_str()));
}


void Registry::removeValue(StringRange valueName) {
	assert("Empty Registry" && *this);

	checkResult(RegDeleteValueW(_handle, valueName.c_str()));
}


void Registry::setBinary(StringRange valueName, ArrayRange<const unsigned char> value, Registry::ValueKind kind) {
	assert("Empty Registry" && *this);
	assert("Invalid Registry::ValueKind" && ValueKind::_validate(kind));

	checkResult(RegSetValueExW(_handle, valueName.c_str(), 0, kind
		, value.empty() ? nullptr : value.begin(), value.size()));
}


void Registry::setDword(StringRange valueName, unsigned long value) {
	assert("Empty Registry" && *this);

	checkResult(RegSetValueExW(_handle, valueName.c_str(), 0, ValueKind::dword
		, reinterpret_cast<const BYTE*>(&value), sizeof(value)));
}


void Registry::setQword(StringRange valueName, unsigned __int64 value) {
	assert("Empty Registry" && *this);

	checkResult(RegSetValueExW(_handle, valueName.c_str(), 0, ValueKind::qword
		, reinterpret_cast<const BYTE*>(&value), sizeof(value)));
}


void Registry::setString(StringRange valueName, StringRange value, Registry::ValueKind kind) {
	assert("Empty Registry" && *this);
	assert("Invalid Registry::ValueKind" && (kind == ValueKind::string || kind == ValueKind::expandString || kind == ValueKind::multiString));

	checkResult(RegSetValueExW(_handle, valueName.c_str(), 0, kind
		, reinterpret_cast<const BYTE*>(value.c_str()), value.length() * sizeof(wchar_t)));
}


Registry Registry::users() {
	Registry registry;
	registry._handle = HKEY_USERS;
	return registry;
}


int Registry::valueCount() const {
	assert("Empty Registry" && *this);

	DWORD count = 0;
	checkResult(RegQueryInfoKeyW(_handle, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &count, nullptr, nullptr, nullptr, nullptr));
	return count;
}


Registry::NamesIterator Registry::valueNamesIterator() const {
	return NamesIterator(_handle, false);
}



	}
}