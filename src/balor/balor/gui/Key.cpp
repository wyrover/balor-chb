#include "Key.hpp"

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace gui {


static_assert(Key::a                  == 'A', "Invalid enum value");
static_assert(Key::add                == VK_ADD, "Invalid enum value");
static_assert(Key::alt                == VK_MENU, "Invalid enum value");
static_assert(Key::apps               == VK_APPS, "Invalid enum value");
static_assert(Key::attn               == VK_ATTN, "Invalid enum value");
static_assert(Key::b                  == 'B', "Invalid enum value");
static_assert(Key::back               == VK_BACK, "Invalid enum value");
static_assert(Key::browserBack        == VK_BROWSER_BACK, "Invalid enum value");
static_assert(Key::browserFavorites   == VK_BROWSER_FAVORITES, "Invalid enum value");
static_assert(Key::browserForward     == VK_BROWSER_FORWARD, "Invalid enum value");
static_assert(Key::browserHome        == VK_BROWSER_HOME, "Invalid enum value");
static_assert(Key::browserRefresh     == VK_BROWSER_REFRESH, "Invalid enum value");
static_assert(Key::browserSearch      == VK_BROWSER_SEARCH, "Invalid enum value");
static_assert(Key::browserStop        == VK_BROWSER_STOP, "Invalid enum value");
static_assert(Key::c                  == 'C', "Invalid enum value");
static_assert(Key::cancel             == VK_CANCEL, "Invalid enum value");
static_assert(Key::capital            == VK_CAPITAL, "Invalid enum value");
static_assert(Key::capsLock           == VK_CAPITAL, "Invalid enum value");
static_assert(Key::clear              == VK_CLEAR, "Invalid enum value");
static_assert(Key::crsel              == VK_CRSEL, "Invalid enum value");
static_assert(Key::ctrl               == VK_CONTROL, "Invalid enum value");
static_assert(Key::d                  == 'D', "Invalid enum value");
static_assert(Key::d0                 == '0', "Invalid enum value");
static_assert(Key::d1                 == '1', "Invalid enum value");
static_assert(Key::d2                 == '2', "Invalid enum value");
static_assert(Key::d3                 == '3', "Invalid enum value");
static_assert(Key::d4                 == '4', "Invalid enum value");
static_assert(Key::d5                 == '5', "Invalid enum value");
static_assert(Key::d6                 == '6', "Invalid enum value");
static_assert(Key::d7                 == '7', "Invalid enum value");
static_assert(Key::d8                 == '8', "Invalid enum value");
static_assert(Key::d9                 == '9', "Invalid enum value");
static_assert(Key::decimal            == '.', "Invalid enum value");
static_assert(Key::deleteKey          == VK_DELETE, "Invalid enum value");
static_assert(Key::divide             == VK_DIVIDE, "Invalid enum value");
static_assert(Key::down               == VK_DOWN, "Invalid enum value");
static_assert(Key::e                  == 'E', "Invalid enum value");
static_assert(Key::end                == VK_END, "Invalid enum value");
static_assert(Key::enter              == VK_RETURN, "Invalid enum value");
static_assert(Key::eraseEof           == VK_EREOF, "Invalid enum value");
static_assert(Key::escape             == VK_ESCAPE, "Invalid enum value");
static_assert(Key::execute            == VK_EXECUTE, "Invalid enum value");
static_assert(Key::exsel              == VK_EXSEL, "Invalid enum value");
static_assert(Key::f                  == 'F', "Invalid enum value");
static_assert(Key::f1                 == VK_F1, "Invalid enum value");
static_assert(Key::f10                == VK_F10, "Invalid enum value");
static_assert(Key::f11                == VK_F11, "Invalid enum value");
static_assert(Key::f12                == VK_F12, "Invalid enum value");
static_assert(Key::f13                == VK_F13, "Invalid enum value");
static_assert(Key::f14                == VK_F14, "Invalid enum value");
static_assert(Key::f15                == VK_F15, "Invalid enum value");
static_assert(Key::f16                == VK_F16, "Invalid enum value");
static_assert(Key::f17                == VK_F17, "Invalid enum value");
static_assert(Key::f18                == VK_F18, "Invalid enum value");
static_assert(Key::f19                == VK_F19, "Invalid enum value");
static_assert(Key::f2                 == VK_F2, "Invalid enum value");
static_assert(Key::f20                == VK_F20, "Invalid enum value");
static_assert(Key::f21                == VK_F21, "Invalid enum value");
static_assert(Key::f22                == VK_F22, "Invalid enum value");
static_assert(Key::f23                == VK_F23, "Invalid enum value");
static_assert(Key::f24                == VK_F24, "Invalid enum value");
static_assert(Key::f3                 == VK_F3, "Invalid enum value");
static_assert(Key::f4                 == VK_F4, "Invalid enum value");
static_assert(Key::f5                 == VK_F5, "Invalid enum value");
static_assert(Key::f6                 == VK_F6, "Invalid enum value");
static_assert(Key::f7                 == VK_F7, "Invalid enum value");
static_assert(Key::f8                 == VK_F8, "Invalid enum value");
static_assert(Key::f9                 == VK_F9, "Invalid enum value");
static_assert(Key::finalMode          == VK_FINAL, "Invalid enum value");
static_assert(Key::g                  == 'G', "Invalid enum value");
static_assert(Key::h                  == 'H', "Invalid enum value");
static_assert(Key::hangulMode         == VK_HANGUL, "Invalid enum value");
static_assert(Key::hanjaMode          == VK_HANJA, "Invalid enum value");
static_assert(Key::help               == VK_HELP, "Invalid enum value");
static_assert(Key::home               == VK_HOME, "Invalid enum value");
static_assert(Key::i                  == 'I', "Invalid enum value");
static_assert(Key::ImeAccept          == VK_ACCEPT, "Invalid enum value");
static_assert(Key::ImeConvert         == VK_CONVERT, "Invalid enum value");
static_assert(Key::ImeModeChange      == VK_MODECHANGE, "Invalid enum value");
static_assert(Key::ImeNonconvert      == VK_NONCONVERT, "Invalid enum value");
static_assert(Key::insert             == VK_INSERT, "Invalid enum value");
static_assert(Key::j                  == 'J', "Invalid enum value");
static_assert(Key::junjaMode          == VK_JUNJA, "Invalid enum value");
static_assert(Key::k                  == 'K', "Invalid enum value");
static_assert(Key::kanaMode           == VK_KANA, "Invalid enum value");
static_assert(Key::kanjiMode          == VK_KANJI, "Invalid enum value");
static_assert(Key::l                  == 'L', "Invalid enum value");
static_assert(Key::launchApplication1 == VK_LAUNCH_APP1, "Invalid enum value");
static_assert(Key::launchApplication2 == VK_LAUNCH_APP2, "Invalid enum value");
static_assert(Key::launchMail         == VK_LAUNCH_MAIL, "Invalid enum value");
static_assert(Key::lButton            == VK_LBUTTON, "Invalid enum value");
static_assert(Key::lControl           == VK_LCONTROL, "Invalid enum value");
static_assert(Key::left               == VK_LEFT, "Invalid enum value");
static_assert(Key::lineFeed           == 0x0a, "Invalid enum value");
static_assert(Key::lAlt               == VK_LMENU, "Invalid enum value");
static_assert(Key::lShift             == VK_LSHIFT, "Invalid enum value");
static_assert(Key::lWin               == VK_LWIN, "Invalid enum value");
static_assert(Key::m                  == 'M', "Invalid enum value");
static_assert(Key::mButton            == VK_MBUTTON, "Invalid enum value");
static_assert(Key::mediaNextTrack     == VK_MEDIA_NEXT_TRACK, "Invalid enum value");
static_assert(Key::mediaPlayPause     == VK_MEDIA_PLAY_PAUSE, "Invalid enum value");
static_assert(Key::mediaPreviousTrack == VK_MEDIA_PREV_TRACK, "Invalid enum value");
static_assert(Key::mediaStop          == VK_MEDIA_STOP, "Invalid enum value");
static_assert(Key::multiply           == VK_MULTIPLY, "Invalid enum value");
static_assert(Key::n                  == 'N', "Invalid enum value");
static_assert(Key::next               == VK_NEXT, "Invalid enum value");
static_assert(Key::noName             == VK_NONAME, "Invalid enum value");
static_assert(Key::none               == 0x00, "Invalid enum value");
static_assert(Key::numLock            == VK_NUMLOCK, "Invalid enum value");
static_assert(Key::numPad0            == VK_NUMPAD0, "Invalid enum value");
static_assert(Key::numPad1            == VK_NUMPAD1, "Invalid enum value");
static_assert(Key::numPad2            == VK_NUMPAD2, "Invalid enum value");
static_assert(Key::numPad3            == VK_NUMPAD3, "Invalid enum value");
static_assert(Key::numPad4            == VK_NUMPAD4, "Invalid enum value");
static_assert(Key::numPad5            == VK_NUMPAD5, "Invalid enum value");
static_assert(Key::numPad6            == VK_NUMPAD6, "Invalid enum value");
static_assert(Key::numPad7            == VK_NUMPAD7, "Invalid enum value");
static_assert(Key::numPad8            == VK_NUMPAD8, "Invalid enum value");
static_assert(Key::numPad9            == VK_NUMPAD9, "Invalid enum value");
static_assert(Key::o                  == 'O', "Invalid enum value");
static_assert(Key::oem1               == VK_OEM_1, "Invalid enum value");
static_assert(Key::oem102             == VK_OEM_102, "Invalid enum value");
static_assert(Key::oem2               == VK_OEM_2, "Invalid enum value");
static_assert(Key::oem3               == VK_OEM_3, "Invalid enum value");
static_assert(Key::oem4               == VK_OEM_4, "Invalid enum value");
static_assert(Key::oem5               == VK_OEM_5, "Invalid enum value");
static_assert(Key::oem6               == VK_OEM_6, "Invalid enum value");
static_assert(Key::oem7               == VK_OEM_7, "Invalid enum value");
static_assert(Key::oem8               == VK_OEM_8, "Invalid enum value");
static_assert(Key::oemBackslash       == VK_OEM_102, "Invalid enum value");
static_assert(Key::oemClear           == VK_OEM_CLEAR, "Invalid enum value");
static_assert(Key::oemCloseBrackets   == VK_OEM_6, "Invalid enum value");
static_assert(Key::oemcomma           == VK_OEM_COMMA, "Invalid enum value");
static_assert(Key::oemMinus           == VK_OEM_MINUS, "Invalid enum value");
static_assert(Key::oemOpenBrackets    == VK_OEM_4, "Invalid enum value");
static_assert(Key::oemPeriod          == VK_OEM_PERIOD, "Invalid enum value");
static_assert(Key::oemPipe            == VK_OEM_5, "Invalid enum value");
static_assert(Key::oemPlus            == VK_OEM_PLUS, "Invalid enum value");
static_assert(Key::oemQuestion        == VK_OEM_2, "Invalid enum value");
static_assert(Key::oemQuotes          == VK_OEM_7, "Invalid enum value");
static_assert(Key::oemSemicolon       == VK_OEM_1, "Invalid enum value");
static_assert(Key::oemTilde           == VK_OEM_3, "Invalid enum value");
static_assert(Key::p                  == 'P', "Invalid enum value");
static_assert(Key::pa1                == VK_PA1, "Invalid enum value");
static_assert(Key::packet             == VK_PACKET, "Invalid enum value");
static_assert(Key::pageDown           == VK_NEXT, "Invalid enum value");
static_assert(Key::pageUp             == VK_PRIOR, "Invalid enum value");
static_assert(Key::pause              == VK_PAUSE, "Invalid enum value");
static_assert(Key::play               == VK_PLAY, "Invalid enum value");
static_assert(Key::print              == VK_PRINT, "Invalid enum value");
static_assert(Key::printScreen        == VK_SNAPSHOT, "Invalid enum value");
static_assert(Key::prior              == VK_PRIOR, "Invalid enum value");
static_assert(Key::processKey         == VK_PROCESSKEY, "Invalid enum value");
static_assert(Key::q                  == 'Q', "Invalid enum value");
static_assert(Key::r                  == 'R', "Invalid enum value");
static_assert(Key::rButton            == VK_RBUTTON, "Invalid enum value");
static_assert(Key::rControl           == VK_RCONTROL, "Invalid enum value");
static_assert(Key::right              == VK_RIGHT, "Invalid enum value");
static_assert(Key::rAlt               == VK_RMENU, "Invalid enum value");
static_assert(Key::rShift             == VK_RSHIFT, "Invalid enum value");
static_assert(Key::rWin               == VK_RWIN, "Invalid enum value");
static_assert(Key::s                  == 'S', "Invalid enum value");
static_assert(Key::scroll             == VK_SCROLL, "Invalid enum value");
static_assert(Key::select             == VK_SELECT, "Invalid enum value");
static_assert(Key::selectMedia        == VK_LAUNCH_MEDIA_SELECT, "Invalid enum value");
static_assert(Key::separator          == VK_SEPARATOR, "Invalid enum value");
static_assert(Key::shift              == VK_SHIFT, "Invalid enum value");
static_assert(Key::sleep              == VK_SLEEP, "Invalid enum value");
static_assert(Key::snapshot           == VK_SNAPSHOT, "Invalid enum value");
static_assert(Key::space              == VK_SPACE, "Invalid enum value");
static_assert(Key::subtract           == VK_SUBTRACT, "Invalid enum value");
static_assert(Key::t                  == 'T', "Invalid enum value");
static_assert(Key::tab                == VK_TAB, "Invalid enum value");
static_assert(Key::u                  == 'U', "Invalid enum value");
static_assert(Key::up                 == VK_UP, "Invalid enum value");
static_assert(Key::v                  == 'V', "Invalid enum value");
static_assert(Key::volumeDown         == VK_VOLUME_DOWN, "Invalid enum value");
static_assert(Key::volumeMute         == VK_VOLUME_MUTE, "Invalid enum value");
static_assert(Key::volumeUp           == VK_VOLUME_UP, "Invalid enum value");
static_assert(Key::w                  == 'W', "Invalid enum value");
static_assert(Key::x                  == 'X', "Invalid enum value");
static_assert(Key::xButton1           == VK_XBUTTON1, "Invalid enum value");
static_assert(Key::xButton2           == VK_XBUTTON2, "Invalid enum value");
static_assert(Key::y                  == 'Y', "Invalid enum value");
static_assert(Key::z                  == 'Z', "Invalid enum value");
static_assert(Key::zoom               == VK_ZOOM, "Invalid enum value");


bool Key::_validate(Key value) {
	return none <= value && value <= oemClear;
}


bool Key::Modifier::_validate(Key::Modifier value) {
	return (value & ~mask) == 0;
}


bool Key::isLocked(Key key) {
	assert("Can't lock key" && (key == Key::insert || key == Key::numLock || key == Key::capsLock || key == Key::scroll));
	int state = GetKeyState(key);
	return (state & 0x0001) != 0;
}


bool Key::isPushed(Key key) {
	assert("Invalid key" && Key::_validate(key));
	return GetKeyState(key) < 0;
}


bool Key::isPushedAsync(Key key) {
	assert("Invalid key" && Key::_validate(key));
	return GetAsyncKeyState(key) < 0;
}


int Key::repeatDelay() {
	int result = 0;
	verify(SystemParametersInfoW(SPI_GETKEYBOARDDELAY , 0, &result, 0));
	return result;
}


int Key::repeatSpeed() {
	DWORD result = 0;
	verify(SystemParametersInfoW(SPI_GETKEYBOARDSPEED , 0, &result, 0));
	return result;
}


wchar_t Key::toChar(Key key) {
	assert("Invalid key" && Key::_validate(key));
	return LOWORD(MapVirtualKeyW(key, MAPVK_VK_TO_CHAR));
}


	}
}