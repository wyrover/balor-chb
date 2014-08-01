#pragma once

#include <vector>

#include <balor/ArrayRange.hpp>
#include <balor/Event.hpp>
#include <balor/Listener.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/Point.hpp>
#include <balor/String.hpp>
#include <balor/UniqueAny.hpp>

struct HBITMAP__;
struct HMENU__;

namespace balor {
	namespace graphics {
		class Bitmap;
		class Font;
	}
	class Rectangle;
	class Size;
}


namespace balor {
	namespace gui {

class Control;



/**
* メニューバーやポップアップメニューの基底クラス。
*
* デフォルトアイテムはサポートしない。項目のダブルクリックで親項目に対して WM_MENUCOMMAND メッセージが発生するので孫項目を特定できない。
* onDrag イベントでドラッグ＆ドロップを開始し、オーナーウインドウの DragDrop::Target::onDrop イベントでメッセージボックスを表示すると
* Vista ではブルースクリーン、XPでは PC 再起動してしまった。メッセージボックスを表示しなければ起きないので何かある。注意が必要。
*/
class Menu : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::HMENU__* HMENU;
	typedef ::balor::graphics::Bitmap Bitmap;
	typedef ::balor::graphics::Font Font;


	class Event;

	typedef Event Click;
	typedef Event PopupBegin;
	typedef Event PopupEnd;
	typedef Event RightClick;
	typedef Event Select;

	class Drag;
	class ItemInfo;
	class DescendantsIterator;

	/// メニュー項目。Menu::operator[] または Menu::Item::operator[] で取得できる。
	class Item : private NonCopyable {
		friend Menu;

		Item();
		Item(Item&& value);
		~Item();
		Item& operator=(Item&& value);

	public:
		/// 情報の反映。
		Item& operator=(const ItemInfo& itemInfo);

	public:
		/// 子項目を追加する。
		void add(const ItemInfo& itemInfo);
		/// 間に線を引いてメニューを改行するかどうか。メニューバーの場合は線は引かない。
		bool barBreak() const;
		void barBreak(bool value);
		/// 間に線を引かずにメニューを改行するかどうか。barBreak() のほうが優先される。
		bool lineBreak() const;
		void lineBreak(bool value);
		/// チェックされているかどうか。
		bool checked() const;
		void checked(bool value);
		/// チェックされた状態のビットマップ画像。ビットマップハンドルは参照されるので破棄しないこと。
		Bitmap checkedMark() const;
		void checkedMark(HBITMAP value);
		/// 子項目を全て削除する。
		void clear();
		/// 子項目の数。
		int count() const;
		/// 全ての子孫項目を列挙するイテレータ。
		Menu::DescendantsIterator descendantsBegin();
		/// 選択可能かどうか。
		bool enabled() const;
		void enabled(bool value);
		/// 子項目を削除する。
		void erase(int index);
		/// メニューハンドルから項目を取得する。見つからない場合は nullptr を返す。
		static Menu::Item* fromHandle(HMENU handle);
		/// オーナーコントロール上での項目のスクリーン座標系での表示位置を取得する。表示されていない場合は大きさが０になる。
		Rectangle getBounds(Control& owner) const;
		/// 項目のインデックス。
		int index() const;
		/// 子項目を挿入する。
		void insert(int index, const ItemInfo& itemInfo);
		/// 子項目の情報構造体配列を一括で取得、設定する。
		std::vector<Menu::ItemInfo> itemInfos() const;
		void itemInfos(ArrayRange<const Menu::ItemInfo> value);
		/// クリックした時のイベント。shortcut() が設定されている場合は発生しない。
		Listener<Menu::Click&>& onClick();
		/// この項目をドラッグした時のイベント。
		Listener<Menu::Drag&>& onDrag();
		/// ポップアップメニューを表示する直前のイベント。
		Listener<Menu::PopupBegin&>& onPopupBegin();
		/// ポップアップメニューを表示し終わったイベント。
		Listener<Menu::PopupEnd&>& onPopupEnd();
		/// 右クリックをしたイベント。
		Listener<Menu::RightClick&>& onRightClick();
		/// マウスカーソルが上に来たイベント。
		Listener<Menu::Select&>& onSelect();
		/// 親項目。ない場合は nullptr。
		Menu::Item* parent();
		/// 親項目のハンドル。ない場合は nullptr。
		HMENU parentHandle() const;
		/// ラジオボタンスタイルのチェックマークを表示するかどうか。
		bool radioCheck() const;
		void radioCheck(bool value);
		/// セパレータかどうか。
		bool separator() const;
		void separator(bool value);
		/// Key と Key::Modifier を組み合わせたショートカットコマンド。
		/// 設定されている場合は onClick() イベントの代わりにオーナーコントロールの onShortcutKey() イベントが発生する。
		int shortcut() const;
		void shortcut(int value);
		/// メニュー文字列。
		String text() const;
		void text(StringRange value);
		void textToBuffer(StringBuffer& buffer) const;
		/// チェックされていない状態のビットマップ画像。ビットマップハンドルは参照されるので破棄しないこと。
		Bitmap uncheckedMark() const;
		void uncheckedMark(HBITMAP value);
		/// ユーザが自由に使える任意のデータ。
		UniqueAny& userData();
		void userData(UniqueAny&& value);

	public:
		/// HMENU への自動変換 & null チェック。
		operator HMENU() const { return _handle; }
		/// 子項目を取得する。
		Item& operator[](int index);
		const Item& operator[](int index) const;

	private:
		void _attachHandle();
		void _setIndex(int value);

		HMENU _parent;
		HMENU _handle;
		int _index;
		Item* _items; // vector だと friend 宣言が沢山必要で面倒。
		int _itemsCapacity;
		int _shortcut;
		UniqueAny _userData;
		Listener<Menu::Click&> _onClick;
		Listener<Menu::Drag&> _onDrag;
		Listener<Menu::PopupBegin&> _onPopupBegin;
		Listener<Menu::PopupEnd&> _onPopupEnd;
		Listener<Menu::RightClick&> _onRightClick;
		Listener<Menu::Select&> _onSelect;
	};


	/// メニュー項目情報構造体。情報を持つだけでメニューは一切操作しない。Menu::itemInfos 関数または Menu::Item::itemInfos 関数等で使用する。
	class ItemInfo {
	public:
		/// セパレータとして作成。
		ItemInfo();
		ItemInfo(ItemInfo&& value);
		/// 情報の取得。
		ItemInfo(const Item& item);
		/// 項目文字列とショートカットコマンドから作成。
		explicit ItemInfo(String text, int shortcut = 0);
		/// 項目文字列と onClick イベントから作成。
		ItemInfo(String text, Listener<Menu::Event&> onClick);
		/// 項目文字列と子項目の情報構造体配列から作成。※ ArrayRange は Listener とあいまいな引数になるので使用できず。
		ItemInfo(String text, std::vector<Menu::ItemInfo> itemInfos);
		template<int Size> ItemInfo(String text, const std::tr1::array<Menu::ItemInfo, Size>& itemInfos) : _text(std::move(text)), _itemInfos(itemInfos.begin(), itemInfos.end()) { _initialize(); }
		template<int Size> ItemInfo(String text, const Menu::ItemInfo(&itemInfos)[Size]) : _text(std::move(text)), _itemInfos(itemInfos, itemInfos + Size) { _initialize(); }
		/// 項目文字列と色々な情報から作成。
		ItemInfo(String text, bool barBreak, bool lineBreak, bool radioCheck
			, bool checked = false, bool enabled = true, HBITMAP checkedMark = nullptr, HBITMAP uncheckedMark = nullptr
			, int shortcut = 0, Listener<Menu::Event&> onClick = [&](Event&) {});
		ItemInfo& operator=(ItemInfo&& value);

	public:
		/// 間に線を引いてメニューを改行するかどうか。メニューバーの場合は線は引かない。初期値は false。
		bool barBreak() const;
		void barBreak(bool value);
		/// 間に線を引かずにメニューを改行するかどうか。初期値は false。
		bool lineBreak() const;
		void lineBreak(bool value);
		/// チェックされているかどうか。初期値は false。
		bool checked() const;
		void checked(bool value);
		/// チェックされた状態のビットマップ画像。ビットマップハンドルは参照されるので破棄しないこと。初期値は nullptr。
		Bitmap checkedMark() const;
		void checkedMark(HBITMAP value);
		/// 選択可能かどうか。初期値は true。
		bool enabled() const;
		void enabled(bool value);
		/// 子項目の情報構造体配列。
		std::vector<Menu::ItemInfo>& itemInfos();
		const std::vector<Menu::ItemInfo>& itemInfos() const;
		void itemInfos(ArrayRange<const Menu::ItemInfo> value);
		/// ニーモニック文字。無い場合は L'\0' が返る。
		wchar_t mnemonic() const;
		/// クリックした時のイベント。shortcut() が設定されている場合は発生しない。
		Listener<Menu::Click&>& onClick();
		const Listener<Menu::Click&>& onClick() const;
		/// この項目をドラッグした時のイベント。。
		Listener<Menu::Drag&>& onDrag();
		const Listener<Menu::Drag&>& onDrag() const;
		/// ポップアップメニューを表示する直前のイベント。
		Listener<Menu::PopupBegin&>& onPopupBegin();
		const Listener<Menu::PopupBegin&>& onPopupBegin() const;
		/// ポップアップメニューを表示し終わったイベント。
		Listener<Menu::PopupEnd&>& onPopupEnd();
		const Listener<Menu::PopupEnd&>& onPopupEnd() const;
		/// 右クリックをしたイベント。
		Listener<Menu::RightClick&>& onRightClick();
		const Listener<Menu::RightClick&>& onRightClick() const;
		/// マウスカーソルが上に来たイベント。
		Listener<Menu::Select&>& onSelect();
		const Listener<Menu::Select&>& onSelect() const;
		/// ラジオボタンスタイルのチェックマークを表示するかどうか。初期値は false。
		bool radioCheck() const;
		void radioCheck(bool value);
		/// セパレータかどうか。デフォルトコンストラクタで作成した場合は true。
		bool separator() const;
		void separator(bool value);
		/// Key と Key::Modifier を組み合わせたショートカットコマンド。
		/// 設定されている場合は onClick() イベントの代わりにオーナーコントロールの onShortcutKey() イベントが発生する。初期値は 0。
		int shortcut() const;
		void shortcut(int value);
		/// メニュー文字列。
		const String& text() const;
		void text(String value);
		/// チェックされていない状態のビットマップ画像。ビットマップハンドルは参照されるので破棄しないこと。初期値は nullptr。
		Bitmap uncheckedMark() const;
		void uncheckedMark(HBITMAP value);

	private:
		friend Item;

		void _initialize();

		String _text;
		int _type;
		int _state;
		HBITMAP _checkedMark;
		HBITMAP _uncheckedMark;
		std::vector<ItemInfo> _itemInfos;
		int _shortcut;
		Listener<Menu::Click&> _onClick;
		Listener<Menu::Drag&> _onDrag;
		Listener<Menu::PopupBegin&> _onPopupBegin;
		Listener<Menu::PopupEnd&> _onPopupEnd;
		Listener<Menu::RightClick&> _onRightClick;
		Listener<Menu::Select&> _onSelect;
	};


	/// メニュー項目で発生するイベント。
	class Event : public EventWithSender<Menu::Item> {
	public:
		Event(Item& sender, Control& owner);

		/// メニューを表示しているオーナーコントロール。
		Control& owner();

	private:
		Control& _owner;
	};


	/// メニュー項目でドラッグを開始したイベント。
	class Drag : public Event {
	public:
		Drag(Item& sender, Control& owner);

		/// ドラッグイベント後にメニューを閉じるかどうか。ただし値にかかわらずメニューの外でマウスボタンを離すとメニューは閉じる。初期値は false。
		bool endMenu() const;
		void endMenu(bool value);

	private:
		bool _endMenu;
	};


	/// 全ての子孫を列挙するイテレータ。列挙し終えるとヌルを返すようになり、以降は操作できない。
	class DescendantsIterator {
	public:
		/// 親項目から作成。
		DescendantsIterator(const Item& parent);

		/// 次の子孫へ。
		DescendantsIterator& operator ++();
		/// 列挙した Item ポインタの取得。
		operator Menu::Item*() const;
		/// 列挙した Item ポインタへのアクセス。
		Menu::Item* operator->();

	private:
		Item* _parent;
		Item* _current;
	};


protected:
	Menu();
	Menu(Menu&& value);
	virtual ~Menu() = 0;
	Menu& operator=(Menu&& value);

public:
	/// 子項目を追加する。
	virtual void add(const ItemInfo& itemInfo);
	/// 項目がチェックされた状態の画像の大きさ。
	static Size checkedMarkSize();
	/// 子項目を全て削除する。
	void clear();
	/// 現在のスレッドのアクティブなメニューを閉じる。
	static void close();
	/// 子項目の数。
	int count() const;
	/// 全ての子孫項目を列挙するイテレータ。
	Menu::DescendantsIterator descendantsBegin();
	/// 子項目を削除する。
	void erase(int index);
	/// メニューで使用するフォント。
	static Font font();
	/// 子項目を挿入する。
	virtual void insert(int index, const ItemInfo& itemInfo);
	/// 項目の高さ。
	static int itemHeight();
	/// 子項目の情報配列を一括で取得、設定する。
	std::vector<Menu::ItemInfo> itemInfos() const;
	virtual void itemInfos(ArrayRange<const Menu::ItemInfo> value);

public:
	/// メニューハンドルへの自動変換 & ヌルチェック用。
	operator HMENU() { return _root._handle; }
	/// 子項目を取得する。
	Menu::Item& operator[](int index);
	const Menu::Item& operator[](int index) const;


protected:
	void _attachHandle(HMENU handle);

	Item _root;
};



	}
}