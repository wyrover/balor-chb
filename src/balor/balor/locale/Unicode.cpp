#include "Unicode.hpp"

#include <cstring>

#include <balor/locale/Charset.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {


using namespace balor::locale;


namespace {
} // namespace


bool Unicode::isWhiteSpace(wchar_t c) {
	if ((c == L' ') || (c >= L'\x0009' && c <= L'\x000d') || c == L'\x00a0' || c == L'\x0085' || c == L'\x2028' || c == L'\x2029') {
		return true;
	}
	WORD type = 0;
	GetStringTypeW(CT_CTYPE2, &c, 1, &type);
	return type == C2_WHITESPACE;
}


wchar_t Unicode::toLower(wchar_t c, const Locale& locale) {
	const wchar_t src[] = {c, '\0'};
	wchar_t dst[2] = {'\0'};
	verify(LCMapStringW(locale.id(), LCMAP_LOWERCASE, src, 2, dst, 2) == 2);
	return dst[0];
}


wchar_t Unicode::toUpper(wchar_t c, const Locale& locale) {
	const wchar_t src[] = {c, '\0'};
	wchar_t dst[2] = {'\0'};
	verify(LCMapStringW(locale.id(), LCMAP_UPPERCASE, src, 2, dst, 2) == 2);
	return dst[0];
}



}