#pragma once

#include <balor/locale/Locale.hpp>


namespace balor {



/**
 * ébíËé¿ëïÅB
 */
class Unicode {
public:
	typedef ::balor::locale::Locale Locale;

public:
	static bool isWhiteSpace(wchar_t c);
	static wchar_t toLower(wchar_t c, const Locale& locale = Locale::current());
	static wchar_t toUpper(wchar_t c, const Locale& locale = Locale::current());
};



}