#include "EnvironmentVariable.hpp"

#include <utility>
#include <cstdlib>

#include <balor/io/Registry.hpp>
#include <balor/locale/Charset.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace system {


using std::move;
using namespace balor::io;
using namespace balor::locale;



EnvironmentVariable::VariablesIterator::VariablesIterator() : _current(0) {
}


String EnvironmentVariable::VariablesIterator::name() const {
	StringBuffer buffer;
	nameToBuffer(buffer);
	return move(buffer);
}


void EnvironmentVariable::VariablesIterator::nameToBuffer(StringBuffer& buffer) const {
	assert("Iterator out of range" && *this);
	const String variable = _wenviron ? String::refer(_wenviron[_current]) : Charset::default().decode(_environ[_current]);
	const int pos = variable.indexOf(L'=');
	assert(pos != -1);
	buffer.reserveAdditionally(pos);
	variable.copyTo(0, buffer.end(), pos + 1, 0, pos);
	buffer.length(buffer.length() + pos);
}


String EnvironmentVariable::VariablesIterator::value() const {
	StringBuffer buffer;
	valueToBuffer(buffer);
	return move(buffer);
}


void EnvironmentVariable::VariablesIterator::valueToBuffer(StringBuffer& buffer) const {
	assert("Iterator out of range" && *this);
	const String variable = _wenviron ? String::refer(_wenviron[_current]) : Charset::default().decode(_environ[_current]);
	const int pos = variable.indexOf(L'=');
	assert(pos != -1);
	int length = variable.length() - pos - 1;
	buffer.reserveAdditionally(length);
	variable.copyTo(pos + 1, buffer.end(), length + 1, 0, length + 1);
	buffer.expandLength(length);
}


EnvironmentVariable::VariablesIterator& EnvironmentVariable::VariablesIterator::operator++() {
	assert("Iterator out of range" && *this);
	++_current;
	return *this;
}


EnvironmentVariable::VariablesIterator::operator bool() const {
	return _wenviron ? _wenviron[_current] != nullptr : _environ[_current] != nullptr;
}


String EnvironmentVariable::expandStrings(StringRange string) {
	StringBuffer buffer;
	expandStringsToBuffer(buffer, string);
	return move(buffer);
}


void EnvironmentVariable::expandStringsToBuffer(StringBuffer& buffer, StringRange string) {
	if (string.empty()) {
		return;
	}
	const DWORD size = ExpandEnvironmentStringsW(string.c_str(), nullptr, 0);
	assert(0 < size);
	buffer.reserveAdditionally(size);
	verify(ExpandEnvironmentStringsW(string.c_str(), buffer.end(), size) == size);
	buffer.expandLength(size - 1);
}


bool EnvironmentVariable::find(StringRange valueName) {
	assert("Empty valueName" && !valueName.empty());
	return 0 < GetEnvironmentVariableW(valueName.c_str(), nullptr, 0);
}


String EnvironmentVariable::get(StringRange valueName) {
	StringBuffer buffer;
	getToBuffer(buffer, valueName);
	return move(buffer);
}


void EnvironmentVariable::getToBuffer(StringBuffer& buffer, StringRange valueName) {
	assert("Empty valueName" && !valueName.empty());
	const DWORD size = GetEnvironmentVariableW(valueName.c_str(), nullptr, 0);
	if (0 < size) {
		buffer.reserveAdditionally(size);
		verify(GetEnvironmentVariableW(valueName.c_str(), buffer.end(), size) == (size - 1));
		buffer.expandLength(size - 1);
	}
}


void EnvironmentVariable::remove(StringRange valueName) {
	assert("Empty valueName" && !valueName.empty());
	if (!SetEnvironmentVariableW(valueName.c_str(), nullptr)) {
		assert("Failed to remove env value" && GetLastError() == ERROR_ENVVAR_NOT_FOUND);
	}
}


void EnvironmentVariable::set(StringRange valueName, StringRange value) {
	assert("Empty valueName" && !valueName.empty());
	verify(SetEnvironmentVariableW(valueName.c_str(), value.c_str()));
}


Registry EnvironmentVariable::systemRegistry() {
	return Registry::localMachine().openKey(L"System\\CurrentControlSet\\Control\\Session Manager\\Environment", true);
}


void EnvironmentVariable::updateAllProcess() {
#ifdef _WIN64 
	PDWORD_PTR returnValue;
	verify(SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, returnValue));
#else
	DWORD returnValue;
	verify(SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &returnValue));
#endif
}


Registry EnvironmentVariable::userRegistry() {
	return Registry::currentUser().openKey(L"Environment", true);
}


EnvironmentVariable::VariablesIterator EnvironmentVariable::variablesBegin() {
	return VariablesIterator();
}



	}
}