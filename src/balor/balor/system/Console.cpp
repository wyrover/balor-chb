#include "Console.hpp"

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <string>

#include <balor/locale/Charset.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace system {

using namespace balor::locale;


void Console::close() {
	verify(FreeConsole());
}


void Console::open() {
	verify(AllocConsole()); // ìØä˙Ç‡Ç»Ç…Ç‡Ç»Ç¢Ç™é∏îsÇµÇƒÇ‡ÇªÇ±Ç‹Ç≈ê[çèÇ…ÇÕÇ»ÇÈÇ‹Ç¢
	verify(freopen("CONOUT$", "w", stdout));
	verify(freopen("CONOUT$", "w", stderr));
}


void Console::write(StringRange text) {
	write(Charset::default().encode(text));
}


void Console::write(ByteStringRange text) {
	verify(0 <= printf(text.c_str()));
}


void Console::writeLine(StringRange text) {
	writeLine(Charset::default().encode(text));
}


void Console::writeLine(ByteStringRange text) {
	verify(0 <= printf("%s\n", text.c_str()));
}



	}
}