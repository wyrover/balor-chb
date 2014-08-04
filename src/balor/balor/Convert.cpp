#include "Convert.hpp"

#include <cerrno>
#include <cfloat>
#include <clocale>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <regex>

#include <balor/test/verify.hpp>
#include <balor/Singleton.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {

using std::move;


namespace {
bool isValidBase(int base) {
	return base == 2 || base == 8 || base == 10 || base == 16;
}


bool isValidFormatBase(int base) {
	return base == 8 || base == 10 || base == 16;
}


bool isValidFormat(StringRange format) {
	return std::regex_match(format.c_str(), std::wregex(L"[-+ #]*0?[0-9]*(\\.[0-9]*)?"));
}


class CLocale {
	friend Singleton<CLocale>;

	CLocale() : locale(_create_locale(LC_NUMERIC, "C")) {
	}
	~CLocale() {
		_free_locale(locale);
	}

public:
	_locale_t& get() { return locale; }

private:
	_locale_t locale;
};


_locale_t& getCLocale() {
	return Singleton<CLocale>::get().get();
}


template<int BufferSize, int TypeNameSize>
void makeFormatString(wchar_t (&buffer)[BufferSize], const wchar_t (&typeName)[TypeNameSize], StringRange format, int base) {
	assert("format too long" && (1 + format.length() + TypeNameSize) < BufferSize);

	const int formatLength = format.length();
	buffer[0] = L'%';
	String::refer(format).copyTo(0, buffer, 1, formatLength);
	String::literal(typeName).copyTo(0, buffer, 1 + formatLength, TypeNameSize);
	if (base == 16) {
		buffer[formatLength + TypeNameSize - 1] = L'x';
	} else if (base == 8) {
		buffer[formatLength + TypeNameSize - 1] = L'o';
	}
}
} // namespace


namespace Convert {


template<> String to<String>(const long& value, int base);
template<> String to<String>(const unsigned long& value, int base);
template<> String to<String>(const long& value, StringRange format, int base);
template<> String to<String>(const unsigned long& value, StringRange format, int base);


template<> __int64 to<__int64>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	if (base == 10) {
		wchar_t* end;
		__int64 result = _wcstoi64(value.c_str(), &end, base);
		if (value == end) {
			throw StringFormatException();
		}
		if (result == _I64_MIN || result == _I64_MAX) { // errnoの取得はかなり命令数が多いので事前にチェックする
			if (errno == ERANGE) {
				_set_errno(0);
				throw OverflowException();
			}
		}
		return static_cast<__int64>(result);
	} else {
		return static_cast<__int64>(to<unsigned __int64>(value, base));
	}
}


template<> char to<char>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	if (base == 10) {
		wchar_t* end;
		long result = wcstol(value.c_str(), &end, base);
		if (value == end) {
			throw StringFormatException();
		}
		if (result < CHAR_MIN || CHAR_MAX < result) {
			_set_errno(0);
			throw OverflowException();
		}
		return static_cast<char>(result);
	} else {
		return static_cast<char>(to<unsigned char>(value, base));
	}
}


#pragma warning(push)
#pragma warning(disable : 4100) // 'base' : 引数は関数の本体部で 1 度も参照されません。
template<> double to<double>(const StringRange& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t* end;
	double result = _wcstod_l(value.c_str(), &end, getCLocale()); // "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	if (value == end) {
		throw StringFormatException();
	}
	if (result == -HUGE_VAL || result == +HUGE_VAL) { // errnoの取得はかなり命令数が多いので事前にチェックする
		if (errno == ERANGE) {
			_set_errno(0);
			throw OverflowException();
		}
	}
	return static_cast<double>(result);
}
#pragma warning(pop)


#pragma warning(push)
#pragma warning(disable : 4100) // 'base' : 引数は関数の本体部で 1 度も参照されません。
template<> float to<float>(const StringRange& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t* end;
	double result = _wcstod_l(value.c_str(), &end, getCLocale()); // "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	if (value == end) {
		throw StringFormatException();
	}
	if (result < -FLT_MAX || FLT_MAX < result) { // アンダーフローは無視するようだ
		_set_errno(0);
		throw OverflowException();
	}
	return static_cast<float>(result);
}
#pragma warning(pop)


template<> int to<int>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	if (base == 10) {
		wchar_t* end;
		long result = wcstol(value.c_str(), &end, base);
		if (value == end) {
			throw StringFormatException();
		}
		if (result == INT_MAX || result == INT_MIN) { // errnoの取得はかなり命令数が多いので事前にチェックする
			if (errno == ERANGE) {
				_set_errno(0);
				throw OverflowException();
			}
		}
		return static_cast<int>(result);
	} else {
		return static_cast<int>(to<unsigned int>(value, base));
	}
}


template<> long to<long>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	if (base == 10) {
		wchar_t* end;
		long result = wcstol(value.c_str(), &end, base);
		if (value == end) {
			throw StringFormatException();
		}
		if (result == LONG_MAX || result == LONG_MIN) { // errnoの取得はかなり命令数が多いので事前にチェックする
			if (errno == ERANGE) {
				_set_errno(0);
				throw OverflowException();
			}
		}
		return static_cast<long>(result);
	} else {
		return static_cast<long>(to<unsigned long>(value, base));
	}
}


template<> short to<short>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	if (base == 10) {
		wchar_t* end;
		long result = wcstol(value.c_str(), &end, base);
		if (value == end) {
			throw StringFormatException();
		}
		if (result < SHRT_MIN || SHRT_MAX < result) {
			_set_errno(0);
			throw OverflowException();
		}
		return static_cast<short>(result);
	} else {
		return static_cast<short>(to<unsigned short>(value, base));
	}
}


template<> unsigned __int64 to<unsigned __int64>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t* end;
	unsigned __int64 result = _wcstoui64(value.c_str(), &end, base);
	if (value == end) {
		throw StringFormatException();
	}
	if (result == _UI64_MAX) { // errnoの取得はかなり命令数が多いので事前にチェックする
		if (errno == ERANGE) {
			_set_errno(0);
			throw OverflowException();
		}
	}
	return static_cast<unsigned __int64>(result);
}


template<> unsigned char to<unsigned char>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t* end;
	unsigned long result = wcstoul(value.c_str(), &end, base);
	if (value == end) {
		throw StringFormatException();
	}
	if (UCHAR_MAX < result) {
		_set_errno(0);
		throw OverflowException();
	}
	return static_cast<unsigned char>(result);
}


template<> unsigned int to<unsigned int>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t* end;
	unsigned long result = wcstoul(value.c_str(), &end, base);
	if (value == end) {
		throw StringFormatException();
	}
	if (result == ULONG_MAX) { // errnoの取得はかなり命令数が多いので事前にチェックする
		if (errno == ERANGE) {
			_set_errno(0);
			throw OverflowException();
		}
	}
	return static_cast<unsigned int>(result);
}


template<> unsigned long to<unsigned long>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t* end;
	unsigned long result = wcstoul(value.c_str(), &end, base);
	if (value == end) {
		throw StringFormatException();
	}
	if (result == ULONG_MAX) { // errnoの取得はかなり命令数が多いので事前にチェックする
		if (errno == ERANGE) {
			_set_errno(0);
			throw OverflowException();
		}
	}
	return static_cast<unsigned long>(result);
}


template<> unsigned short to<unsigned short>(const StringRange& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t* end;
	unsigned long result = wcstoul(value.c_str(), &end, base);
	if (value == end) {
		throw StringFormatException();
	}
	if (USHRT_MAX < result) {
		_set_errno(0);
		throw OverflowException();
	}
	return static_cast<unsigned short>(result);
}


template<> String to<String>(const __int64& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[66];
	verify(!_i64tow_s(value, buffer, sizeof(buffer) / sizeof(buffer[0]), base));
	return String(buffer);
}


template<> String to<String>(const char& value, int base) {
	return to<String>(base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned char)value), base);
}


#pragma warning(push)
#pragma warning(disable : 4100) // 'base' : 引数は関数の本体部で 1 度も参照されません。
template<> String to<String>(const double& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t buffer[_CVTBUFSIZE];
	// "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.16g", getCLocale(), value));
	return String(buffer);
}


template<> String to<String>(const float& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t buffer[_CVTBUFSIZE];
	// "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.8g", getCLocale(), value));
	return String(buffer);
	//return to<String>(static_cast<double>(value), base);
}
#pragma warning(pop)


template<> String to<String>(const int& value, int base) {
	return to<String>(static_cast<long>(value), base);
}


template<> String to<String>(const long& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[34];
	verify(!_ltow_s(value, buffer, base));
	return String(buffer);
}


template<> String to<String>(const short& value, int base) {
	return to<String>(base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned short)value), base);
}


template<> String to<String>(const unsigned __int64& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[66];
	verify(!_ui64tow_s(value, buffer, sizeof(buffer) / sizeof(buffer[0]), base));
	return String(buffer);
}


template<> String to<String>(const unsigned char& value, int base) {
	return to<String>(static_cast<unsigned long>(value), base);
}


template<> String to<String>(const unsigned int& value, int base) {
	return to<String>(static_cast<unsigned long>(value), base);
}


template<> String to<String>(const unsigned long& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[34];
	verify(!_ultow_s(value, buffer, base));
	return String(buffer);
}


template<> String to<String>(const unsigned short& value, int base) {
	return to<String>(static_cast<unsigned long>(value), base);
}


template<> String to<String>(const bool& value, int ) {
	return value ? String::literal(L"true") : String::literal(L"false");
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const __int64& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[66];
	verify(!_i64tow_s(value, buffer, sizeof(buffer) / sizeof(buffer[0]), base));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const char& value, int base) {
	to<StringBuffer>(stringBuffer, base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned char)value), base);
}


#pragma warning(push)
#pragma warning(disable : 4100) // 'base' : 引数は関数の本体部で 1 度も参照されません。
template<> void to<StringBuffer>(StringBuffer& stringBuffer, const double& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t buffer[_CVTBUFSIZE];
	// "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.16g", getCLocale(), value));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const float& value, int base) {
	assert("Invalid base" && base == 10);

	wchar_t buffer[_CVTBUFSIZE];
	// "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%.8g", getCLocale(), value));
	stringBuffer += buffer;
}
#pragma warning(pop)


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const int& value, int base) {
	to<StringBuffer>(stringBuffer, static_cast<long>(value), base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const long& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[34];
	verify(!_ltow_s(value, buffer, base));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const short& value, int base) {
	to<StringBuffer>(stringBuffer, base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned short)value), base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned __int64& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[66];
	verify(!_ui64tow_s(value, buffer, sizeof(buffer) / sizeof(buffer[0]), base));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned char& value, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned int& value, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned long& value, int base) {
	assert("Invalid base" && isValidBase(base));

	wchar_t buffer[34];
	verify(!_ultow_s(value, buffer, base));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned short& value, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const bool& value, int ) {
	stringBuffer += value ? String::literal(L"true") : String::literal(L"false");
}


template<> String to<String>(const __int64& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const char& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const double& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const float& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const int& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const long& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const short& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const unsigned __int64& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const unsigned char& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const unsigned int& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const unsigned long& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> String to<String>(const unsigned short& value, StringRange format, int base) {
	StringBuffer buffer;
	to<StringBuffer>(buffer, value, format, base);
	return String(move(buffer));
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const __int64& value, StringRange format, int base) {
	assert("Invalid format" && isValidFormat(format));
	assert("Invalid base" && isValidFormatBase(base));

	wchar_t formatBuffer[128];
	makeFormatString(formatBuffer, L"I64d", format, base);

	wchar_t buffer[66];
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), formatBuffer, getCLocale(), value));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const char& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned char)value), format, base);
}


#pragma warning(push)
#pragma warning(disable : 4100) // 'base' : 引数は関数の本体部で 1 度も参照されません。
template<> void to<StringBuffer>(StringBuffer& stringBuffer, const double& value, StringRange format, int base) {
	assert("Invalid format" && std::regex_match(format.c_str(), std::wregex(L"[-+ #]*0?[0-9]*(\\.[0-9]*)?[eEfgG]?")));
	assert("Invalid base" && base == 10);

	wchar_t formatBuffer[128];
	assert("format too long" && (1 + format.length() + 1) < (sizeof(formatBuffer) / sizeof(formatBuffer[0])));
	const int formatLength = format.length();
	formatBuffer[0] = L'%';
	String::refer(format).copyTo(0, formatBuffer, 1, formatLength + 1);
	if (format.empty() || String::literal(L"eEfgG").indexOf(format.c_str()[formatLength - 1]) == -1) {
		formatBuffer[formatLength + 1] = L'g';
		formatBuffer[formatLength + 2] = L'\0';
	}

	wchar_t buffer[66];
	// "German_Germany"ロケール等で小数点の文字コードが変化しないように"C"ロケールを使用する
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), formatBuffer, getCLocale(), value));
	stringBuffer += buffer;
}
#pragma warning(pop)


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const float& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, static_cast<double>(value), format, base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const int& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, static_cast<long>(value), format, base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const long& value, StringRange format, int base) {
	assert("Invalid format" && isValidFormat(format));
	assert("Invalid base" && isValidFormatBase(base));

	wchar_t formatBuffer[128];
	makeFormatString(formatBuffer, L"ld", format, base);

	wchar_t buffer[66];
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), formatBuffer, getCLocale(), value));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const short& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, base == 10 ? static_cast<long>(value) : static_cast<long>((unsigned short)value), format, base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned __int64& value, StringRange format, int base) {
	assert("Invalid format" && isValidFormat(format));
	assert("Invalid base" && isValidFormatBase(base));

	wchar_t formatBuffer[128];
	makeFormatString(formatBuffer, L"I64u", format, base);

	wchar_t buffer[66];
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), formatBuffer, getCLocale(), value));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned char& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), format, base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned int& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), format, base);
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned long& value, StringRange format, int base) {
	assert("Invalid format" && isValidFormat(format));
	assert("Invalid base" && isValidFormatBase(base));

	wchar_t formatBuffer[128];
	makeFormatString(formatBuffer, L"lu", format, base);

	wchar_t buffer[66];
	verify(0 <= _swprintf_l(buffer, sizeof(buffer) / sizeof(buffer[0]), formatBuffer, getCLocale(), value));
	stringBuffer += buffer;
}


template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned short& value, StringRange format, int base) {
	to<StringBuffer>(stringBuffer, static_cast<unsigned long>(value), format, base);
}



}


}