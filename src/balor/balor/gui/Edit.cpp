#include "Edit.hpp"

#include <algorithm>
#include <utility>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Clipboard.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/gui/ScrollBar.hpp>
#include <balor/system/System.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <Imm.h>
#pragma comment(lib, "imm32.lib")


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;
using namespace balor::graphics;
using namespace balor::system;


namespace {
static_assert(Edit::Align::left   == ES_LEFT, "Invalid enum value");
static_assert(Edit::Align::center == ES_CENTER, "Invalid enum value");
static_assert(Edit::Align::right  == ES_RIGHT, "Invalid enum value");

static_assert(Edit::ImeMode::hiragana		== (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE), "Invalid enum value");
static_assert(Edit::ImeMode::katakana		== (IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_KATAKANA), "Invalid enum value");
static_assert(Edit::ImeMode::katakanaHalf	== (IME_CMODE_NATIVE | IME_CMODE_KATAKANA), "Invalid enum value");
static_assert(Edit::ImeMode::alpha			== IME_CMODE_FULLSHAPE, "Invalid enum value");
static_assert(Edit::ImeMode::alphaHalf		== 0, "Invalid enum value");

static_assert(Edit::ImeConvert::general				== IME_SMODE_PHRASEPREDICT, "Invalid enum value");
static_assert(Edit::ImeConvert::personAndPlaceName	== IME_SMODE_PLAURALCLAUSE, "Invalid enum value");
static_assert(Edit::ImeConvert::informal			== IME_SMODE_CONVERSATION, "Invalid enum value");
static_assert(Edit::ImeConvert::none				== IME_SMODE_NONE, "Invalid enum value");

static_assert(Edit::Options::multiline      == ES_MULTILINE, "Invalid enum value");
static_assert(Edit::Options::noHScroll      == ES_AUTOHSCROLL, "Invalid enum value");
static_assert(Edit::Options::noVScroll      == ES_AUTOVSCROLL, "Invalid enum value");
static_assert(Edit::Options::alwaysSelected == ES_NOHIDESEL, "Invalid enum value");

const int optionsMask = ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL;
const int optionsXorMask = ES_AUTOHSCROLL | ES_AUTOVSCROLL;


HBITMAP const defaultCaret = reinterpret_cast<HBITMAP>(0xffffffff);


struct ImeContext {
	typedef Edit::ImeConvert ImeConvert;
	typedef Edit::ImeMode ImeMode;

	ImeContext(HWND control) : _control(control), _handle(nullptr) {
		if (available()) {
			_handle = ImmGetContext(control);
		}
	}
	~ImeContext() {
		if (_handle) {
			verify(ImmReleaseContext(_control, _handle));
		}
	}

	static bool available() {
		HKL layout = GetKeyboardLayout(0);
		LCID locale = LOWORD(layout);
		return locale == MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN); // 入力ロケールが 0x411（ja-JP）の場合のみ IME 処理を行う。
	}
	bool enabled() const {
		return *this != nullptr;
	}
	void enabled(bool value) {
		if (value == enabled()) {
			return;
		}
		if (value) {
			verify(ImmAssociateContextEx(_control, nullptr, IACE_DEFAULT));
			_handle = ImmGetContext(_control);
		} else {
			verify(ImmReleaseContext(_control, _handle));
			_handle = nullptr;
			verify(ImmAssociateContext(_control, nullptr));
		}
	}
	bool open() const {
		return *this && ImmGetOpenStatus(*this) != 0;
	}
	void open(bool value) {
		if (*this && value != open()) {
			verify(ImmSetOpenStatus(*this, value ? TRUE : FALSE));
		}
	}
	static const int modeMask = IME_CMODE_NATIVE | IME_CMODE_KATAKANA | IME_CMODE_FULLSHAPE;
	void getConversion(ImeMode& mode, ImeConvert& convert) {
		mode = ImeMode::default;
		convert = ImeConvert::default;
		if (!available()) {
			return;
		}
		if (!enabled()) {
			mode = ImeMode::disabled;
			return;
		}
		DWORD conversion = 0;
		DWORD sentence = 0;
		verify(ImmGetConversionStatus(*this, &conversion, &sentence));
		mode = static_cast<ImeMode>(conversion & modeMask);
		convert = static_cast<ImeConvert>(sentence);
		if (!open()) {
			mode = ImeMode::off;
		}
	}
	void setConversion(ImeMode mode, ImeConvert convert) {
		if (!available()) {
			return;
		}
		if (mode == ImeMode::disabled) {
			enabled(false);
			return;
		}
		enabled(true);
		if (!*this) {
			return;
		}
		open(true);
		DWORD oldConversion = 0;
		DWORD oldSentence   = 0;
		verify(ImmGetConversionStatus(*this, &oldConversion, &oldSentence));
		DWORD newConversion = oldConversion;
		DWORD newSentence = oldSentence;
		if (mode != ImeMode::default && mode != ImeMode::on) {
			newConversion = (oldConversion & ~modeMask) | mode;
		}
		if (convert != ImeConvert::default) {
			newSentence = convert;
		}
		if (oldConversion != newConversion || oldSentence != newSentence) {
			verify(ImmSetConversionStatus(*this, newConversion, newSentence));
		}
		if (mode == ImeMode::off) {
			open(false);
		}
	}

	operator HIMC() const { return _handle; }

private:
	HWND _control;
	HIMC _handle;
};
} // namespace



bool Edit::Align::_validate(Align value) {
	return left <= value && value <= right;
}



bool Edit::ImeConvert::_validate(ImeConvert value) {
	switch (value) {
		case ImeConvert::default			:
		case ImeConvert::general			:
		case ImeConvert::personAndPlaceName	:
		case ImeConvert::informal			:
		case ImeConvert::none				: return true;
		default : return false;
	}
}


bool Edit::ImeMode::_validate(ImeMode value) {
	switch (value) {
		case ImeMode::default		:
		case ImeMode::on			:
		case ImeMode::off			:
		case ImeMode::disabled		:
		case ImeMode::hiragana		:
		case ImeMode::katakana		:
		case ImeMode::katakanaHalf	:
		case ImeMode::alpha			:
		case ImeMode::alphaHalf		: return true;
		default : return false;
	}
}


bool Edit::ImeStatus::_validate(ImeStatus value) {
	return (value & ~(
		noKeyPress | cancelOnGotFocus | completeOnLostFocus
		)) == 0;
}


bool Edit::Options::_validate(Options value) {
	return (value & ~optionsMask) == 0;
}



Edit::Paste::Paste(Edit& sender) : Edit::Event(sender), _cancel(false) {}


bool Edit::Paste::cancel() const { return _cancel; }
void Edit::Paste::cancel(bool value) { _cancel = value; }
String Edit::Paste::text() const { return Clipboard::getText(); }



Edit::TextChange::TextChange(Edit& sender) : Edit::Event(sender) , _cancel(false) {}


bool Edit::TextChange::cancel() const { return _cancel; }
void Edit::TextChange::cancel(bool value) { _cancel = value; }
Edit::Selection Edit::TextChange::newSelection() const { return sender().selection(); }
void Edit::TextChange::newSelection(const Edit::Selection& value) { sender().selection(value); }
String Edit::TextChange::newText() const { return sender().text(); }
void Edit::TextChange::newText(StringRange value) {
	auto selection = newSelection();
	sender().text(value);
	newSelection(selection);
}
Edit::Selection Edit::TextChange::oldSelection() const { return sender()._oldSelection; }
String Edit::TextChange::oldText() const { return sender()._oldText; }



void Edit::_initialize() {
	_caret = defaultCaret;
	_caretSize = Size(1, 12);
	_caretIndex = 0;
	_caretVisible = true;
	_imeMode = ImeMode::default;
	_imeModeBackup = ImeMode::default;
	_imeModeChanging = false;
	_imeConvert = ImeConvert::default;
	_imeConvertBackup = ImeConvert::default;
	_imeFixed = false;
	_margin = Padding(2, 0, 2, 0);
	_preferredCharCount = Size(10, 1);
	_textChanging = false;
	_textColor = Color::controlText();
	_oldSelection = Selection(0, 0);
}


Edit::Edit() : _owned(true) {
	_initialize();
}


Edit::Edit(Edit&& value, bool checkSlicing)
	: Control(move(value))
	, _owned(move(value._owned))
	, _caret(move(value._caret))
	, _caretSize(move(value._caretSize))
	, _caretIndex(move(value._caretIndex))
	, _caretVisible(move(value._caretVisible))
	, _imeMode(move(value._imeMode))
	, _imeModeBackup(move(value._imeModeBackup))
	, _imeModeChanging(move(value._imeModeChanging))
	, _imeConvert(move(value._imeConvert))
	, _imeConvertBackup(move(value._imeConvertBackup))
	, _imeFixed(move(value._imeFixed))
	, _margin(move(value._margin))
	, _preferredCharCount(move(value._preferredCharCount))
	, _textChanging(move(value._textChanging))
	, _textColor(move(value._textColor))
	, _oldText(move(value._oldText))
	, _oldSelection(move(value._oldSelection))
	, _onPaste(move(value._onPaste))
	, _onTextChange(move(value._onTextChange))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Edit::Edit(Control& parent, int x, int y, int width, int height, Edit::Options options) : _owned(true) {
	assert("Invalid Edit::Options" && Options::_validate(options));
	_initialize();
	attachHandle(CreateWindowExW(0, L"EDIT", nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | (options ^ optionsXorMask)
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	edge(Control::Edge::client); // 初期を none 以外で始めると変更した時に margin や sizeFromClient がおかしくなる。
	margin(margin());
}


Edit::Edit(Control& parent, int x, int y, int width, int height, int lineLength, int lineCount, Edit::Options options) : _owned(true) {
	_initialize();
	*this = Edit(parent, x, y, width, height, options);
	preferredCharCount(lineLength, lineCount);
	size(getPreferredSize(width, height));
}


Edit::Edit(HWND handle) : _owned(false) {
	_initialize();
	attachHandle(handle);
}



Edit::~Edit() {
	if (IsWindow(_handle)) {
		if (_owned) {
			destroyHandle();
		} else {
			_handle.procedure(_defaultProcedure);
			_handle.control(nullptr);
			_handle = Handle();
		}
	}
}


Edit& Edit::operator=(Edit&& value) {
	if (&value != this) {
		this->~Edit();
		new (this) Edit(move(value));
	}
	return *this;
}


Edit::Align Edit::align() const {
	return static_cast<Align>(_handle.style() & (ES_LEFT | ES_CENTER | ES_RIGHT));
}


void Edit::align(Edit::Align value) {
	assert("Invalid Edit::Align" && Align::_validate(value));
	if (value != align()) {
		auto style = toFlag(_handle.style()).set(ES_LEFT | ES_CENTER | ES_RIGHT, false);
		_handle.style(style | value);
		_handle.updateStyle();
	}
}


bool Edit::canUndo() const {
	return SendMessageW(handle(), EM_CANUNDO, 0, 0) != 0;
}


int Edit::caretBlinkTime() {
	return GetCaretBlinkTime();
}


bool Edit::caretVisible() const {
	return _caretVisible;
}


void Edit::caretVisible(bool value) {
	if (value != caretVisible()) {
		if (focused()) {
			if (value) {
				verify(ShowCaret(*this));
			} else {
				verify(HideCaret(*this));
			}
		}
		_caretVisible = value;
	}
}


int Edit::caretIndex() const {
	if (!focused()) {
		return _caretIndex;
	}
	POINT point;
	if (!GetCaretPos(&point)) { // フォーカスが無くとも一応取得はできるが、最後に編集したエディットのものと思われる
		return -1;
	}
	return getIndexAt(point);
}


void Edit::caretIndex(int value) {
	assert("carentIndex out of range" && 0 <= value);
	assert("carentIndex out of range" && value <= length());
	selection(value, 0);
}


void Edit::changeCaret(const Size& size, bool grayed) {
	HBITMAP caret = grayed ? reinterpret_cast<HBITMAP>(1) : nullptr;
	if (caret != _caret || size != _caretSize) {
		if (focused()) {
				verify(CreateCaret(*this, caret, size.width, size.height));
				verify(ShowCaret(*this));
		}
		_caret = caret;
		_caretSize = size;
	}
}


void Edit::changeCaret(int width, int height, bool grayed) {
	changeCaret(Size(width, height), grayed);
}


void Edit::changeCaret(HBITMAP value) {
	if (value != _caret) {
		if (focused()) {
			verify(CreateCaret(*this, value, 0, 0));
			verify(ShowCaret(*this));
		}
		_caret = value;
	}
}


void Edit::clearUndo() {
	SendMessageW(handle(), EM_EMPTYUNDOBUFFER, 0, 0);
}


void Edit::copy() const {
	SendMessageW(handle(), WM_COPY, 0, 0);
}


String Edit::cueBanner() const {
	assert("This function needs visual style" && System::visualStyleEnabled());
	wchar_t buffer[2048];
	buffer[0] = L'\0';
	verify(SendMessageW(handle(), EM_GETCUEBANNER, (WPARAM)buffer, sizeof(buffer) / sizeof(buffer[0])));
	return buffer;
}


void Edit::cueBanner(StringRange value) {
	assert("This function needs visual style" && System::visualStyleEnabled());
	assert("Can't set cueBanner to multiline edit" && !multiline());
	verify(SendMessageW(handle(), EM_SETCUEBANNER, FALSE, (LPARAM)value.c_str()));
}


void Edit::cut() {
	SendMessageW(handle(), WM_CUT, 0, 0);
}


int Edit::defaultCaretWidth() {
	DWORD result = 0;
	verify(SystemParametersInfoW(SPI_GETCARETWIDTH, 0, &result, 0));
	return static_cast<int>(result);
}


Control::Edge Edit::edge() const {
	return _handle.edge();
}


void Edit::edge(Control::Edge value) {
	_handle.edge(value);
}


int Edit::firstVisibleLine() const {
	return SendMessageW(handle(), EM_GETFIRSTVISIBLELINE, 0, 0);
}


int Edit::getIndexAt(const Point& position) const {
	return getIndexAt(position.x, position.y);
}


int Edit::getIndexAt(int x, int y) const {
	//assert("position out of range" && 0 <= x);
	//assert("position out of range" && x <= SHRT_MAX);
	//assert("position out of range" && 0 <= y);
	//assert("position out of range" && y <= SHRT_MAX);
	int index = SendMessageW(handle(), EM_CHARFROMPOS, 0, MAKELPARAM(x, y));
	index = LOWORD(index);
	if (index == 65535) {
		return -1;
	}
	assert(0 <= index && index <= length());
	return index;
}


int Edit::getIndexFromLine(int line) const {
	assert("line out of range" && 0 <= line);
	assert("line out of range" && line < lineCount());
	return SendMessageW(handle(), EM_LINEINDEX, line, 0);
}


int Edit::getLineFromIndex(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= length());
	return SendMessageW(handle(), EM_LINEFROMCHAR, index, 0);
}


int Edit::getLineLength(int line) const {
	assert("line out of range" && 0 <= line);
	assert("line out of range" && line < lineCount());
	return SendMessageW(handle(), EM_LINELENGTH, getIndexFromLine(line), 0);
}


String Edit::getLineText(int line) const {
	assert("line out of range" && 0 <= line);
	assert("line out of range" && line < lineCount());
	return text().substring(getIndexFromLine(line), getLineLength(line));
}


Point Edit::getPositionFromIndex(int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < length());
	auto result = SendMessageW(handle(), EM_POSFROMCHAR, index, 0);
	return Point(LOWORD(result), HIWORD(result));
}


Size Edit::getPreferredSize(int width, int height) const {
	if (width && height) {
		return Size(width, height);
	}
	Graphics graphics(*this);
	auto font = graphics.font(this->font());
	auto size = graphics.measureText(L"0");
	graphics.font(font);
	size.width  *= preferredCharCount().width ;
	size.height *= preferredCharCount().height;
	size += margin().size();
	size = sizeFromClientSize(size);
	return Size(width  ? width  : size.width
			   ,height ? height : size.height);
}


bool Edit::hScrollBar() const {
	return _handle.hasStyle(WS_HSCROLL);
}


void Edit::hScrollBar(bool value) {
	assert("Can't set hScrollBar to singleline edit" && multiline());
	auto textBackup = text();
	text(L"");
	_handle.setStyle(WS_HSCROLL, value);
	text(textBackup);
}


Edit::ImeConvert Edit::imeConvert() const {
	return _imeConvert;
}


void Edit::imeConvert(Edit::ImeConvert value) {
	assert("Invalid Edit::ImeConvert" && ImeConvert::_validate(value));
	_imeConvert = value;
	imeMode(imeMode());
}


bool Edit::imeFixed() const {
	return _imeFixed;
}


void Edit::imeFixed(bool value) {
	_imeFixed = value;
}


Edit::ImeMode Edit::imeMode() const {
	return _imeMode;
}


void Edit::imeMode(Edit::ImeMode value) {
	assert("Invalid Edit::ImeMode" && ImeMode::_validate(value));
	if (_imeMode == value && value == ImeMode::default) {
		return;
	}
	_imeMode = value;
	if (!focused()) {
		return;
	}
	if (!_imeModeChanging) {
		_imeModeChanging = true;
		scopeExit([&] () {
			_imeModeChanging = false;
		});
		ImeContext context(*this);
		context.setConversion(value, imeConvert());
	}
}


Edit::ImeStatus Edit::imeStatus() const {
	return static_cast<ImeStatus>(SendMessageW(handle(), EM_GETIMESTATUS, EMSIS_COMPOSITIONSTRING, 0));
}


void Edit::imeStatus(Edit::ImeStatus value) {
	assert("Invalid Edit::ImeStatus" && ImeStatus::_validate(value));
	SendMessageW(handle(), EM_SETIMESTATUS, EMSIS_COMPOSITIONSTRING, value);
}


int Edit::length() const {
	return GetWindowTextLengthW(*this);
}


int Edit::lineCount() const {
	return SendMessageW(handle(), EM_GETLINECOUNT, 0, 0);
}


bool Edit::lowercaseOnly() const {
	return _handle.hasStyle(ES_LOWERCASE);
}


void Edit::lowercaseOnly(bool value) {
	if (value != lowercaseOnly()) {
		auto style = toFlag(_handle.style());
		if (value) {
			style.set(ES_UPPERCASE, false);
		}
		style.set(ES_LOWERCASE, value);
		_handle.style(style);
		_handle.updateStyle();
	}
}


Padding Edit::margin() const {
	return _margin; // 実際のマージンは文字サイズの関係であく隙間を考慮したものになるので必ずしも一致しない。
}


void Edit::margin(const Padding& value) {
	assert("Negative margin" && 0 <= value.left);
	assert("Negative margin" && 0 <= value.top);
	assert("Negative margin" && 0 <= value.right);
	assert("Negative margin" && 0 <= value.bottom);
	assert("Can't set top margin to singleline edit" && !(!multiline() && value.top != 0));
	assert("Can't set bottom margin to singleline edit" && !(!multiline() && value.bottom != 0));
	_margin = value;
	if (multiline()) {
		auto size = this->size();
		RECT rect = {_margin.left, _margin.top, size.width - _margin.right, size.height - _margin.bottom};
		if (vScrollBar()) { rect.right  -= ScrollBar::defaultVerticalWidth(); }
		if (hScrollBar()) { rect.bottom -= ScrollBar::defaultHorizontalHeight(); }
		SendMessageW(handle(), EM_SETRECT, 0, (LPARAM)&rect);
	} else {
		SendMessageW(handle(), EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(_margin.left, _margin.right));
	}
}


void Edit::margin(int left, int top, int right, int bottom) {
	margin(Padding(left, top, right, bottom));
}


int Edit::maxLength() const {
	return SendMessageW(handle(), EM_GETLIMITTEXT, 0, 0);
}


void Edit::maxLength(int value) {
	assert("Negative maxLength" && 0 <= value);
	SendMessageW(handle(), EM_SETLIMITTEXT, value, 0);
}


bool Edit::modified() const {
	return SendMessageW(handle(), EM_GETMODIFY, 0, 0) != 0;
}


void Edit::modified(bool value) {
	if (value != modified()) {
		SendMessageW(handle(), EM_SETMODIFY, value ? TRUE : FALSE, 0);
	}
}


bool Edit::multiline() const {
	return toFlag(_handle.style())[ES_MULTILINE];
}


bool Edit::numberOnly() const {
	return _handle.hasStyle(ES_NUMBER);
}


void Edit::numberOnly(bool value) {
	_handle.setStyle(ES_NUMBER, value);
}


Listener<Edit::Paste&>& Edit::onPaste() { return _onPaste; }
Listener<Edit::TextChange&>& Edit::onTextChange() { return _onTextChange; }


Edit::Options Edit::options() const {
	return static_cast<Options>((_handle.style() & optionsMask) ^ optionsXorMask);
}


wchar_t Edit::passwordChar() const {
	return static_cast<wchar_t>(SendMessageW(handle(), EM_GETPASSWORDCHAR, 0, 0));
}


void Edit::passwordChar(wchar_t value) {
	if (value != passwordChar()) {
		SendMessageW(handle(), EM_SETPASSWORDCHAR, value, 0);
		invalidate();
	}
}


void Edit::paste() {
	SendMessageW(handle(), WM_PASTE, 0, 0);
}


Size Edit::preferredCharCount() const {
	return _preferredCharCount;
}


void Edit::preferredCharCount(const Size& value) {
	assert("Negative preferredCharCount" && 0 <= value.width);
	assert("Negative preferredCharCount" && 0 <= value.height);
	_preferredCharCount = value;
}


void Edit::preferredCharCount(int lineLength, int lineCount) {
	preferredCharCount(Size(lineLength, lineCount));
}


bool Edit::readOnly() const {
	return _handle.hasStyle(ES_READONLY);
}


void Edit::readOnly(bool value) {
	if (value != readOnly()) {
		SendMessageW(handle(), EM_SETREADONLY, value ? TRUE : FALSE, 0);
	}
}


void Edit::scale(const Scaler& scaler) {
	Control::scale(scaler);
	if (scalable()) {
		margin(scaler.scale(margin()));
	}
}


void Edit::scroll(int x, int y) {
	SendMessageW(handle(), EM_LINESCROLL, x, y);
}


void Edit::scrollInto(int x, int y) {
	auto oldY = firstVisibleLine();
	auto deltaY = y - oldY;
	auto deltaX = -length();
	scroll(deltaX, deltaY);
	scroll(x, 0);
}


void Edit::scrollLineDown() {
	SendMessageW(handle(), EM_SCROLL, SB_LINEDOWN, 0);
}


void Edit::scrollLineUp() {
	SendMessageW(handle(), EM_SCROLL, SB_LINEUP, 0);
}


void Edit::scrollPageDown() {
	SendMessageW(handle(), EM_SCROLL, SB_PAGEDOWN, 0);
}


void Edit::scrollPageUp() {
	SendMessageW(handle(), EM_SCROLL, SB_PAGEUP, 0);
}


void Edit::scrollToCaret() {
	SendMessageW(handle(), EM_SCROLLCARET, 0, 0);
}


String Edit::selectedText() const {
	int start = 0;
	int end = 0;
	SendMessageW(handle(), EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
	assert(0 <= start);
	assert(start <= end);
	return text().substring(start, end - start);
}


void Edit::selectedText(StringRange value) {
	SendMessageW(handle(), EM_REPLACESEL, TRUE, (LPARAM)value.c_str());
}


Edit::Selection Edit::selection() const {
	int start = 0;
	int end = 0;
	SendMessageW(handle(), EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
	assert(0 <= start);
	assert(start <= end);
	return Selection(start, end - start);
}


void Edit::selection(const Edit::Selection& value) {
	assert("Negative start" && 0 <= value.index);
	assert("Negative length" && 0 <= value.length);
	assert("length orverflow" && (__int64)value.index + (__int64)value.length <= INT_MAX);
	auto length = this->length();
	int start = min(value.index, length);
	int end = min(value.index + value.length, length);
	SendMessageW(handle(), EM_SETSEL, start, end);
}


void Edit::selection(int index, int length) {
	selection(Selection(index, length));
}


Size Edit::sizeFromClientSize(const Size& clientSize) const {
	auto size = Control::sizeFromClientSize(clientSize);
	if (vScrollBar()) { size.width  += ScrollBar::defaultVerticalWidth(); }
	if (hScrollBar()) { size.height += ScrollBar::defaultHorizontalHeight(); }
	return size;
}


void Edit::tabWidth(int value) {
	assert("Invalid tabWidth" && 0 < value);
	unsigned int width = value * 4;
	verify(SendMessageW(handle(), EM_SETTABSTOPS, 1, (LPARAM)&width));
}


void Edit::text(StringRange value) {
	_oldText.clear();
	textToBuffer(_oldText);
	_oldSelection = selection();
	Control::text(value);
}


Color Edit::textColor() const {
	return _textColor;
}


void Edit::textColor(const Color& value) {
	if (value != _textColor) {
		_textColor = value;
		invalidate();
	}
}


void Edit::undo() {
	SendMessageW(handle(), EM_UNDO, 0, 0);
}


bool Edit::uppercaseOnly() const {
	return _handle.hasStyle(ES_UPPERCASE);
}


void Edit::uppercaseOnly(bool value) {
	if (value != uppercaseOnly()) {
		auto style = toFlag(_handle.style());
		if (value) {
			style.set(ES_LOWERCASE, false);
		}
		style.set(ES_UPPERCASE, value);
		_handle.style(style);
		_handle.updateStyle();
	}
}


bool Edit::vScrollBar() const {
	return _handle.hasStyle(WS_VSCROLL);
}


void Edit::vScrollBar(bool value) {
	assert("Can't set vScrollBar to singleline edit" && multiline());
	auto textBackup = text();
	text(L"");
	_handle.setStyle(WS_VSCROLL, value);
	text(textBackup);
}


void Edit::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_CHAR :
		case WM_CUT : {
			_oldText.clear();
			textToBuffer(_oldText);
			_oldSelection = selection();
			Control::processMessage(msg);
		} break;
		case WM_DESTROY : {
			if (focused() && _caret != defaultCaret) {
				verify(DestroyCaret());
			}
			Control::processMessage(msg);
		} break;
		case WM_KEYDOWN : {
			if (imeFixed()) {
				imeMode(imeMode());
			}
			Control::processMessage(msg);
		} break;
		case WM_KILLFOCUS : {
			if (imeMode() != ImeMode::default) {
				if (!_imeModeChanging) {
					_imeModeChanging = true;
					scopeExit([&] () {
						_imeModeChanging = false;
					});
					ImeContext context(*this);
					context.setConversion(_imeModeBackup, _imeConvertBackup);
				}
			}
			POINT point;
			if (!GetCaretPos(&point)) { // フォーカスが無くとも一応取得はできるが、最後に編集したエディットのものと思われる
				_caretIndex = -1;
			} else {
				_caretIndex = getIndexAt(point); // フォーカスがなくても取れるように保存しておく。EDIT もどこかに保存しているはずだが取得方法がわからない。
			}
			if (_caret != defaultCaret) {
				verify(DestroyCaret());
			}
			Control::processMessage(msg);
		} break;
		case WM_PASTE : {
			Paste event(*this);
			onPaste()(event);
			if (event.cancel()) {
				return;
			}
			_oldText.clear();
			textToBuffer(_oldText);
			_oldSelection = selection();
			Control::processMessage(msg);
		} break;
		case wmReflect + WM_COMMAND : {
			switch (HIWORD(msg.wparam)) {
				//case EN_CHANGE : {
				//} break;
				case EN_UPDATE : {
					if (!_textChanging) { // 無限再帰防止
						_textChanging = true;
						scopeExit([&] () {
							_textChanging = false;
						});
						TextChange event(*this);
						onTextChange()(event);
						if (event.cancel()) { // undo() では保存のタイミングが不明瞭で使えなかった。
							verify(SetWindowTextW(handle(), _oldText.begin())); // バックアップの復元でバックアップを取らないように text() は使わない。
							selection(_oldSelection);
							return;
						}
					}
					_caretIndex = 0; // フォーカスが無い状態で文字列を更新したならキャレット位置は初期化される。
				} break;
			}
			Control::processMessage(msg);
		} break;
		case wmReflect + WM_CTLCOLOREDIT :
		case wmReflect + WM_CTLCOLORSTATIC : {
			Graphics graphics((HDC)msg.wparam);
			auto textColor = this->textColor();
			HBRUSH brush = this->brush();
			if (brush || textColor != Color::controlText()) {
				graphics.backTransparent(true);
				graphics.textColor(textColor);
				graphics.brushOrigin(brushOrigin());
				if (!brush) {
					brush = (!enabled() || readOnly()) ? Brush::control() : Brush::window();
				}
				msg.result = (LRESULT)brush;
			} else {
				Control::processMessage(msg);
			}
		} break;
		case WM_SETFOCUS : {
			Control::processMessage(msg);
			ImeContext context(*this);
			context.getConversion(_imeModeBackup, _imeConvertBackup);
			if (imeMode() != ImeMode::default) {
				imeMode(imeMode());
			}
			if (!_caretVisible) {
				verify(HideCaret(*this));
			}
			if (_caret != defaultCaret) {
				verify(CreateCaret(*this, _caret, _caretSize.width, _caretSize.height));
				verify(ShowCaret(*this));
			}
		} break;
		case WM_SIZE : {
			Control::processMessage(msg);
			margin(margin()); // マージンはサイズが変わるたびにリセットされるので復元する。
		} break;
		case EM_UNDO : {
			Control::processMessage(msg);
			auto selection = this->selection();
			_caretIndex = selection.index + selection.length; // 完璧に追跡するのはおそらく無理だ・・
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}