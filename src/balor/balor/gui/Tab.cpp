#include "Tab.hpp"

#include <utility>

#include <balor/graphics/Font.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/gui/ToolTip.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/StringBuffer.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::graphics;


namespace {
static_assert(Tab::Style::singleline      == TCS_SINGLELINE, "Invalid enum value");
static_assert(Tab::Style::multiline       == TCS_MULTILINE, "Invalid enum value");
static_assert(Tab::Style::multilineScroll == (TCS_MULTILINE | TCS_SCROLLOPPOSITE), "Invalid enum value");

static_assert(Tab::TextAlign::center   == 0, "Invalid enum value");
static_assert(Tab::TextAlign::iconLeft == TCS_FORCEICONLEFT, "Invalid enum value");
static_assert(Tab::TextAlign::left     == TCS_FORCELABELLEFT, "Invalid enum value");

struct ItemData : public TCITEMW {
	ItemData() {}
	ItemData(UINT mask) {
		this->mask = mask;
	}
	ItemData(const Tab::ItemInfo& info) {
		this->mask = TCIF_TEXT | TCIF_IMAGE;
		this->pszText = const_cast<wchar_t*>(info.text().c_str());
		this->iImage = info.imageIndex();
	}

	void getFrom(const Tab::Item& item) {
		verify(SendMessageW(item.ownerHandle(), TCM_GETITEMW, item.index(), (LPARAM)this));
	}
	void setTo(Tab::Item& item) {
		verify(SendMessageW(item.ownerHandle(), TCM_SETITEMW, item.index(), (LPARAM)this));
	}
};


void resetItemPagesVisibility(Tab& tab) {
	auto selected = tab.selectedIndex();
	for (int i = 0, end = tab.count(); i < end; ++i) {
		auto page = tab[i].page();
		if (page && i != selected) {
			page->visible(false);
		}
	}
	if (selected != -1) { // 複数のタブに同じコントロールを指定した場合の為に全て消してから表示する。
		auto page = tab[selected].page();
		if (page) {
			page->visible(true);
		}
	}
}
} // namespace



bool Tab::Style::_validate(Style value) {
	switch (value) {
		case singleline :
		case multiline :
		case multilineScroll : return true;
		default : return false;
	}
}



bool Tab::TextAlign::_validate(TextAlign value) {
	switch (value) {
		case center :
		case iconLeft :
		case left : return true;
		default : return false;
	}
}



Tab::Item::Item(HWND ownerHandle, int index) : _ownerHandle(ownerHandle), _index(index) {
	assert("Invalid Tab handle" && IsWindow(ownerHandle));
	assert("item index out of range" && 0 <= index);
	assert("item index out of range" && index <  SendMessageW(ownerHandle, TCM_GETITEMCOUNT, 0, 0));
}


Tab::Item& Tab::Item::operator=(const ItemInfo& itemInfo) {
	ItemData data(itemInfo);
	data.setTo(*this);
	page(itemInfo.page());
	return *this;
}


Rectangle Tab::Item::bounds() const {
	RECT rect;
	verify(SendMessageW(ownerHandle(), TCM_GETITEMRECT, index(), (LPARAM)&rect));
	return rect;
}


int Tab::Item::imageIndex() const {
	ItemData data(TCIF_IMAGE);
	data.getFrom(*this);
	return data.iImage;
}


void Tab::Item::imageIndex(int value) {
	assert("Negative imageIndex" && -1 <= value);
	ItemData data(TCIF_IMAGE);
	data.iImage = value;
	data.setTo(*this);
}


int Tab::Item::index() const {
	return _index;
}


HWND Tab::Item::ownerHandle() const {
	return _ownerHandle;
}


Control* Tab::Item::page() const {
	ItemData data(TCIF_PARAM);
	data.getFrom(*this);
	return fromHandle((HWND)data.lParam);
}


void Tab::Item::page(Control* value) {
	auto oldPage = page();
	if (value != oldPage) {
		if (oldPage) {
			oldPage->visible(false);
		}
		ItemData data(TCIF_PARAM);
		data.lParam = LPARAM(value ? (HWND)*value : nullptr);
		data.setTo(*this);
		auto tab = dynamic_cast<Tab*>(fromHandle(ownerHandle()));
		if (value) {
			value->parent(tab);
		}
		assert("Tab is not created" && tab);
		resetItemPagesVisibility(*tab);
	}
}


String Tab::Item::text() const {
	ItemData data(TCIF_TEXT);
	wchar_t buffer[1024];
	buffer[0] = 0;
	data.pszText = buffer;
	data.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);
	data.getFrom(*this);
	return buffer;
}


void Tab::Item::text(StringRange value) {
	ItemData data(TCIF_TEXT);
	data.pszText = (wchar_t*)value.c_str();
	data.setTo(*this);
}



Tab::ItemInfo::ItemInfo()
	: _imageIndex(-1)
	, _page(nullptr) {
}



Tab::ItemInfo::ItemInfo(ItemInfo&& value)
	: _text(move(value._text))
	, _imageIndex(move(value._imageIndex))
	, _page(move(value._page)) {
}


Tab::ItemInfo::ItemInfo(const Item& item) {
	wchar_t buffer[1024];
	buffer[0] = 0;
	ItemData data(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM);
	data.pszText = buffer;
	data.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);
	data.getFrom(item);

	_text = buffer;
	_imageIndex = data.iImage;
	_page = (HWND)data.lParam;
}


Tab::ItemInfo::ItemInfo(String text, int imageIndex)
	: _text(move(text)), _imageIndex(imageIndex), _page(nullptr) {
	assert("Invalid imageIndex" && -1 <= imageIndex);
}


Tab::ItemInfo::ItemInfo(String text, const Control& page, int imageIndex)
	: _text(move(text)), _imageIndex(imageIndex), _page(page) {
	assert("page not created" && page);
	assert("Invalid imageIndex" && -1 <= imageIndex);
}


Tab::ItemInfo& Tab::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_text = move(value._text);
		_imageIndex = move(value._imageIndex);
		_page = move(value._page);
	}
	return *this;
}


int Tab::ItemInfo::imageIndex() const { return _imageIndex; }
void Tab::ItemInfo::imageIndex(int value) {
	assert("Invalid imageIndex" && -1 <= value);
	_imageIndex = value;
}
Control* Tab::ItemInfo::page() const { return fromHandle(_page); }
void Tab::ItemInfo::page(Control* value) {
	assert("page not created" && (!value || *value));
	_page = value ? (HWND)*value : nullptr;
}
const String& Tab::ItemInfo::text() const { return _text; }
void Tab::ItemInfo::text(String value) { _text = move(value); }



Tab::ItemEvent::ItemEvent(Tab& sender, int itemIndex)
	: Tab::Event(sender), _itemIndex(itemIndex) {
}


Tab::Item Tab::ItemEvent::item() { return Item(sender(), _itemIndex); }



Tab::ItemSelecting::ItemSelecting(Tab& sender)
	: Tab::Event(sender), _cancel(false) {
}


bool Tab::ItemSelecting::cancel() const { return _cancel; }
void Tab::ItemSelecting::cancel(bool value) { _cancel = value; }



Tab::ItemTipPopup::ItemTipPopup(Tab& sender, int itemIndex, String& text)
	: ItemEvent(sender, itemIndex), _text(text) {
}


void Tab::ItemTipPopup::setText(String value) { _text = move(value); }



Tab::Tab() {
}


Tab::Tab(Tab&& value, bool checkSlicing)
	: Control(move(value))
	, _temporaryText(move(value._temporaryText))
	, _onItemSelect(move(value._onItemSelect))
	, _onItemSelecting(move(value._onItemSelecting))
	, _onItemTipPopup(move(value._onItemTipPopup))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Tab::Tab(Control& parent, int x, int y, int width, int height, Tab::Style style) {
	assert("Inavlid Tab::Style" && Style::_validate(style));
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_TAB_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, WC_TABCONTROLW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| TCS_TOOLTIPS | TCS_FOCUSONBUTTONDOWN | style
		, x, y, width, height, parent, nullptr,nullptr, nullptr));
	font(defaultFont());
}


Tab::Tab(Control& parent, int x, int y, int width, int height, ArrayRange<const Tab::ItemInfo> itemInfos, Tab::Style style) {
	*this = Tab(parent, x, y, width, height, style);
	this->itemInfos(itemInfos);
}


Tab::~Tab() {
	destroyHandle();
}


Tab& Tab::operator=(Tab&& value) {
	if (this != &value) {
		this->~Tab();
		new (this) Tab(move(value));
	}
	return *this;
}


void Tab::add(const ItemInfo& itemInfo) {
	insert(count(), itemInfo);
}


bool Tab::bottomItems() const {
	return _handle.hasStyle(TCS_BOTTOM);
}


void Tab::bottomItems(bool value) {
	_handle.setStyle(TCS_BOTTOM, value);
	resizeItemPages();
}


void Tab::clear() {
	auto index = selectedIndex();
	if (0 <= index) {
		auto page = (*this)[index].page();
		if (page) {
			page->visible(false);
		}
	}
	verify(SendMessageW(handle(), TCM_DELETEALLITEMS, 0, 0));
}


int Tab::count() const {
	return SendMessageW(handle(), TCM_GETITEMCOUNT, 0, 0);
}


Rectangle Tab::displayRectangle() const {
	auto size = this->size();
	RECT rect = {0, 0, size.width, size.height};
	SendMessageW(handle(), TCM_ADJUSTRECT, FALSE, (LPARAM)&rect);
	return rect;
}


void Tab::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	auto oldPage = (*this)[index].page();
	if (oldPage) {
		oldPage->visible(false);
	}
	verify(SendMessageW(handle(), TCM_DELETEITEM, index, 0));
	resetItemPagesVisibility(*this);
	resizeItemPages();
}


int Tab::getIndexAt(const Point& point) const {
	return getIndexAt(point.x, point.y);
}


int Tab::getIndexAt(int x, int y) const {
	TCHITTESTINFO info;
	info.pt.x = x;
	info.pt.y = y;
	return SendMessageW(handle(), TCM_HITTEST, 0, (LPARAM)&info);
}


ImageList Tab::imageList() const {
	return ImageList((HIMAGELIST)SendMessageW(handle(), TCM_GETIMAGELIST, 0, 0));
}


void Tab::imageList(HIMAGELIST value) {
	SendMessageW(handle(), TCM_SETIMAGELIST, 0, (LPARAM)value);
	resizeItemPages();
}


void Tab::insert(int index, const ItemInfo& itemInfo) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= count());
	ItemData data(itemInfo);
	verify(SendMessageW(handle(), TCM_INSERTITEMW, index, (LPARAM)&data) == index);
	(*this)[index].page(itemInfo.page());
	resizeItemPages();
}


vector<Tab::ItemInfo> Tab::itemInfos() const {
	vector<ItemInfo> infos;
	int count = this->count();
	infos.reserve(count);
	for (int i = 0; i < count; ++i) {
		infos.push_back(Item(handle(), i));
	}
	return infos;
}


void Tab::itemInfos(ArrayRange<const Tab::ItemInfo> value) {
	clear();
	for (int i = 0, end = value.size(); i < end; ++i) {
		insert(i, value[i]);
	}
}


void Tab::itemPadding(const Size& value) {
	SendMessageW(handle(), TCM_SETPADDING, 0, MAKELPARAM(value.width, value.height));
	// 大きさを反映させる。他にも方法はあるかもしれない。
	itemWidthFixed(!itemWidthFixed());
	itemWidthFixed(!itemWidthFixed());
	resizeItemPages();
}


void Tab::itemSize(const Size& value) {
	itemSize(value.width, value.height);
}


void Tab::itemSize(int width, int height) {
	SendMessageW(handle(), TCM_SETITEMSIZE, 0, MAKELPARAM(width, height));
	resizeItemPages();
}


ToolTip Tab::itemTip() {
	return ToolTip((HWND)SendMessageW(handle(), TCM_GETTOOLTIPS, 0, 0));
}


bool Tab::itemWidthFixed() const {
	return _handle.hasStyle(TCS_FIXEDWIDTH);
}


void Tab::itemWidthFixed(bool value) {
	_handle.setStyle(TCS_FIXEDWIDTH, value);
	
}


void Tab::minItemWidth(int value) {
	SendMessageW(handle(), TCM_SETMINTABWIDTH, 0, value);
	resizeItemPages();
}


Listener<Tab::ItemSelect&>& Tab::onItemSelect() { return _onItemSelect; }
Listener<Tab::ItemSelecting&>& Tab::onItemSelecting() { return _onItemSelecting; }
Listener<Tab::ItemTipPopup&>& Tab::onItemTipPopup() { return _onItemTipPopup; }


void Tab::resizeItemPages() {
	for (int i = 0, end = count(); i < end; ++i) {
		auto page = (*this)[i].page();
		if (page) {
			page->bounds(displayRectangle());
		}
	}
}


int Tab::rowCount() const {
	return SendMessageW(handle(), TCM_GETROWCOUNT, 0, 0);
}


int Tab::selectedIndex() const {
	return SendMessageW(handle(), TCM_GETCURSEL, 0, 0);
}


void Tab::selectedIndex(int value) {
	assert("index out of range" && 0 <= value);
	assert("index out of range" && value < count());
	SendMessageW(handle(), TCM_SETCURSEL, value, 0);
	resetItemPagesVisibility(*this);
}


Tab::Style Tab::style() const {
	return static_cast<Style::_enum>(_handle.style() & (Tab::Style::singleline
													  | Tab::Style::multiline
													  | Tab::Style::multilineScroll
													  ));
}


Tab::TextAlign Tab::textAlign() const {
	return static_cast<TextAlign::_enum>(_handle.style() & (Tab::TextAlign::center
														  | Tab::TextAlign::iconLeft
														  | Tab::TextAlign::left
														  ));
}


void Tab::textAlign(Tab::TextAlign value) {
	assert("Invalid Tab::TextAlign" && TextAlign::_validate(value));
	assert("itemWidthFixed must be true" && itemWidthFixed());
	auto style = _handle.style();
	style = toFlag(style).set(Tab::TextAlign::center
							| Tab::TextAlign::iconLeft
							| Tab::TextAlign::left
							, false);
	style |= value;
	_handle.style(style);
	_handle.updateStyle();
}


Tab::Item Tab::operator[](int index) {
	return Item(handle(), index);
}


void Tab::processMessage(Message& msg) {
	switch (msg.message) {
		//case WM_ERASEBKGND : { // Visual Style が有効だと背景をブラシで塗っても上書きされてしまう。.NET はどうやっているんだろう？
		//	Graphics graphics((HDC)msg.wparam);
		//	HBRUSH brush = this->brush();
		//	auto origin = brushOrigin();
		//	if (!brush) { // 親コントロールと地続きな背景を持つので親のブラシがあれば使う ;
		//		auto parent = this->parent();
		//		if (parent) {
		//			brush = parent->brush();
		//			origin = parent->displayRectangle().position() + parent->brushOrigin() - position();
		//		}
		//	}
		//	if (brush) {
		//		brush = graphics.brush(brush);
		//		origin = graphics.brushOrigin(origin);
		//		graphics.clear();
		//		graphics.brushOrigin(origin);
		//		graphics.brush(brush);
		//		msg.result = TRUE;
		//	}
		//} break;
		case WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case TTN_GETDISPINFOW : {
					if (onItemTipPopup()) {
						auto info = (NMTTDISPINFOW*)msg.lparam;
						_temporaryText = String();
						ItemTipPopup event(*this, info->hdr.idFrom, _temporaryText);
						onItemTipPopup()(event);
						info->lpszText = const_cast<wchar_t*>(_temporaryText.c_str());
					}
				} break;
				default : {
					Control::processMessage(msg);
				} break;
			}
		} break;
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case TCN_SELCHANGE : {
					if (style() == Style::multilineScroll) {
						auto page = (*this)[selectedIndex()].page();
						if (page) {
							page->bounds(displayRectangle());
						}
					}
					resetItemPagesVisibility(*this);
					ItemSelect event(*this);
					onItemSelect()(event);
				} break;
				case TCN_SELCHANGING : {
					ItemSelecting event(*this);
					onItemSelecting()(event);
					if (event.cancel()) {
						msg.result = TRUE;
						return;
					}
				} break;
				case TTN_GETDISPINFOW : {
					if (onItemTipPopup()) {
						auto info = (NMTTDISPINFOW*)msg.lparam;
						_temporaryText = String();
						ItemTipPopup event(*this, info->hdr.idFrom, _temporaryText);
						onItemTipPopup()(event);
						info->lpszText = const_cast<wchar_t*>(_temporaryText.c_str());
						return;
					}
				} break;
			}
			Control::processMessage(msg);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}