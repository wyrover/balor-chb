#include "ScrollableControl.hpp"

#include <algorithm>

#include <balor/gui/Scaler.hpp>
#include <balor/gui/ScrollBar.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>


namespace balor {
	namespace gui {

using std::max;
using std::min;
using std::move;



ScrollableControl::Scroll::Scroll(ScrollableControl& sender, const Point& oldPosition, const Point& newPosition)
	: ScrollableControl::Event(sender), _oldPosition(oldPosition), _newPosition(newPosition) {
}


const Point& ScrollableControl::Scroll::newPosition() const { return _newPosition; }
const Point& ScrollableControl::Scroll::oldPosition() const { return _oldPosition; }



ScrollableControl::ScrollableControl()
	: _scrollSize(0, 0)
	, _scrollMinSize(0, 0)
	, _hScrollBarFixed(false)
	, _vScrollBarFixed(false)
	, _updating(false)
	{
}


ScrollableControl::ScrollableControl(ScrollableControl&& value)
	: Control(move(value))
	, _scrollMinSize(move(value._scrollMinSize))
	, _scrollSize(move(value._scrollSize))
	, _hScrollBarFixed(move(value._hScrollBarFixed))
	, _vScrollBarFixed(move(value._vScrollBarFixed))
	, _updating(move(value._updating))
	, _onScroll(move(value._onScroll))
	{
}


ScrollableControl::~ScrollableControl() {
}


void ScrollableControl::clientSize(const Size& value) {
	Control::clientSize(value);
	auto newValue = clientSize();
	bool change = false;
	if (newValue.width  < value.width ) {
		newValue.width  = value.width  + ScrollBar::defaultVerticalWidth();
		change = true;
	}
	if (newValue.height < value.height) {
		newValue.height = value.height + ScrollBar::defaultHorizontalHeight();
		change = true;
	}
	if (change) {
		Control::clientSize(newValue);
	}
	//assert(value == clientSize()); // WM_GETMINMAXINFO で差が出る
}


Rectangle ScrollableControl::displayRectangle() const {
	return Rectangle(scrollPosition(), scrollSize());
}


bool ScrollableControl::hScrollBar() const {
	return _handle.hasStyle(WS_HSCROLL);
}


bool ScrollableControl::hScrollBarFixed() const {
	return _hScrollBarFixed;
}


void ScrollableControl::hScrollBarFixed(bool value) {
	if (value != hScrollBarFixed()) {
		_hScrollBarFixed = value;
		updateScrollBars();
	}
}


Listener<ScrollableControl::Scroll&>& ScrollableControl::onScroll() { return _onScroll; }


void ScrollableControl::scale(const Scaler& scaler) {
	if (scalable()) {
		_scrollMinSize = scaler.scale(_scrollMinSize);
	}
	Control::scale(scaler);
}


void ScrollableControl::scrollControlIntoView(Control& control, bool recursive) {
	assert("Not descendant control" && this == &control || contains(control));
	if (recursive) {
		Control* i = &control;
		do {
			ScrollableControl* scrollable;
			do {
				i = i->parent();
				scrollable = dynamic_cast<ScrollableControl*>(i);
			} while (i && !scrollable);
			if (i) {
				scrollable->scrollControlIntoView(control, false);
			}
		} while (i && i != this);
	} else if (hScrollBar() || vScrollBar()) {
		const auto clientSize = this->clientSize();
		const auto oldPosition = scrollPosition();
		const Point margin = Point(10, 10); // 仮に 10, 10 としておく
		auto bounds = control.bounds();
		if (control.parent() != this) {
			bounds = screenToClient(control.parent()->clientToScreen(bounds));
		}

		auto position = oldPosition;
		if (bounds.x < margin.x) {
			position.x = oldPosition.x + margin.x - bounds.x;
		} else if (clientSize.width  < bounds.right()  + margin.x) {
			position.x = oldPosition.x + clientSize.width  - (bounds.right()  + margin.x);
			if (bounds.x + position.x - oldPosition.x < margin.x) {
				position.x = oldPosition.x + margin.x - bounds.x;
			}
		}
		if (bounds.y < margin.y) {
			position.y = oldPosition.y + margin.y - bounds.y;
		} else if (clientSize.height < bounds.bottom() + margin.y) {
			position.y = oldPosition.y + clientSize.height - (bounds.bottom() + margin.y);
			if (bounds.y + position.y - oldPosition.y < margin.y) {
				position.y = oldPosition.y + margin.y - bounds.y;
			}
		}
		scrollPosition(position);
	}
}


Size ScrollableControl::scrollMinSize() const {
	return _scrollMinSize;
}


void ScrollableControl::scrollMinSize(const Size& value) {
	if (value != scrollMinSize()) {
		_scrollMinSize = value;
		if (value == Size(0, 0)) {
			if (vScrollBar() || hScrollBar()) {
				scrollPosition(0, 0); // 画面のスクロールを元に戻す
				// ShowScrollBar だけでは消えないケースがあるので確実に消えるように設定する
				SCROLLINFO info;
				info.cbSize = sizeof(info);
				info.fMask = SIF_RANGE | SIF_PAGE;
				info.nMin  = 0;
				info.nMax  = 1;
				info.nPage = 2;
				SetScrollInfo(handle(), SB_HORZ, &info, TRUE);
				SetScrollInfo(handle(), SB_VERT, &info, TRUE);
				// スクロールバーを消す
				verify(ShowScrollBar(*this, SB_BOTH, FALSE));
			}
		} else {
			updateScrollBars();
		}
	}
}


void ScrollableControl::scrollMinSize(int width, int height) {
	scrollMinSize(Size(width, height));
}


Point ScrollableControl::scrollPosition() const {
	return Point(hScrollBar() ? -GetScrollPos(handle(), SB_HORZ) : 0
				,vScrollBar() ? -GetScrollPos(handle(), SB_VERT) : 0);
}


void ScrollableControl::scrollPosition(const Point& value) {
	Point position = Point::minimize(value, Point(0, 0)); // 最大値（負数）に切り捨て
	const auto clientRect = clientRectangle();
	position = Point::maximize(position, Point::minimize(clientRect.size() - scrollSize(), Point(0, 0))); // 最小値に切り上げ
	const auto oldPosition = scrollPosition();
	const auto delta = position - oldPosition;
	if (delta == Point(0, 0)) {
		return;
	}
	RECT rect = clientRect;
	verify(ScrollWindowEx(handle(), delta.x, delta.y, nullptr, &rect, nullptr, nullptr, SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN) != ERROR);

	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_POS;
	if (delta.x && hScrollBar()) {
		info.nPos  = -position.x;
		SetScrollInfo(handle(), SB_HORZ, &info, TRUE);
	}
	if (delta.y && vScrollBar()) {
		info.nPos  = -position.y;
		SetScrollInfo(handle(), SB_VERT, &info, TRUE);
	}
	Scroll event(*this, oldPosition, position);
	onScroll()(event);
}


void ScrollableControl::scrollPosition(int x, int y) {
	scrollPosition(Point(x, y));
}


Size ScrollableControl::scrollSize() const {
	return _scrollSize;
}


bool ScrollableControl::vScrollBar() const {
	return _handle.hasStyle(WS_VSCROLL);
}


bool ScrollableControl::vScrollBarFixed() const {
	return _vScrollBarFixed;
}


void ScrollableControl::vScrollBarFixed(bool value) {
	if (value != vScrollBarFixed()) {
		_vScrollBarFixed = value;
		updateScrollBars();
	}
}


void ScrollableControl::updateScrollBars() {
	if (scrollMinSize() == Size(0, 0) || _updating) {
		return;
	}
	_updating = true; // スクロールバーの更新で WM_SIZE メッセージが投げられて無限再帰になるのを防ぐ
	scopeExit([&] {
		_updating = false;
	});

	const bool oldHScroll = hScrollBar();
	const bool oldVScroll = vScrollBar();
	Size newScrollSize = scrollMinSize();
	bool newHScroll;
	bool newVScroll;
	{// スクロールバーの表示有無とスクロールサイズを求める
		auto newClient    = clientRectangle().size();
		if (oldHScroll) {
			newClient.height += ScrollBar::defaultHorizontalHeight();
		}
		if (oldVScroll) {
			newClient.width  += ScrollBar::defaultVerticalWidth();
		}
		newHScroll = newClient.width  < newScrollSize.width  || hScrollBarFixed();
		newVScroll = newClient.height < newScrollSize.height || vScrollBarFixed();
		if (newHScroll) {
			newClient.height -= ScrollBar::defaultHorizontalHeight();
		}
		if (newVScroll) {
			newClient.width  -= ScrollBar::defaultVerticalWidth();
		}
		if (!newVScroll && newHScroll && newClient.height < newScrollSize.height) { // 水平スクロールバーの表示によって垂直スクロールバーが表示される
			newClient.width  -= ScrollBar::defaultVerticalWidth();
			newVScroll = true;
		}
		if (!newHScroll && newVScroll && newClient.width  < newScrollSize.width ) { // 垂直スクロールバーの表示によって水平スクロールバーが表示される
			newClient.height -= ScrollBar::defaultHorizontalHeight();
			newHScroll = true;
		}
		newScrollSize = Size::maximize(newScrollSize, newClient); // 最終的なスクロール範囲を求める
	}
	// スクロールバーの表示有無を更新する
	const auto oldScrollPosition = scrollPosition();
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	info.nPos  = 0; // 消えるスクロールバーのスクロール位置を 0 にしておく（再び表示した時にスクロール位置が保存されている為）
	info.nMin  = 0; // ShowScrollBar だけでは消えないケースがあるので確実に消えるように範囲とページを設定する
	info.nMax  = 1;
	info.nPage = 2;
	info.nTrackPos = 0;
	if (newHScroll != oldHScroll) { 
		if (!newHScroll) {
			SetScrollInfo(handle(), SB_HORZ, &info, TRUE);
		}
		ShowScrollBar(handle(), SB_HORZ, newHScroll ? TRUE : FALSE);
	}
	if (newVScroll != oldVScroll) { 
		if (!newVScroll) {
			SetScrollInfo(handle(), SB_VERT, &info, TRUE);
		}
		ShowScrollBar(handle(), SB_VERT, newVScroll ? TRUE : FALSE);
	}
	{// 긚긏깓?깑긫?귩뛛륷궥귡
		const auto clientRect = clientRectangle();
		auto newScrollPosition = oldScrollPosition;
		if (!newHScroll) {
			newScrollPosition.x = 0;
		}
		if (!newVScroll) {
			newScrollPosition.y = 0;
		}
		newScrollPosition = Point::minimize(newScrollPosition, Point(0, 0)); // 最大値（負数）に切り捨て
		newScrollPosition = Point::maximize(newScrollPosition, Point::minimize(clientRect.size() - newScrollSize, Point(0, 0))); // 最小値に切り上げ
		const auto delta = newScrollPosition - oldScrollPosition;
		if (delta != Point(0, 0)) {
			RECT rect = clientRect;
			verify(ScrollWindowEx(handle(), delta.x, delta.y, nullptr, &rect, nullptr, nullptr, SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN) != ERROR);
		}
		if (newHScroll) {
			info.fMask = hScrollBarFixed() ? SIF_ALL | SIF_DISABLENOSCROLL : SIF_ALL;
			info.nMax  = newScrollSize.width  - 1;
			info.nPage = clientRect.width;
			info.nPos  = -newScrollPosition.x;
			SetScrollInfo(handle(), SB_HORZ, &info, TRUE);
			assert(hScrollBar());
		}
		if (newVScroll) {
			info.fMask = vScrollBarFixed() ? SIF_ALL | SIF_DISABLENOSCROLL : SIF_ALL;
			info.nMax  = newScrollSize.height - 1;
			info.nPage = clientRect.height;
			info.nPos  = -newScrollPosition.y;
			SetScrollInfo(handle(), SB_VERT, &info, TRUE);
			assert(vScrollBar());
		}
		_scrollSize = newScrollSize;

		if (oldScrollPosition != newScrollPosition) {
			Scroll event(*this, oldScrollPosition, newScrollPosition);
			onScroll()(event);
		}
	}
}


void ScrollableControl::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_VSCROLL : {
			if (msg.lparam) { // スクロールバーコントロール（独立したコントロールのほう）は親に直接メッセージを送る
				reflectMessage((HWND)msg.lparam, msg);
				return;
			}
			const auto displayRect = displayRectangle();
			const auto clientSize = this->clientSize();
			const int smallChange = 5;
			const int largeChange = clientSize.height;
			const int maxPos = -(clientSize.height - displayRect.height);
			const int oldPos = -displayRect.y;
			int pos = oldPos;
			switch (LOWORD(msg.wparam)) {
				case SB_THUMBPOSITION :
				case SB_THUMBTRACK : pos = HIWORD(msg.wparam); break;
				case SB_LINEUP     : pos = smallChange < pos          ? pos - smallChange : 0     ; break;
				case SB_LINEDOWN   : pos = pos < maxPos - smallChange ? pos + smallChange : maxPos; break;
				case SB_PAGEUP     : pos = largeChange < pos          ? pos - largeChange : 0     ; break;
				case SB_PAGEDOWN   : pos = pos < maxPos - largeChange ? pos + largeChange : maxPos; break;
				case SB_TOP        : pos = 0; break;
				case SB_BOTTOM     : pos = maxPos; break;
			}
			scrollPosition(Point(displayRect.x, -pos));
		} break;
		case WM_HSCROLL : {
			if (msg.lparam) { // スクロールバーコントロール（独立したコントロールのほう）は親に直接メッセージを送る
				reflectMessage((HWND)msg.lparam, msg);
				return;
			}
			const auto displayRect = displayRectangle();
			const auto clientSize = this->clientSize();
			const int smallChange = 5;
			const int largeChange = clientSize.width ;
			const int maxPos = -(clientSize.width  - displayRect.width );
			const int oldPos = -displayRect.x;
			int pos = oldPos;
			switch (LOWORD(msg.wparam)) {
				case SB_THUMBPOSITION :
				case SB_THUMBTRACK : pos = HIWORD(msg.wparam); break;
				case SB_LINEUP     : pos = smallChange < pos          ? pos - smallChange : 0     ; break;
				case SB_LINEDOWN   : pos = pos < maxPos - smallChange ? pos + smallChange : maxPos; break;
				case SB_PAGEUP     : pos = largeChange < pos          ? pos - largeChange : 0     ; break;
				case SB_PAGEDOWN   : pos = pos < maxPos - largeChange ? pos + largeChange : maxPos; break;
				case SB_TOP        : pos = 0; break;
				case SB_BOTTOM     : pos = maxPos; break;
			}
			scrollPosition(Point(-pos, displayRect.y));
		} break;
		case WM_MOUSEWHEEL : {
			const int delta = GET_WHEEL_DELTA_WPARAM(msg.wparam);
			if (vScrollBar()) {
				const auto displayRect = displayRectangle();
				int pos = -displayRect.y;
				pos = max(pos - delta, 0);
				pos = min(pos, displayRect.height - clientSize().height);
				scrollPosition(Point(displayRect.x, -pos));
			} else if (hScrollBar()) {
				const auto displayRect = displayRectangle();
				int pos = -displayRect.x;
				pos = max(pos - delta, 0);
				pos = min(pos, displayRect.width  - clientSize().width );
				scrollPosition(Point(-pos, displayRect.y));
			}
			Control::processMessage(msg);
		} break;
		case WM_SIZE : {
			Control::processMessage(msg);
			if (msg.wparam != SIZE_MINIMIZED) {
				updateScrollBars();
				if (_scrollMinSize == Size(0, 0)) {
					_scrollSize = clientSize();
				}
			}
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}