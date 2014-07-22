#include "Locale.hpp"

#include <vector>
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_THREAD_NO_LIB
#include <boost/thread/mutex.hpp>

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Convert.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace locale {


using std::move;
using std::vector;
using boost::mutex;


namespace {
void getLocaleString(StringBuffer& buffer, LCID lcid, LCTYPE type) {
	if (lcid == LOCALE_INVARIANT) {
		type |= LOCALE_NOUSEROVERRIDE;
	}
	const int size = GetLocaleInfoW(lcid, type, nullptr, 0);
	if (!size) {
		const DWORD errorCode = GetLastError();
		if (errorCode == ERROR_INVALID_FLAGS) { // 見つからなかった
			return;
		}
		assert("Failed to GetLocaleInfoW" && false);
	}
	buffer.reserveAdditionally(size);
	verify(GetLocaleInfoW(lcid, type, buffer.end(), size) == size);
	buffer.expandLength(size - 1);
}


int findLcid = 0; // これはスレッド同士で衝突さえしなければDLLごとに実体を持って良い。
StringRange* searchName = nullptr; // これはスレッド同士で衝突さえしなければDLLごとに実体を持って良い。


BOOL CALLBACK findLocaleProc(LPWSTR lcidString) {
	assert(searchName);

	long lcid = Convert::to<long>(lcidString, 16);
	wchar_t name[64];
	name[0] = L'\0';
	int size = GetLocaleInfoW(lcid, LOCALE_SISO639LANGNAME, name, sizeof(name) / sizeof(name[0]));
	assert(size);
	name[size - 1] = L'-';
	size = GetLocaleInfoW(lcid, LOCALE_SISO3166CTRYNAME, name + size, sizeof(name) / sizeof(name[0]) - size);
	assert(size);
	if (*searchName == name) {
		findLcid = lcid;
		return FALSE;
	}
	return TRUE;
}


mutex& getFindLocaleMutex() {
	static mutex findLocaleMutex; // これはスレッド同士で衝突さえしなければDLLごとに実体を持って良い。
	return findLocaleMutex;
}


mutex& findLocaleMutex = getFindLocaleMutex(); // マルチスレッドになるまえに初期化されることを保証する


vector<Locale>* localesPtr = nullptr; // これはスレッド同士で衝突さえしなければDLLごとに実体を持って良い。


BOOL CALLBACK enumLocalesProc(LPWSTR lcidString) {
	long lcid = Convert::to<long>(lcidString, 16);
	assert(localesPtr);
	localesPtr->push_back(Locale(lcid));
	return TRUE;
}


mutex& getEnumLocalesMutex() {
	static mutex enumLocalesMutex; // これはスレッド同士で衝突さえしなければDLLごとに実体を持って良い。
	return enumLocalesMutex;
}


mutex& enumLocalesMutex = getEnumLocalesMutex(); // マルチスレッドになるまえに初期化されることを保証する
} // namespace



Locale::Locale() : _id(LOCALE_INVARIANT) {
}


Locale::Locale(int lcid) : _id(static_cast<unsigned int>(lcid)) {
	if (!GetLocaleInfoW(lcid, LOCALE_SISO639LANGNAME, nullptr, 0)) {
		throw NotFoundException();
	}
	//if (!IsValidLocale(lcid, LCID_SUPPORTED)) { // ニュートラルでもエラーになる
	//	throw NotFoundException();
	//}
}


Locale::Locale(StringRange name) : _id(LOCALE_INVARIANT) {
	mutex::scoped_lock lock(getFindLocaleMutex());
	findLcid = -1;
	searchName = &name;
	verify(EnumSystemLocalesW(findLocaleProc, LCID_SUPPORTED)); // 重いが仕方ない。Vista 以降ならば LocaleNameToLCID が使えそうだが・・
	if (findLcid == -1) {
		throw NotFoundException();
	}
	_id = findLcid;
}


String Locale::cLocaleName() const {
	if (_id == LOCALE_INVARIANT) {
		return String();
	} else {
		StringBuffer buffer;
		getLocaleString(buffer, _id, LOCALE_SENGLISHLANGUAGENAME);
		buffer += L"_";
		getLocaleString(buffer, _id, LOCALE_SENGLISHCOUNTRYNAME);
		return move(buffer);
	}
}


String Locale::countryName() const {
	StringBuffer buffer;
	getLocaleString(buffer, _id, LOCALE_SISO3166CTRYNAME);
	return move(buffer);
}


Locale Locale::current() {
	Locale culture;
	culture._id = GetThreadLocale();
	return culture;
}


void Locale::current(const Locale& value) {
	verify(SetThreadLocale(value.id()));
}


String Locale::displayName() const {
	StringBuffer buffer;
	getLocaleString(buffer, _id, LOCALE_SLOCALIZEDDISPLAYNAME);
	return move(buffer);
}


String Locale::englishCountryName() const {
	StringBuffer buffer;
	getLocaleString(buffer, _id, LOCALE_SENGLISHCOUNTRYNAME);
	return move(buffer);
}


String Locale::englishLanguageName() const {
	StringBuffer buffer;
	getLocaleString(buffer, _id, LOCALE_SENGLISHLANGUAGENAME);
	return move(buffer);
}


int Locale::id() const {
	return _id;
}


Locale Locale::invariant() {
	return Locale();
}


int Locale::languageCode() const {
	wchar_t array[32];
	StringBuffer buffer(array);
	getLocaleString(buffer, _id, LOCALE_ILANGUAGE);
	assert(!buffer.empty());
	return Convert::to<int>(String::refer(buffer), 16);
}


String Locale::languageName() const {
	StringBuffer buffer;
	getLocaleString(buffer, _id, LOCALE_SISO639LANGNAME);
	return move(buffer);
}


vector<Locale> Locale::locales() {
	mutex::scoped_lock lock(getEnumLocalesMutex());
	vector<Locale> result;
	localesPtr = &result;
	scopeExit([&] () {
		localesPtr = nullptr;
	});
	verify(EnumSystemLocalesW(enumLocalesProc, LCID_INSTALLED));
	return result;
}


String Locale::name() const {
	StringBuffer buffer;
	nameToBuffer(buffer);
	return move(buffer);
}


void Locale::nameToBuffer(StringBuffer& buffer) const {
	getLocaleString(buffer, _id, LOCALE_SISO639LANGNAME);
	buffer += L"-";
	getLocaleString(buffer, _id, LOCALE_SISO3166CTRYNAME);
}


bool Locale::operator==(const Locale& rhs) const {
	return id() == rhs.id();
}


bool Locale::operator!=(const Locale& rhs) const {
	return !(*this == rhs);
}



	}
}