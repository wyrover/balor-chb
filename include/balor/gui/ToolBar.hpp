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
 * ツールバー。
 *
 * ツールバー上に他のコントロールを置くにはツールバーを親コントロールとして作成し、
 * bounds() を置きたいセパレータの ToolBar::Item::bounds() に設定してやれば良い。
 * 
 * TB_SETANCHORHIGHLIGHT は効果を確認できず。
 * TB_SETCOLORSCHEME は Visual Style 有効時には無効なので実装せず。
 * TB_GETITEMDROPDOWNRECT は MSDN では XP 以降となっているがヘッダファイル内では Vista 以降となっている。
 * TBSTATE_ELLIPSES は指定してもしなくても文字列の省略記号が表示される為、効果不明。
 * TBSTYLE_TRANSPARENT は動作せず。Visual Style 無効、後からスタイル変更でもダメ。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ToolBar Sample");

	typedef PopupMenu::ItemInfo MenuInfo;
	MenuInfo menuInfos[] = {
		MenuInfo(L"ドロップダウン0"),
		MenuInfo(L"ドロップダウン1"),
		MenuInfo(L"ドロップダウン2"),
	};
	PopupMenu dropDown(menuInfos);

	typedef ToolBar::ItemInfo Info;
	Info infos[] = {
		Info(ImageList::StandardImage::cut, L"切り取り", [&] (ToolBar::Click& ) {
			MsgBox::show(L"切り取った");
		}),
		Info(ImageList::StandardImage::copy, L"コピー", [&] (ToolBar::Click& ) {
			MsgBox::show(L"コピーした");
		}),
		Info(ImageList::StandardImage::paste, L"貼り付け", [&] (ToolBar::Click& ) {
			MsgBox::show(L"貼り付けた");
		}),
		Info(ImageList::StandardImage::properties, L"プロパティ", ToolBar::ItemStyle::dropDownButton, [&] (ToolBar::Click& e) {
			if (e.dropDown()) {
				dropDown.show(frame, e.item().bounds().bottomLeft());
			} else {
				MsgBox::show(L"プロパティ");
			}
		})
	};
	ImageList imageList(ImageList::SystemResource::largeStandardImages);
	ToolBar tool(frame, 0, 0, 0, 0, imageList, infos);

	frame.runMessageLoop();
 * </code></pre>
 */
class ToolBar : public Control {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::balor::graphics::ImageList ImageList;


	///	項目のスタイル。
	struct ItemStyle {
		enum _enum {
			button         = 0x0000, /// 普通のボタン。
			separator      = 0x0001, /// ボタン同士の区切り。
			checkBox       = 0x0002, /// トグルボタン。
			radioButton    = 0x0006, /// ラジオボタン。
			dropDown       = 0x0088, /// ドロップダウン。
			dropDownButton = 0x0008, /// ボタンとドロップダウン。このボタンを追加するとツールバーの縦幅が大きくなる。
		};
		BALOR_NAMED_ENUM_MEMBERS(ItemStyle);
	};


	class ItemInfo;
	struct ItemTipPopup;
	struct Click;


	/// 項目。ToolBar::operator[] で取得できる。
	class Item {
	public:
		Item(HWND ownerHandle, int index);
		/// 項目情報を反映。
		Item& operator=(const ItemInfo& itemInfo);

		/// 項目の幅を文字列の幅に合わせるかどうか。ToolTip::bottomText() が true の場合のみ有効。false の場合は最大幅で統一される。
		bool autoSize() const;
		void autoSize(bool value);
		/// 項目のクライアント座標領域。
		Rectangle bounds() const;
		/// トグルで押された状態かどうか。
		bool checked() const;
		void checked(bool value);
		/// 押せる状態かどうか。
		bool enabled() const;
		void enabled(bool value);
		/// アイコンの画像リスト（ToolBar::imageList()）のインデックス。-1 なら表示しない。
		/// style() が ToolBar::ItemStyle::separator の場合はセパレータのピクセル幅になる。別名の separatorSize() 関数を使うと良い。
		int imageIndex() const;
		void imageIndex(int value);
		/// 項目のインデックス。
		int index() const;
		/// 文字列の & を特別処理するかどうか。文字列をツールチップに表示する場合は無効。
		bool noPrefix() const;
		void noPrefix(bool value);
		/// ボタンを押したイベント。
		Listener<ToolBar::Click&>& onClick();
		/// 項目のツールチップを表示するイベント。
		Listener<ToolBar::ItemTipPopup&>& onItemTipPopup();
		/// ツールバーのハンドル。
		HWND ownerHandle() const;
		/// text() をボタンの右に表示するかどうか。false の場合は text() をツールチップ文字列とする。
		/// ToolTip::bottomText() が false の場合のみ有効。
		bool rightText() const;
		void rightText(bool value);
		/// ボタンを押した状態かどうか。
		bool pushed() const;
		/// セパレータのピクセル幅。wrap() が true の場合は縦方向の大きさ。
		/// style() が ToolBar::ItemStyle::separator 以外の場合はアイコンの画像リストのインデックスになる。別名の imageIndex() 関数を使うと良い。
		int separatorSize() const;
		void separatorSize(int value);
		/// 項目のスタイル。
		ToolBar::ItemStyle style() const;
		void style(ToolBar::ItemStyle value);
		/// 項目のツールチップ文字列。ToolTip::bottomText() が true の場合はボタン下部に表示される。そうではなく rightText() が true の場合はボタンの右に表示される。
		/// 直接表示しつつツールチップ文字列も表示したい場合は onItemTipPopup() イベントを処理する。
		String text() const;
		void text(StringRange value);
		/// 表示するかどうか。。
		bool visible() const;
		void visible(bool value);
		/// 項目のピクセル幅。文字列が表示されている場合（ToolBar::bottomText() が true か、または ToolBar::bottomText() が false かつ rightText() が true の場合）のみ変更できる。
		/// セパレータの大きさを変更する場合は separatorSize() を使用しないと縦方向の大きさが変わらない。
		int width() const;
		void width(int value);
		/// 次の項目から改行するかどうか。
		bool wrap() const;
		void wrap(bool value);

	private:
		HWND _ownerHandle;
		int _index;
	};


	/// 項目情報構造体。情報を持つだけでツールバーは一切操作しない。ToolBar::itemInfos 関数等で使用する。
	/// デフォルトコンストラクタで作成するとセパレータになる。
	class ItemInfo {
	public:
		/// セパレータとして作成。
		ItemInfo();
		ItemInfo(ItemInfo&& value);
		/// 項目情報の取得。
		ItemInfo(const Item& item);
		/// 画像インデックス、文字列、クリックイベントから普通のボタンとして作成。
		explicit ItemInfo(int imageIndex, String text = String(), Listener<ToolBar::Click&> onClick = Listener<ToolBar::Click&>());
		/// 画像インデックス、文字列、項目スタイル、クリックイベントから作成。
		ItemInfo(int imageIndex, String text, ToolBar::ItemStyle style, Listener<ToolBar::Click&> onClick);
		ItemInfo& operator=(ItemInfo&& value);

		/// 項目の幅を文字列の幅に合わせるかどうか。ToolTip::bottomText() が true の場合のみ有効。false の場合は最大幅で統一される。初期値は false。
		bool autoSize() const;
		void autoSize(bool value);
		/// トグルで押された状態かどうか。
		bool checked() const;
		void checked(bool value);
		/// 押せる状態かどうか。
		bool enabled() const;
		void enabled(bool value);
		/// アイコンの画像リスト（ToolBar::imageList()）のインデックス。-1 なら表示しない。
		/// style() が ToolBar::ItemStyle::separator の場合はセパレータのピクセル幅になる。別名の separatorSize() 関数を使うと良い。
		int imageIndex() const;
		void imageIndex(int value);
		/// 文字列の & を特別処理するかどうか。文字列をツールチップに表示する場合は無効。初期値は false。
		bool noPrefix() const;
		void noPrefix(bool value);
		/// ボタンを押したイベント。
		Listener<ToolBar::Click&>& onClick();
		const Listener<ToolBar::Click&>& onClick() const;
		/// 項目のツールチップを表示するイベント。
		Listener<ToolBar::ItemTipPopup&>& onItemTipPopup();
		const Listener<ToolBar::ItemTipPopup&>& onItemTipPopup() const;
		/// text() をボタンの右に表示するかどうか。false の場合は text() をツールチップ文字列とする。初期値は false。
		/// ToolTip::bottomText() が false の場合のみ有効。
		bool rightText() const;
		void rightText(bool value);
		/// セパレータのピクセル幅。0 の場合はデフォルトの大きさ。wrap() が true の場合は縦方向の大きさ。
		/// style() が ToolBar::ItemStyle::separator 以外の場合はアイコンの画像リストのインデックスになる。別名の imageIndex() 関数を使うと良い。
		int separatorSize() const;
		void separatorSize(int value);
		/// 項目のスタイル。
		ToolBar::ItemStyle style() const;
		void style(ToolBar::ItemStyle value);
		/// 項目のツールチップ文字列。ToolTip::bottomText() が true の場合はボタン下部に表示される。そうではなく rightText() が true の場合はボタンの右に表示される。
		/// 直接表示しつつツールチップ文字列も表示したい場合は onItemTipPopup() イベントを処理する。
		const String& text() const;
		void text(String value);
		/// 表示するかどうか。初期値は true。
		bool visible() const;
		void visible(bool value);
		/// 次の項目から改行するかどうか。初期値は false。
		bool wrap() const;
		void wrap(bool value);

	private:
		friend ToolBar;
		friend Item;

		int _imageIndex;
		String _text;
		int _itemDataStyle;
		int _itemDataState;
		Listener<ToolBar::Click&> _onClick;
		Listener<ToolBar::ItemTipPopup&> _onItemTipPopup;
	};


	/// ツールバーのイベントの親クラス。
	typedef EventWithSubclassSender<ToolBar, Control::Event> Event;


	/// 項目で発生するイベント。
	struct ItemEvent : public Event {
		ItemEvent(ToolBar& sender, int itemIndex);

		/// イベントの発生した項目。
		ToolBar::Item item() const;

	private:
		int _itemIndex;
	};


	/// ボタンをクリックしたイベント。
	struct Click : public ItemEvent {
		Click(ToolBar& sender, int itemIndex, bool dropDown);

		/// ドロップダウンをクリックしたかどうか。
		bool dropDown() const;

	private:
		bool _dropDown;
	};


	/// 項目のツールチップを表示するイベント。
	struct ItemTipPopup : public ItemEvent {
		ItemTipPopup(ToolBar& sender, int itemIndex, wchar_t* buffer, int bufferSize);

		/// 表示する文字列を設定する。文字列の長さには上限があって切り捨てられる。
		void setText(StringRange value);

	private:
		wchar_t* _buffer;
		int _bufferSize;
	};


public:
	/// ヌルポインタで作成。
	ToolBar();
	ToolBar(ToolBar&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	ToolBar(Control& parent, int x, int y, int width, int height, HIMAGELIST imageList);
	/// 親、位置、大きさ、項目情報配列から作成。
	ToolBar(Control& parent, int x, int y, int width, int height, HIMAGELIST imageList, ArrayRange<const ItemInfo> itemInfos);
	virtual ~ToolBar();
	ToolBar& operator=(ToolBar&& value);

public:
	/// 項目を追加する。
	void add(const ItemInfo& itemInfo);
	/// ボタンの下部に文字列を表示するかどうか。true の場合は ToolTip::Item::text() がボタンの下部に常に表示される。
	/// ツールチップを表示する場合は ToolTip::Item::onItemTipPopup イベントを処理する必要がある。
	/// false の場合はボタン下部に表示されず、ツールチップ文字列として表示される。ただし ToolTip::Item::rightText() が true の場合はボタンの右側に文字列が表示される。
	/// 初期値は false。
	bool bottomText() const;
	void bottomText(bool value);
	/// 項目を全て削除する。
	void clear();
	/// 項目数。
	int count() const;
	/// 入力を受け付けない項目の画像リスト。設定しない場合は imageList() の画像がグレイスケールで表示される。初期値は nullptr。ToolBar は画像リストのハンドルを破棄しない。
	ImageList disabledImageList() const;
	void disabledImageList(HIMAGELIST value);
	/// 上部に2ピクセル分のハイライト表示をするかどうか。初期値は false。
	bool divider() const;
	void divider(bool value);
	/// コントロールの境界線の種類。初期値は Control::Edge::none。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 項目を削除する。
	void erase(int index);
	/// フォーカスされている項目インデックス。無い場合は -1。
	int focusedIndex() const;
	void focusedIndex(int value);
	/// 指定したクライアント座標にある項目のインデックスを取得する。セパレータのインデックスは返さない。見つからない場合は負数を返す。
	int getIndexAt(const Point& point) const;
	int getIndexAt(int x, int y) const;
	/// 適切なコントロールサイズ。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 少しでも表示しきれない項目を隠すかどうか。初期値は false。
	bool hideClippedItems() const;
	void hideClippedItems(bool value);
	/// マウスカーソルの下の項目の画像リスト。設定しない場合は imageList() の画像が表示される。初期値は nullptr。ToolBar は画像リストのハンドルを破棄しない。
	ImageList hotImageList() const;
	void hotImageList(HIMAGELIST value);
	/// 項目の画像リスト。ToolBar は画像リストのハンドルを破棄しない。
	ImageList imageList() const;
	void imageList(HIMAGELIST value);
	/// 最初の項目の横方向のピクセル位置。初期値はおそらく０。
	void indent(int value);
	/// 項目を挿入する。
	void insert(int index, const ItemInfo& itemInfo);
	/// 項目の情報配列を一括で取得、設定する。
	std::vector<ToolBar::ItemInfo> itemInfos() const;
	void itemInfos(ArrayRange<const ToolBar::ItemInfo> value);
	/// 項目全てを含む大きさ。
	Size itemsSize() const;
	/// 項目に表示するツールチップコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。ある程度 ToolBar が制御しているので動作しない設定もある。
	ToolTip itemTip();
	/// クライアント領域の大きさからコントロールの大きさを求める。
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// 背景が透けるかどうか。現状では親が Rebar である場合にのみ透ける。初期値は false。
	bool transparent() const;
	void transparent(bool value);
	/// ツールバーを縦表示にするかどうか。true の場合、セパレータの表示が縦方向になり全ての項目の ToolBar::Item::wrap() を true に設定する。
	bool vertical() const;
	void vertical(bool value);
	/// ツールバーの幅で項目を表示しきれない時に改行するかどうか。vertical() と同時に true にすることはできない。初期値は false。
	/// 改行の入るセパレータは縦表示になる。改行の位置はシステムで計算される。
	/// bottomText() が false の場合、ラジオボタン等を表示しきれなくても改行しない場合がある。
	bool wrappable() const;
	void wrappable(bool value);

public:
	/// index 番目の項目。
	ToolBar::Item operator[](int index);
	const ToolBar::Item operator[](int index) const;

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	struct ItemListener {
		ItemListener();
		ItemListener(const ItemInfo& info);

		Listener<ToolBar::Click&> onClick;
		Listener<ToolBar::ItemTipPopup&> onItemTipPopup;
	};
	std::vector<ItemListener> _itemListeners;
};



	}
}