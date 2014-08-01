#include "MessageBox.hpp"

#include <balor/graphics/Font.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>


namespace balor {
	namespace gui {

using namespace balor::graphics;


namespace {
static_assert(MsgBox::Buttons::ok                == MB_OK, "Invalid enum value");
static_assert(MsgBox::Buttons::okCancel          == MB_OKCANCEL, "Invalid enum value");
static_assert(MsgBox::Buttons::abortRetryIgnore  == MB_ABORTRETRYIGNORE, "Invalid enum value");
static_assert(MsgBox::Buttons::yesNoCancel       == MB_YESNOCANCEL, "Invalid enum value");
static_assert(MsgBox::Buttons::yesNo             == MB_YESNO, "Invalid enum value");
static_assert(MsgBox::Buttons::retryCancel       == MB_RETRYCANCEL, "Invalid enum value");
static_assert(MsgBox::Buttons::cancelTryContinue == MB_CANCELTRYCONTINUE, "Invalid enum value");

static_assert(MsgBox::Icon::hand        == MB_ICONHAND, "Invalid enum value");
static_assert(MsgBox::Icon::question    == MB_ICONQUESTION, "Invalid enum value");
static_assert(MsgBox::Icon::exclamation == MB_ICONEXCLAMATION, "Invalid enum value");
static_assert(MsgBox::Icon::asterisk    == MB_ICONASTERISK, "Invalid enum value");
static_assert(MsgBox::Icon::stop        == MB_ICONSTOP, "Invalid enum value");
static_assert(MsgBox::Icon::error       == MB_ICONERROR, "Invalid enum value");
static_assert(MsgBox::Icon::warning     == MB_ICONWARNING, "Invalid enum value");
static_assert(MsgBox::Icon::information == MB_ICONINFORMATION, "Invalid enum value");

static_assert(MsgBox::Default::button1 == MB_DEFBUTTON1, "Invalid enum value");
static_assert(MsgBox::Default::button2 == MB_DEFBUTTON2, "Invalid enum value");
static_assert(MsgBox::Default::button3 == MB_DEFBUTTON3, "Invalid enum value");

static_assert(MsgBox::Options::topMost             == MB_TOPMOST, "Invalid enum value");
static_assert(MsgBox::Options::serviceNotification == MB_SERVICE_NOTIFICATION, "Invalid enum value");
static_assert(MsgBox::Options::defaultDesktopOnly  == MB_DEFAULT_DESKTOP_ONLY, "Invalid enum value");
static_assert(MsgBox::Options::rightAlign          == MB_RIGHT, "Invalid enum value");

static_assert(MsgBox::Result::ok        == IDOK, "Invalid enum value");
static_assert(MsgBox::Result::cancel    == IDCANCEL, "Invalid enum value");
static_assert(MsgBox::Result::abort     == IDABORT, "Invalid enum value");
static_assert(MsgBox::Result::retry     == IDRETRY, "Invalid enum value");
static_assert(MsgBox::Result::ignore    == IDIGNORE, "Invalid enum value");
static_assert(MsgBox::Result::yes       == IDYES, "Invalid enum value");
static_assert(MsgBox::Result::no        == IDNO, "Invalid enum value");
static_assert(MsgBox::Result::tryAgain  == IDTRYAGAIN, "Invalid enum value");
static_assert(MsgBox::Result::continues == IDCONTINUE, "Invalid enum value");


MsgBox::Result showMessageBox(HWND owner, StringRange text, StringRange caption, MsgBox::Buttons buttons, MsgBox::Icon icon, MsgBox::Default defaultButton, MsgBox::Options options) {
	assert("Invalid buttons" && MsgBox::Buttons::_validate(buttons));
	assert("Invalid icon" && MsgBox::Icon::_validate(icon));
	assert("Invalid defaultButton" && MsgBox::Default::_validate(defaultButton));
	assert("Invalid options" && MsgBox::Options::_validate(options));

	int result = MessageBoxExW(owner, text.c_str(), caption.c_str(), buttons | icon | defaultButton | options | MB_TASKMODAL, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	assert(result); // メッセージボックスの表示に失敗した
	return static_cast<MsgBox::Result>(result);
}
} // namespace


bool MsgBox::Buttons::_validate(Buttons value) {
	return ok <= value && value <= cancelTryContinue;
}


bool MsgBox::Icon::_validate(Icon value) {
	switch (value) {
		case none        :
		case hand        :
		case question    :
		case exclamation :
		case asterisk    : return true;
		//case stop        :
		//case error       :
		//case warning     :
		//case information :
		default : return false;
	}
}


bool MsgBox::Default::_validate(Default value) {
	switch (value) {
		case button1 :
		case button2 :
		case button3 : return true;
		default : return false;
	}
}


bool MsgBox::Options::_validate(Options value) {
	return (value & ~(topMost
					| serviceNotification
					| defaultDesktopOnly
					| rightAlign)) == 0;
}


Font MsgBox::font() {
	NONCLIENTMETRICSW  metrics;
	ZeroMemory(&metrics, sizeof(metrics));
	metrics.cbSize = sizeof(metrics);
	verify(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS , sizeof(metrics), &metrics, 0));
	return Font::fromLogFont(metrics.lfMessageFont);
}


MsgBox::Result MsgBox::show(StringRange text, StringRange caption, MsgBox::Buttons buttons, MsgBox::Icon icon, MsgBox::Default defaultButton, MsgBox::Options options) {
	return showMessageBox(nullptr, text, caption, buttons, icon, defaultButton, options);
}


MsgBox::Result MsgBox::show(HWND owner, StringRange text, StringRange caption, MsgBox::Buttons buttons, MsgBox::Icon icon, MsgBox::Default defaultButton, MsgBox::Options options) {
	return showMessageBox(owner, text, caption, buttons, icon, defaultButton, options);
}



	}
}