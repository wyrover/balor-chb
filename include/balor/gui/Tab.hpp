#pragma once

#include <vector>

#include <balor/gui/Control.hpp>

struct _IMAGELIST;

namespace balor {
	namespace graphics {
class ImageList;
	}
}


namespace balor {
	namespace gui {

class ToolTip;



/**
 * タブコントロール。
 *
 * マウスイベントはクリックできるタブ部分でのみ発生する。
 *
 * TCS_BUTTONS、TCS_FLATBUTTONS、TCS_VERTICAL、TCS_RIGHT は Visual Style が無効になるのでサポートしない。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Tab Sample");

	Panel page0(frame, 0, 0, 0, 0);
	Button button0(page0, 20, 10, 0, 0, L"button0", [&] (Button::Click& ) {
		MsgBox::show(L"button0");
	});
	Button button1(page0, 20, 50, 0, 0, L"button1", [&] (Button::Click& ) {
		MsgBox::show(L"button1");
	});
	Panel page1(frame, 0, 0, 0, 0);
	CheckBox check(page1, 20, 10, 0, 0, L"check");

	typedef Tab::ItemInfo Info;
	Info infos[] = {
		Info(L"タブ0", page0),
		Info(L"タブ1", page1),
		Info(L"タブ2"),
	};
	Tab tab(frame, 20, 10, 400, 300, infos);

	frame.runMessageLoop();
 * </code></pre>
 */
class Tab : public Control {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::balor::graphics::ImageList ImageList;


	/// タブのスタイル。
	struct Style {
		enum _enum {
			singleline      = 0x0000, /// タブは一列で表示される。はみ出る場合はスクロールできる。
			multiline       = 0x0200, /// タブがはみ出る場合は複数列で表示される。その場合タブの幅はコントロールの幅に合わせられる。
			multilineScroll = 0x0201, /// タブがはみ出る場合は複数列で表示され、選択されていない列が下に移動する。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};


	/// 項目の文字列の配置。
	struct TextAlign {
		enum _enum {
			center    = 0x0000, /// 中央揃え。
			iconLeft  = 0x0010, /// アイコンだけ左揃え、文字列は中央揃え。
			left      = 0x0020, /// 左揃え。
		};
		BALOR_NAMED_ENUM_MEMBERS(TextAlign);
	};


	class ItemInfo;

	/// 項目。Tab::operator[] で取得できる。
	class Item {
	public:
		Item(HWND ownerHandle, int index);
		/// 項目情報を反映。
		Item& operator=(const ItemInfo& itemInfo);

		/// 項目のクライアント座標領域。
		Rectangle bounds() const;
		/// アイコンの画像リスト（Tab::imageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// タブインデックス。
		int index() const;
		/// タブコントロールのハンドル。
		HWND ownerHandle() const;
		/// タブをクリックした時に表示するコントロール。Tab の子ウインドウにする。指定しない場合は nullptr。
		Control* page() const;
		void page(Control* value);
		/// 項目の文字列。
		String text() const;
		void text(StringRange value);

	private:
		HWND _ownerHandle;
		int _index;
	};


	/// 項目情報構造体。情報を持つだけでタブコントロールは一切操作しない。Tab::itemInfos 関数等で使用する。
	class ItemInfo {
	public:
		ItemInfo();
		ItemInfo(ItemInfo&& value);
		/// 項目情報の取得。
		ItemInfo(const Item& item);
		/// 文字列とアイコン画像インデックスから作成。
		explicit ItemInfo(String text, int imageIndex = -1);
		/// 文字列とページコントロールとアイコン画像インデックスから作成。
		ItemInfo(String text, const Control& page, int imageIndex = -1);
		ItemInfo& operator=(ItemInfo&& value);

		/// アイコンの画像リスト（Tab::imageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// タブをクリックした時に表示するコントロール。指定しない場合は nullptr。
		Control* page() const;
		void page(Control* value);
		/// 項目の文字列。
		const String& text() const;
		void text(String value);

	private:
		String _text;
		int _imageIndex;
		HWND _page;
	};



	/// タブコントロールのイベントの親クラス。
	typedef EventWithSubclassSender<Tab, Control::Event> Event;


	/// 項目に関するイベント。
	struct ItemEvent : public Event {
		ItemEvent(Tab& sender, int itemIndex);

		/// イベントの発生した項目。
		Tab::Item item();

	private:
		int _itemIndex;
	};


	typedef Event ItemSelect;


	/// 項目を選択するイベント。
	struct ItemSelecting : public Event {
		ItemSelecting(Tab& sender);

		/// 選択をキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	/// 項目のツールチップを表示するイベント。
	struct ItemTipPopup : public ItemEvent {
		ItemTipPopup(Tab& sender, int itemIndex, String& text);

		/// イベントの発生した項目。
		void setText(String value);

	private:
		String& _text;
	};


public:
	/// ヌルポインタで作成。
	Tab();
	Tab(Tab&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	Tab(Control& parent, int x, int y, int width, int height, Tab::Style style = Tab::Style::singleline);
	/// 親、位置、大きさ、項目情報配列から作成。
	Tab(Control& parent, int x, int y, int width, int height, ArrayRange<const Tab::ItemInfo> itemInfos, Tab::Style style = Tab::Style::singleline);
	virtual ~Tab();
	Tab& operator=(Tab&& value);

public:
	/// 項目を追加する。
	void add(const ItemInfo& itemInfo);
	/// タブをコントロールの下部に表示するかどうか。初期値は false。
	bool bottomItems() const;
	void bottomItems(bool value);
	/// 項目を全て削除する。
	void clear();
	/// 項目数。
	int count() const;
	/// Tab::Item::page() の表示範囲のクライアント座標領域。タブコントロールの境界と厳密には一致しないようだがテーマによって領域が変化する為かもしれない。
	virtual Rectangle displayRectangle() const;
	/// 項目を削除する。
	void erase(int index);
	/// 指定したクライアント座標にある項目のインデックスを取得する。無い場合は -1。
	int getIndexAt(const Point& point) const;
	int getIndexAt(int x, int y) const;
	/// アイコンの画像リスト。Tab は画像リストのハンドルを破棄しない。初期値はヌルハンドルの画像リスト。
	ImageList imageList() const;
	void imageList(HIMAGELIST value);
	/// 項目を挿入する。
	void insert(int index, const ItemInfo& itemInfo);
	/// 項目の情報配列を一括で取得、設定する。
	std::vector<Tab::ItemInfo> itemInfos() const;
	void itemInfos(ArrayRange<const Tab::ItemInfo> value);
	/// 項目の文字列と外枠の余白ピクセル数。
	void itemPadding(const Size& value);
	/// 項目のピクセルサイズを変更する。itemWidthFixed() が false の場合は高さのみ変更する。
	void itemSize(const Size& value);
	void itemSize(int width, int height);
	/// 項目ごとに表示するツールチップコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。ある程度 Tab が制御しているので動作しない設定もある。
	ToolTip itemTip();
	/// 項目の幅を固定にするかどうか。false の場合は文字列の長さに合わせる。itemSize() 関数で固定幅を指定できる。初期値は false。
	bool itemWidthFixed() const;
	void itemWidthFixed(bool value);
	/// 項目の最小ピクセル幅。-1 を指定するとデフォルトの幅に戻す。
	void minItemWidth(int value);
	/// 項目を選択したイベント。
	Listener<Tab::ItemSelect&>& onItemSelect();
	/// 項目を選択するイベント。
	Listener<Tab::ItemSelecting&>& onItemSelecting();
	/// 項目のツールチップを表示するイベント。
	Listener<Tab::ItemTipPopup&>& onItemTipPopup();
	/// Tab::Item::page() を項目の大きさに合わせてリサイズする。
	void resizeItemPages();
	/// タブの列数。
	int rowCount() const;
	/// 選択されている項目インデックス。無い場合は -1。
	/// タブページを表示したまま他のコントロールがフォーカスを得られるので選択されていてもフォーカスを得ているとは限らない。
	int selectedIndex() const;
	void selectedIndex(int value);
	/// タブのスタイル。
	Tab::Style style() const;
	/// 項目の文字列の配置。itemWidthFixed() が true の場合のみ変更できる。初期値は Tab::TextAlign::center。
	Tab::TextAlign textAlign() const;
	void textAlign(Tab::TextAlign value);

public:
	/// index 番目の項目。
	Tab::Item operator[](int index);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	String _temporaryText;
	Listener<Tab::ItemSelect&> _onItemSelect;
	Listener<Tab::ItemSelecting&> _onItemSelecting;
	Listener<Tab::ItemTipPopup&> _onItemTipPopup;
};



	}
}