#pragma once

#include <vector>

#include <balor/gui/Control.hpp>

struct HBITMAP__;

namespace balor {
	namespace graphics {
		class Bitmap;
	}
}


namespace balor {
	namespace gui {



/**
 * レバーコントロール。
 *
 * RB_MOVEBAND は動作せず。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Rebar sample");

	ImageList imageList(ImageList::SystemResource::largeStandardImages);
	ToolBar::ButtonInfo toolItems[] = {
		ToolBar::ButtonInfo(ImageList::StandardImage::cut, L"切り取り", [&] (ToolBar::Click& ) {
			MsgBox::show(L"切り取った");
		}),
		ToolBar::ButtonInfo(ImageList::StandardImage::copy, L"コピー", [&] (ToolBar::Click& ) {
			MsgBox::show(L"コピーした");
		}),
		ToolBar::ButtonInfo(ImageList::StandardImage::paste, L"貼り付け", [&] (ToolBar::Click& ) {
			MsgBox::show(L"貼り付けた");
		}),
	};
	ToolBar tool(frame, 0, 0, 0, 0, imageList, toolItems);
	tool.size(tool.buttonsSize()); // ボタンの大きさに合わせる
	tool.transparent(true);

	const wchar_t* comboItems[] = {
		L"リンゴ",
		L"メロン",
		L"スイカ",
	};
	ComboBox combo(frame, 0, 0, 0, 0, comboItems);
	combo.selectedIndex(0);

	Edit edit(frame, 0, 0, 0, 0, 10);
	edit.size(edit.size().width, tool.size().height);

	Rebar::ItemInfo rebarItems[] = {
		Rebar::ItemInfo(tool),
		Rebar::ItemInfo(combo, 0, L"果物"),
		Rebar::ItemInfo(edit),
		Rebar::ItemInfo(),
	};
	Rebar rebar(frame, rebarItems);
	frame.onResized() = [&] (Frame::Resized& ) {
		rebar.size(frame.clientSize().width, rebar.size().height);
		rebar.invalidate();
	};

	frame.runMessageLoop();
 * </code></pre>
 *
 * <h3>・シェブロンサンプルコード</h3>
 * <pre><code>
	Frame frame(L"Rebar Chevron sample");

	std::vector<Menu::ItemInfo> menuItems;
	std::vector<Bitmap> menuBitmaps;
	std::vector<ToolBar::ButtonInfo> toolItems;
	ImageList imageList(ImageList::SystemResource::largeStandardImages);
	for (auto i = 0, end = imageList.count(); i < end; ++i) {
		String text = String(L"項目") + i;
		menuBitmaps.push_back(imageList.getIcon(i).bitmap());
		Menu::ItemInfo menuItem(text);
		menuItem.uncheckedMark(menuBitmaps.back());
		menuItems.push_back(menuItem);
		toolItems.push_back(ToolBar::ButtonInfo(i, text));
	}
	ToolBar tool(frame, 0, 0, 0, 0, imageList, toolItems);
	tool.size(tool.buttonsSize());
	tool.transparent(true);
	tool.hideClippedButtons(true); // ボタンが少しでも隠れるなら表示しない。

	Rebar rebar(frame);
	Rebar::ItemInfo rebarItem(tool);
	rebarItem.controlResizable(true); // コントロールをリサイズ可能にする。
	rebarItem.minControlSize(0, tool.size().height); // 幅０までリサイズを許可する。
	rebarItem.displayWidth(tool.size().width); // 最初の幅以下ならシェブロンを表示する。
	rebar.add(rebarItem);
	PopupMenu chevronMenu;
	rebar[0].onChevronClick() = [&] (Rebar::ChevronClick& e) {
		for (auto i = 0, end = tool.count(); i < end; ++i) {
			if (tool.bounds().width < tool[i].bounds().right()) {
				chevronMenu = PopupMenu(ArrayRange<const Menu::ItemInfo>(menuItems.data() + i, end - i));
				chevronMenu.show(frame, e.chevronBounds().bottomLeft());
				break;
			}
		}
	};

	frame.onResized() = [&] (Frame::Resized& ) {
		rebar.size(frame.clientSize().width, rebar.size().height);
		rebar.invalidate();
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class Rebar : public Control {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::balor::graphics::Bitmap Bitmap;


	class ItemInfo;
	struct ItemEvent;
	struct ChevronClick;

	/// レバーコントロール上のバンド項目。Rebar::operator[] で取得できる。
	class Item {
	public:
		Item(HWND ownerHandle, int index);
		/// 項目情報を反映。
		Item& operator=(const ItemInfo& itemInfo);

		/// 項目の背景ビットマップ。タイル表示される。
		Bitmap bitmap() const;
		void bitmap(HBITMAP value);
		/// 項目の背景ビットマップの表示原点を項目の座標によらず固定するかどうか。
		bool bitmapOriginFixed() const;
		void bitmapOriginFixed(bool value);
		/// 項目のクライアント座標領域。
		Rectangle bounds() const;
		/// 項目を改行するかどうか。後ろの項目も全て一緒に改行されるので index() は変化しない。
		bool lineBreak() const;
		void lineBreak(bool value);
		/// 項目上に置くコントロール。置かない場合は nullptr。変更する前に minControlSize() を control の大きさに設定したほうが良い。
		Control* control() const;
		void control(Control* value);
		/// 項目上に置くコントロールの上下に空白を入れるどうか。
		bool controlEdge() const;
		void controlEdge(bool value);
		/// 項目上に置くコントロールが項目の幅に合わせて大きさを変えるかどうか。
		bool controlResizable() const;
		void controlResizable(bool value);
		/// 項目の表示可能なピクセル幅。width() がこの幅未満である場合、シェブロンを表示する。初期値は 0。
		int displayWidth() const;
		void displayWidth(int value);
		/// 項目インデックス。
		int index() const;
		/// 項目を最大化する。
		void maximize();
		/// ユーザが項目をリサイズする場合の最小の大きさ。
		Size minControlSize() const;
		void minControlSize(const Size& value);
		void minControlSize(int width, int height);
		/// 項目を最小化する。
		void minimize();
		/// シェブロンをクリックしたイベント。
		Listener<Rebar::ChevronClick&>& onChevronClick();
		/// レバーコントロールのハンドル。
		HWND ownerHandle() const;
		/// 項目上の大きさを変更できるかどうか。false の場合はグリップが表示されない。
		bool resizable() const;
		void resizable(bool value);
		/// 項目の文字列。
		String text() const;
		void text(StringRange value);
		/// 項目のピクセル幅。
		int width() const;
		void width(int value);

	private:
		HWND _ownerHandle;
		int _index;
	};


	/// 項目情報構造体。情報を持つだけでレバーコントロールは一切操作しない。Rebar::itemInfos 関数等で使用する。
	class ItemInfo {
	public:
		ItemInfo();
		ItemInfo(ItemInfo&& value);
		/// 項目情報の取得。
		ItemInfo(const Item& item);
		/// 項目上に置くコントロールから作成。
		explicit ItemInfo(Control& control, int width = 0, String text = L"");
		ItemInfo& operator=(ItemInfo&& value);

		/// 項目の背景ビットマップ。タイル表示される。初期値はヌルハンドルの Bitmap。
		Bitmap bitmap() const;
		void bitmap(HBITMAP value);
		/// 項目の背景ビットマップの表示原点を項目の座標によらず固定するかどうか。初期値は false。
		bool bitmapOriginFixed() const;
		void bitmapOriginFixed(bool value);
		/// 項目を改行するかどうか。初期値は false。
		bool lineBreak() const;
		void lineBreak(bool value);
		/// 項目上に置くコントロール。置かない場合は nullptr。
		Control* control() const;
		void control(Control* value);
		/// 項目上に置くコントロールの上下に空白を入れるどうか。初期値は true。
		bool controlEdge() const;
		void controlEdge(bool value);
		/// 項目上に置くコントロールが項目の幅に合わせて大きさを変えるかどうか。初期値は false。
		bool controlResizable() const;
		void controlResizable(bool value);
		/// 項目の表示可能なピクセル幅。width() がこの幅未満である場合、シェブロンを表示する。初期値は 0。
		int displayWidth() const;
		void displayWidth(int value);
		/// 項目上のコントロールをリサイズする場合の最小の大きさ。
		Size minControlSize() const;
		void minControlSize(const Size& value);
		void minControlSize(int width, int height);
		/// シェブロンをクリックしたイベント。
		Listener<Rebar::ChevronClick&>& onChevronClick();
		const Listener<Rebar::ChevronClick&>& onChevronClick() const;
		/// 項目上の大きさを変更できるかどうか。false の場合はグリップが表示されない。初期値は true。
		bool resizable() const;
		void resizable(bool value);
		/// 項目の文字列。初期値は空文字列。
		const String& text() const;
		void text(String value);
		/// 項目のピクセル幅。0 の場合は最小の大きさになる。
		int width() const;
		void width(int value);

	private:
		friend Rebar;
		friend Item;

		HWND _control;
		bool _controlResizable;
		HBITMAP _bitmap;
		int _style;
		String _text;
		int _width;
		int _displayWidth;
		Size _minControlSize;
		Listener<Rebar::ChevronClick&> _onChevronClick;
	};



	/// レバーコントロールのイベントの親クラス。
	typedef EventWithSubclassSender<Rebar, Control::Event> Event;

	typedef Event Resize;


	/// 項目に関するイベント。
	struct ItemEvent : public Event {
		ItemEvent(Rebar& sender, int itemIndex);

		/// イベントの発生した項目。
		Rebar::Item item();

	private:
		int _itemIndex;
	};


	/// シェブロンを押したイベント。
	struct ChevronClick : public ItemEvent {
		ChevronClick(Rebar& sender, int itemIndex, const Rectangle& bounds);

		/// シェブロンのクライアント座標領域。
		const Rectangle& chevronBounds() const;

	private:
		Rectangle _chevronBounds;
	};


public:
	/// ヌルポインタで作成。
	Rebar();
	Rebar(Rebar&& value, bool checkSlicing = true);
	/// 親から作成。親の上端に親の幅にあわせた大きさで作成する。
	Rebar(Control& parent);
	/// 親と項目情報配列から作成。親の上端に親の幅にあわせた大きさで作成する。
	Rebar(Control& parent, ArrayRange<const Rebar::ItemInfo> itemInfos);
	virtual ~Rebar();
	Rebar& operator=(Rebar&& value);

public:
	/// 項目を追加する。
	void add(const ItemInfo& itemInfo);
	/// 項目を全て削除する。
	void clear();
	/// 項目数。
	int count() const;
	/// 項目の境界をダブルクリックしたら最大化/最小化するかどうか。false の場合はシングルクリック。初期値は false。
	bool doubleClickToggle() const;
	void doubleClickToggle(bool value);
	/// 項目を削除する。
	void erase(int index);
	/// 指定したクライアント座標にある項目のインデックスを取得する。無い場合は -1。
	int getIndexAt(const Point& point) const;
	int getIndexAt(int x, int y) const;
	/// 項目を挿入する。
	void insert(int index, const ItemInfo& itemInfo);
	/// 項目の情報配列を一括で取得、設定する。
	std::vector<Rebar::ItemInfo> itemInfos() const;
	void itemInfos(ArrayRange<const Rebar::ItemInfo> value);
	/// 項目同士の境界に線を引くかどうか。初期値は true。
	bool itemBorderLines() const;
	void itemBorderLines(bool value);
	/// 項目の順番が固定されるかどうか。初期値は false。
	bool itemHeightVariable() const;
	void itemHeightVariable(bool value);
	/// 項目の順番が固定されるかどうか。初期値は false。
	bool itemOrderFixed() const;
	void itemOrderFixed(bool value);
	/// コントロールの大きさが変わったイベント。
	Listener<Rebar::Resize&>& onResize();
	/// 項目の列数。
	int rowCount() const;

public:
	/// index 番目の項目。順番は追加した順とは限らずユーザのドラッグ操作によって変化する。
	Rebar::Item operator[](int index);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Listener<Rebar::Resize&> _onResize;
};



	}
}