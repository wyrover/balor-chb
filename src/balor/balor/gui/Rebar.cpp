#include "Rebar.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
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
struct ItemExData {
	ItemExData() {}
	ItemExData(Listener<Rebar::ChevronClick&> _onChevronClick)
		: onChevronClick(move(_onChevronClick)) {
	}

	Listener<Rebar::ChevronClick&> onChevronClick;
};


struct ItemData : public REBARBANDINFOW {
	static const int defaultStyle = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS | RBBS_USECHEVRON;

	ItemData() {
		this->cbSize = sizeof(REBARBANDINFOW);
	}
	ItemData(UINT mask) {
		this->cbSize = sizeof(REBARBANDINFOW);
		this->fMask = mask;
	}
	ItemData(const Rebar::ItemInfo& info, int style) {
		this->cbSize = sizeof(REBARBANDINFOW);
		this->fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_IDEALSIZE | RBBIM_CHILDSIZE | RBBIM_LPARAM | RBBIM_ID;
		this->fStyle = style;
		this->cx = info.width();
		this->cxIdeal = info.displayWidth();
		this->cxMinChild = info.minControlSize().width ;
		this->cyMinChild = info.minControlSize().height;
		this->cyChild = this->cyIntegral = this->cyMaxChild = 0;
		this->lParam = 0;
		if (info.onChevronClick()) {
			this->lParam = (LPARAM)new ItemExData(info.onChevronClick());
		}
		this->wID = info.controlResizable() ? TRUE : FALSE;

		auto control = info.control();
		if (control) {
			this->fMask |= RBBIM_CHILD;
			this->hwndChild = *control;
		}
		if (!info.text().empty()) {
			this->fMask |= RBBIM_TEXT;
			this->lpText =  const_cast<wchar_t*>(info.text().c_str());
		}
		if (info.bitmap()) {
			this->fMask |= RBBIM_BACKGROUND;
			this->hbmBack = info.bitmap();
		}
	}

	void getFrom(const Rebar::Item& item) {
		verify(SendMessageW(item.ownerHandle(), RB_GETBANDINFOW, item.index(), (LPARAM)this));
	}
	void setTo(Rebar::Item& item) {
		if (this->fMask & RBBIM_LPARAM) {
			ItemData data(RBBIM_LPARAM);
			data.getFrom(item);
			if (data.lParam) {
				delete (ItemExData*)data.lParam;
			}
		}
		verify(SendMessageW(item.ownerHandle(), RB_SETBANDINFOW, item.index(), (LPARAM)this));
	}
};


bool itemControlDuplicated(HWND reberHandle, Control* control) {
	if (!control) {
		return false;
	}
	auto reber = dynamic_cast<Rebar*>(Control::fromHandle(reberHandle));
	assert(reber);
	for (auto i = 0, end = reber->count(); i < end; ++i) {
		if ((*reber)[i].control() == control) {
			return true;
		}
	}
	return false;
}


void invalidateRebar(HWND reberHandle) {
	verify(RedrawWindow(reberHandle, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN));
}
} // namespace



Rebar::Item::Item(HWND ownerHandle, int index) : _ownerHandle(ownerHandle), _index(index) {
	assert("Invalid Rebar handle" && IsWindow(ownerHandle));
	assert("item index out of range" && 0 <= index);
	assert("item index out of range" && index <  SendMessageW(ownerHandle, RB_GETBANDCOUNT, 0, 0));
}


Rebar::Item& Rebar::Item::operator=(const ItemInfo& itemInfo) {
	assert("control duplicated" && !itemControlDuplicated(ownerHandle(), itemInfo.control()));
	ItemData data(itemInfo, itemInfo._style);
	data.setTo(*this);
	return *this;
}


Bitmap Rebar::Item::bitmap() const {
	ItemData data(RBBIM_BACKGROUND);
	data.getFrom(*this);
	return Bitmap(data.hbmBack);
}


void Rebar::Item::bitmap(HBITMAP value) {
	ItemData data(RBBIM_BACKGROUND);
	data.hbmBack = value;
	data.setTo(*this);
	invalidateRebar(ownerHandle());
}


bool Rebar::Item::bitmapOriginFixed() const {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	return toFlag(data.fStyle)[RBBS_FIXEDBMP];
}


void Rebar::Item::bitmapOriginFixed(bool value) {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	data.fStyle = toFlag(data.fStyle).set(RBBS_FIXEDBMP, value);
	data.setTo(*this);
}


Rectangle Rebar::Item::bounds() const {
	RECT rect;
	verify(SendMessageW(ownerHandle(), RB_GETRECT, index(), (LPARAM)&rect));
	return rect;
}


bool Rebar::Item::lineBreak() const {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	return toFlag(data.fStyle)[RBBS_BREAK];
}


void Rebar::Item::lineBreak(bool value) {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	data.fStyle = toFlag(data.fStyle).set(RBBS_BREAK, value);
	data.setTo(*this);
}


Control* Rebar::Item::control() const {
	ItemData data(RBBIM_CHILD);
	data.getFrom(*this);
	return fromHandle((HWND)data.hwndChild);
}


void Rebar::Item::control(Control* value) {
	assert("control not created" && (!value || *value));
	assert("control duplicated" && !itemControlDuplicated(ownerHandle(), value));
	ItemData data(RBBIM_CHILD);
	data.hwndChild = value ? (HWND)*value : nullptr;
	data.setTo(*this);
}


bool Rebar::Item::controlEdge() const {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	return toFlag(data.fStyle)[RBBS_CHILDEDGE];
}


void Rebar::Item::controlEdge(bool value) {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	data.fStyle = toFlag(data.fStyle).set(RBBS_CHILDEDGE, value);
	data.setTo(*this);
}


bool Rebar::Item::controlResizable() const {
	ItemData data(RBBIM_ID);
	data.getFrom(*this);
	return data.wID == TRUE;
}


void Rebar::Item::controlResizable(bool value) {
	ItemData data(RBBIM_ID);
	data.wID = value ? TRUE : FALSE;
	data.setTo(*this);
}


int Rebar::Item::displayWidth() const {
	ItemData data(RBBIM_IDEALSIZE);
	data.getFrom(*this);
	return data.cxIdeal;
}


void Rebar::Item::displayWidth(int value) {
	ItemData data(RBBIM_IDEALSIZE);
	data.cxIdeal = value;
	data.setTo(*this);
}


int Rebar::Item::index() const {
	return _index;
}


void Rebar::Item::maximize() {
	SendMessageW(ownerHandle(), RB_MAXIMIZEBAND, index(), 0);
}


Size Rebar::Item::minControlSize() const {
	ItemData data(RBBIM_CHILDSIZE);
	data.getFrom(*this);
	return Size(data.cxMinChild, data.cyMinChild);
}


void Rebar::Item::minControlSize(const Size& value) {
	minControlSize(value.width, value.height);
}


void Rebar::Item::minControlSize(int width, int height) {
	ItemData data(RBBIM_CHILDSIZE);
	data.cxMinChild = width;
	data.cyMinChild = height;
	data.setTo(*this);
}


void Rebar::Item::minimize() {
	SendMessageW(ownerHandle(), RB_MINIMIZEBAND, index(), 0);
}


Listener<Rebar::ChevronClick&>& Rebar::Item::onChevronClick() {
	ItemData data(RBBIM_LPARAM);
	data.getFrom(*this);
	if (!data.lParam) {
		data.lParam = (LPARAM)new ItemExData();
		data.setTo(*this);
	}
	return ((ItemExData*)data.lParam)->onChevronClick;
}


HWND Rebar::Item::ownerHandle() const {
	return _ownerHandle;
}


bool Rebar::Item::resizable() const {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	return toFlag(data.fStyle)[RBBS_GRIPPERALWAYS];
}


void Rebar::Item::resizable(bool value) {
	ItemData data(RBBIM_STYLE);
	data.getFrom(*this);
	data.fStyle = toFlag(data.fStyle).set(RBBS_GRIPPERALWAYS | RBBS_FIXEDSIZE, false)
		.set(value ? RBBS_GRIPPERALWAYS : RBBS_FIXEDSIZE, true);
	data.setTo(*this);
}


String Rebar::Item::text() const {
	ItemData data(RBBIM_TEXT);
	wchar_t buffer[1024];
	buffer[0] = 0;
	data.lpText = buffer;
	data.cch = sizeof(buffer) / sizeof(buffer[0]);
	data.getFrom(*this);
	return buffer;
}


void Rebar::Item::text(StringRange value) {
	ItemData data(RBBIM_TEXT);
	data.lpText = (wchar_t*)value.c_str();
	data.setTo(*this);
}


int Rebar::Item::width() const {
	ItemData data(RBBIM_SIZE);
	data.getFrom(*this);
	return data.cx;
}


void Rebar::Item::width(int value) {
	ItemData data(RBBIM_SIZE);
	data.cx = value;
	data.setTo(*this);
}



Rebar::ItemInfo::ItemInfo()
	: _control(nullptr)
	, _controlResizable(false)
	, _bitmap(nullptr)
	, _style(ItemData::defaultStyle)
	, _width(0)
	, _displayWidth(0)
	, _minControlSize(0, 0)
	{
}



Rebar::ItemInfo::ItemInfo(ItemInfo&& value)
	: _control(move(value._control))
	, _controlResizable(move(value._controlResizable))
	, _bitmap(move(value._bitmap))
	, _style(move(value._style))
	, _text(move(value._text))
	, _width(move(value._width))
	, _displayWidth(move(value._displayWidth))
	, _minControlSize(move(value._minControlSize))
	, _onChevronClick(move(value._onChevronClick))
	{
}


Rebar::ItemInfo::ItemInfo(const Item& item) {
	wchar_t buffer[1024];
	buffer[0] = 0;
	ItemData data(RBBIM_STYLE | RBBIM_SIZE | RBBIM_IDEALSIZE | RBBIM_CHILDSIZE | RBBIM_LPARAM | RBBIM_ID | RBBIM_CHILD | RBBIM_TEXT | RBBIM_BACKGROUND);
	data.lpText = buffer;
	data.cch = sizeof(buffer) / sizeof(buffer[0]);
	data.getFrom(item);

	_control = data.hwndChild;
	_controlResizable = data.wID == TRUE;
	_bitmap = data.hbmBack;
	_style = data.fStyle;
	_text = buffer;
	_width = data.cx;
	_displayWidth = data.cxIdeal;
	_minControlSize.width  = data.cxMinChild;
	_minControlSize.height = data.cyMinChild;
	if (data.lParam) {
		_onChevronClick = ((ItemExData*)data.lParam)->onChevronClick;
	} else {
		_onChevronClick = Listener<Rebar::ChevronClick&>();
	}
}


Rebar::ItemInfo::ItemInfo(Control& control, int width, String text)
	: _control(control)
	, _controlResizable(false)
	, _bitmap(nullptr)
	, _style(ItemData::defaultStyle)
	, _text(move(text))
	, _width(width)
	, _displayWidth(0)
	, _minControlSize(control.size())
	{
}


Rebar::ItemInfo& Rebar::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_control = move(value._control);
		_controlResizable = move(value._controlResizable);
		_bitmap = move(value._bitmap);
		_style = move(value._style);
		_text = move(value._text);
		_width = move(value._width);
		_displayWidth = move(value._displayWidth);
		_minControlSize = move(value._minControlSize);
		_onChevronClick = move(value._onChevronClick);
	}
	return *this;
}


Bitmap Rebar::ItemInfo::bitmap() const { return Bitmap(_bitmap); }
void Rebar::ItemInfo::bitmap(HBITMAP value) { _bitmap = value; }
bool Rebar::ItemInfo::bitmapOriginFixed() const { return toFlag(_style)[RBBS_FIXEDBMP]; }
void Rebar::ItemInfo::bitmapOriginFixed(bool value) { _style = toFlag(_style).set(RBBS_FIXEDBMP, value); }
bool Rebar::ItemInfo::lineBreak() const { return toFlag(_style)[RBBS_BREAK]; }
void Rebar::ItemInfo::lineBreak(bool value) { _style = toFlag(_style).set(RBBS_BREAK, value); }
Control* Rebar::ItemInfo::control() const { return fromHandle(_control); }
void Rebar::ItemInfo::control(Control* value) {
	assert("control not created" && (!value || *value));
	_control = value ? (HWND)*value : nullptr;
}
bool Rebar::ItemInfo::controlEdge() const { return toFlag(_style)[RBBS_CHILDEDGE]; }
void Rebar::ItemInfo::controlEdge(bool value) { _style = toFlag(_style).set(RBBS_CHILDEDGE, value); }
bool Rebar::ItemInfo::controlResizable() const { return _controlResizable; }
void Rebar::ItemInfo::controlResizable(bool value) { _controlResizable = value; }
int Rebar::ItemInfo::displayWidth() const { return _displayWidth; }
void Rebar::ItemInfo::displayWidth(int value) { _displayWidth = value; }
Size Rebar::ItemInfo::minControlSize() const { return _minControlSize; }
void Rebar::ItemInfo::minControlSize(const Size& value) { _minControlSize = value; }
void Rebar::ItemInfo::minControlSize(int width, int height) { _minControlSize = Size(width, height); }
Listener<Rebar::ChevronClick&>& Rebar::ItemInfo::onChevronClick() { return _onChevronClick; }
const Listener<Rebar::ChevronClick&>& Rebar::ItemInfo::onChevronClick() const { return _onChevronClick; }
bool Rebar::ItemInfo::resizable() const { return toFlag(_style)[RBBS_GRIPPERALWAYS]; }
void Rebar::ItemInfo::resizable(bool value) {
	_style = toFlag(_style).set(RBBS_GRIPPERALWAYS | RBBS_FIXEDSIZE, false)
		.set(value ? RBBS_GRIPPERALWAYS : RBBS_FIXEDSIZE, true);
}
const String& Rebar::ItemInfo::text() const { return _text; }
void Rebar::ItemInfo::text(String value) { _text = move(value); }
int Rebar::ItemInfo::width() const { return _width; }
void Rebar::ItemInfo::width(int value) { _width = value; }



Rebar::ItemEvent::ItemEvent(Rebar& sender, int itemIndex)
	: Rebar::Event(sender), _itemIndex(itemIndex) {
}


Rebar::Item Rebar::ItemEvent::item() { return Item(sender(), _itemIndex); }



Rebar::ChevronClick::ChevronClick(Rebar& sender, int itemIndex, const Rectangle& bounds)
	: ItemEvent(sender, itemIndex), _chevronBounds(bounds) {
}


const Rectangle& Rebar::ChevronClick::chevronBounds() const { return _chevronBounds; }



Rebar::Rebar() {
}


Rebar::Rebar(Rebar&& value, bool checkSlicing)
	: Control(move(value))
	, _onResize(move(value._onResize))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


Rebar::Rebar(Control& parent) {
	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_COOL_CLASSES};
	verify(InitCommonControlsEx(&init));
	attachHandle(CreateWindowExW(0, REBARCLASSNAMEW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
		| CCS_NODIVIDER | RBS_BANDBORDERS // | RBS_VARHEIGHT | CCS_NOPARENTALIGN | CCS_NORESIZE
		, 0, 0, 0, 0, parent, nullptr,nullptr, nullptr));
}


Rebar::Rebar(Control& parent, ArrayRange<const Rebar::ItemInfo> itemInfos) {
	*this = Rebar(parent);
	this->itemInfos(itemInfos);
}


Rebar::~Rebar() {
	destroyHandle();
}


Rebar& Rebar::operator=(Rebar&& value) {
	if (this != &value) {
		this->~Rebar();
		new (this) Rebar(move(value));
	}
	return *this;
}


void Rebar::add(const ItemInfo& itemInfo) {
	insert(-1, itemInfo);
}


void Rebar::clear() {
	for (auto i = count() - 1; 0 <= i; --i) {
		erase(i);
	}
}


int Rebar::count() const {
	return SendMessageW(handle(), RB_GETBANDCOUNT, 0, 0);
}


bool Rebar::doubleClickToggle() const {
	return _handle.hasStyle(RBS_DBLCLKTOGGLE);
}


void Rebar::doubleClickToggle(bool value) {
	_handle.setStyle(RBS_DBLCLKTOGGLE, value);
}


void Rebar::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	verify(SendMessageW(handle(), RB_DELETEBAND, index, 0));
}


int Rebar::getIndexAt(const Point& point) const {
	return getIndexAt(point.x, point.y);
}


int Rebar::getIndexAt(int x, int y) const {
	RBHITTESTINFO info;
	info.pt.x = x;
	info.pt.y = y;
	return SendMessageW(handle(), RB_HITTEST, 0, (LPARAM)&info);
}


void Rebar::insert(int index, const ItemInfo& itemInfo) {
	assert("index out of range" && -1 <= index);
	assert("index out of range" && index <= count());
	assert("control not created" && (!itemInfo.control() || *itemInfo.control()));
	assert("control duplicated" && !itemControlDuplicated(handle(), itemInfo.control()));
	ItemData data(itemInfo, itemInfo._style);
	verify(SendMessageW(handle(), RB_INSERTBANDW, index, (LPARAM)&data));
}


vector<Rebar::ItemInfo> Rebar::itemInfos() const {
	vector<ItemInfo> infos;
	int count = this->count();
	infos.reserve(count);
	for (int i = 0; i < count; ++i) {
		infos.push_back(Item(handle(), i));
	}
	return infos;
}


void Rebar::itemInfos(ArrayRange<const Rebar::ItemInfo> value) {
	clear();
	for (int i = 0, end = value.size(); i < end; ++i) {
		insert(i, value[i]);
	}
	if (1 < value.size()) { // バンドが右端に寄ってしまうのを防ぐ
		(*this)[value.size() - 1].maximize();
	}
}


bool Rebar::itemBorderLines() const {
	return _handle.hasStyle(RBS_BANDBORDERS);
}


void Rebar::itemBorderLines(bool value) {
	_handle.setStyle(RBS_BANDBORDERS, value);
}


bool Rebar::itemHeightVariable() const {
	return _handle.hasStyle(RBS_VARHEIGHT);
}


void Rebar::itemHeightVariable(bool value) {
	_handle.setStyle(RBS_VARHEIGHT, value);
}


bool Rebar::itemOrderFixed() const {
	return _handle.hasStyle(RBS_FIXEDORDER);
}


void Rebar::itemOrderFixed(bool value) {
	_handle.setStyle(RBS_FIXEDORDER, value);
}


Listener<Rebar::Resize&>& Rebar::onResize() { return _onResize; }


int Rebar::rowCount() const {
	return SendMessageW(handle(), RB_GETROWCOUNT, 0, 0);
}


Rebar::Item Rebar::operator[](int index) {
	return Item(handle(), index);
}


void Rebar::processMessage(Message& msg) {
	switch (msg.message) {
		case wmReflect + WM_NOTIFY : {
			switch (((NMHDR*)msg.lparam)->code) {
				case RBN_CHEVRONPUSHED : {
					auto info = (NMREBARCHEVRON*)msg.lparam;
					ChevronClick event(*this, info->uBand, info->rc);
					Item(*this, info->uBand).onChevronClick()(event);
				} break;
				case RBN_CHILDSIZE : {
					auto info = (NMREBARCHILDSIZE*)msg.lparam;
					Item item(*this, info->uBand);
					auto control = item.control();
					if (control && !item.controlResizable()) {
						info->rcChild.right = info->rcChild.left + control->size().width;
					}
				} break;
				case RBN_DELETINGBAND : {
					auto info = (NMREBAR*)msg.lparam;
					if (info->dwMask & RBNM_LPARAM && info->lParam) {
						delete (ItemExData*)info->lParam;
					}
				} break;
			}
			Control::processMessage(msg);
		} break;
		case WM_SIZE : {
			Control::processMessage(msg);
			Resize event(*this);
			onResize()(event);
		} break;
		default : {
			Control::processMessage(msg);
		} break;
	}
}



	}
}