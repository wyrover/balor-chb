#include "String.hpp"

#include <algorithm>
#include <cstring>
#include <hash_map>
#include <vector>

#include <balor/locale/Charset.hpp>
#include <balor/locale/Locale.hpp>
#include <balor/locale/Unicode.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Singleton.hpp>
#include <balor/StringBuffer.hpp>


typedef enum _NORM_FORM {
    NormalizationOther  = 0,       // Not supported
    NormalizationC      = 0x1,     // Each base plus combining characters to the canonical precomposed equivalent.
    NormalizationD      = 0x2,     // Each precomposed character to its canonical decomposed equivalent.
    NormalizationKC     = 0x5,     // Each base plus combining characters to the canonical precomposed
                                   //   equivalents and all compatibility characters to their equivalents.
    NormalizationKD     = 0x6      // Each precomposed character to its canonical decomposed equivalent
                                   //   and all compatibility characters to their equivalents.
} NORM_FORM;


WINNORMALIZEAPI
int
WINAPI NormalizeString( __in                          NORM_FORM NormForm,
                        __in_ecount(cwSrcLength)      LPCWSTR   lpSrcString,
                        __in                          int       cwSrcLength,
                        __out_ecount_opt(cwDstLength) LPWSTR    lpDstString,
                        __in                          int       cwDstLength );

WINNORMALIZEAPI
BOOL
WINAPI IsNormalizedString( __in                   NORM_FORM NormForm,
                           __in_ecount(cwLength)  LPCWSTR   lpString,
                           __in                   int       cwLength );


namespace balor {

using std::max;
using std::min;
using std::move;
using std::string;
using std::swap;
using std::vector;
using std::wstring;
using namespace balor::locale;
using namespace balor::system;


namespace {
static_assert(String::CompareOptions::ignoreCase     == NORM_IGNORECASE, "Invalid enum value");
static_assert(String::CompareOptions::ignoreNonSpace == NORM_IGNORENONSPACE, "Invalid enum value");
static_assert(String::CompareOptions::ignoreSymbols  == NORM_IGNORESYMBOLS, "Invalid enum value");
static_assert(String::CompareOptions::ignoreKanaType == NORM_IGNOREKANATYPE, "Invalid enum value");
static_assert(String::CompareOptions::ignoreWidth    == NORM_IGNOREWIDTH, "Invalid enum value");
static_assert(String::CompareOptions::stringSort     == SORT_STRINGSORT, "Invalid enum value");

static_assert(String::NormalizationForm::c  == NormalizationC, "Innvalid enum value");
static_assert(String::NormalizationForm::d  == NormalizationD, "Innvalid enum value");
static_assert(String::NormalizationForm::kc == NormalizationKC, "Innvalid enum value");
static_assert(String::NormalizationForm::kd == NormalizationKD, "Innvalid enum value");

wchar_t nullString[] = L""; // DLLごとに実体をもってもかまわない


bool isValidStringCompareOptionsForSearch(String::CompareOptions value) { // 検索系処理では?ッ?する文字数が変動するような検索をサ??トしない
	return (value & ~(
		  String::CompareOptions::none
		| String::CompareOptions::ignoreCase
//		| String::CompareOptions::ignoreNonSpace
//		| String::CompareOptions::ignoreSymbols
		| String::CompareOptions::ignoreKanaType
		| String::CompareOptions::ignoreWidth
		| String::CompareOptions::stringSort
		)) == 0;
}


bool isOverlapped(const wchar_t* lhs, const wchar_t* rhs, int length) {
	const wchar_t* lhsend = lhs + length;
	const wchar_t* rhsend = rhs + length;
	return lhs <= rhsend && rhs <= lhsend;
}


inline const wchar_t* nextElement(const wchar_t* c_str, const wchar_t* end) {
	assert(c_str);
	assert(c_str <= end);

	// このやり方は結合文字がサロゲ?トペアであった場合に正しく対処できない。
	// どのみち CharNextW と GetStringTypeW がサロゲ?トに対応しなければ完璧になりはしない。
	if (c_str == end) {
		return nullptr;
	}
	while (++c_str < end) {
		if (IS_LOW_SURROGATE(*c_str)) {
			continue;
		}
		WORD type;
		// Vista 以?であれば C3_LOWSURROGATE や C3_LOWSURROGATE が帰ってくるようだ。
		// しかし C3_LOWSURROGATE や C3_LOWSURROGATE が帰ったとしてもそれ以外のフラグがたっていないので文字種が判らない。
		if (!GetStringTypeW(CT_CTYPE3, c_str, 1, &type)) {
			break;
		}
		if ((type & (C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK | C3_LOWSURROGATE)) == 0 || (type & C3_ALPHA) != 0) {
			break;
		}
	}
	return c_str;

	// これがVistaの CharNextWの実装（多分）。サロゲ?トペアは対応されているように見えて対応されていない。
	// 文字列の頭にサロゲ?トペアがあった場合は２文字分カウントされ、文字列の途中にあった場合は１文字カウントになる。おまけに?イン?は半?な位置を指す。

	//if (!*c_str) {
	//	return c_str;
	//}
	//while (*(++c_str)) {
	//	WORD type;
	//	if (!GetStringTypeW(CT_CTYPE3, c_str, 1, &type)) {
	//		break;
	//	}
	//	if (type & 0x801) { // C3_NONSPACING | C3_HIGHSURROGATE
	//		if (!(type & 0x9000)) { // C3_ALPHA | C3_LOWSURROGATE
	//			continue;
	//		}
	//	}
	//	break;
	//}
	//return c_str;
}


inline const wchar_t* prevElement(const wchar_t* start, const wchar_t* c_str) {
	assert(start);
	assert(start <= c_str);

	// このやり方は結合文字がサロゲ?トペアであった場合に正しく対処できない。
	if (c_str == start) {
		return nullptr;
	}
	const wchar_t* last = c_str;
	do {
		--c_str;
		if (IS_LOW_SURROGATE(*c_str)) {
			return start < c_str ? --c_str : start;
		}
		WORD type;
		// Vista 以?であれば C3_LOWSURROGATE や C3_LOWSURROGATE が帰ってくるようだ。
		// しかし C3_LOWSURROGATE や C3_LOWSURROGATE が帰ったとしてもそれ以外のフラグがたっていないので文字種が判らない。
		if (!GetStringTypeW(CT_CTYPE3, c_str, 1, &type)) {
			return c_str;
		}
		if ((type & (C3_NONSPACING | C3_DIACRITIC | C3_VOWELMARK | C3_LOWSURROGATE)) == 0 || (type & C3_ALPHA) != 0) {
			return c_str;
		}
	} while(start < c_str);

	// ここに来たという事は最後まで結合文字だったということ。１ワ?ド分だけ戻す（CharPrevW?拠）
	return --last;

	//// これがVistaの CharPrevWの実装（多分）。サロゲ?トペアは対応されているように見えて対応されていない。
	//wchar_t* i = c_str;
	//if (c_str == start) {
	//	return i;
	//}
	//while () {
	//	--c_str;
	//	wchar_t type; [ebp+0Ch]
	//	if (!GetStringTypeW()) {
	//		break;
	//	}
	//	if (type & 0x801) {
	//		if (!(type & 0x9000)) {
	//			if (start < c_str) {
	//				continue;
	//			} else {
	//				--c_str;
	//				break;
	//			}
	//		}
	//	}
	//	i = c_str;
	//	break;
	//}

	//if (c_str < start) {
	//	--i;
	//}
	//return i;
}


inline const wchar_t* skipElement(const wchar_t* c_str, const wchar_t* end, int length) {
	while (length) {
		if (end <= c_str) {
			return nullptr;
		}
		c_str = nextElement(c_str, end);
		--length;
	}
	return c_str;
}


inline const wchar_t* reverseSkipElement(const wchar_t* start, const wchar_t* c_str, int length) {
	while (length) {
		if (c_str <= start) {
			return nullptr;
		}
		c_str = prevElement(start, c_str);
		--length;
	}
	return c_str;
}


inline wchar_t toLower(wchar_t c) {
	if (L'A' <= c && c <= L'Z') {
		c += L'a' - L'A';
	}
	return c;
}


// _wcsicmp はロケ?ルに依存してしまう。_wcsicmp_lに常にＣロケ?ルを?えるという手もあるがここは効率を取りたい。
int compareOrdinalIgnoreCase(const wchar_t* lhs, const wchar_t* rhs) {
	wchar_t l, r;
	do {
		l = toLower(*lhs);
		r = toLower(*rhs);
		++lhs;
		++rhs;
	} while ((l) && (l == r));
	if (l < r) {
		return -1;
	} else if (r < l) {
		return 1;
	} else {
		return 0;
	}
}


// _wcsnicmp はロケ?ルに依存してしまう。_wcsnicmp_lに常にＣロケ?ルを?えるという手もあるがここは効率を取りたい。
int compareOrdinalIgnoreCase(const wchar_t* lhs, const wchar_t* rhs, int length) {
	wchar_t l, r;
	if (length) {
		do {
			l = toLower(*lhs);
			r = toLower(*rhs);
			++lhs;
			++rhs;
		} while ((--length) && (l == r));
		if (l < r) {
			return -1;
		} else if (r < l) {
			return 1;
		}
	}
	return 0;
}


inline bool equalIgnoreCase(wchar_t lhs, wchar_t rhs) {
	return toLower(lhs) == toLower(rhs);
}


int indexOfOrdinalIgnoreCase(const wchar_t* c_str, StringRange value, int beginIndex, int length) {
	if (value.empty()) {
		return beginIndex;
	}
	const wchar_t* i = c_str + beginIndex;
	const wchar_t* compareEnd = i + length;
	const wchar_t* end = compareEnd - (0 < value.rawLength() ?  value.rawLength() - 1 : 0);
	while (i < end) {
		const wchar_t* j = i;
		const wchar_t* k = value.c_str();
		while (j < compareEnd && *k && equalIgnoreCase(*j, *k)) {
			++j;
			++k;
		}
		if (!*k) {
			return i - c_str;
		}
		++i;
	}
	return -1;
}


int lastIndexOfOrdinalIgnoreCase(const wchar_t* c_str, StringRange value, int beginIndex, int length) {
	if (value.empty()) {
		return beginIndex;
	}
	const wchar_t* start = c_str + beginIndex;
	const wchar_t* end = start + length;
	const wchar_t* i = end - value.length();
	while (start <= i) {
		const wchar_t* j = i;
		const wchar_t* k = value.c_str();
		while (j < end && *k && equalIgnoreCase(*j, *k)) {
			++j;
			++k;
		}
		if (!*k) {
			return i - c_str;
		}
		--i;
	}
	return -1;
}


class NormalizeModule {
	friend Singleton<NormalizeModule>;

	NormalizeModule() : module(L"Normaliz.dll") {
		if (!module) {
			throw String::NormalizeDllNotFoundException();
		}
		normalizeStringPtr = module.getFunction<decltype(NormalizeString)>("NormalizeString");
		assert(normalizeStringPtr);
		isNormalizedStringPtr = module.getFunction<decltype(IsNormalizedString)>("IsNormalizedString");
		assert(isNormalizedStringPtr);
	}
	~NormalizeModule() {
	}

public:
	bool isNormalizedString(String::NormalizationForm frame, const wchar_t* string, int length) {
		return (*isNormalizedStringPtr)(static_cast<NORM_FORM>(static_cast<int>(frame)), string, length) != FALSE;
	}

	int normalizeString(String::NormalizationForm frame, const wchar_t* source, int sourceLength, wchar_t* dest, int destLength) {
		return (*normalizeStringPtr)(static_cast<NORM_FORM>(static_cast<int>(frame)), source, sourceLength, dest, destLength);
	}

private:
	Module module;
	decltype(NormalizeString)* normalizeStringPtr;
	decltype(IsNormalizedString)* isNormalizedStringPtr;
};


inline NormalizeModule& getNormalizeModule() {
	return Singleton<NormalizeModule>::get();
}


std::vector<String, std::allocator<String> > splitOnWhiteSpace(const wchar_t* c_str, int length) {
	vector<String> result;
	const wchar_t* i = c_str;
	const wchar_t* end = c_str + length;
	for (;;) {
		const wchar_t* j = i;
		while (j < end && !Unicode::isWhiteSpace(*j)) {
			++j;
		}
		if (end <= j) {
			const int elementLength = end - i;
			result.push_back(String(i, elementLength));
			break;
		} else {
			const int elementLength = j - i;
			result.push_back(String(i, elementLength));
			i = j + 1;
		}
	}
	return result;
}
} // namespace



bool String::CompareOptions::_validate(CompareOptions value) {
	return (value & ~(
		  none
		| ignoreCase
		| ignoreNonSpace
		| ignoreSymbols
		| ignoreKanaType
		| ignoreWidth
		| stringSort
		)) == 0;
}


bool String::NormalizationForm::_validate(NormalizationForm value) {
	switch (value) {
		case c  :
		case d  :
		case kc :
		case kd : return true;
		default : return false;
	}
}



String::String() : _c_str(nullString), _length(0) {
}


String::String(const String& value) : _length(value._length) {
	if (value.referred()) {
		_c_str = value._c_str;
	} else {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemcpy(buffer, value._c_str, _length + 1);
		_c_str = buffer;
	}
}


String::String(String&& value) : _c_str(value._c_str), _length(value._length) {
	value._c_str = nullString;
	value._length = 0;
}


String::String(const StringBuffer& value) : _length(value._length) {
	if (!_length) {
		_c_str = nullString;
	} else {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemcpy(buffer, value.begin(), _length + 1);
		_c_str = buffer;
	}
}


String::String(StringBuffer&& value) : _length(value._length) {
	if (!_length) {
		_c_str = nullString;
	} else if (value.allocatable()) {
		_c_str = value._begin;
		value._begin = StringBuffer()._begin;
		value._length = 0;
		value._capacity = 1;
	} else {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemcpy(buffer, value.begin(), _length + 1);
		_c_str = buffer;
	}
}


String::String(const wstring& value) : _length(value.length()) {
	if (0 < _length) {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemcpy(buffer, value.c_str(), _length + 1);
		_c_str = buffer;
	} else {
		_c_str = nullString;
	}
}


String::String(const wchar_t* value) {
	assert("Null value" && value);

	_length = wcslen(value);
	if (0 < _length) {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemcpy(buffer, value, _length + 1);
		_c_str = buffer;
	} else {
		_c_str = nullString;
	}
}


String::String(wchar_t c, int count) {
	assert("Negative count" && 0 <= count);

	_length = count;
	if (0 < _length) {
		wchar_t* buffer = new wchar_t[_length + 1];
		wmemset(buffer, c, _length);
		buffer[_length] = L'\0';
		_c_str = buffer;
	} else {
		_c_str = nullString;
	}
}


String::String(const wchar_t* value, int length) {
	assert("Null value" && value);
	assert("Negative length" && 0 <= length);

	this->_length = length;
	if (0 < length) {
		wchar_t* buffer = new wchar_t[length + 1];
		wmemcpy(buffer, value, length);
		buffer[length] = L'\0';
		_c_str = buffer;
	} else {
		_c_str = nullString;
	}
}


String::String(ByteStringRange value, const balor::locale::Charset& charset) : _c_str(nullString), _length(0) {
	*this = charset.decode(value);
}


String::~String() {
	if (!referred()) {
		delete[] _c_str;
	}
}


String& String::operator=(const String& value) {
	if (&value != this) {
		*this = String(value);
	}
	return *this;
}


String& String::operator=(String&& value) {
	swap(_c_str, value._c_str);
	swap(_length, value._length);
	return *this;
}


String& String::operator=(const StringBuffer& value) {
	return *this = String(value);
}


String& String::operator=(StringBuffer&& value) {
	return *this = String(move(value));
}


String& String::operator=(const wstring& value) {
	return *this = String(value);
}


String& String::operator=(const wchar_t* value) {
	assert("Null value" && value);
	return *this = String(value);
}


int String::compare(StringRange lhs, StringRange rhs, bool ignoreCase) {
	return ignoreCase ? compareOrdinalIgnoreCase(lhs.c_str(), rhs.c_str()) : wcscmp(lhs.c_str(), rhs.c_str());
}


int String::compare(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options) {
	assert("Invalid options" && CompareOptions::_validate(options));
	return CompareStringW(locale.id(), options, lhs.c_str(), lhs.rawLength(), rhs.c_str(), rhs.rawLength()) - 2;
}


int String::compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, bool ignoreCase) {
	assert("lindex out of range" && 0 <= lindex);
	assert("rindex out of range" && 0 <= rindex);
	assert("lindex out of range" && lindex <= lhs.length());
	assert("rindex out of range" && rindex <= rhs.length());
	assert("Negative length" && 0 <= length);

	const wchar_t* lptr = lhs.c_str() + lindex;
	const wchar_t* rptr = rhs.c_str() + rindex;
	return  ignoreCase ? compareOrdinalIgnoreCase(lptr, rptr, length) : wcsncmp(lptr, rptr, length);
}


int String::compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, const Locale& locale, String::CompareOptions options) {
	assert("lindex out of range" && 0 <= lindex);
	assert("rindex out of range" && 0 <= rindex);
	assert("lindex out of range" && lindex <= lhs.length());
	assert("rindex out of range" && rindex <= rhs.length());
	assert("Negative length" && 0 <= length);
	assert("Invalid options" && CompareOptions::_validate(options));

	const wchar_t* lptr = lhs.c_str() + lindex;
	const wchar_t* rptr = rhs.c_str() + rindex;
	return CompareStringW(locale.id(), options, lptr, min(lhs.length() - lindex, length), rptr, min(rhs.length() - rindex, length)) - 2;
}


int String::compareTo(StringRange rhs) const {
	return CompareStringW(Locale::current().id(), String::CompareOptions::none, _c_str, length(), rhs.c_str(), rhs.rawLength()) - 2;
}


bool String::contains(StringRange value) const {
	return 0 <= indexOf(value);
}


#pragma warning(push)
#pragma warning(disable : 4100) // 'destinationSize' : 引数は関数の?体部で 1 度も参照されません。
void String::copyTo(wchar_t* array, int arraySize) const {
	assert("Null array" && array);
	assert("not enough arraySize" && length() < arraySize);
	assert("pointer overlapped" && !isOverlapped(array, _c_str, length()));
	assert("array is bad write pointer" && !IsBadWritePtr(array, arraySize));

	wmemcpy(array, _c_str, length() + 1);
}


void String::copyTo(int beginIndex, wchar_t* array, int arraySize, int arrayIndex, int length) const {
	assert("sourceIndex out of range" && 0 <= beginIndex);
	assert("sourceIndex out of range" && beginIndex <= this->length());
	assert("Null array" && array);
	assert("arrayIndex out of range" && 0 <= arrayIndex);
	assert("arrayIndex out of range" && arrayIndex < arraySize);
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex + 1);
	assert("length out of range" && length <= arraySize - arrayIndex);
	assert("pointer overlapped" && !isOverlapped(array + arrayIndex, _c_str + beginIndex, length));
	assert("array is bad write pointer" && !IsBadWritePtr(array, arraySize));

	wmemcpy(array + arrayIndex, _c_str + beginIndex, length);
}
#pragma warning(pop)


bool String::empty() const {
	return !*_c_str;
}


bool String::endsWith(StringRange value, bool ignoreCase) const {
	const int sourceLength = length();
	const int valueLength = value.length();
	if (sourceLength < valueLength) {
		return false;
	}
	const wchar_t* first = _c_str + sourceLength - valueLength;
	return ignoreCase ? compareOrdinalIgnoreCase(first, value.c_str()) == 0 : wcscmp(first, value.c_str()) == 0;
}


bool String::endsWith(StringRange value, const Locale& locale, String::CompareOptions options) const {
	assert("Invalid options" && isValidStringCompareOptionsForSearch(options));

	// Vista 以?ならば FindNLSString が使えて楽なのだが・・
	const int valueLength = value.length();
	if (!valueLength) {
		return true;
	}
	const wchar_t* compareEnd = _c_str + length();
	const wchar_t* compareBegin = reverseSkipElement(_c_str, compareEnd, String::refer(value.c_str(), valueLength).lengthInLetter());
	if (!compareBegin) {
		return false;
	}
	return CompareStringW(locale.id(), options, compareBegin, compareEnd - compareBegin, value.c_str(), valueLength) == CSTR_EQUAL;
}


bool String::equals(ByteStringRange lhs, ByteStringRange rhs, bool ignoreCase) {
	return ignoreCase ? _stricmp(lhs.c_str(), rhs.c_str()) == 0 : strcmp(lhs.c_str(), rhs.c_str()) == 0;
}


bool String::equals(StringRange lhs, StringRange rhs, bool ignoreCase) {
	return ignoreCase ? compareOrdinalIgnoreCase(lhs.c_str(), rhs.c_str()) == 0 : wcscmp(lhs.c_str(), rhs.c_str()) == 0;
}


bool String::equals(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options) {
	assert("Invalid options" && CompareOptions::_validate(options));

	return CompareStringW(locale.id(), options, lhs.c_str(), lhs.rawLength(), rhs.c_str(), rhs.rawLength()) == CSTR_EQUAL;
}


bool String::equalTo(StringRange rhs, bool ignoreCase) const {
	return (ignoreCase ? compareOrdinalIgnoreCase(_c_str, rhs.c_str()) : wcscmp(_c_str, rhs.c_str())) == 0;
}


bool String::equalTo(StringRange rhs, const Locale& locale, String::CompareOptions options) const {
	assert("Invalid options" && CompareOptions::_validate(options));

	return CompareStringW(locale.id(), options, _c_str, length(), rhs.c_str(), rhs.rawLength()) == CSTR_EQUAL;
}


String String::erase(int beginIndex) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= length());

	if (beginIndex == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[beginIndex + 1];
	String result(beginIndex, buffer);
	wmemcpy(buffer, _c_str, beginIndex);
	buffer[beginIndex] = L'\0';
	return result;
}


String String::erase(int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	const int thisLength = this->length();
	int newLength = thisLength - length;
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	wmemcpy(buffer, _c_str, beginIndex);
	wmemcpy(buffer + beginIndex, _c_str + beginIndex + length, newLength - beginIndex + 1);
	return result;
}


int String::getLength(const char* value) {
	assert("Null value" && value);
	return strlen(value);
}


int String::getLength(const wchar_t* value) {
	assert("Null value" && value);
	return wcslen(value);
}


int String::getNextLetterIndex(int beginIndex) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= length());
	
	const int thisLength = length();
	if (beginIndex == thisLength) {
		return beginIndex;
	}
	const wchar_t* next = nextElement(_c_str + beginIndex, _c_str + thisLength);
	return next - _c_str;
}


std::size_t String::hashCode() const {
	return std::hash<std::wstring>()(c_str());
}


int String::indexOf(StringRange value, bool ignoreCase) const {
	return indexOf(value, 0, length(), ignoreCase);
}


int String::indexOf(StringRange value, const Locale& locale, String::CompareOptions options) const {
	return indexOf(value, 0, length(), locale, options);
}


int String::indexOf(wchar_t value, int beginIndex) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= length());

	const wchar_t* i = _c_str + beginIndex;
	while (*i && *i != value) {
		++i;
	}
	return *i == value ? i - _c_str : -1;
}


int String::indexOf(StringRange value, int beginIndex, bool ignoreCase) const {
	return indexOf(value, beginIndex, length() - beginIndex, ignoreCase);
}


int String::indexOf(StringRange value, int beginIndex, const Locale& locale, String::CompareOptions options) const {
	return indexOf(value, beginIndex, length() - beginIndex, locale, options);
}


int String::indexOf(wchar_t value, int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	const wchar_t* i = _c_str + beginIndex;
	const wchar_t* end = i + length;
	while (i < end && *i != value) {
		++i;;
	}
	return i < end ? i - _c_str : -1;
}


int String::indexOf(StringRange value, int beginIndex, int length, bool ignoreCase) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	if (ignoreCase) {
		return indexOfOrdinalIgnoreCase(_c_str, value, beginIndex, length);
	} else {
		if (value.empty()) {
			return beginIndex;
		}
		const wchar_t* i = _c_str + beginIndex;
		const wchar_t* compareEnd = i + length;
		const wchar_t* end = compareEnd - (0 < value.rawLength() ?  value.rawLength() - 1 : 0);
		while (i < end) {
			const wchar_t* j = i;
			const wchar_t* k = value.c_str();
			while (j < compareEnd && *k && !(*j - *k)) {
				++j;
				++k;
			}
			if (!*k) {
				return i - _c_str;
			}
			++i;
		}
		return -1;
	}
}


int String::indexOf(StringRange value, int beginIndex, int length, const Locale& locale, String::CompareOptions options) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);
	assert("Invalid options" && isValidStringCompareOptionsForSearch(options));

	// Vista 以?ならば FindNLSString が使えて楽なのだが・・
	const int valueLength = value.length();
	if (!valueLength) {
		return beginIndex;
	}
	const wchar_t* i = _c_str + beginIndex;
	const wchar_t* end = i + length;
	const wchar_t* compareEnd = skipElement(i, end, String::refer(value.c_str(), valueLength).lengthInLetter());
	if (!compareEnd) {
		return -1;
	}
	do {
		if (CompareStringW(locale.id(), options, i, compareEnd - i, value.c_str(), valueLength) == CSTR_EQUAL) {
			return i - _c_str;
		}
		i = nextElement(i, end);
		compareEnd = nextElement(compareEnd, end);
	} while (compareEnd);
	return -1;
}


int String::indexOfAny(StringRange anyOf, int beginIndex) const {
	return indexOfAny(anyOf, beginIndex, this->length() - beginIndex);
}


int String::indexOfAny(StringRange anyOf, int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	const wchar_t* i = _c_str + beginIndex;
	const wchar_t* end = i + length;
	while (i < end) {
		for (const wchar_t* j = anyOf.c_str(); *j; ++j) {
			if (*j == *i) {
				return i - _c_str;
			}
		}
		i++;
	}
	return -1;
}


String String::insert(int beginIndex, StringRange value) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= length());

	const int thisLength = length();
	const int valueLength = value.length();
	const int newLength = thisLength + valueLength;
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	wmemcpy(buffer, _c_str, beginIndex);
	wmemcpy(buffer + beginIndex, value.c_str(), valueLength);
	wmemcpy(buffer + beginIndex + valueLength, _c_str + beginIndex, thisLength - beginIndex + 1);
	return result;
}


bool String::isNormalized(String::NormalizationForm frame) const {
	assert("Invalid String::NormalizationForm" && NormalizationForm::_validate(frame));
	return getNormalizeModule().isNormalizedString(frame, _c_str, length());
}


bool String::isWhiteSpace() const {
	for (auto i = _c_str, end = _c_str + length(); i != end; ++i) {
		if (!Unicode::isWhiteSpace(*i)) {
			return false;
		}
	}
	return true;
}


String String::join(StringRange separator, StringRangeArray values) {
	if (values.empty()) {
		return String();
	}
	const int separatorLength = separator.length();
	int newSize = separatorLength * (values.length() - 1);
	for (auto i = 0, end = values.length(); i != end; ++i) {
		newSize += values[i].length();
	}
	if (newSize == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newSize + 1];
	String result(newSize, buffer);
	int i = 0;
	wchar_t* j = buffer;
	const wchar_t* separatorPtr = separator.c_str();
	int iLength = values[i].length();
	wmemcpy(j, values[i].c_str(), iLength);
	j += iLength;
	++i;
	for (auto end = values.length(); i != end; ++i) {
		wmemcpy(j, separatorPtr, separatorLength);
		j += separatorLength;
		auto value = values[i];
		iLength = value.length();
		wmemcpy(j, value.c_str(), iLength);
		j += iLength;
	}
	*j = L'\0';
	return result;
}


int String::lastIndexOf(StringRange value, bool ignoreCase) const {
	return lastIndexOf(value, 0, length(), ignoreCase);
}


int String::lastIndexOf(StringRange value, const Locale& locale, String::CompareOptions options) const {
	return lastIndexOf(value, 0, length(), locale, options);
}


int String::lastIndexOf(wchar_t value, int beginIndex) const {
	return lastIndexOf(value, beginIndex, length() - beginIndex);
}


int String::lastIndexOf(StringRange value, int beginIndex, bool ignoreCase) const {
	return lastIndexOf(value, beginIndex, length() - beginIndex, ignoreCase);
}


int String::lastIndexOf(StringRange value, int beginIndex, const Locale& locale, String::CompareOptions options) const {
	return lastIndexOf(value, beginIndex, length() - beginIndex, locale, options);
}


int String::lastIndexOf(wchar_t value, int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	const wchar_t* start = _c_str + beginIndex;
	const wchar_t* i = start + length;
	while (start <= --i && *i != value) {
	}
	return start <= i ? i - _c_str : -1;
}


int String::lastIndexOf(StringRange value, int beginIndex, int length, bool ignoreCase) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	if (ignoreCase) {
		return lastIndexOfOrdinalIgnoreCase(_c_str, value, beginIndex, length);
	} else {
		if (value.empty()) {
			return beginIndex;
		}
		const wchar_t* start = _c_str + beginIndex;
		const wchar_t* end = start + length;
		const wchar_t* i = end - value.length();
		while (start <= i) {
			const wchar_t* j = i;
			const wchar_t* k = value.c_str();
			while (j < end && *k && !(*j - *k)) {
				++j;
				++k;
			}
			if (!*k) {
				return i - _c_str;
			}
			--i;
		}
		return -1;
	}
}


int String::lastIndexOf(StringRange value, int beginIndex, int length, const Locale& locale, String::CompareOptions options) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);
	assert("Invalid options" && isValidStringCompareOptionsForSearch(options));

	// Vista 以?ならば FindNLSString が使えて楽なのだが・・
	const int valueLength = value.length();
	if (!valueLength) {
		return beginIndex;
	}
	const wchar_t* start = _c_str + beginIndex;
	const wchar_t* end = start + length;
	const wchar_t* i = reverseSkipElement(start, end, String::refer(value).lengthInLetter());
	if (!i) {
		return -1;
	}
	const wchar_t* compareEnd = end;
	do {
		if (CompareStringW(locale.id(), options, i, compareEnd - i, value.c_str(), valueLength) == CSTR_EQUAL) {
			return i - _c_str;
		}
		i = prevElement(start, i);
		compareEnd = prevElement(start, compareEnd);
	} while (i);
	return -1;
}


int String::lastIndexOfAny(StringRange anyOf, int beginIndex) const {
	return lastIndexOfAny(anyOf, beginIndex, this->length() - beginIndex);
}


int String::lastIndexOfAny(StringRange anyOf, int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	const wchar_t* start = _c_str + beginIndex;
	const wchar_t* i = start + length;
	while (start <= --i) {
		for (const wchar_t* j = anyOf.c_str(); *j; ++j) {
			if (*j == *i) {
				return i - _c_str;
			}
		}
	}
	return -1;
}


int String::length() const {
	return abs(_length);
}


int String::lengthInLetter() const {
	const wchar_t* i = _c_str;
	const wchar_t* end = _c_str + length();
	int count = 0;
	while (i < end) {
		++count;
		i = nextElement(i, end);
	}
	return count;
}


String String::normalize(String::NormalizationForm frame) const {
	assert("Invalid String::NormalizationForm" && NormalizationForm::_validate(frame));
	if (empty()) {
		return String();
	}
	const int newLength = getNormalizeModule().normalizeString(frame, _c_str, length(), nullptr, 0);
	assert(0 < newLength);
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	if (getNormalizeModule().normalizeString(frame, _c_str, -1, buffer, newLength) <= 0) {
		if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
			throw NormalizeFailedException();
		}
		assert(false);
	}
	return result;
}


String String::padLeft(int length, wchar_t paddingChar) const {
	assert("Negative length" && 0 <= length);

	const int oldLength = this->length();
	const int newLength = std::max(oldLength, length);
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	if (oldLength < length) {
		const int paddingLength = length - oldLength;
		wmemset(buffer, paddingChar, paddingLength);
		buffer += paddingLength;
	}
	wmemcpy(buffer, _c_str, oldLength + 1);
	return result;
}


String String::padRight(int length, wchar_t paddingChar) const {
	assert("Negative length" && 0 <= length);

	const int oldLength = this->length();
	const int newLength = max(oldLength, length);
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	wmemcpy(buffer, _c_str, oldLength);
	if (oldLength < length) {
		const int paddingLength = length - oldLength;
		wmemset(buffer + oldLength, paddingChar, paddingLength);
	}
	buffer[newLength] = L'\0';
	return result;
}


String String::refer(StringRange value) {
	return String(-value.length(), value.c_str());
}


String String::refer(const wchar_t* value, int length) {
	assert("Null value" && value);
	assert("Negative length" && 0 <= length);
	assert("value not terminated" && value[length] == L'\0');

	return String(-length, value);
}


String String::replace(wchar_t oldValue, wchar_t newValue) const {
	if (empty()) {
		return String();
	}
	const int newLength = length();
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	const wchar_t* j = _c_str;
	for (auto i = buffer, end = buffer + newLength; i != end; ++i, ++j) {
		*i = *j == oldValue ? newValue : *j;
	}
	buffer[newLength] = L'\0';
	return result;
}


String String::replace(StringRange oldValue, StringRange newValue) const {
	assert("Empty oldValue" && !oldValue.empty());

	// 最悪ケ?スの長さを計算してメモリ割り当てしておく
	const int oldValueLength = oldValue.length();
	const int newValueLength = newValue.length();
	const int oldLength = length();
	const int newLength = oldLength + max(0, newValueLength - oldValueLength) * (oldLength / oldValueLength);
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String temp(newLength, buffer);
	int startOld = 0;
	int startNew = 0;
	int pos = 0;
	while (0 <= (pos = indexOf(oldValue, startOld))) {
		int distance = pos - startOld;
		wmemcpy(buffer + startNew, _c_str + startOld, distance);
		wmemcpy(buffer + startNew + distance, newValue.c_str(), newValueLength);
		startOld += distance + oldValueLength;
		startNew += distance + newValueLength;
	}
	const int rest =  oldLength - startOld;
	wmemcpy(buffer + startNew, _c_str + startOld, rest);
	const int trueLength = startNew + rest;
	assert(trueLength <= newLength);
	buffer[trueLength] = L'\0';
	return String(temp.c_str(), trueLength); // ?分に割り当てしてるかもしれんので作り直す
}


std::vector<String, std::allocator<String> > String::split(StringRange separators) const {
	if (separators.empty()) {
		return splitOnWhiteSpace(_c_str, length());
	}
	vector<String> result;
	const int thisLength = length();
	int oldPos = 0;
	int pos = 0;
	for (;;) {
		if ((pos = indexOfAny(separators, oldPos)) == -1) {
			const int elementLength = thisLength - oldPos;
			result.push_back(String(_c_str + oldPos, elementLength));
			break;
		} else {
			const int elementLength = pos - oldPos;
			result.push_back(String(_c_str + oldPos, elementLength));
			oldPos = pos + 1;
		}
	}
	return result;
}


std::vector<String, std::allocator<String> > String::split(StringRangeArray separators) const {
	assert("Empty separators" && !separators.empty());
	vector<String> result;
	const int thisLength = length();
	int pos = 0;
	for (;;) {
		int newPos = thisLength;
		int separatorLength = 0;
		for (auto i = 0, end = separators.length(); i != end; ++i) {
			auto separator = separators[i];
			const int tempPos = indexOf(separator, pos);
			if (0 <= tempPos && tempPos < newPos) {
				newPos = tempPos;
				separatorLength = separator.length();
			}
		}
		const int elementLength = newPos - pos;
		result.push_back(String(_c_str + pos, elementLength));
		if (newPos == thisLength) {
			break;
		}
		pos = newPos + separatorLength;
	}
	return result;
}


bool String::startsWith(StringRange value, bool ignoreCase) const {
	const int valueLength = value.length();
	return (ignoreCase ? compareOrdinalIgnoreCase(_c_str, value.c_str(), valueLength) : wcsncmp(_c_str, value.c_str(), valueLength)) == 0;
}


bool String::startsWith(StringRange value, const Locale& locale, String::CompareOptions options) const {
	assert("Invalid options" && isValidStringCompareOptionsForSearch(options));

	// Vista 以?ならば FindNLSString が使えて楽なのだが・・
	const int valueLength = value.length();
	if (!valueLength) {
		return true;
	}
	const wchar_t* compareEnd = skipElement(_c_str, _c_str + length(), String::refer(value).lengthInLetter());
	if (!compareEnd) {
		return false;
	}
	return CompareStringW(locale.id(), options, _c_str, compareEnd - _c_str, value.c_str(), valueLength) == CSTR_EQUAL;
}


String String::substring(int beginIndex) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= length());

	const int newLength = length() - beginIndex;
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	wmemcpy(buffer, _c_str + beginIndex, newLength + 1);
	return result;
}


String String::substring(int beginIndex, int length) const {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= this->length());
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= this->length() - beginIndex);

	if (length == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[length + 1];
	String result(length, buffer);
	wmemcpy(buffer, _c_str + beginIndex, length);
	buffer[length] = L'\0';
	return result;
}


String String::toLower() const {
	return toLower(Locale::invariant());
}


String String::toLower(const Locale& locale) const {
	if (empty()) {
		return String();
	}
	const int thisLength = length();
	wchar_t* buffer = new wchar_t[thisLength + 1];
	String result(thisLength, buffer);
	verify(LCMapStringW(locale.id(), LCMAP_LOWERCASE, _c_str, thisLength + 1, buffer, thisLength + 1) == thisLength + 1);
	return result;
}


String String::toUpper() const {
	return toUpper(Locale::invariant());
}


String String::toUpper(const Locale& locale) const {
	if (empty()) {
		return String();
	}
	const int thisLength = length();
	wchar_t* buffer = new wchar_t[thisLength + 1];
	String result(thisLength, buffer);
	verify(LCMapStringW(locale.id(), LCMAP_UPPERCASE, _c_str, thisLength + 1, buffer, thisLength + 1) == thisLength + 1);
	return result;
}


String String::trim(StringRange trimChars) const {
	const wchar_t* begin = _c_str;
	const wchar_t* end = _c_str + length();
	if (trimChars.empty()) {
		while (begin != end) {
			if (!Unicode::isWhiteSpace(*begin)) {
				break;
			}
			++begin;
		}
		while (begin <= --end) {
			if (!Unicode::isWhiteSpace(*end)) {
				break;
			}
		}
		++end;
	} else {
		while (begin != end) {
			bool find = false;
			for (const wchar_t* i = trimChars.c_str(); *i; ++i) {
				if (*begin == *i) {
					find = true;
					break;
				}
			}
			if (!find) {
				break;
			}
			++begin;
		}
		while (begin <= --end) {
			bool find = false;
			for (const wchar_t* i = trimChars.c_str(); *i; ++i) {
				if (*end == *i) {
					find = true;
					break;
				}
			}
			if (!find) {
				break;
			}
		}
		++end;
	}
	return String(begin, end - begin);
}


String::operator wstring() const {
	return wstring(_c_str, length());
}


wchar_t String::operator[] (int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= length());
	return _c_str[index];
}


String& String::operator+=(const String& rhs) {
	*this = *this + rhs;
	return *this;
}


String& String::operator+=(const wstring& rhs) {
	*this = *this + rhs;
	return *this;
}


String& String::operator+=(const wchar_t* rhs) {
	*this = *this + rhs;
	return *this;
}


String& String::operator+=(wchar_t* rhs) {
	*this = *this + rhs;
	return *this;
}


String& String::operator+=(wchar_t rhs) {
	*this = *this + rhs;
	return *this;
}


String operator+(const String& lhs, const String& rhs) {
	const int lLength = lhs.length();
	const int rLength = rhs.length();
	const int newLength = lLength + rLength;
	if (newLength == 0) {
		return String();
	}
	wchar_t* buffer = new wchar_t[newLength + 1];
	String result(newLength, buffer);
	wmemcpy(buffer, lhs.c_str(), lLength);
	wmemcpy(buffer + lLength, rhs.c_str(), rLength + 1);
	return result;
}


String operator+(const String& lhs, const wstring& rhs) {
	return lhs + String::refer(rhs.c_str(), rhs.length());
}


String operator+(const String& lhs, const wchar_t* rhs) {
	return lhs + String::refer(rhs);
}


String operator+(const String& lhs, wchar_t* rhs) {
	return lhs + String::refer(rhs);
}


String operator+(const String& lhs, wchar_t rhs) {
	wchar_t buffer[] = {rhs, L'\0'};
	return lhs + String::refer(buffer, 1);
}


String operator+(const wstring& lhs, const String& rhs) {
	return String::refer(lhs.c_str(), lhs.length()) + rhs;
}


String operator+(const wchar_t* lhs, const String& rhs) {
	return String::refer(lhs) + rhs;
}


String operator+(wchar_t* lhs, const String& rhs) {
	return String::refer(lhs) + rhs;
}


String operator+(wchar_t lhs, const String& rhs) {
	wchar_t buffer[] = {lhs, L'\0'};
	return String::refer(buffer, 1) + rhs;
}


bool operator==(StringRange lhs, StringRange rhs) {
	return String::equals(lhs, rhs);
}


bool operator!=(StringRange lhs, StringRange rhs) {
	return !String::equals(lhs, rhs);
}


bool operator< (StringRange lhs, StringRange rhs) {
	return String::compare(lhs, rhs) < 0;
}


bool operator<=(StringRange lhs, StringRange rhs) {
	return String::compare(lhs, rhs) <= 0;
}


bool operator> (StringRange lhs, StringRange rhs) {
	return String::compare(lhs, rhs) > 0;
}


bool operator>=(StringRange lhs, StringRange rhs) {
	return String::compare(lhs, rhs) >= 0;
}


String::String(int length, const wchar_t* value) : _c_str(value), _length(length) {
}



}