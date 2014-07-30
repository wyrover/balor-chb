#include "Control.hpp"

#include <algorithm>
#include <functional>
#include <type_traits>

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Cursor.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Region.hpp>
#include <balor/gui/DragDrop.hpp>
#include <balor/gui/Frame.hpp>
#include <balor/gui/Menu.hpp>
#include <balor/gui/Scaler.hpp>
#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>
#include <balor/Flag.hpp>
#include <balor/Singleton.hpp>
#include <balor/StringBuffer.hpp>

#include <WindowsX.h>
#include <Uxtheme.h> // XP以降。SetWindowTheme/GetWindowTheme
#pragma comment(lib, "UxTheme.lib")


namespace balor {
namespace gui {

	using std::is_same;
	using std::max;
	using std::min;
	using std::move;
	using std::swap;
	using namespace balor::graphics;
	using namespace balor::system;


	namespace {
		static_assert(is_same<Control::Procedure, WNDPROC>::value, "Invalid Procedure type");


		inline ATOM getThisPointerPropAtom() {
			class ThisPointerPropAtomHolder {
				friend Singleton<ThisPointerPropAtomHolder>;

				ThisPointerPropAtomHolder() {
					static const wchar_t* atomName = L"balor::gui::Control::thisPointerPropAtom";
					assert("Duplicate local atom name" && !FindAtomW(atomName)); // まさかとは思うがこんな長い名前が既に使われていた
					atom = AddAtomW(atomName); // ローカルアトムはアプリケーション終了時にすべて開放されるので後始末は考えなくて良い
				}
				~ThisPointerPropAtomHolder() {}

			public:
				ATOM atom;
			};

			return Singleton<ThisPointerPropAtomHolder>::get().atom;
		}


		Mouse getXButton(const Message& msg) {
			switch (HIWORD(msg.wparam)) {
			case XBUTTON1: return Mouse::xButton1;
			case XBUTTON2: return Mouse::xButton2;
			}
			assert(false);
			return Mouse::none;
		}


		const int wmInvokeAsynchronous = WM_USER + 0x6000;
		const int wmInvoke = WM_USER + 0x6001;

		const Rectangle hugeBox(-100000, -100000, 200000, 200000);
	} // namespace



	bool Control::Edge::_validate(Edge value) {
		return none <= value && value <= client;
	}



	Control::Handle::Handle(HWND handle) : _handle(handle) {
	}


	Control::Handle::Handle(Handle&& value) : _handle(value._handle) {
		value._handle = nullptr;
	}


	Control::Handle::~Handle() {
		//_handle = nullptr;
	}


	Control::Handle& Control::Handle::operator=(Handle&& value) {
		swap(_handle, value._handle);
		return *this;
	}


	Rectangle Control::Handle::bounds() const {
		RECT rect;
		verify(GetWindowRect(_handle, &rect));
		auto parent = this->parent();
		if (parent) {
			verify(MapWindowPoints(nullptr, parent, reinterpret_cast<POINT*>(&rect), 2));
		}
		return rect;
	}


	void Control::Handle::bounds(const Rectangle& value) {
		auto oldBounds = bounds();
		UINT flags = SWP_NOACTIVATE | SWP_NOZORDER;
		if (oldBounds.position() == value.position()) {
			flags |= SWP_NOMOVE;
		}
		if (oldBounds.size() == value.size()) {
			flags |= SWP_NOSIZE;
		}
		if (flags != (SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE)) {
			verify(SetWindowPos(_handle, nullptr, value.x, value.y, value.width, value.height, flags));
		}
	}


	void Control::Handle::bounds(int x, int y, int width, int height) {
		bounds(Rectangle(x, y, width, height));
	}


	bool Control::Handle::checkParentingCycle(HWND parent) const {
		assert("Different thread" && !differentThread());
		if (!parent) {
			return false;
		}
		for (; parent; parent = GetParent(parent)) {
			if (parent == _handle) {
				return false;
			}
		}
		return true;
	}


	Control* Control::Handle::control() const {
		if (!_handle) {
			return nullptr;
		}
		assert("Different process" && !differentProcess());
		// 残念ながら GWLP_USERDATA は XP の IME など、一部のアプリケーションで使われていて
		// 特に IME は同じスレッドにウインドウ作成されるので大いに問題が発生する。
		//return reinterpret_cast<Control*>(GetWindowLongPtrW(_handle, GWLP_USERDATA));
		return static_cast<Control*>(GetPropW(_handle, reinterpret_cast<LPCWSTR>(getThisPointerPropAtom())));
	}


	void Control::Handle::control(Control* value) {
		assert("Different thread" && !differentThread());
		verify(SetPropW(_handle, reinterpret_cast<LPCWSTR>(getThisPointerPropAtom()), static_cast<HANDLE>(value)));
		//SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(value));
	}


	bool Control::Handle::differentProcess() const {
		assert("Invalid handle" && IsWindow(_handle));
		DWORD pid = 0;
		GetWindowThreadProcessId(_handle, &pid);
		return pid != GetCurrentProcessId();
	}


	bool Control::Handle::differentThread() const {
		assert("Invalid handle" && IsWindow(_handle));
		return GetWindowThreadProcessId(_handle, nullptr) != GetCurrentThreadId();
	}


	Control::Edge Control::Handle::edge() const {
		assert("Different thread" && !differentThread());
		if (toFlag(style())[WS_BORDER]) {
			return Edge::line;
		}
		auto exStyle = toFlag(this->exStyle());
		if (exStyle[WS_EX_STATICEDGE]) {
			return Edge::sunken;
		}
		if (exStyle[WS_EX_CLIENTEDGE]) {
			return Edge::client;
		}
		return Edge::none;
	}


	void Control::Handle::edge(Control::Edge value) {
		assert("Different thread" && !differentThread());
		assert("Invalid Control::Edge" && Edge::_validate(value));
		if (value == edge()) {
			return;
		}
		auto style = toFlag(this->style()).set(WS_BORDER, false);
		auto exStyle = toFlag(this->exStyle()).set(WS_EX_STATICEDGE | WS_EX_CLIENTEDGE, false);
		switch (value) {
		case Edge::line: style.set(WS_BORDER, true); break;
		case Edge::sunken: exStyle.set(WS_EX_STATICEDGE, true); break;
		case Edge::client: exStyle.set(WS_EX_CLIENTEDGE, true); break;
		}
		this->style(style);
		this->exStyle(exStyle);
		updateStyle();
	}


	int Control::Handle::exStyle() const {
		assert("Different thread" && !differentThread());
		return static_cast<int>(GetWindowLongPtrW(_handle, GWL_EXSTYLE));
	}


	void Control::Handle::exStyle(int value) {
		assert("Different thread" && !differentThread());
		SetWindowLongPtrW(_handle, GWL_EXSTYLE, value);
	}


	bool Control::Handle::hasStyle(int bitFlags) const {
		return toFlag(style())[bitFlags];
	}


	bool Control::Handle::hasExStyle(int bitFlags) const {
		return toFlag(exStyle())[bitFlags];
	}


	Control::Handle Control::Handle::parent() const {
		assert("Invalid handle" && IsWindow(_handle));
		auto parent = GetAncestor(_handle, GA_PARENT);
		if (parent == GetDesktopWindow()) {
			return Handle();
		}
		return Handle(parent);
	}


	Control::Procedure Control::Handle::procedure() const {
		assert("Different thread" && !differentThread());
		return reinterpret_cast<WNDPROC>(GetWindowLongPtrW(_handle, GWLP_WNDPROC));
	}


	Control::Procedure Control::Handle::procedure(Procedure value) {
		assert("Different thread" && !differentThread());
		//#pragma warning(push)
		//#pragma warning(disable : 4244) // '引数' : 'LONG_PTR' から 'LONG' への変換です。データが失われる可能性があります。
		//#pragma warning(disable : 4312) // reinterpret_cast' : 'LONG' からより大きいサイズの 'void *' へ変換します。
		return reinterpret_cast<WNDPROC>(SetWindowLongPtrW(_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(value)));
		//#pragma warning(pop)
	}


	void Control::Handle::setStyle(int bitFlags, bool on) {
		auto newStyle = toFlag(style());
		if (on != newStyle[bitFlags]) {
			newStyle.set(bitFlags, on);
			style(newStyle);
			updateStyle();
		}
	}


	void Control::Handle::setExStyle(int bitFlags, bool on) {
		auto newExStyle = toFlag(exStyle());
		if (on != newExStyle[bitFlags]) {
			newExStyle.set(bitFlags, on);
			exStyle(newExStyle);
			updateStyle();
		}
	}


	Message::LRESULT __stdcall Control::Handle::standardProcedure(HWND handle, unsigned int message, Message::WPARAM wparam, Message::LPARAM lparam) {
		Control* control = Handle(handle).control();
		assert(control);
		Message msg(handle, message, wparam, lparam);
		control->processMessage(msg);
		return msg.result;
	}


	int Control::Handle::style() const {
		assert("Different thread" && !differentThread());
		return static_cast<int>(GetWindowLongPtrW(_handle, GWL_STYLE));
	}


	void Control::Handle::style(int value) {
		assert("Different thread" && !differentThread());
		SetWindowLongPtrW(_handle, GWL_STYLE, value);
	}


	void Control::Handle::updateStyle() {
		assert("Different thread" && !differentThread());
		verify(SetWindowPos(_handle, nullptr, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER));
		verify(RedrawWindow(_handle, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN));
	}



	Control::ControlsIterator::ControlsIterator(const Control& parent) {
		assert("Control not created" && parent);
		HWND i = GetTopWindow(parent.handle());
		if (!i) {
			_current = nullptr;
			return;
		}
		Control* control = Handle(i).control();
		while (i && !control) {
			i = GetNextWindow(i, GW_HWNDNEXT);
			control = Handle(i).control();
		}
		_current = control;
	}


	Control::ControlsIterator::ControlsIterator(Control* current) : _current(current) {
	}


	Control::ControlsIterator& Control::ControlsIterator::operator ++() {
		assert("Iterator out of range" && _current);
		HWND i = *_current;
		do {
			i = GetNextWindow(i, GW_HWNDNEXT);
			_current = Handle(i).control();
		} while (i && !_current);
		return *this;
	}


	Control::ControlsIterator& Control::ControlsIterator::operator --() {
		assert("Iterator out of range" && _current);
		HWND i = *_current;
		do {
			i = GetNextWindow(i, GW_HWNDPREV);
			_current = Handle(i).control();
		} while (i && !_current);
		return *this;
	}


	Control::ControlsIterator::operator Control*() const {
		return _current;
	}


	Control* Control::ControlsIterator::operator->() {
		assert("Iterator out of range" && _current);
		return _current;
	}


	Control::ControlsIterator Control::ControlsIterator::getLast(const Control& parent) {
		assert("Control not created" && parent);
		HWND i = GetTopWindow(parent.handle());
		if (!i) {
			return ControlsIterator(nullptr);
		}
		i = GetWindow(i, GW_HWNDLAST);
		Control* control = Handle(i).control();
		while (i && !control) {
			i = GetNextWindow(i, GW_HWNDPREV);
			control = Handle(i).control();
		}
		return ControlsIterator(control);
	}



	Control::DescendantsIterator::DescendantsIterator(const Control& parent)
		: _parent(const_cast<Control*>(&parent)), _current(nullptr) {
		assert("Control not created" && parent);
		_current = ControlsIterator(parent);
	}


	Control::DescendantsIterator& Control::DescendantsIterator::operator ++() {
		assert("Iterator out of range" && _current);
		Control* next = ControlsIterator(*_current);
		if (next) {
			_current = next;
			return *this;
		}
		next = ++ControlsIterator(_current);
		if (next) {
			_current = next;
			return *this;
		}
		next = _current;
		for (;;) {
			next = next->parent();
			if (next == _parent) {
				_current = nullptr;
				break;
			}
			Control* parentNext = ++ControlsIterator(next);
			if (parentNext) {
				_current = parentNext;
				break;
			}
		}
		return *this;
	}


	Control::DescendantsIterator::operator Control*() const {
		return _current;
	}


	Control* Control::DescendantsIterator::operator->() {
		assert("Iterator out of range" && _current);
		return _current;
	}



	Control::CursorChange::CursorChange(Control& sender)
		: Control::Event(sender), _cursor(nullptr) {
	}


	HCURSOR Control::CursorChange::cursor() const { return _cursor; }
	void Control::CursorChange::cursor(HCURSOR value) { _cursor = value; }



	Control::Focus::Focus(Control& sender, HWND prevFocusedHandle)
		: Control::Event(sender), _prevFocusedHandle(prevFocusedHandle) {
	}


	Control* Control::Focus::prevFocused() const { return Control::fromHandle(_prevFocusedHandle); }
	Control::Handle Control::Focus::prevFocusedHandle() const { return Handle(_prevFocusedHandle); }



	Control::Defocus::Defocus(Control& sender, HWND nextFocusedHandle)
		: Control::Event(sender), _nextFocusedHandle(nextFocusedHandle) {
	}


	Control* Control::Defocus::nextFocused() const { return Control::fromHandle(_nextFocusedHandle); }
	Control::Handle Control::Defocus::nextFocusedHandle() const { return Handle(_nextFocusedHandle); }



	Control::HelpRequest::HelpRequest(Control& sender, Control& target, const Point& position)
		: Control::Event(sender), _target(target), _position(position), _cancel(false) {
	}


	bool Control::HelpRequest::cancel() const { return _cancel; }
	void Control::HelpRequest::cancel(bool value) { _cancel = value; }
	const Point& Control::HelpRequest::position() const { return _position; }
	Control& Control::HelpRequest::target() { return _target; }



	Control::KeyEvent::KeyEvent(Control& sender, Key key, int flags)
		: Control::Event(sender), _key(key), _flags(flags), _handled(false) {
	}


	bool Control::KeyEvent::alt() const { return (_flags & (1 << 29)) != 0; }
	bool Control::KeyEvent::handled() const { return _handled; }
	void Control::KeyEvent::handled(bool value) { _handled = value; }
	Key Control::KeyEvent::key() const { return _key; }
	bool Control::KeyEvent::previousDown() const { return (_flags & (1 << 30)) != 0; }
	int Control::KeyEvent::repeatCount() const { return _flags & 0xffff; }



	Control::KeyPress::KeyPress(Control& sender, wchar_t charCode)
		: Control::Event(sender), _charCode(charCode), _handled(false) {
	}


	wchar_t Control::KeyPress::charCode() const { return _charCode; }
	void Control::KeyPress::charCode(wchar_t value) { _charCode = value; }
	bool Control::KeyPress::handled() const { return _handled; }
	void Control::KeyPress::handled(bool value) { _handled = value; }



	Control::MouseEvent::MouseEvent(Control& sender, Mouse button, Message& message)
		: Control::Event(sender), _button(button), _message(message) {
	}


	Mouse Control::MouseEvent::button() const { return _button; }
	bool Control::MouseEvent::ctrl() const { return (_message.wparam & MK_CONTROL) != 0; }
	bool Control::MouseEvent::lButton() const { return (_message.wparam & MK_LBUTTON) != 0; }
	bool Control::MouseEvent::mButton() const { return (_message.wparam & MK_MBUTTON) != 0; }
	bool Control::MouseEvent::rButton() const { return (_message.wparam & MK_RBUTTON) != 0; }
	bool Control::MouseEvent::xButton1() const { return (_message.wparam & MK_XBUTTON1) != 0; }
	bool Control::MouseEvent::xButton2() const { return (_message.wparam & MK_XBUTTON2) != 0; }
	Point Control::MouseEvent::position() const { return Point(GET_X_LPARAM(_message.lparam), GET_Y_LPARAM(_message.lparam)); }
	bool Control::MouseEvent::shift() const { return (_message.wparam & MK_SHIFT) != 0; }



	Control::MouseDown::MouseDown(Control& sender, Mouse button, Message& message, Rectangle& dragBox)
		: MouseEvent(sender, button, message), _dragBox(dragBox) {
	}


	Rectangle Control::MouseDown::dragBox() const { return _dragBox; }
	void Control::MouseDown::dragBox(const Rectangle& value) { _dragBox = value; }
	void Control::MouseDown::dragBox(int x, int y, int width, int height) { _dragBox = Rectangle(x, y, width, height); }



	Control::MouseWheel::MouseWheel(Control& sender, Message& message)
		: MouseEvent(sender, Mouse::none, message) {
	}


	Point Control::MouseWheel::position() const {
		POINT point = { GET_X_LPARAM(_message.lparam), GET_Y_LPARAM(_message.lparam) };
		verify(ScreenToClient(_message.handle, &point));
		return point;
	}


	int Control::MouseWheel::wheelDelta() const {
		return GET_WHEEL_DELTA_WPARAM(_message.wparam);
	}



	Control::PopupMenu::PopupMenu(Control& sender, const Point& position, bool clicked)
		: Control::Event(sender), _position(position), _clicked(clicked), _cancel(false) {
	}


	bool Control::PopupMenu::cancel() const { return _cancel; }
	void Control::PopupMenu::cancel(bool value) { _cancel = value; }
	bool Control::PopupMenu::clicked() const { return _clicked; }
	const Point& Control::PopupMenu::position() const { return _position; }



	Control::ShortcutKey::ShortcutKey(Control& sender, int shortcut, bool isInputKey)
		: Control::Event(sender), _shortcut(shortcut), _isInputKey(isInputKey), _handled(false) {
	}


	bool Control::ShortcutKey::handled() const { return _handled; }
	void Control::ShortcutKey::handled(bool value) { _handled = value; }
	bool Control::ShortcutKey::isInputKey() const { return _isInputKey; }
	void Control::ShortcutKey::isInputKey(bool value) { _isInputKey = value; }
	int Control::ShortcutKey::shortcut() const { return _shortcut; }
	void Control::ShortcutKey::shortcut(int value) { _shortcut = value; }



	Control::Control()
		: _defaultProcedure(nullptr)
		, _mouseTracked(false)
		, _dragBox(hugeBox)
		, _brush(nullptr)
		, _brushOrigin(0, 0)
		, _mouseHoverTime(100)
		, _tabIndex(0)
		, _scalable(true)
	{
	}


	Control::Control(Control&& value)
		: Referenceable(move(value))
		, _handle(move(value._handle))
		, _defaultProcedure(move(value._defaultProcedure))
		, _mouseTracked(move(value._mouseTracked))
		, _dragBox(move(value._dragBox))
		, _brush(move(value._brush))
		, _brushOrigin(move(value._brushOrigin))
		, _mouseHoverTime(move(value._mouseHoverTime))
		, _name(move(value._name))
		, _tabIndex(move(value._tabIndex))
		, _scalable(move(value._scalable))
		, _userData(move(value._userData))
		, _onActivate(move(value._onActivate))
		, _onCursorChange(move(value._onCursorChange))
		, _onDeactivate(move(value._onDeactivate))
		, _onDrag(move(value._onDrag))
		, _onFocus(move(value._onFocus))
		, _onHelpRequest(move(value._onHelpRequest))
		, _onKeyDown(move(value._onKeyDown))
		, _onKeyPress(move(value._onKeyPress))
		, _onKeyUp(move(value._onKeyUp))
		, _onDefocus(move(value._onDefocus))
		, _onMouseDoubleClick(move(value._onMouseDoubleClick))
		, _onMouseDown(move(value._onMouseDown))
		, _onMouseEnter(move(value._onMouseEnter))
		, _onMouseHover(move(value._onMouseHover))
		, _onMouseLeave(move(value._onMouseLeave))
		, _onMouseMove(move(value._onMouseMove))
		, _onMouseUp(move(value._onMouseUp))
		, _onMouseWheel(move(value._onMouseWheel))
		, _onPopupMenu(move(value._onPopupMenu))
		, _onShortcutKey(move(value._onShortcutKey))
	{
		value._defaultProcedure = nullptr;
		// this ポインタが変わったので関連を繋ぎなおす
		if (_handle) {
			_handle.control(this);
		}
	}


	Control::~Control() {
		destroyHandle();
	}


	Rectangle Control::bounds() const {
		return _handle.bounds();
	}


	void Control::bounds(const Rectangle& value) {
		_handle.bounds(value);
	}


	void Control::bounds(int x, int y, int width, int height) {
		bounds(Rectangle(x, y, width, height));
	}


	Brush Control::brush() const {
		return Brush(_brush);
	}


	void Control::brush(HBRUSH value) {
		if (value != _brush) {
			_brush = value;
			invalidate();
		}
	}


	Point Control::brushOrigin() const {
		return _brushOrigin;
	}


	void Control::brushOrigin(const Point& value) {
		if (value != _brushOrigin) {
			_brushOrigin = value;
			invalidate();
		}
	}


	void Control::brushOrigin(int x, int y) {
		brushOrigin(Point(x, y));
	}


	bool Control::captured() const {
		return GetCapture() == handle();
	}


	void Control::captured(bool value) {
		if (value) {
			SetCapture(handle());
		}
		else {
			//verify(ReleaseCapture());
			ReleaseCapture();
		}
	}


	Point Control::clientOrigin() const {
		auto position = this->position();
		auto parent = this->parent();
		if (parent) {
			position = parent->clientToScreen(position);
		}
		auto client = clientToScreen(Point(0, 0));
		return client - position;
	}


	Rectangle Control::clientRectangle() const {
		RECT rect;
		verify(GetClientRect(handle(), &rect));
		return rect;
	}


	Size Control::clientSize() const {
		RECT rect;
		verify(GetClientRect(handle(), &rect));
		return Size(rect.right - rect.left, rect.bottom - rect.top);
	}


	void Control::clientSize(const Size& value) {
		size(sizeFromClientSize(value));
	}


	void Control::clientSize(int width, int height) {
		clientSize(Size(width, height));
	}


	Size Control::clientSizeFromSize(const Size& size) const {
		return Size::maximize(Size(0, 0), size - sizeFromClientSize(Size(0, 0)));
	}


	Point Control::clientToScreen(const Point& point) const {
		POINT winPoint = point;
		verify(ClientToScreen(handle(), &winPoint));
		return winPoint;
	}


	Rectangle Control::clientToScreen(const Rectangle& rect) const {
		RECT winRect = rect;
		MapWindowPoints(handle(), nullptr, reinterpret_cast<POINT*>(&winRect), 2);
		return winRect;
	}


	bool Control::contains(HWND control) const {
		assert("Control not created" && *this);
		assert("Control not created" && IsWindow(control));
		return IsChild(*this, control) != 0;
	}


	bool Control::containsFocus() const {
		HWND focusHandle = GetFocus();
		return focusHandle == *this || contains(focusHandle);
	}


	Control::ControlsIterator Control::controlsBegin() const {
		return ControlsIterator(*this);
	}


	Control::ControlsIterator Control::controlsLast() const {
		return ControlsIterator::getLast(*this);
	}


	Control::DescendantsIterator Control::descendantsBegin() const {
		return DescendantsIterator(*this);
	}


	Font Control::defaultFont() {
		return Font::defaultGui();
	}


	Rectangle Control::displayRectangle() const {
		return clientRectangle();
	}


	void Control::drawTo(HBITMAP bitmap) const {
		assert("Null bitmap" && bitmap);
		Graphics graphics(bitmap);
		SendMessageW(handle(), WM_PRINT, (WPARAM)static_cast<HDC>(graphics), PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT);
	}


	bool Control::enabled() const {
		return IsWindowEnabled(*this) != 0;
	}


	void Control::enabled(bool value) {
		EnableWindow(handle(), value ? TRUE : FALSE);
		for (auto i = controlsBegin(); i; ++i) {
			i->enabled(value);
		}
	}


	Frame* Control::findFrame() {
		assert("Control not created" && *this);
		auto frame = fromHandle(GetAncestor(handle(), GA_ROOT));
		return dynamic_cast<Frame*>(frame);
	}


	Rectangle Control::firstBounds() const {
		auto i = controlsBegin();
		return i ? i->bounds() : Rectangle(0, 0, 0, 0);
	}


	void Control::focus() {
		assert("focus invidible control" && visible()); // 非表示のコントロールにフォーカスを移す事ができるが無意味である。
		SetFocus(handle());
	}


	bool Control::focusable() const {
		return visible() && enabled();
	}


	bool Control::focused() const {
		return *this == GetFocus();
	}


	Font Control::font() const {
		auto font = reinterpret_cast<HFONT>(SendMessageW(handle(), WM_GETFONT, 0, 0));
		return font ? Font(font) : Font::system();
	}


	void Control::font(HFONT value) {
		SendMessageW(handle(), WM_SETFONT, (WPARAM)value, FALSE);
		invalidate(); // Label や GroupBox 等一部コントロールは更新されないので
	}


	Control* Control::fromHandle(HWND handle) {
		if (!handle || Handle(handle).differentProcess()) {
			return nullptr;
		}
		assert("Invalid window handle" && IsWindow(handle));
		return Handle(handle).control();
	}


	Control* Control::getChildAt(const Point& clientPoint, bool skipInvisible, bool skipDisabled, bool skipTransparent) const {
		UINT skip = 0;
		if (skipInvisible) {
			skip |= CWP_SKIPINVISIBLE;
		}
		if (skipDisabled) {
			skip |= CWP_SKIPDISABLED;
		}
		if (skipTransparent) {
			skip |= CWP_SKIPTRANSPARENT;
		}
		const HWND childHandle = ChildWindowFromPointEx(handle(), clientPoint, skip);
		Control* child = Handle(childHandle).control();
		return child == this ? nullptr : child;
	}


	Control* Control::getNextControl(Control& start, bool forward) const {
		assert("start not created" && start);
		assert("Invalid start" && &start == this || contains(start));
		const Control* control = &start;
		// start に this を指定した場合、自分の子供のなかからタブオーダーの最小（forward) あるいは最大のものを返す
		if (forward) {
			// 子コントロールから探す
			auto i = ControlsIterator(*control);
			if (i) {
				Control* found = &(*i);
				while (++i) { // 子コントロールの中で最小の tabIndex を持つものを探す
					if (i->tabIndex() < found->tabIndex()) {
						found = &(*i);
					}
				}
				return found;
			}

			// 兄弟コントロールから探す
			while (control != this) {
				auto parent = control->parent();
				bool hitControl = false;
				Control* found = nullptr;
				for (auto i = parent->controlsBegin(); i; ++i) { // 兄弟の中で次のタブオーダーを持つものを探す
					if (control != i) {
						if (control->tabIndex() <= i->tabIndex()) { // control よりも tabIndex が大きく
							if (!found || i->tabIndex() < found->tabIndex()) { // 以前見つけたものより tabIndex が小さい。（より control に近い）
								if (i->tabIndex() != control->tabIndex() || hitControl) { // control と同じタブオーダーなら Z オーダーが次のもの
									found = i;
								}
							}
						}
					}
					else {
						hitControl = true;
					}
				}
				if (found) {
					return found;
				}
				control = parent; // みつからなければ親の階層へ
			}
		}
		else {
			// 兄弟コントロールから探す
			if (control != this) {
				auto parent = control->parent();
				bool hitControl = false;
				Control* found = nullptr;
				for (auto i = parent->controlsLast(); i; --i) { // 兄弟の中で前のタブオーダーを持つものを探す
					if (control != i) {
						if (i->tabIndex() <= control->tabIndex()) { // control より tabIndex が小さく
							if (!found || found->tabIndex() < i->tabIndex()) { // 以前見つけたものより tabIndex が大きい。（より control に近い）
								if (i->tabIndex() != control->tabIndex() || hitControl) { // control と同じタブオーダーなら Z オーダーが前のもの
									found = i;
								}
							}
						}
					}
					else {
						hitControl = true;
					}
				}

				if (found) {
					control = found;
				}
				else if (parent == this) {
					return nullptr;
				}
				else {
					return parent;
				}
			}

			// 子コントロールから探す
			auto i = ControlsIterator::getLast(*control);
			while (i) { // 最も低い子孫の階層で最大の tabIndex を持つものを探す
				Control* found = &(*i);
				while (--i) { // 子コントロールの中で最後の tabIndex を持つものを探す
					if (found->tabIndex() < i->tabIndex()) {
						found = &(*i);
					}
				}
				control = found;
				i = ControlsIterator::getLast(*control);
			}
		}
		return const_cast<Control*>(control == this ? nullptr : control);
	}


	Control* Control::getNextControl(Control& start, bool forward, bool tabStopOnly, bool nested, bool wrap) {
		assert("start not created" && start);
		assert("Invalid start" && (&start == this || contains(start)));
		assert("Invalid start" && (nested || start.parent() == this));

		Control* i = &start;
		bool alreadyWrapped = false;
		do {
			i = getNextControl(*i, forward);
			if (i) {
				if (i->focusable() && (!tabStopOnly || i->tabStop()) && (nested || i->parent() == this)) {
					return i;
				}
			}
			else {
				if (!wrap || alreadyWrapped) {
					break;
				}
				alreadyWrapped = true;
				i = this;
			}
		} while (i != &start);
		return nullptr;
	}


	Size Control::getPreferredSize(int width, int height) const {
		if (width && height) {
			return Size(width, height);
		}
		Size size;
		auto i = controlsBegin();
		if (i) {
			size = Size(0, 0);
			const auto displayRect = displayRectangle();
			for (; i; ++i) {
				if (!i->visibleExceptParent()) {
					continue;
				}
				Rectangle bounds = i->bounds();
				bounds.position(bounds.position() - displayRect.position()); // スクロールを打ち消す。
				size.width = max(size.width, bounds.right());
				size.height = max(size.height, bounds.bottom());
			}
			size = sizeFromClientSize(size);
		}
		else {
			size = this->size();
		}
		return Size(width ? width : size.width
			, height ? height : size.height);
	}


	HWND Control::handle() const {
		assert("Handle not created" && *this);
		assert("Invoke required" && !invokeRequired());
		return _handle;
	}


	bool Control::hot() const {
		return _mouseTracked;
	}


	int Control::id() const {
		return static_cast<int>(GetWindowLongPtrW(_handle, GWLP_ID));
	}


	void Control::id(int value) {
		SetWindowLongPtrW(_handle, GWLP_ID, value);
	}


	void Control::invalidate(bool invalidateChildren) {
		if (invalidateChildren) {
			verify(RedrawWindow(*this, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN));
		}
		else {
			verify(InvalidateRect(handle(), nullptr, TRUE));
		}
	}


	void Control::invalidate(const Rectangle& rect, bool invalidateChildren) {
		const RECT winRect = rect;
		if (invalidateChildren) {
			verify(RedrawWindow(*this, &winRect, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN));
		}
		else {
			verify(InvalidateRect(handle(), &winRect, TRUE));
		}
	}


	void Control::invalidate(HRGN region, bool invalidateChildren) {
		if (!region) {
			invalidate(invalidateChildren);
			return;
		}
		if (invalidateChildren) {
			verify(RedrawWindow(*this, nullptr, region, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN));
		}
		else {
			verify(InvalidateRgn(handle(), region, TRUE));
		}
	}


	void Control::invoke(const std::function<void()>& function, bool synchronous) {
		assert("Control not created" && *this);
		assert("Different process handle" && !_handle.differentProcess());
		if (synchronous) {
			DWORD lresult;
			verify(SendMessageTimeoutW(*this, wmInvoke, (WPARAM)&function, 0, SMTO_NOTIMEOUTIFNOTHUNG, 5000, &lresult));
		}
		else {
			std::function<void()>* newFunction = new std::function<void()>(function);
			verify(PostMessageW(*this, wmInvokeAsynchronous, (WPARAM)newFunction, 0));
		}
	}


	Rectangle Control::lastBounds() const {
		auto i = controlsLast();
		return i ? i->bounds() : Rectangle(0, 0, 0, 0);
	}


	bool Control::invokeRequired() const {
		assert("Control not created" && *this);
		return _handle.differentThread();
	}


	int Control::mouseHoverTime() const {
		return _mouseHoverTime;
	}


	void Control::mouseHoverTime(int value) {
		assert("Negative mouseHoverTime" && 0 <= value);
		_mouseHoverTime = value;
	}


	const String& Control::name() const {
		return _name;
	}


	void Control::name(String value) {
		_name = move(value);
	}


	Listener<Control::Activate&>& Control::onActivate() { return _onActivate; }
	Listener<Control::CursorChange&>& Control::onCursorChange() { return _onCursorChange; }
	Listener<Control::Deactivate&>& Control::onDeactivate() { return _onDeactivate; }
	Listener<Control::Defocus&>& Control::onDefocus() { return _onDefocus; }
	Listener<Control::Drag&>& Control::onDrag() { return _onDrag; }
	Listener<Control::Focus&>& Control::onFocus() { return _onFocus; }
	Listener<Control::HelpRequest&>& Control::onHelpRequest() { return _onHelpRequest; }
	Listener<Control::KeyDown&>& Control::onKeyDown() { return _onKeyDown; }
	Listener<Control::KeyPress&>& Control::onKeyPress() { return _onKeyPress; }
	Listener<Control::KeyUp&>& Control::onKeyUp() { return _onKeyUp; }
	Listener<Control::MouseDoubleClick&>& Control::onMouseDoubleClick() { return _onMouseDoubleClick; }
	Listener<Control::MouseDown&>& Control::onMouseDown() { return _onMouseDown; }
	Listener<Control::MouseEnter&>& Control::onMouseEnter() { return _onMouseEnter; }
	Listener<Control::MouseHover&>& Control::onMouseHover() { return _onMouseHover; }
	Listener<Control::MouseLeave&>& Control::onMouseLeave() { return _onMouseLeave; }
	Listener<Control::MouseMove&>& Control::onMouseMove() { return _onMouseMove; }
	Listener<Control::MouseUp&>& Control::onMouseUp() { return _onMouseUp; }
	Listener<Control::MouseWheel&>& Control::onMouseWheel() { return _onMouseWheel; }
	Listener<Control::PopupMenu&>& Control::onPopupMenu() { return _onPopupMenu; }
	Listener<Control::ShortcutKey&>& Control::onShortcutKey() { return _onShortcutKey; }


	Control* Control::parent() const {
		auto parent = _handle.parent().control();
		assert("Invalid parent control pointer" && !parent || dynamic_cast<Control*>(parent));
		return reinterpret_cast<Control*>(parent);
	}


	void Control::parent(Control* value) {
		auto oldParent = parent();
		if (value == oldParent) {
			return;
		}
		auto frame = findFrame();
		if (frame && (!value || value->findFrame() != frame)) {
			frame->processDescendantErased(*this);
		}
		if (value) {
			assert("Different thread parent" && !value->_handle.differentThread());
			assert("Circular owner" && _handle.checkParentingCycle(*value));

			SetParent(handle(), value->handle());
			if (!value->enabled()) {
				enabled(false);
			}
		}
		else {
			SetParent(handle(), nullptr);
		}
	}


	Point Control::position() const {
		return bounds().position();
	}


	void Control::position(const Point& value) {
		if (value != position()) {
			verify(SetWindowPos(handle(), nullptr, value.x, value.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE));
		}
	}


	void Control::position(int x, int y) {
		position(Point(x, y));
	}


	Region Control::region() const {
		Region region(0, 0, 0, 0);
		if (GetWindowRgn(handle(), region) == ERROR) { // リージョン領域が設定されていなければ作成しない（空領域のままだとウインドウに設定すると表示されなくなるので挙動がおかしくなる）
			region = Region();
		}
		return region;
	}


	void Control::region(HRGN value) {
		assert("Control not created" && *this);
		Region region;
		if (value) {
			region = Region::clone(value);
		}
		if (SetWindowRgn(handle(), region, visible())) {
			region.owned(false); // 所有権はウインドウに移った
		}
		else {
			assert("Failed to SetWindowRgn" && false);
		}
	}


	void Control::resize() {
		size(getPreferredSize());
	}


	Point Control::screenToClient(const Point& point) const {
		POINT winPoint = point;
		verify(ScreenToClient(handle(), &winPoint));
		return winPoint;
	}


	Rectangle Control::screenToClient(const Rectangle& rect) const {
		RECT winRect = rect;
		MapWindowPoints(nullptr, handle(), reinterpret_cast<POINT*>(&winRect), 2);
		return winRect;
	}


	bool Control::scalable() const {
		return _scalable;
	}


	void Control::scalable(bool value) {
		_scalable = value;
	}


	void Control::scale(const Scaler& scaler) {
		if (scalable()) {
			bounds(scaler.scale(bounds()));
			for (auto i = controlsBegin(); i; ++i) {
				i->scale(scaler);
			}
		}
		else {
			position(scaler.scale(position()));
			// 子をスケールすべきか？どうだろうか。
		}
	}


	Size Control::size() const {
		RECT rect;
		verify(GetWindowRect(handle(), &rect));
		return Size(rect.right - rect.left, rect.bottom - rect.top);
	}


	void Control::size(const Size& value) {
		if (value != size()) {
			verify(SetWindowPos(handle(), nullptr, 0, 0, value.width, value.height, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE));
		}
	}


	void Control::size(int width, int height) {
		size(Size(width, height));
	}


	Size Control::sizeFromClientSize(const Size& clientSize) const {
		RECT rect = { 0, 0, clientSize.width, clientSize.height };
		verify(AdjustWindowRectEx(&rect, _handle.style(), FALSE, _handle.exStyle()));
		return Size(rect.right - rect.left, rect.bottom - rect.top);
	}


	int Control::tabIndex() const {
		return _tabIndex;
	}


	void Control::tabIndex(int value) {
		assert("Negative tabIndex" && 0 <= value);
		_tabIndex = value;
	}


	bool Control::tabStop() const {
		return _handle.hasStyle(WS_TABSTOP);
	}


	void Control::tabStop(bool value) {
		_handle.style(toFlag(_handle.style()).set(WS_TABSTOP, value));
	}


	String Control::text() const {
		assert("Control not created" && *this);
		int length = GetWindowTextLengthW(*this);
		StringBuffer buffer(length + 1);
		GetWindowTextW(*this, buffer.begin(), length + 1);
		buffer.length(length);
		return move(buffer);
	}


	void Control::text(StringRange value) {
		verify(SetWindowTextW(handle(), value.c_str()));
	}


	void Control::textToBuffer(StringBuffer& buffer) const {
		assert("Control not created" && *this);
		int length = GetWindowTextLengthW(*this);
		buffer.reserveAdditionally(length);
		GetWindowTextW(*this, buffer.end(), length + 1);
		buffer.expandLength(length);
	}


	void Control::updatable(bool value) {
		SendMessageW(handle(), WM_SETREDRAW, value ? TRUE : FALSE, 0);
	}


	void Control::update() {
		verify(UpdateWindow(handle()));
	}


	UniqueAny& Control::userData() {
		return _userData;
	}


	void Control::userData(UniqueAny&& value) {
		_userData = move(value);
	}


	bool Control::visible() const {
		assert("Control not created" && *this);
		return IsWindowVisible(*this) != 0;
	}


	void Control::visible(bool value) {
		//if (value != visible() || value != visibleExceptParent()) {
		if (value != visibleExceptParent()) {
			verify(SetWindowPos(handle(), nullptr, 0, 0, 0, 0
				, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | (value ? SWP_SHOWWINDOW : SWP_HIDEWINDOW)));
		}
	}


	bool Control::visibleExceptParent() const {
		return _handle.hasStyle(WS_VISIBLE);
	}


	bool Control::visualStyle() const {
		return GetWindowTheme(handle()) != nullptr;
	}


	void Control::visualStyle(bool value) {
		if (value != visualStyle()) {
			verify(SetWindowTheme(handle(), value ? 0 : L" ", value ? 0 : L" ") == S_OK);
		}
	}


	int Control::zOrder() const {
		Control* parent = this->parent();
		if (!parent) {
			return 0;
		}
		int order = 0;
		for (auto i = parent->controlsBegin(); i->handle() != handle(); ++i, ++order) {
		}
		return order;
	}


	void Control::zOrder(int value) {
		HWND next;
		if (value == 0) {
			next = HWND_TOP;
		}
		else if (value == -1) {
			next = HWND_BOTTOM;
		}
		else {
			Control* parent = this->parent();
			if (!parent) {
				return;
			}
			auto j = parent->controlsBegin();
			for (int i = 1; i < value && j; ++j) {
				if (j->handle() != handle()) { // 自分自身は飛ばしてカウント
					++i;
				}
			}
			if (j) {
				next = j->handle();
			}
			else {
				next = HWND_BOTTOM;
			}
		}
		verify(SetWindowPos(handle(), next, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
	}


	void Control::attachHandle(HWND handle) {
		assert("handle already created" && !*this);
		assert("Null handle" && handle);
		assert("handle already attached" && !Handle(handle).control());
		_handle = Handle(handle);
		_handle.control(this);
		_defaultProcedure = _handle.procedure(Handle::standardProcedure);
		if (!SendMessageW(handle, WM_GETFONT, 0, 0)) {
			font(defaultFont());
		}
	}


	void Control::beginMouseTracking(int mouseHoverTime) {
		if (!_mouseTracked) {
			_mouseTracked = true;
			resetMouseTracking(mouseHoverTime);
			MouseEnter event(*this);
			onMouseEnter()(event);
		}
	}


	void Control::destroyHandle() {
		if (_handle) {
			if (IsWindow(_handle)) {
				assert("Invalid operation. invoke required" && !invokeRequired());
				verify(DestroyWindow(_handle));
			}
			_handle = Handle();
			_defaultProcedure = nullptr;
		}
	}


	void Control::endMouseTracking() {
		if (_mouseTracked) {
			MouseLeave event(*this);
			onMouseLeave()(event);
			_mouseTracked = false;
		}
	}


	bool Control::isInputKey(int shortcut) const {
		if (shortcut & Key::Modifier::alt) {
			return false;
		}
		LRESULT result = SendMessageW(handle(), WM_GETDLGCODE, 0, 0);
		int code = shortcut & ~Key::Modifier::mask;
		int mask;
		if (code == Key::tab) {
			mask = DLGC_WANTALLKEYS | DLGC_WANTTAB;
		}
		else if (Key::left <= code && code <= Key::down) {
			mask = DLGC_WANTALLKEYS | DLGC_WANTARROWS;
		}
		else {
			mask = DLGC_WANTALLKEYS;
		}
		return (result & mask) != 0;
	}


	bool Control::preTranslateMessage(Message& msg) {
		if (msg.message != WM_KEYDOWN && msg.message != WM_SYSKEYDOWN) {
			return false;
		}
		Control* control = Handle(msg.handle).control();
		if (!control) {
			return false;
		}

		int shortcut = msg.wparam;
		if (msg.lparam & (1 << 29)) {
			shortcut |= Key::Modifier::alt;
		}
		if (Key::isPushed(Key::ctrl)) {
			shortcut |= Key::Modifier::ctrl;
		}
		if (Key::isPushed(Key::shift)) {
			shortcut |= Key::Modifier::shift;
		}

		ShortcutKey event(*control, shortcut, control->isInputKey(shortcut));
		control->onShortcutKey()(event);
		if (event.handled()) {
			return !event.isInputKey();
		}
		Frame* frame = control->findFrame();
		if (frame) {
			frame->onShortcutKey()(event);
			if (event.handled()) {
				return !event.isInputKey();
			}
		}
		if (event.isInputKey()) {
			return false;
		}
		return control->processDialogKey(event.shortcut());
	}


	bool Control::processDialogKey(int shortcut) {
		Control* frame = findFrame();
		if (frame) {
			return frame->processDialogKey(shortcut);
		}
		return false;
	}


	void Control::processMessage(Message& msg) {
		switch (msg.message) {
		case WM_CHAR: {
			KeyPress event(*this, (wchar_t)msg.wparam);
			onKeyPress()(event);
			msg.wparam = event.charCode();
			if (!event.handled()) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_COMMAND: {
			if (!reflectMessage((HWND)msg.lparam, msg)) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_CONTEXTMENU: {
			if (onPopupMenu()) {
				Point position;
				const bool clicked = msg.lparam != -1;
				if (clicked) {
					position = Point(GET_X_LPARAM(msg.lparam), GET_Y_LPARAM(msg.lparam));
					position = screenToClient(position);
				}
				else {
					position = Point(0, 0);
				}
				if (clientRectangle().contains(position)) { // クライアント領域に限定する
					auto frame = findFrame();
					if (!frame || !frame->menuBarFocused()) { // メニューバーの onRightClick イベントがうまくいかなくなるのでメニューバーを操作中は表示しない。
						PopupMenu event(*this, position, clicked);
						onPopupMenu()(event);
						if (!event.cancel()) {
							return;
						}
					}
				}
			}
			processMessageByDefault(msg);
		} break;
		case WM_CTLCOLORBTN:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC: {
			if (!reflectMessage((HWND)msg.lparam, msg)) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_DELETEITEM: {
			auto info = (DELETEITEMSTRUCT*)msg.lparam;
			if (!reflectMessage(info->hwndItem, msg)) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_DESTROY: {
			endMouseTracking();
			auto frame = findFrame();
			if (frame) {
				frame->processDescendantErased(*this);
			}
			processMessageByDefault(msg);
		} break;
		case WM_HELP: {
			if (onHelpRequest()) {
				const HELPINFO* info = (HELPINFO*)msg.lparam;
				if (info->iContextType == HELPINFO_WINDOW) { // メニューのヘルプなんて使い方知ってる人少ないだろう。
					Control* target = fromHandle((HWND)info->hItemHandle);
					if (target) {
						HelpRequest event(*this, *target, screenToClient(info->MousePos));
						onHelpRequest()(event);
						if (!event.cancel()) {
							msg.result = 1;
							return;
						}
					}
				}
			}
			processMessageByDefault(msg);
		} break;
		case WM_HSCROLL:
		case WM_VSCROLL: {
			if (!reflectMessage((HWND)msg.lparam, msg)) { // スクロールバーコントロール（独立したコントロールのほう）は親に直接メッセージを送る。
				processMessageByDefault(msg);
			}
		} break;
		case WM_INITMENUPOPUP: {
			HMENU handle = (HMENU)msg.wparam;
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::PopupBegin event(*menu, *this);
				menu->onPopupBegin()(event);
			}
			processMessageByDefault(msg);
		} break;
		case wmInvoke: {
			auto function = (std::function<void()>*)msg.wparam;
			assert(function);
			(*function)();
		} break;
		case wmInvokeAsynchronous: {
			auto function = (std::function<void()>*)msg.wparam;
			assert(function);
			scopeExit([&]() {
				delete function;
			});
			(*function)();
		} break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN: {
			KeyDown event(*this, (Key)msg.wparam, msg.lparam);
			onKeyDown()(event);
			if (!event.handled()) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			KeyUp event(*this, (Key)msg.wparam, msg.lparam);
			onKeyUp()(event);
			if (!event.handled()) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_KILLFOCUS: {
			processMessageByDefault(msg);
			Defocus event(*this, (HWND)msg.wparam);
			onDefocus()(event);
		} break;
		case WM_LBUTTONDBLCLK: {
			processMessageByDefault(msg);
			MouseDoubleClick event(*this, Mouse::lButton, msg);
			onMouseDoubleClick()(event);
		} break;
		case WM_LBUTTONDOWN: {
			processMessageByDefault(msg);
			processMouseDown(MouseDown(*this, Mouse::lButton, msg, _dragBox));
		} break;
		case WM_LBUTTONUP: {
			processMessageByDefault(msg);
			processMouseUp(MouseUp(*this, Mouse::lButton, msg));
		} break;
		case WM_MBUTTONDBLCLK: {
			processMessageByDefault(msg);
			MouseDoubleClick event(*this, Mouse::mButton, msg);
			onMouseDoubleClick()(event);
		} break;
		case WM_MBUTTONDOWN: {
			processMessageByDefault(msg);
			processMouseDown(MouseDown(*this, Mouse::mButton, msg, _dragBox));
		} break;
		case WM_MBUTTONUP: {
			processMessageByDefault(msg);
			processMouseUp(MouseUp(*this, Mouse::mButton, msg));
		} break;
		case WM_MENUCOMMAND: {
			int index = msg.wparam;
			HMENU handle = (HMENU)msg.lparam;
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::Item& item = (*menu)[index];
				if (item.shortcut()) {
					ShortcutKey event(*this, item.shortcut(), false);
					onShortcutKey()(event);
				}
				else {
					Menu::Click event(item, *this);
					item.onClick()(event);
				}
			}
			//processMessageByDefault(msg);
		} break;
		case WM_MENUDRAG: {
			HMENU handle = (HMENU)msg.lparam;
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::Item& item = (*menu)[msg.wparam];
				Menu::Drag event(item, *this);
				item.onDrag()(event);
				msg.result = event.endMenu() ? MND_ENDMENU : MND_CONTINUE;
			}
			//processMessageByDefault(msg);
		} break;
		case WM_MENURBUTTONUP: {
			HMENU handle = (HMENU)msg.lparam;
			int index = msg.wparam;
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::Item& item = (*menu)[index];
				Menu::RightClick event(item, *this);
				item.onRightClick()(event);
			}
		} break;
		case WM_MENUSELECT: {
			HMENU handle = (HMENU)msg.lparam;
			int id = LOWORD(msg.wparam);
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::Item& item = (*menu)[id];
				Menu::Select event(item, *this);
				item.onSelect()(event);
			}
			processMessageByDefault(msg);
		} break;
		case WM_MOUSEHOVER: {
			processMessageByDefault(msg);
			MouseHover event(*this);
			onMouseHover()(event);
		} break;
		case WM_MOUSELEAVE: {
			processMessageByDefault(msg);
			endMouseTracking();
			_dragBox = hugeBox; // ウインドウのはじっこでボタンを押してウインドウ外にでて戻ってくるとドラッグが発生するのを防ぐ
		} break;
		case WM_MOUSEMOVE: {
			beginMouseTracking(mouseHoverTime());
			processMessageByDefault(msg);
			MouseMove event(*this, Mouse::none, msg);
			onMouseMove()(event);
			if (!_dragBox.contains(event.position())) {
				onDrag()(event);
				_dragBox = hugeBox;
			}
		} break;
		case WM_MOUSEWHEEL: {
			resetMouseTracking(mouseHoverTime());
			processMessageByDefault(msg);
			MouseWheel event(*this, msg);
			onMouseWheel()(event);
		} break;
		case WM_NOTIFY: {
			NMHDR* nmhdr = (NMHDR*)msg.lparam;
			if (!reflectMessage(nmhdr->hwndFrom, msg)) {
				processMessageByDefault(msg);
			}
		} break;
		case WM_RBUTTONDBLCLK: {
			processMessageByDefault(msg);
			MouseDoubleClick event(*this, Mouse::rButton, msg);
			onMouseDoubleClick()(event);
		} break;
		case WM_RBUTTONDOWN: {
			processMessageByDefault(msg);
			processMouseDown(MouseDown(*this, Mouse::rButton, msg, _dragBox));
		} break;
		case WM_RBUTTONUP: {
			processMessageByDefault(msg);
			processMouseUp(MouseUp(*this, Mouse::rButton, msg));
		} break;
		case wmReflect + WM_COMMAND:
		case wmReflect + WM_CTLCOLORBTN:
		case wmReflect + WM_CTLCOLOREDIT:
		case wmReflect + WM_CTLCOLORLISTBOX:
		case wmReflect + WM_CTLCOLORSCROLLBAR:
		case wmReflect + WM_CTLCOLORSTATIC:
		case wmReflect + WM_DELETEITEM:
		case wmReflect + WM_HSCROLL:
		case wmReflect + WM_VSCROLL:
		case wmReflect + WM_NOTIFY: {
			auto parent = fromHandle(msg.handle);
			assert(parent);
			if (parent) {
				msg.message -= wmReflect;
				parent->processMessageByDefault(msg);
				msg.message += wmReflect;
			}
		} break;
		case WM_SETCURSOR: {
			if ((HWND)msg.wparam == handle() && LOWORD(msg.lparam) == HTCLIENT) {
				CursorChange event(*this);
				onCursorChange()(event);
				if (event.cursor()) {
					Mouse::cursor(event.cursor());
					msg.result = TRUE;
					return;
				}
			}
			processMessageByDefault(msg);
		} break;
		case WM_SETFOCUS: {
			processMessageByDefault(msg);
			auto frame = findFrame();
			if (frame) {
				frame->processDescendantFocused(*this);
			}
			Focus event(*this, (HWND)msg.wparam);
			onFocus()(event);
		} break;
		case WM_UNINITMENUPOPUP: {
			HMENU handle = (HMENU)msg.wparam;
			auto menu = Menu::Item::fromHandle(handle);
			if (menu) {
				Menu::PopupEnd event(*menu, *this);
				menu->onPopupEnd()(event);
			}
			processMessageByDefault(msg);
		} break;
		case WM_XBUTTONDBLCLK: {
			processMessageByDefault(msg);
			MouseDoubleClick event(*this, getXButton(msg), msg);
			onMouseDoubleClick()(event);
		} break;
		case WM_XBUTTONDOWN: {
			processMessageByDefault(msg);
			MouseDown event(*this, getXButton(msg), msg, _dragBox);
			onMouseDown()(event);
		} break;
		case WM_XBUTTONUP: {
			processMessageByDefault(msg);
			MouseUp event(*this, getXButton(msg), msg);
			onMouseUp()(event);
		} break;
		default: {
			processMessageByDefault(msg);
		} break;
		}
	}


	void Control::processMessage(Control& control, Message& msg) {
		control.processMessage(msg);
	}


	void Control::processMessageByDefault(Message& msg) {
		assert("Invalid handle" && IsWindow(_handle));
		assert("Null defaultProcedure" && _defaultProcedure);
		assert("Different thread" && !Handle(msg.handle).differentThread());
		assert("Different handle" && handle() == msg.handle);
		//message.result = (*_defaultProcedure)(_handle, message.msg, message.wparam, message.lparam);
		msg.result = CallWindowProcW(_defaultProcedure, _handle, msg.message, msg.wparam, msg.lparam);
	}


	void Control::processMouseDown(MouseDown&& event) {
		auto dragSize = DragDrop::defaultDragSize();
		_dragBox = Rectangle(event.position() - Point(dragSize / 2), dragSize);
		onMouseDown()(event);
	}


	void Control::processMouseUp(MouseUp&& event) {
		if (!event.lButton()
			&& !event.mButton()
			&& !event.rButton()) {
			_dragBox = hugeBox;
		}
		onMouseUp()(event);
	}


	void Control::resetMouseTracking(int mouseHoverTime) {
		if (_mouseTracked) {
			TRACKMOUSEEVENT track;
			track.cbSize = sizeof(track);
			track.dwFlags = TME_LEAVE | TME_HOVER;
			track.dwHoverTime = mouseHoverTime;
			track.hwndTrack = handle();
			verify(TrackMouseEvent(&track));
		}
	}


	bool Control::reflectMessage(HWND handle, Message& msg) {
		auto control = fromHandle(handle);
		if (control) {
			msg.message += wmReflect;
			processMessage(*control, msg);
			msg.message -= wmReflect;
			return true;
		}
		return false;
	}


	const wchar_t* Control::userClassName() {
		// ウインドウクラスの登録
		class UserClassNameHolder {
			friend Singleton<UserClassNameHolder>;

			UserClassNameHolder() {
				StringBuffer buffer(className);
				// クラス名はモジュールパスとする
				buffer += String::refer(Module::current().file());
				// 記号が含まれるとダメなので置換
				buffer.replace(L'\\', L'_');
				buffer.replace(L':', L'_');
				buffer.replace(L'.', L'_');

				const HINSTANCE instanceHandle = GetModuleHandleW(nullptr);
				assert(instanceHandle);

				WNDCLASSEXW wndclass;
				memset(&wndclass, 0, sizeof(wndclass));
				wndclass.cbSize = sizeof(wndclass);
				wndclass.style = CS_DBLCLKS;//CS_HREDRAW | CS_VREDRAW; // 指定するとリサイズ時に激しくちらつく
				wndclass.lpfnWndProc = DefWindowProcW;
				wndclass.cbClsExtra = 0;
				wndclass.cbWndExtra = 0;
				wndclass.hInstance = instanceHandle;
				wndclass.hIcon = nullptr;
				wndclass.hIconSm = nullptr;
				wndclass.hCursor = Cursor::default();
				wndclass.hbrBackground = Brush::control(); //SystemBrushes::hollow();
				wndclass.lpszClassName = className;
				wndclass.lpszMenuName = nullptr;

				atom = RegisterClassExW(&wndclass);
				assert(atom);
			}
			~UserClassNameHolder() {}

		public:
			wchar_t className[MAX_PATH];
			ATOM atom;
		};
		return Singleton<UserClassNameHolder>::get().className;
	}


	Control::operator HWND() const {
		//assert(!_handle || IsWindow(_handle));
		return _handle;
	}



	}
}