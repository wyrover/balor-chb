#include "Frame.hpp"

#include <algorithm>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/gui/Button.hpp>
#include <balor/gui/MenuBar.hpp>
#include <balor/gui/Monitor.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>

#include <WindowsX.h>


namespace balor {
	namespace gui {


using std::max;
using std::min;
using std::move;
using namespace balor::graphics;



bool Frame::Style::_validate(Style value) {
	return none <= value && value <= sizableToolWindow;
}



Frame::Close::Close(Frame& sender, CloseReason closeReason)
	: Frame::Event(sender), _closeReason(closeReason) {
}


Frame::CloseReason Frame::Close::closeReason() const { return _closeReason; }



Frame::Closing::Closing(Frame& sender, CloseReason closeReason)
	: Close(sender, closeReason), _cancel(false) {
}


bool Frame::Closing::cancel() const { return _cancel; }
void Frame::Closing::cancel(bool value) { _cancel = value; }



Frame::HelpButtonClick::HelpButtonClick(Frame& sender)
	: Frame::Event(sender), _cancel(false) {
}


bool Frame::HelpButtonClick::cancel() const { return _cancel; }
void Frame::HelpButtonClick::cancel(bool value) { _cancel = value; }



Frame::Move::Move(Frame& sender, const Point& position)
	: Frame::Event(sender), _position(position) {
}


const Point& Frame::Move::position() const { return _position; }



Frame::Moving::Moving(Frame& sender, const Point& position)
	: Move(sender, position) {
}


void Frame::Moving::position(const Point& value) { _position = value; }
void Frame::Moving::position(int x, int y) { _position = Point(x, y); }



Frame::Resizing::Resizing(Frame& sender, ::tagMINMAXINFO* info)
	: Frame::Event(sender), _info(info) {
}


Point Frame::Resizing::maximizedPosition() const            { return _info->ptMaxPosition; }
void Frame::Resizing::maximizedPosition(const Point& value) {        _info->ptMaxPosition = value; }
void Frame::Resizing::maximizedPosition(int x, int y)       {        _info->ptMaxPosition = Point(x, y); }
Size Frame::Resizing::maximizedSize() const                 { return _info->ptMaxSize; }
void Frame::Resizing::maximizedSize(const Size& value)      {        _info->ptMaxSize = Point(value); }
void Frame::Resizing::maximizedSize(int width, int height)  {        _info->ptMaxSize = Point(width, height); }
Size Frame::Resizing::maxTrackSize() const                  { return _info->ptMaxTrackSize; }
void Frame::Resizing::maxTrackSize(const Size& value)       { if (!sender().maximized()) { _info->ptMaxTrackSize = Point(value); } }
void Frame::Resizing::maxTrackSize(int width, int height)   { maxTrackSize(Size(width, height)); }
Size Frame::Resizing::minTrackSize() const                  { return _info->ptMinTrackSize; }
void Frame::Resizing::minTrackSize(const Size& value)       { if (!sender().maximized()) { _info->ptMinTrackSize = Point(value); } }
void Frame::Resizing::minTrackSize(int width, int height)   { minTrackSize(Size(width, height)); }



Frame::Resize::Resize(Frame& sender, int flag)
	: Frame::Event(sender), _flag(flag) {
}


bool Frame::Resize::maximized() const { return _flag == SIZE_MAXIMIZED; }
bool Frame::Resize::minimized() const { return _flag == SIZE_MINIMIZED; }



Frame::Frame() {
}


Frame::Frame(StringRange text, int clientWidth, int clientHeight, Frame::Style style)
	: _canPostQuitMessage(false), _closeReason(CloseReason::none), _style(Style::sizable) {
	attachHandle(CreateWindowExW(WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT | WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME
		, userClassName(), text.c_str()
		, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr,nullptr, nullptr));
	this->style(style);

	if (clientWidth || clientHeight) {
		auto size = clientSize();
		clientSize(clientWidth  ? clientWidth  : size.width
				 , clientHeight ? clientHeight : size.height);
	}
	_scrollSize = clientSize();
}


Frame::Frame(Frame&& value, bool checkSlicing)
	: ScrollableControl(move(value))
	, _canPostQuitMessage(move(value._canPostQuitMessage))
	, _closeReason(move(value._closeReason))
	, _acceptButton(move(value._acceptButton))
	, _activeControl(move(value._activeControl))
	, _cancelButton(move(value._cancelButton))
	, _style(move(value._style))
	, _onClose(move(value._onClose))
	, _onClosing(move(value._onClosing))
	, _onDisplayChange(move(value._onDisplayChange))
	, _onHelpButtonClick(move(value._onHelpButtonClick))
	, _onMenuLoopBegin(move(value._onMenuLoopBegin))
	, _onMenuLoopEnd(move(value._onMenuLoopEnd))
	, _onMove(move(value._onMove))
	, _onMoving(move(value._onMoving))
	, _onPaint(move(value._onPaint))
	, _onResize(move(value._onResize))
	, _onResizing(move(value._onResizing))
	, _onResizeMoveBegin(move(value._onResizeMoveBegin))
	, _onResizeMoveEnd(move(value._onResizeMoveEnd))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Frame::~Frame() {
	destroyHandle();
}


Frame& Frame::operator=(Frame&& value) {
	if (&value != this) {
		this->~Frame();
		new (this) Frame(move(value));
	}
	return *this;
}


Button* Frame::acceptButton() const {
	assert("acceptButton is not child" && (!_acceptButton || contains(*_acceptButton)));
	return _acceptButton;
}


void Frame::acceptButton(Button* value) {
	assert("acceptButton not created" && (!value || *value));
	assert("acceptButton not contained" && (!value || contains(*value)));
	if (value != acceptButton()) {
		_acceptButton = value;
		if (value && !dynamic_cast<Button*>(activeControl())) {
			value->defaultButton(true);
		}
	}
}



void Frame::activate() {
	assert("Non top level frame can't activate" && !parent());
	assert("Invisible frame can't activate" && visible());
	assert("Disabled frame can't activate" && enabled());
	verify(SetForegroundWindow(handle()));
}


Control* Frame::activeControl() const {
	assert("activeControl is not child" && (!_activeControl || *_activeControl == *this || contains(*_activeControl)));
	return _activeControl;
}


void Frame::activeControl(Control* value) {
	//assert("Cant't set null activeControl" && value); // ントロールの削除等によってヌルにせざるを得ない場合もある
	assert("non child activeControl" && (!value || contains(*value)));
	//assert("Can't focus activeControl" && (!value || value->focusable()));

	Control* oldValue = _activeControl;
	if (value == oldValue) {
		return;
	}
	_activeControl = value;
	if(value && !value->focused() && containsFocus() && value->focusable()) {
		value->focus();
	}

	// デフォルトボタンの設定
	auto oldButton = dynamic_cast<Button*>(oldValue);
	if (!oldButton) {
		oldButton = acceptButton();
	}
	auto newButton = dynamic_cast<Button*>(value);
	if (!newButton) {
		newButton = acceptButton();
	}
	if (oldButton != newButton) {
		if (oldButton) {
			oldButton->defaultButton(false);
		}
		if (newButton) {
			newButton->defaultButton(true);
		}
	}

	if (oldValue) {
		Deactivate event(*oldValue);
		oldValue->onDeactivate()(event);
	}
	if (value) {
		Activate event(*value);
		value->onActivate()(event);
	}
}


Frame* Frame::activeFrame() {
	Control* control = Control::fromHandle(GetForegroundWindow());
	return dynamic_cast<Frame*>(control);
}


Button* Frame::cancelButton() const {
	assert("_cancelButton is not child" && (!_cancelButton || contains(*_cancelButton)));
	return _cancelButton;
}


void Frame::cancelButton(Button* value) {
	assert("cancelButton not created" && (!value || *value));
	assert("cancelButton not contained" && (!value || contains(*value)));
	_cancelButton = value;
}


Font Frame::captionFont() {
	NONCLIENTMETRICSW  metrics;
	ZeroMemory(&metrics, sizeof(metrics));
	metrics.cbSize = sizeof(metrics);
	verify(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS , sizeof(metrics), &metrics, 0));
	return Font::fromLogFont(metrics.lfCaptionFont);
}


void Frame::centerToOwner() {
	const HWND owner = ownerHandle();
	assert("owner not found" && owner);
	Monitor monitor = Monitor::fromControl(owner);
	auto workingArea = monitor.workingArea();

	RECT ownerRect;
	verify(GetWindowRect(owner, &ownerRect));
	auto size = this->size();
	Point point;
	point.x = max(workingArea.x, static_cast<int>(ownerRect.left + ownerRect.right  - size.width ) / 2);
	point.y = max(workingArea.y, static_cast<int>(ownerRect.top  + ownerRect.bottom - size.height) / 2);
	position(point);
}


void Frame::centerToScreen() {
	Monitor monitor;
	const HWND owner = ownerHandle();
	if (owner) {
		monitor = Monitor::fromControl(owner);
	} else {
		monitor = Monitor::fromPoint(Mouse::position());
	}
	auto workingArea = monitor.workingArea();
	auto size = this->size();
	int x = max(workingArea.x, workingArea.x + (workingArea.width  - size.width ) / 2);
	int y = max(workingArea.y, workingArea.y + (workingArea.height - size.height) / 2);
	position(x, y);
}


void Frame::close() {
	_closeReason = CloseReason::userClosing;
	SendMessageW(handle(), WM_CLOSE, 0, 0);
}


Point Frame::desktopPosition() const {
	const auto screen = Monitor().workingArea();
	return position() - screen.position();
}


void Frame::desktopPosition(const Point& value) {
	const auto screen = Monitor().workingArea();
	position(value + screen.position());
}


void Frame::desktopPosition(int x, int y) {
	desktopPosition(Point(x, y));
}


void Frame::exitMessageLoop() {
	PostQuitMessage(0);
}


bool Frame::focusable() const {
	return false; // 厳密にはフォーカスを受けられる子コントロールが無ければフォーカスを得られるがこれは getNextControl 用なのでよしとする
}


bool Frame::helpButton() const {
	return _handle.hasExStyle(WS_EX_CONTEXTHELP);
}


void Frame::helpButton(bool value) {
	auto style = toFlag(_handle.style());
	if (value) {
		style.set(WS_MAXIMIZEBOX | WS_MINIMIZEBOX, false);
	}
	const int exStyle = toFlag(_handle.exStyle()).set(WS_EX_CONTEXTHELP, value);
	updateHandleStyle(style, exStyle, icon());
}


Icon Frame::icon() const {
	return Icon(reinterpret_cast<HICON>(SendMessageW(handle(), WM_GETICON, ICON_BIG, 0)));
}


void Frame::icon(HICON value) {
	// 先にスタイルを更新しないと表示に反映されないことがある
	updateHandleStyle(_handle.style(), _handle.exStyle(), value);
	if (!value) {
		SendMessageW(handle(), WM_SETICON, ICON_SMALL, 0);
	}
	SendMessageW(handle(), WM_SETICON, ICON_BIG, (LPARAM)value);
	verify(RedrawWindow(handle(), nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME));
}


bool Frame::maximizeButton() const {
	return _handle.hasStyle(WS_MAXIMIZEBOX);
}


void Frame::maximizeButton(bool value) {
	const int style = toFlag(_handle.style()).set(WS_MAXIMIZEBOX, value);
	updateHandleStyle(style, _handle.exStyle(), icon());
}


bool Frame::maximized() const {
	return IsZoomed(handle()) != 0;
}


void Frame::maximized(bool value) {
	assert("Invisible frame Can't maximize" && visible());
	if (value != maximized()) {
		ShowWindow(handle(), value ? SW_MAXIMIZE : SW_RESTORE);
	}
}


void Frame::menuBar(MenuBar* value) {
	assert("MenuBar not created" && (!value || *value));
	verify(SetMenu(handle(), value ? static_cast<HMENU>(*value) : nullptr));
	updateMenubar();
}


Rectangle Frame::menuBarBounds() const {
	if (!GetMenu(handle())) {
		return Rectangle(0, 0, 0, 0);
	}
	MENUBARINFO info;
	info.cbSize = sizeof(info);
	verify(GetMenuBarInfo(handle(), OBJID_MENU, 0, &info));
	return info.rcBar;
}


bool Frame::menuBarFocused() const {
	MENUBARINFO info;
	info.fFocused = 0;
	if (GetMenu(handle())) {
		info.cbSize = sizeof(info);
		verify(GetMenuBarInfo(handle(), OBJID_MENU, 0, &info));
	}
	return info.fBarFocused != 0;
}


bool Frame::minimizeButton() const {
	return _handle.hasStyle(WS_MINIMIZEBOX);
}


void Frame::minimizeButton(bool value) {
	const int style = toFlag(_handle.style()).set(WS_MINIMIZEBOX, value);
	updateHandleStyle(style, _handle.exStyle(), icon());
}


bool Frame::minimized() const {
	return IsIconic(handle()) != 0;
}


void Frame::minimized(bool value) {
	if (value != minimized()) {
		ShowWindow(handle(), value ? SW_MINIMIZE : SW_RESTORE);
	}
}


Listener<Frame::Close&>& Frame::onClose() { return _onClose; }
Listener<Frame::Closing&>& Frame::onClosing() { return _onClosing; }
Listener<Frame::DisplayChange&>& Frame::onDisplayChange() { return _onDisplayChange; }
Listener<Frame::HelpButtonClick&>& Frame::onHelpButtonClick() { return _onHelpButtonClick; }
Listener<Frame::MenuLoopBegin&>& Frame::onMenuLoopBegin() { return _onMenuLoopBegin; }
Listener<Frame::MenuLoopEnd&>& Frame::onMenuLoopEnd() { return _onMenuLoopEnd; }
Listener<Frame::Move&>& Frame::onMove() { return _onMove; }
Listener<Frame::Moving&>& Frame::onMoving() { return _onMoving; }
Listener<Frame::Paint&>& Frame::onPaint() { return _onPaint; }
Listener<Frame::Resize&>& Frame::onResize() { return _onResize; }
Listener<Frame::Resizing&>& Frame::onResizing() { return _onResizing; }
Listener<Frame::ResizeMoveBegin&>& Frame::onResizeMoveBegin() { return _onResizeMoveBegin; }
Listener<Frame::ResizeMoveEnd&>& Frame::onResizeMoveEnd() { return _onResizeMoveEnd; }


Frame* Frame::owner() const {
	Control* owner = Control::fromHandle(ownerHandle());
	return dynamic_cast<Frame*>(owner);
}


void Frame::owner(Frame* value) {
	HWND handle = nullptr;
	if (value) {
		handle = *value;
	}
	ownerHandle(handle);
}


HWND Frame::ownerHandle() const {
	return reinterpret_cast<HWND>(GetWindowLongPtrW(handle(), GWLP_HWNDPARENT)); //GWLP_HWNDPARENT(GWL_HWNDPARENT)
}


void Frame::ownerHandle(HWND value) {
	assert("Double owned" && !parent());
	if (value != ownerHandle()) {
		if (value) {
			assert("owner not created" && IsWindow(value));
			assert("Circular owner" && _handle.checkParentingCycle(value));
			assert("Circular owner" && Handle(value).checkParentingCycle(*this));
			assert("Owned different threads" && GetWindowThreadProcessId(handle(), nullptr) == GetWindowThreadProcessId(value, nullptr));
		}
#pragma warning(push)
#pragma warning(disable : 4244) // '引数' : 'LONG_PTR' から 'LONG' への変換です。データが失われる可能性があります
#pragma warning(disable : 4312) // reinterpret_cast' : 'LONG' からより大きいサイズの 'void *' へ変換します
		SetWindowLongPtrW(handle(), GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(value)); //GWLP_HWNDPARENT(GWL_HWNDPARENT)
#pragma warning(pop)
	}
}


void Frame::parent(Control* value) {
	assert("Double owned" && !owner());
	auto style = toFlag(_handle.style());
	style.set(WS_CHILD, value != nullptr); // これを設定しないと contains がうまく動かなくなったりそれぞれがアクティブになったりして大変な事になる
	_handle.style(style);
	_handle.updateStyle();

	Control::parent(value);
}


void Frame::resetLayer() {
	assert("Non top level frame can't set layer" && !parent());
	_handle.setExStyle(WS_EX_LAYERED, false);
}


Rectangle Frame::restoreBounds() const {
	WINDOWPLACEMENT placement;
	placement.length = sizeof(placement);
	verify(GetWindowPlacement(handle(), &placement));
	return placement.rcNormalPosition;
}


void Frame::runMessageLoop() {
	assert("frame disabled" && enabled());
	assert("Non top level frame can't run message loop" && !parent());
	assert("Nested messageLoop" && !_canPostQuitMessage);
	assert("Different thread" && !invokeRequired());

	visible(true);
	_canPostQuitMessage = true;
	scopeExit([&] () {
		_canPostQuitMessage = false;
	});
	Message message;
	for ( ; ; ) {
		const BOOL result = GetMessageW(reinterpret_cast<MSG*>(&message), nullptr, 0, 0);
		assert(result != -1);
		if (!result) {
			break;
		}
		if (!Control::preTranslateMessage(message)) {
			TranslateMessage(reinterpret_cast<MSG*>(&message));
			DispatchMessageW(reinterpret_cast<MSG*>(&message));
		}
	}
}


void Frame::runModalMessageLoop() {
	assert("frame disabled" && enabled());
	assert("Non top level frame can't run message loop" && !parent());
	assert("Nested messageLoop" && !_canPostQuitMessage);
	assert("Not found modal owner" && ownerHandle());
	assert("Different thread" && !invokeRequired());

	auto capturedHandle = GetCapture();
	if (capturedHandle) {
		SendMessageW(capturedHandle, WM_CANCELMODE, 0, 0);
		verify(ReleaseCapture());
	}
	auto owner = this->ownerHandle();
	auto activeFrame = this->activeFrame();
	auto activeFrameHandle = activeFrame ? (HWND)*activeFrame : nullptr;
	scopeExit([&] () {
		if (!IsWindowEnabled(owner)) {
			EnableWindow(owner, TRUE);
		}
		if (activeFrameHandle && IsWindow(activeFrameHandle) && IsWindowVisible(activeFrameHandle)) {
			SetActiveWindow(activeFrameHandle);
		}
	});
	struct ThreadModal {
		HWND dialog;

		ThreadModal(HWND dialog) : dialog(dialog) {
			verify(EnumThreadWindows(GetCurrentThreadId(), beginProcedure, (LPARAM)dialog));
		}
		~ThreadModal() {
			verify(EnumThreadWindows(GetCurrentThreadId(), endProcedure, (LPARAM)dialog));
		}

		static BOOL CALLBACK beginProcedure(HWND handle, LPARAM param) {
			if (handle != reinterpret_cast<HWND>(param) && IsWindowVisible(handle) && IsWindowEnabled(handle)) {
				EnableWindow(handle, FALSE);
			}
			return TRUE;
		}
		static BOOL CALLBACK endProcedure(HWND handle, LPARAM param) {
			if (handle != reinterpret_cast<HWND>(param) && IsWindowVisible(handle) && !IsWindowEnabled(handle)) {
				EnableWindow(handle, TRUE);
			}
			return TRUE;
		}
	} threadModal(handle());
	if (owner && IsWindowEnabled(owner)) { // owner は別スレッドのウインドウかもしれない
		EnableWindow(owner, FALSE);
	}
	runMessageLoop();
}


bool Frame::runPostedMessages() {
	assert("frame disabled" && enabled());
	assert("Non top level frame can't run message loop" && !parent());
	assert("Different thread" && !invokeRequired());

	_canPostQuitMessage = true;
	Message message;
	while(PeekMessageW(reinterpret_cast<MSG*>(&message), nullptr, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) {
			_canPostQuitMessage = false;
			return false;
		}
		if (!Control::preTranslateMessage(message)) {
			TranslateMessage(reinterpret_cast<MSG*>(&message));
			DispatchMessageW(reinterpret_cast<MSG*>(&message));
		}
	}
	return true;
}


void Frame::setLayer(float alpha) {
	assert("Non top level frame can't set layer" && !parent());

	_handle.setExStyle(WS_EX_LAYERED, true);
	const BYTE byteAlpha = static_cast<BYTE>(max(0, min(255, static_cast<int>(alpha * 255 + 0.5f))));
	verify(SetLayeredWindowAttributes(handle(), 0, byteAlpha, LWA_ALPHA));
}


void Frame::setLayer(const Color& colorKey) {
	assert("Non top level frame can't set layer" && !parent());

	_handle.setExStyle(WS_EX_LAYERED, true);
	verify(SetLayeredWindowAttributes(handle(), colorKey.toCOLORREF(), 0, LWA_COLORKEY));
}


void Frame::setLayer(HDC graphicsWithAlphaChannel, float alpha) {
	assert("Non top level frame can't set layer" && !parent());
	style(Frame::Style::none);
	_handle.setExStyle(WS_EX_LAYERED, true);
	Graphics graphics(graphicsWithAlphaChannel);
	auto size = graphics.visibleClipBounds().size();
	SIZE layerSize = {size.width, size.height};
	POINT layerPos = {0, 0};
	const BYTE byteAlpha = static_cast<BYTE>(max(0, min(255, static_cast<int>(alpha * 255 + 0.5f))));
	BLENDFUNCTION function = {AC_SRC_OVER, 0, byteAlpha, AC_SRC_ALPHA};
	verify(UpdateLayeredWindow(handle(), nullptr, nullptr, &layerSize, graphicsWithAlphaChannel, &layerPos, 0, &function, ULW_ALPHA));
}


Size Frame::sizeFromClientSize(const Size& clientSize) const {
	RECT rect = {0, 0, clientSize.width, clientSize.height};
	verify(AdjustWindowRectEx(&rect, _handle.style(), GetMenu(handle()) ? TRUE : FALSE, _handle.exStyle()));
	return Size(rect.right - rect.left, rect.bottom - rect.top);
}


bool Frame::showInTaskbar() const {
	return _handle.hasExStyle(WS_EX_APPWINDOW);
}


void Frame::showInTaskbar(bool value) {
	assert("Can't change showInTaskbar" && (style() == Style::toolWindow || style() == Style::sizableToolWindow || ownerHandle()));
	if (value != showInTaskbar()) {
		const int exStyle = toFlag(_handle.exStyle()).set(WS_EX_APPWINDOW, value);
		bool oldVisible = visible();
		visible(false);
		updateHandleStyle(_handle.style(), exStyle, icon());
		visible(oldVisible);
	}
}


Frame::Style Frame::style() const {
	return _style;
}


void Frame::style(Frame::Style value) {
	assert("Invalid Frame::Style" && Style::_validate(value));

	if (value != style()) {
		_style = value;

		DWORD style = _handle.style();
		DWORD exStyle = _handle.exStyle();
		style = toFlag(style).set(WS_BORDER | WS_THICKFRAME, false);
		exStyle = toFlag(exStyle).set(WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME, false);
		switch (value) {
			case Frame::Style::none : {
			} break;
			case Frame::Style::singleLine : {
				style   |= WS_BORDER;
			} break;
			case Frame::Style::sizable : { 
				style   |= WS_BORDER | WS_THICKFRAME;
			} break;
			case Frame::Style::threeDimensional : {
				style   |= WS_BORDER;
				exStyle |= WS_EX_CLIENTEDGE;
			} break;
			case Frame::Style::dialog       : {
				style   |= WS_BORDER;
				exStyle |= WS_EX_DLGMODALFRAME;
			} break;
			case Frame::Style::toolWindow   : {
				style   |= WS_BORDER;
				exStyle |= WS_EX_TOOLWINDOW;
			} break;
			case Frame::Style::sizableToolWindow : {
				style   |= WS_BORDER | WS_THICKFRAME;
				exStyle |= WS_EX_TOOLWINDOW;
			} break;
		}
		updateHandleStyle(style, exStyle, icon());
	}
}


bool Frame::systemMenu() const {
	return _handle.hasStyle(WS_SYSMENU);
}


void Frame::systemMenu(bool value) {
	const int style = toFlag(_handle.style()).set(WS_SYSMENU, value);
	updateHandleStyle(style, _handle.exStyle(), icon());
}


void Frame::text(StringRange value) {
	auto oldText = text();
	ScrollableControl::text(value);
	if (value.empty() || oldText.empty()) {
		updateHandleStyle(_handle.style(), _handle.exStyle(), icon());
	}
}


bool Frame::topMost() const {
	return _handle.hasExStyle(WS_EX_TOPMOST);
}


void Frame::topMost(bool value) {
	assert("non top level frame can't be topmost" && !parent());
	verify(SetWindowPos(handle(), value ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
}


void Frame::updateMenubar() {
	verify(DrawMenuBar(handle()));
}


void Frame::visible(bool value) {
	if (parent()) {
		ScrollableControl::visible(value);
	} else if (value != visible()) {
		int show = SW_SHOW;
		if (maximized()) {
			show = SW_MAXIMIZE;
		}
		if (minimized()) {
			show = SW_MINIMIZE;
		}
		ShowWindow(handle(), value ? show : SW_HIDE);
	}
}


void Frame::processDescendantErased(Control& descendant) {
	if (activeControl() == &descendant) {
		activeControl(nullptr);
	}
	if (acceptButton() == &descendant) {
		acceptButton(nullptr);
	}
	if (cancelButton() == &descendant) {
		cancelButton(nullptr);
	}
}


void Frame::processDescendantFocused(Control& descendant) {
	activeControl(&descendant);
	scrollControlIntoView(descendant, true);
}


bool Frame::processDialogKey(int shortcut) {
	if (!(shortcut & (Key::Modifier::alt | Key::Modifier::ctrl))) {
		auto key = static_cast<Key>(shortcut & ~Key::Modifier::mask);
		switch (key) {
			case Key::tab : {
				bool forward = !(shortcut & Key::Modifier::shift);
				auto next = getNextControl(activeControl() ? *activeControl() : *this, forward, true, true, true);
				if (next) {
					next->focus();
					return true;
				}
			} break;
			case Key::left :
			case Key::right :
			case Key::up :
			case Key::down : {
				bool forward = key == Key::right || key == Key::down;
				auto active = activeControl();
				if (active) {
					auto activeParent = active->parent();
					if (activeParent) {
						auto next = activeParent->getNextControl(*active, forward, false, false, true);
						if (next) {
							next->focus();
							return true;
						}
					}
				}
			} break;
			case Key::enter : {
				auto button = dynamic_cast<Button*>(activeControl());
				if (!button) {
					button = acceptButton();
				}
				if (button) {
					button->performClick();
					return true;
				}
			} break;
			case Key::escape : {
				if (_cancelButton) {
					_cancelButton->performClick();
					return true;
				}
			} break;
		}
	}
	return false;
}


void Frame::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_ACTIVATE : {
			if (LOWORD(msg.wparam) != WA_INACTIVE) {
				Control* active = activeControl();
				if (!active) {
					auto next = getNextControl(*this, true, true, true, false);
					if (next) {
						next->focus();
					}
				} else if (!active->focused() && active->focusable()) {
					active->focus();
				}
				Activate event(*this);
				onActivate()(event);
			} else {
				Deactivate event(*this);
				onDeactivate()(event);
			}
		} break;
		case WM_DISPLAYCHANGE : {
			processMessageByDefault(msg);
			DisplayChange event(*this);
			onDisplayChange()(event);
		} break;
		case WM_QUERYENDSESSION :
		case WM_ENDSESSION :
			_closeReason = CloseReason::windowShutDown;
		case WM_CLOSE : {
			if (_closeReason == CloseReason::none) {
				_closeReason = CloseReason::taskManagerClosing;
			}
			Closing event(*this, _closeReason);
			if (msg.message != WM_ENDSESSION) {
				onClosing()(event);
				if (msg.message == WM_QUERYENDSESSION) {
					msg.result = event.cancel() ? FALSE : TRUE;
				}
			} else {
				event.cancel(msg.wparam == 0);
			}
			if (msg.message != WM_QUERYENDSESSION && !event.cancel()) {
				onClose()(event);
				visible(false);
				if (_canPostQuitMessage)  {
					PostQuitMessage(0);
				}
			}
			_closeReason = CloseReason::none;
		} break;
		case WM_DESTROY : {
			visible(false); // アイコンがタスクバーに残るバグ回避
			ScrollableControl::processMessage(msg);
		} break;
		case WM_ENTERMENULOOP : {
			MenuLoopBegin event(*this);
			onMenuLoopBegin()(event);
			processMessageByDefault(msg);
		} break;
		case WM_ENTERSIZEMOVE : {
			ResizeMoveBegin event(*this);
			onResizeMoveBegin()(event);
			processMessageByDefault(msg);
		} break;
		case WM_ERASEBKGND : {
			if (onPaint() || brush()) {
				msg.result = TRUE;
			} else {
				processMessageByDefault(msg);
			}
		} break;
		case WM_EXITMENULOOP : {
			MenuLoopEnd event(*this);
			onMenuLoopEnd()(event);
			processMessageByDefault(msg);
		} break;
		case WM_EXITSIZEMOVE : {
			processMessageByDefault(msg);
			Event event(*this);
			onResizeMoveEnd()(event);
		} break;
		case WM_GETMINMAXINFO : {
			MINMAXINFO* info = (MINMAXINFO*)msg.lparam;
			Resizing event(*this, info);
			onResizing()(event);
		} break;
		case WM_PAINT : {
			PAINTSTRUCT paint;
			Graphics graphics(BeginPaint(handle(), &paint));
			scopeExit([&] () {
				EndPaint(handle(), &paint);
			});
			if (onPaint()) {
				Paint event(*this, graphics);
				onPaint()(event);
			} else {
				auto brush = this->brush();
				if (brush) {
					const auto origin = graphics.brushOrigin(graphics.brushOrigin() +  displayRectangle().position() + brushOrigin());
					brush = graphics.brush(brush);
					graphics.clear();
					graphics.brushOrigin(origin);
					graphics.brush(brush);
				} 
			}
		} break;
		case WM_PRINTCLIENT : {
			if (onPaint()) {
				Paint event(*this, (HDC)msg.wparam);
				onPaint()(event);
			}
			processMessageByDefault(msg);
		} break;
		case WM_SETFOCUS : {
			processMessageByDefault(msg);
			if (parent()) {
				auto next = getNextControl(*this, true, true, true, false);
				if (next) {
					next->focus();
					return;
				}
			} else {
				auto active = activeControl();
				if (active && active->focusable()) {
					active->focus();
					return;
				}
			}
			Focus event(*this, (HWND)msg.wparam);
			onFocus()(event);
		} break;
		case WM_SIZE : {
			ScrollableControl::processMessage(msg); // ScrollableControl の処理で clientSize は変化する
			Resize event(*this, msg.wparam);
			onResize()(event);
		} break;
		case WM_SYSCOMMAND : {
			int sc = LOWORD(msg.wparam) & 0xfff0;
			switch (sc) {
				case SC_CLOSE : {
					_closeReason = CloseReason::userClosing;
				} break;
				case SC_CONTEXTHELP : {
					HelpButtonClick event(*this);
					onHelpButtonClick()(event);
					if (event.cancel()) {
						return;
					}
				} break;
			}
			processMessageByDefault(msg);
		} break;
		case WM_WINDOWPOSCHANGED : {
			auto info = (WINDOWPOS*)msg.lparam;
			if (!toFlag(info->flags)[SWP_NOMOVE]) {
				Move event(*this, Point(info->x, info->y));
				onMove()(event);
			}
			ScrollableControl::processMessage(msg);
		} break;
		case WM_WINDOWPOSCHANGING : {
			auto info = (WINDOWPOS*)msg.lparam;
			if (!toFlag(info->flags)[SWP_NOMOVE]) {
				auto position = this->position();
				if (position.x != info->x || position.y != info->y) {
					Moving event(*this, Point(info->x, info->y));
					onMoving()(event);
					info->x = event.position().x;
					info->y = event.position().y;
				}
			}
			ScrollableControl::processMessage(msg);
		} break;
		default: {
			ScrollableControl::processMessage(msg);
		} break;
	}
}


void Frame::updateHandleStyle(int style, int exStyle, HICON icon) {
	const auto oldClientSize = clientSize();

	const auto formStyle = this->style();
	const bool controlBox =  toFlag(style)[WS_SYSMENU];
	const bool caption = formStyle != Frame::Style::none && (!text().empty() || controlBox);
	style = toFlag(style).set(WS_DLGFRAME, caption); // WS_CAPCION = WS_BORDER | WS_DLGFRAME であり、Frame::Style::none 以外ならば WS_BORDER は確定している

	bool dialogModalFrame = !icon && (formStyle == Frame::Style::sizable
								   || formStyle == Frame::Style::threeDimensional
								   || formStyle == Frame::Style::singleLine);
	if (formStyle == Frame::Style::dialog) {
		dialogModalFrame = true;
	}
	exStyle = toFlag(exStyle).set(WS_EX_DLGMODALFRAME, dialogModalFrame);

	_handle.style(style);
	_handle.exStyle(exStyle);
	_handle.updateStyle();
	clientSize(oldClientSize);
}



	}
}