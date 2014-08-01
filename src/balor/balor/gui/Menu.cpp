#include "Menu.hpp"

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Font.hpp>
#include <balor/gui/Control.hpp>
#include <balor/gui/Key.hpp>
#include <balor/locale/Unicode.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>
#include <balor/Flag.hpp>
#include <balor/Size.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::graphics;
using namespace balor::locale;


namespace {
struct ItemData : public MENUITEMINFOW {
	ItemData() {
		this->cbSize = sizeof(MENUITEMINFOW);
	}
	ItemData(UINT mask) {
		this->cbSize = sizeof(MENUITEMINFOW);
		this->fMask = mask;
	}
	ItemData(Menu::Item& item, const Menu::ItemInfo& itemInfo, int type, int state) {
		this->cbSize = sizeof(MENUITEMINFOW);
		this->fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_CHECKMARKS;
		this->fType = type;
		this->fState = state;
		this->wID = item.index();
		this->dwTypeData = const_cast<wchar_t*>(itemInfo.text().empty() ? L" " : itemInfo.text().c_str());
		this->cch = 0;
		this->hbmpChecked   = itemInfo.checkedMark();
		this->hbmpUnchecked = itemInfo.uncheckedMark();

		// ここはちょっと判りにくい。ついでに item にイベントをコピーする
		item.shortcut(itemInfo.shortcut());
		item.onClick() = itemInfo.onClick();
		item.onDrag() = itemInfo.onDrag();
		item.onPopupBegin() = itemInfo.onPopupBegin();
		item.onPopupEnd() = itemInfo.onPopupEnd();
		item.onRightClick() = itemInfo.onRightClick();
		item.onSelect() = itemInfo.onSelect();
	}

	void getFrom(const Menu::Item& item) {
		verify(GetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
	}
	void setTo(Menu::Item& item) {
		verify(SetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
	}
	bool getType(const Menu::Item& item, UINT flag) {
		fMask = MIIM_FTYPE;
		verify(GetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		return toFlag(fType)[flag];
	}
	void setType(Menu::Item& item, UINT flag, bool value) {
		fMask = MIIM_FTYPE;
		verify(GetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		if (toFlag(fType)[flag] != value) {
			fType = toFlag(fType).set(flag, value);
			verify(SetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		}
	}
	bool getState(const Menu::Item& item, UINT flag) {
		fMask = MIIM_STATE;
		verify(GetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		return toFlag(fState)[flag];
	}
	void setState(Menu::Item& item, UINT flag, bool value) {
		fMask = MIIM_STATE;
		verify(GetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		if (toFlag(fState)[flag] != value) {
			fState = toFlag(fState).set(flag, value);
			verify(SetMenuItemInfoW(item.parentHandle(), item.index(), TRUE, this));
		}
	}
};
} // namespace



Menu::Item::Item()
	: _parent(nullptr)
	, _handle(nullptr)
	, _index(0)
	, _items(nullptr)
	, _itemsCapacity(0)
	, _shortcut(0) {
}


Menu::Item::Item(Item&& value)
	: _parent(move(value._parent))
	, _handle(move(value._handle))
	, _index(move(value._index))
	, _items(move(value._items))
	, _itemsCapacity(move(value._itemsCapacity))
	, _shortcut(move(value._shortcut))
	, _userData(move(value._userData))
	, _onClick(move(value._onClick))
	, _onDrag(move(value._onDrag))
	, _onPopupBegin(move(value._onPopupBegin))
	, _onPopupEnd(move(value._onPopupEnd))
	, _onRightClick(move(value._onRightClick))
	, _onSelect(move(value._onSelect))
	{
	value._parent = nullptr;
	value._handle = nullptr;
	value._items = nullptr;
	if (_handle) {
		_attachHandle();
	}
}


Menu::Item::~Item() {
	if (_items) {
		delete [] _items;
		verify(DestroyMenu(_handle));
		if (_parent) {
			ItemData data(MIIM_SUBMENU);
			data.hSubMenu = nullptr;
			data.setTo(*this);
		}
	}
}


Menu::Item& Menu::Item::operator=(Item&& value) {
	if (this != &value) {
		this->~Item();
		new (this) Item(move(value));
	}
	return *this;
}


Menu::Item& Menu::Item::operator=(const ItemInfo& itemInfo) {
	if (parentHandle()) {
		ItemData data(*this, itemInfo, itemInfo._type, itemInfo._state);
		data.setTo(*this);
	}
	itemInfos(itemInfo.itemInfos());
	return *this;
}


void Menu::Item::add(const ItemInfo& itemInfo) {
	insert(count(), itemInfo);
}


bool Menu::Item::barBreak() const {
	return ItemData().getType(*this, MFT_MENUBARBREAK);
}


void Menu::Item::barBreak(bool value) {
	ItemData().setType(*this, MFT_MENUBARBREAK, value);
}


bool Menu::Item::lineBreak() const {
	return ItemData().getType(*this, MFT_MENUBREAK);
}


void Menu::Item::lineBreak(bool value) {
	ItemData().setType(*this, MFT_MENUBREAK, value);
}


bool Menu::Item::checked() const {
	return ItemData().getState(*this, MFS_CHECKED);
}


void Menu::Item::checked(bool value) {
	ItemData().setState(*this, MFS_CHECKED, value);
}


Bitmap Menu::Item::checkedMark() const {
	ItemData data(MIIM_CHECKMARKS);
	data.getFrom(*this);
	return Bitmap(data.hbmpChecked);
}


void Menu::Item::checkedMark(HBITMAP value) {
	ItemData data(MIIM_CHECKMARKS);
	data.getFrom(*this);
	data.hbmpChecked = value;
	data.setTo(*this);
}


void Menu::Item::clear() {
	for (int i = count() - 1; 0 <= i; --i) {
		erase(i);
	}
}


int Menu::Item::count() const {
	if (*this) {
		int count = GetMenuItemCount(*this);
		assert("Failed to GetMenuItemCount" && count != -1);
		return count;
	}
	return 0;
}


Menu::DescendantsIterator Menu::Item::descendantsBegin() {
	return DescendantsIterator(*this);
}


bool Menu::Item::enabled() const {
	return !ItemData().getState(*this, MFS_DISABLED);
}


void Menu::Item::enabled(bool value) {
	ItemData().setState(*this, MFS_DISABLED, !value);
}


void Menu::Item::erase(int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	int count = this->count();
	_items[index] = Item();
	verify(DeleteMenu(_handle, index, MF_BYPOSITION));
	for (int i = index, end = count - 1; i < end; ++i) {
		Item& item = _items[i];
		item = move(_items[i + 1]);
		item._setIndex(item.index() - 1);
	}
	if (count <= 1) {
		delete [] _items;
		_items = nullptr;
		_itemsCapacity = 0;
		verify(DestroyMenu(_handle));
		_handle = nullptr;
		if (parentHandle()) {
			ItemData data(MIIM_SUBMENU);
			data.hSubMenu = nullptr;
			data.setTo(*this);
		}
	}
}


Menu::Item* Menu::Item::fromHandle(HMENU handle) {
	if (!handle) {
		return nullptr;
	}
	MENUINFO info;
	info.cbSize = sizeof(info);
	info.fMask = MIM_MENUDATA;
	info.dwMenuData = 0;
	verify(GetMenuInfo(handle, &info));
	return reinterpret_cast<Item*>(info.dwMenuData);
}


Rectangle Menu::Item::getBounds(Control& owner) const {
	if (parentHandle()) {
		RECT rect;
		if (GetMenuItemRect(owner, parentHandle(), index(), &rect)) {
			return rect;
		}
	}
	return Rectangle(0, 0, 0, 0);
}


int Menu::Item::index() const {
	return _index;
}


void Menu::Item::insert(int index, const ItemInfo& itemInfo) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= count());

	if (!_handle) {
		_handle = CreatePopupMenu();
		assert("Failed to CreatePopupMenu" && _handle);
		_attachHandle();
		if (parentHandle()) {
			ItemData data(MIIM_SUBMENU);
			data.hSubMenu = _handle;
			data.setTo(*this);
		}
	}
	int count = this->count() + 1;
	if (_itemsCapacity < count) {
		if (!_itemsCapacity) {
			_itemsCapacity = 4;
		} else {
			while (_itemsCapacity < count) {
				_itemsCapacity *= 2;
			}
		}
		auto newItems = new Item[_itemsCapacity];
		for (int i = 0; i < index; ++i) {
			newItems[i] = move(_items[i]);
		}
		Item& item = newItems[index];
		item._parent = _handle;
		item._index = index;
		ItemData data(item, itemInfo, itemInfo._type, itemInfo._state);
		verify(InsertMenuItemW(*this, index, TRUE, &data));
		item.itemInfos(itemInfo.itemInfos());
		for (int i = index + 1; i < count; ++i) {
			Item& item = newItems[i];
			item = move(_items[i - 1]);
			item._setIndex(item.index() + 1);
		}
		delete [] _items;
		_items = newItems;
	} else {
		for (int i = count - 1; index < i; --i) {
			Item& item = _items[i];
			item = move(_items[i - 1]);
		}
		Item& item = _items[index];
		item._parent = _handle;
		item._index = index;
		ItemData data(item, itemInfo, itemInfo._type, itemInfo._state);
		verify(InsertMenuItemW(*this, index, TRUE, &data));
		item.itemInfos(itemInfo.itemInfos());
		for (int i = count - 1; index < i; --i) {
			Item& item = _items[i];
			item._setIndex(item.index() + 1);
		}
	}
}


vector<Menu::ItemInfo> Menu::Item::itemInfos() const {
	vector<Menu::ItemInfo> infos;
	for (int i = 0, end = count(); i < end; ++i) {
		infos.push_back((*this)[i]);
	}
	return infos;
}


void Menu::Item::itemInfos(ArrayRange<const Menu::ItemInfo> value) {
	clear();
	if (!value.empty()) {
		_items = new Item[value.size()];
		_itemsCapacity = value.size();
		for (auto i = value.begin(), end = value.end(); i != end; ++i) {
			add(*i);
		}
	}
}


Listener<Menu::Click&>& Menu::Item::onClick() { return _onClick; }
Listener<Menu::Drag&>& Menu::Item::onDrag() { return _onDrag; }
Listener<Menu::PopupBegin&>& Menu::Item::onPopupBegin() { return _onPopupBegin; }
Listener<Menu::PopupEnd&>& Menu::Item::onPopupEnd() { return _onPopupEnd; }
Listener<Menu::RightClick&>& Menu::Item::onRightClick() { return _onRightClick; }
Listener<Menu::Select&>& Menu::Item::onSelect() { return _onSelect; }


Menu::Item* Menu::Item::parent() {
	return fromHandle(_parent);
}


HMENU Menu::Item::parentHandle() const {
	return _parent;
}


bool Menu::Item::radioCheck() const {
	return ItemData().getType(*this, MFT_RADIOCHECK);
}


void Menu::Item::radioCheck(bool value) {
	ItemData().setType(*this, MFT_RADIOCHECK, value);
}


bool Menu::Item::separator() const {
	return ItemData().getType(*this, MFT_SEPARATOR);
}


void Menu::Item::separator(bool value) {
	ItemData().setType(*this, MFT_SEPARATOR, value);
}


int Menu::Item::shortcut() const {
	return _shortcut;
}


void Menu::Item::shortcut(int value) {
	assert("Invalid shortcut" && Key::_validate(static_cast<Key>(value & ~Key::Modifier::mask)));
	_shortcut = value;
}


String Menu::Item::text() const {
	StringBuffer buffer;
	textToBuffer(buffer);
	return move(buffer);
}


void Menu::Item::text(StringRange value) {
	ItemData data(MIIM_STRING);
	data.dwTypeData = const_cast<wchar_t*>(value.c_str());
	data.setTo(*this);
}


void Menu::Item::textToBuffer(StringBuffer& buffer) const {
	ItemData data(MIIM_STRING);
	data.dwTypeData = 0;
	data.getFrom(*this);
	if (0 < data.cch) {
		buffer.reserveAdditionally(data.cch);
		data.fMask = MIIM_FTYPE | MIIM_STRING;
		data.dwTypeData = buffer.end();
		++data.cch;
		data.getFrom(*this);
		buffer.expandLength(data.cch);
	}
}


Bitmap Menu::Item::uncheckedMark() const {
	ItemData data(MIIM_CHECKMARKS);
	data.getFrom(*this);
	return Bitmap(data.hbmpUnchecked);
}


void Menu::Item::uncheckedMark(HBITMAP value) {
	ItemData data(MIIM_CHECKMARKS);
	data.getFrom(*this);
	data.hbmpUnchecked = value;
	data.setTo(*this);
}


UniqueAny& Menu::Item::userData() {
	return _userData;
}


void Menu::Item::userData(UniqueAny&& value) {
	_userData = move(value);
}


Menu::Item& Menu::Item::operator[] (int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	return _items[index];
}


const Menu::Item& Menu::Item::operator[] (int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	return _items[index];
}


void Menu::Item::_attachHandle() {
	assert("Null handle" && *this);
	MENUINFO info;
	info.cbSize = sizeof(info);
	info.fMask = MIM_STYLE | MIM_MENUDATA;
	info.dwStyle = MNS_NOTIFYBYPOS | MNS_DRAGDROP;
	info.dwMenuData = reinterpret_cast<ULONG_PTR>(this);
	verify(SetMenuInfo(*this, &info));
}


void Menu::Item::_setIndex(int value) {
	_index = value;
	ItemData data(MIIM_ID);
	data.wID = value;
	data.setTo(*this);
}



void Menu::ItemInfo::_initialize() {
	_type = 0;
	_state = 0;
	_checkedMark = nullptr;
	_uncheckedMark = nullptr;
	_shortcut = 0;
}


Menu::ItemInfo::ItemInfo() {
	_initialize();
	_type = MFT_SEPARATOR;
}


Menu::ItemInfo::ItemInfo(ItemInfo&& value)
	: _text(move(value._text))
	, _type(move(value._type))
	, _state(move(value._state))
	, _checkedMark(move(value._checkedMark))
	, _uncheckedMark(move(value._uncheckedMark))
	, _itemInfos(move(value._itemInfos))
	, _shortcut(move(value._shortcut))
	, _onClick(move(value._onClick))
	, _onDrag(move(value._onDrag))
	, _onPopupBegin(move(value._onPopupBegin))
	, _onPopupEnd(move(value._onPopupEnd))
	, _onRightClick(move(value._onRightClick))
	, _onSelect(move(value._onSelect))
	{
	value._initialize();
}


Menu::ItemInfo::ItemInfo(const Item& item) {
	_initialize();
	if (item.parentHandle()) {
		ItemData data(MIIM_STRING);
		data.dwTypeData = 0;
		data.cch = 0;
		data.getFrom(item);

		++data.cch;
		StringBuffer buffer(data.cch);
		data.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING | MIIM_CHECKMARKS;
		data.dwTypeData = buffer.begin();
		data.getFrom(item);
		buffer.length(data.cch);

		if (!(data.fType & MFT_SEPARATOR)) {
			_text = move(buffer);
		}
		_type = data.fType;
		_state = data.fState;
		_checkedMark = data.hbmpChecked;
		_uncheckedMark = data.hbmpUnchecked;
	}
	_itemInfos = item.itemInfos();
	_shortcut = item.shortcut();
	_onClick = item._onClick;
	_onDrag = item._onDrag;
	_onPopupBegin = item._onPopupBegin;
	_onPopupEnd = item._onPopupEnd;
	_onRightClick = item._onRightClick;
	_onSelect = item._onSelect;
}


Menu::ItemInfo::ItemInfo(String text, int shortcut) : _text(move(text)) {
	assert("Invalid shortcut" && Key::_validate(static_cast<Key>(shortcut & ~Key::Modifier::mask)));
	_initialize();
	_shortcut = shortcut;
}


Menu::ItemInfo::ItemInfo(String text, Listener<Menu::Event&> onClick) : _text(move(text)), _onClick(move(onClick)) {
	_initialize();
}


Menu::ItemInfo::ItemInfo(String text, vector<Menu::ItemInfo> itemInfos) : _text(move(text)), _itemInfos(move(itemInfos)) {
	_initialize();
}


Menu::ItemInfo::ItemInfo(String text, bool barBreak, bool lineBreak, bool radioCheck
	, bool checked, bool enabled, HBITMAP checkedMark, HBITMAP uncheckedMark, int shortcut, Listener<Menu::Event&> onClick)
	: _text(move(text)), _type(0), _state(0)
	, _checkedMark(checkedMark), _uncheckedMark(uncheckedMark)
	, _shortcut(shortcut), _onClick(move(onClick))
	{
	assert("Invalid shortcut" && Key::_validate(static_cast<Key>(shortcut & ~Key::Modifier::mask)));
	this->barBreak(barBreak);
	this->lineBreak(lineBreak);
	this->radioCheck(radioCheck);
	this->checked(checked);
	this->enabled(enabled);
}


Menu::ItemInfo& Menu::ItemInfo::operator=(ItemInfo&& value) {
	if (this != &value) {
		_text = move(value._text);
		_type = move(value._type);
		_state = move(value._state);
		_checkedMark = move(value._checkedMark);
		_uncheckedMark = move(value._uncheckedMark);
		_itemInfos = move(value._itemInfos);
		_shortcut = move(value._shortcut);
		_onClick = move(value._onClick);
		_onDrag = move(value._onDrag);
		_onPopupBegin = move(value._onPopupBegin);
		_onPopupEnd = move(value._onPopupEnd);
		_onRightClick = move(value._onRightClick);
		_onSelect = move(value._onSelect);
	}
	return *this;
}


bool Menu::ItemInfo::barBreak() const { return toFlag(_type)[MFT_MENUBARBREAK]; }
void Menu::ItemInfo::barBreak(bool value) { _type = toFlag(_type).set(MFT_MENUBARBREAK, value); }
bool Menu::ItemInfo::lineBreak() const { return toFlag(_type)[MFT_MENUBREAK]; }
void Menu::ItemInfo::lineBreak(bool value) { _type = toFlag(_type).set(MFT_MENUBREAK, value); }
bool Menu::ItemInfo::checked() const { return toFlag(_state)[MFS_CHECKED]; }
void Menu::ItemInfo::checked(bool value) { _state = toFlag(_state).set(MFS_CHECKED, value); }
Bitmap Menu::ItemInfo::checkedMark() const { return Bitmap(_checkedMark); }
void Menu::ItemInfo::checkedMark(HBITMAP value) { _checkedMark = value; }
bool Menu::ItemInfo::enabled() const { return !toFlag(_state)[MFS_DISABLED]; }
void Menu::ItemInfo::enabled(bool value) { _state = toFlag(_state).set(MFS_DISABLED, !value); }
vector<Menu::ItemInfo>& Menu::ItemInfo::itemInfos() { return _itemInfos; }
const vector<Menu::ItemInfo>& Menu::ItemInfo::itemInfos() const { return _itemInfos; }
void Menu::ItemInfo::itemInfos(ArrayRange<const Menu::ItemInfo> value) { _itemInfos.assign(value.begin(), value.end()); }
wchar_t Menu::ItemInfo::mnemonic() const {
	int i = 0;
	for ( ; ; ) {
		i = _text.indexOf(L'&', i);
		if (i == -1) {
			return 0;
		}
		++i;
		if (_text[i] != L'&') {
			return Unicode::toUpper(_text[i]);
		}
		++i;
	}
}
Listener<Menu::Click&>& Menu::ItemInfo::onClick() { return _onClick; }
const Listener<Menu::Click&>& Menu::ItemInfo::onClick() const { return _onClick; }
Listener<Menu::Drag&>& Menu::ItemInfo::onDrag() { return _onDrag; }
const Listener<Menu::Drag&>& Menu::ItemInfo::onDrag() const { return _onDrag; }
Listener<Menu::PopupBegin&>& Menu::ItemInfo::onPopupBegin() { return _onPopupBegin; }
const Listener<Menu::PopupBegin&>& Menu::ItemInfo::onPopupBegin() const { return _onPopupBegin; }
Listener<Menu::PopupEnd&>& Menu::ItemInfo::onPopupEnd() { return _onPopupEnd; }
const Listener<Menu::PopupEnd&>& Menu::ItemInfo::onPopupEnd() const { return _onPopupEnd; }
Listener<Menu::RightClick&>& Menu::ItemInfo::onRightClick() { return _onRightClick; }
const Listener<Menu::RightClick&>& Menu::ItemInfo::onRightClick() const { return _onRightClick; }
Listener<Menu::Select&>& Menu::ItemInfo::onSelect() { return _onSelect; }
const Listener<Menu::Select&>& Menu::ItemInfo::onSelect() const { return _onSelect; }
bool Menu::ItemInfo::radioCheck() const { return toFlag(_type)[MFT_RADIOCHECK]; }
void Menu::ItemInfo::radioCheck(bool value) { _type = toFlag(_type).set(MFT_RADIOCHECK, value); }
bool Menu::ItemInfo::separator() const { return toFlag(_type)[MFT_SEPARATOR]; }
void Menu::ItemInfo::separator(bool value) { _type = toFlag(_type).set(MFT_SEPARATOR, value); }
int Menu::ItemInfo::shortcut() const { return _shortcut; }
void Menu::ItemInfo::shortcut(int value) {
	assert("Invalid shortcut" && Key::_validate(static_cast<Key>(value & ~Key::Modifier::mask)));
	_shortcut = value;
}
const String& Menu::ItemInfo::text() const { return _text; }
void Menu::ItemInfo::text(String value) { _text = move(value); }
Bitmap Menu::ItemInfo::uncheckedMark() const { return Bitmap(_uncheckedMark); }
void Menu::ItemInfo::uncheckedMark(HBITMAP value) { _uncheckedMark = value; }



Menu::Event::Event(Item& sender, Control& owner)
	: EventWithSender<Menu::Item>(sender), _owner(owner) {
}


Control& Menu::Event::owner() { return _owner; }



Menu::Drag::Drag(Item& sender, Control& owner)
	: Menu::Event(sender, owner), _endMenu(false) {
}


bool Menu::Drag::endMenu() const { return _endMenu; }
void Menu::Drag::endMenu(bool value) { _endMenu = value; }



Menu::DescendantsIterator::DescendantsIterator(const Item& parent)
	: _parent(const_cast<Item*>(&parent)), _current(nullptr) {
	if (_parent->count()) {
		_current = &(*_parent)[0];
	}
}


Menu::DescendantsIterator& Menu::DescendantsIterator::operator ++() {
	assert("Iterator out of range" && _current);
	Item* next = nullptr;
	if (_current->count()) {
		next = &(*_current)[0];
	}
	if (next) {
		_current = next;
		return *this;
	}
	Item* parent = _current->parent();
	if (parent && _current->index() + 1 < parent->count()) {
		_current = &(*parent)[_current->index() + 1];
		return *this;
	}
	next = _current;
	for (;;) {
		next = next->parent();
		if (!next || next == _parent) {
			_current = nullptr;
			break;
		}
		Item* parentParent = next->parent();
		if (!parentParent) {
			_current = nullptr;
			break;
		}
		if (next->index() + 1 < parentParent->count()) {
			_current = &(*parentParent)[next->index() + 1];
			break;
		}
	}
	return *this;
}


Menu::DescendantsIterator::operator Menu::Item*() const {
	return _current;
}


Menu::Item* Menu::DescendantsIterator::operator->() {
	assert("Iterator out of range" && _current);
	return _current;
}



Menu::Menu() {
}


Menu::Menu(Menu&& value) : _root(move(value._root)) {
}


Menu::~Menu() {
}


Menu& Menu::operator=(Menu&& value) {
	if (&value != this) {
		_root = move(value._root);
	}
	return *this;
}


void Menu::add(const ItemInfo& itemInfo) {
	_root.add(itemInfo);
}


Size Menu::checkedMarkSize() {
	return Size(GetSystemMetrics(SM_CXMENUCHECK), GetSystemMetrics(SM_CYMENUCHECK));
}


void Menu::clear() {
	return _root.clear();
}


void Menu::close() {
	verify(EndMenu());
}


int Menu::count() const {
	return _root.count();
}


Menu::DescendantsIterator Menu::descendantsBegin() {
	return _root.descendantsBegin();
}


void Menu::erase(int index) {
	_root.erase(index);
}


Font Menu::font() {
	NONCLIENTMETRICSW  metrics;
	ZeroMemory(&metrics, sizeof(metrics));
	metrics.cbSize = sizeof(metrics);
	verify(SystemParametersInfoW(SPI_GETNONCLIENTMETRICS , sizeof(metrics), &metrics, 0));
	return Font::fromLogFont(metrics.lfMenuFont);
}


void Menu::insert(int index, const ItemInfo& itemInfo) {
	_root.insert(index, itemInfo);
}


int Menu::itemHeight() {
	return GetSystemMetrics(SM_CYMENU);
}


vector<Menu::ItemInfo> Menu::itemInfos() const {
	return _root.itemInfos();
}


void Menu::itemInfos(ArrayRange<const Menu::ItemInfo> value) {
	_root.itemInfos(value);
}


Menu::Item& Menu::operator[](int index) {
	return _root[index];
}


const Menu::Item& Menu::operator[](int index) const {
	return _root[index];
}


void Menu::_attachHandle(HMENU handle) {
	assert("handle already attached" && !*this);
	_root._handle = handle;
	_root._attachHandle();
}



	}
}