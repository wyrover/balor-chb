#pragma once

#include <vector>

#include <balor/graphics/Color.hpp>
#include <balor/gui/Control.hpp>
#include <balor/StringRangeArray.hpp>

struct _IMAGELIST;
struct tagNMLVCUSTOMDRAW;

namespace std {
	namespace tr1 {
template<typename T> class function;
	}
}

namespace balor {
	namespace graphics {
class ImageList;
	}
}


namespace balor {
	namespace gui {

class Edit;
class ToolTip;



/**
 * リストビュー。
 * 
 * マウス左ボタンか右ボタンを押した場合、onMouseDown イベントはボタンを離した時に発生し、onMouseUp イベントは発生しない。
 * 代わりに onItemClick イベントや onItemRightClick イベントを使用できる。
 * onDrag イベントはうまく拾えない。onDrag の代わりに onItemDrag イベントを使用できる。
 * グループは未実装。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ListView Sample");

	typedef ListView::ColumnInfo Column;
	Column columns[] = {
		Column(L"果物名"),
		Column(L"色"),
		Column(L"大きさ")
	};
	typedef ListView::ItemInfo Item;
	const wchar_t* item0Texts[] = {L"ブドウ", L"紫", L"小"};
	const wchar_t* item1Texts[] = {L"メロン", L"緑", L"中"};
	const wchar_t* item2Texts[] = {L"スイカ", L"赤", L"大"};
	Item items[] = {
		Item(item0Texts),
		Item(item1Texts),
		Item(item2Texts)
	};
	ListView list(frame, 20, 10, 0, 0, columns, items);
	list.fullRowSelect(true);
 
	frame.runMessageLoop();
 * </code></pre>
 */
class ListView : public Control {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::balor::graphics::ImageList ImageList;

	/// 項目の文字列の終端文字を含めた最大長。
	static const int maxTextLength = 512;


	/// 項目の文字列の配置。
	struct Align {
		enum _enum {
			left   = 0x0000, /// 左揃え。
			right  = 0x0001, /// 右揃え。
			center = 0x0002, /// 中央揃え。
		};
		BALOR_NAMED_ENUM_MEMBERS(Align);
	};


	/// 項目の部分。
	struct ItemPortion {
		enum _enum {
			whole        = 0x0000, /// 項目全体。
			icon         = 0x0001, /// アイコン部分。
			text         = 0x0002, /// 文字列部分。
			selectBounds = 0x0003, /// 選択可能部分。fullRowSelect() によって範囲が変わる。
		};
		BALOR_NAMED_ENUM_MEMBERS(ItemPortion);
	};


	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none          = 0     , 
			checkBoxes    = 0x0004, /// 項目ごとにチェックボックスを表示する。stateImageList() を操作してチェック画像を変更できる。画像を追加するとチェック状態が増える。
			noHeaderClick = 0x8000, /// ヘッダをボタンのようにクリックできないようにする。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// 表示スタイル。
	struct Style {
		enum _enum {
			largeIcon = 0x0000, /// 大きなアイコン表示。
			details   = 0x0001, /// 詳細表示。
			smallIcon = 0x0002, /// 小さなアイコン表示。
			list      = 0x0003, /// 一覧表示。
			tile      = 0x0004, /// 並べて表示。EnableVisualStyle.hpp のインクルードが必要。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};



	class ColumnInfo;

	/// 縦の行。ListView::getColumn 関数で取得できる。
	class Column {
	public:
		Column(HWND ownerHandle, int index);
		/// 縦の行の情報反映。
		Column& operator=(const ColumnInfo& columnInfo);

		/// 縦の行の文字列配置。最初の行は ListView::Align::left で固定される。
		ListView::Align align() const;
		void align(ListView::Align value);
		/// 幅をヘッダーの文字列幅に合わせる。項目の文字列幅以下にはならない。
		void fitWidthToHeader();
		/// 幅を項目の文字列幅に合わせる。
		void fitWidthToItem();
		/// ヘッダの画像リスト（ListView::smallImageList()）のインデックス。-1 なら表示しない。
		int imageIndex() const;
		void imageIndex(int value);
		/// 縦の行のインデックス。
		int index() const;
		/// 表示順序。
		int order() const;
		void order(int value);
		/// リストビューのハンドル。
		HWND ownerHandle() const;
		/// 画像を文字列の右に表示するかどうか。最初の行は false で固定される。
		bool rightImage() const;
		void rightImage(bool value);
		/// 選択されているように表示するかどうか。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
		bool selected() const;
		void selected(bool value);
		/// ヘッダの文字列。
		String text() const;
		void text(StringRange value);
		/// 縦の行の幅。
		int width() const;
		void width(int value);

	private:
		HWND _ownerHandle;
		int _index;
	};


	/// 縦の行の情報構造体。情報を持つだけでリストビューは一切操作しない。ListView::columnInfos 関数等で使用する。
	class ColumnInfo {
	public:
		ColumnInfo(ColumnInfo&& value);
		/// 縦の行の情報取得。
		ColumnInfo(const Column& column);
		/// 全要素から作成。引数は同名の関数を参照。
		explicit ColumnInfo(String text = L"", int width = 0, ListView::Align align = Align::left, int imageIndex = -1, bool rightImage = false, int order = -1);
		ColumnInfo& operator=(ColumnInfo&& value);

		/// 縦の行の文字列配置。最初の行は ListView::Align::left で固定される。
		ListView::Align align() const;
		void align(ListView::Align value);
		/// ヘッダの画像リスト（ListView::smallImageList）のインデックス。-1 なら表示しない。
		int imageIndex() const;
		void imageIndex(int value);
		/// 表示順序。-1 なら追加した順。
		int order() const;
		void order(int value);
		/// 画像を文字列の右に表示するかどうか。最初の行は false で固定される。
		bool rightImage() const;
		void rightImage(bool value);
		/// ヘッダの文字列。
		const String& text() const;
		void text(String value);
		/// 縦の行の幅。
		int width() const;
		void width(int value);

	private:
		String _text;
		int _width;
		ListView::Align _align;
		int _imageIndex;
		bool _rightImage;
		int _order;
	};


	class ItemInfo;

	/// 項目。ListView::operator[] で取得できる。
	class Item {
	public:
		Item(HWND ownerHandle, int index);
		/// 項目情報を反映。
		Item& operator=(const ItemInfo& itemInfo);

		/// チェックボックスがチェックされているかどうか。チェックのオンとオフは state() の 1 と 0 に該当する。
		bool checked() const;
		void checked(bool value);
		/// 切り取りされた状態かどうか。アイコンが薄く表示される。
		bool cut() const;
		void cut(bool value);
		/// 項目が見えるようにスクロールする。
		void ensureVisible();
		/// リストビュー内での項目の全体または部分領域。
		Rectangle getBounds(ListView::ItemPortion portion = ListView::ItemPortion::whole) const;
		/// リストビュー内でのサブ項目の領域。columnIndex が 0 の場合は項目全体の領域。
		Rectangle getSubBounds(int columnIndex) const;
		/// 文字列を取得する。
		String getText(int columnIndex) const;
		void getTextToBuffer(StringBuffer& buffer, int columnIndex) const;
		/// ハイライト表示かどうか。選択状態と同じ表示。
		bool highlight() const;
		void highlight(bool value);
		/// アイコンの画像リスト（ListView::smallImageList() または ListView::largeImageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// インデント。1 につき状態アイコンの幅ほどインデントする。
		int indent() const;
		void indent(int value);
		/// 項目のインデックス。
		int index() const;
		/// リストビューのハンドル。
		HWND ownerHandle() const;
		/// 項目の位置。
		Point position() const;
		void position(const Point& value);
		void position(int x, int y);
		/// 選択されているかどうか。
		bool selected() const;
		void selected(bool value);
		/// 文字列を設定する。ListView::maxTextLength 文字以上は設定できない。
		void setText(int columnIndex, StringRange text);
		/// 状態。状態画像リスト（ListView::stateImageList()）のインデックスで、-1 なら表示しない。
		int state() const;
		void state(int value);
		/// ０番目の文字列。
		String text() const;
		void text(StringRange value);
		/// 文字列配列。
		std::vector<String> texts() const;
		void texts(StringRangeArray value);
		/// ユーザ定義のデータ。取得する場合は UniqueAny の中身の型を指定する。
		template<typename T> T userData() {
			auto data = _userData();
			return any_cast<T>(*reinterpret_cast<UniqueAny*>(&data));
		}
		template<typename T> T userData() const {
			auto data = _userData();
			return any_cast<T>(*reinterpret_cast<const UniqueAny*>(&data));
		}
		void userData(UniqueAny&& value);
		bool userDataIsEmpty() const;

	private:
		void* _userData() const;

		HWND _ownerHandle;
		int _index;
	};


	/// 項目情報構造体。情報を持つだけでリストビューは一切操作しない。ListView::itemInfos 関数等で使用する。
	class ItemInfo {
	public:
		ItemInfo(ItemInfo&& value);
		/// 項目情報の取得。
		ItemInfo(const Item& item);
		/// 文字列とアイコン画像インデックス等から作成。
		explicit ItemInfo(String text = L"", int imageIndex = 0, int state = -1, int indent = 0);
		/// 文字列配列とアイコン画像インデックス等から作成。
		explicit ItemInfo(StringRangeArray texts, int imageIndex = 0, int state = -1, int indent = 0);
		ItemInfo& operator=(ItemInfo&& value);

		/// 切り取りされた状態かどうか。アイコンが薄く表示される。初期値は false。
		bool cut() const;
		void cut(bool value);
		/// ハイライト表示かどうか。選択状態と同じ表示。初期値は false。
		bool highlight() const;
		void highlight(bool value);
		/// アイコンの画像リスト（ListView::smallImageList() または ListView::largeImageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// インデント。1 につき状態アイコンの幅ほどインデントする。
		int indent() const;
		void indent(int value);
		/// 選択されているかどうか。
		bool selected() const;
		void selected(bool value);
		/// 状態。状態画像リスト（ListView::stateImageList()）のインデックスで、-1 なら表示しない。
		int state() const;
		void state(int value);
		/// 項目の文字列。
		String text() const;
		void text(String value);
		/// 項目の文字列配列。
		std::vector<String>& texts();
		const std::vector<String>& texts() const;
		void texts(StringRangeArray value);

	private:
		friend ListView;
		friend Item;

		std::vector<String> _texts;
		int _imageIndex;
		int _itemDataState;
		int _indent;
	};



	/// リストビューのイベントの親クラス。
	typedef EventWithSubclassSender<ListView, Control::Event> Event;


	/// ヘッダをクリックしたイベント。
	struct HeaderClick : public Event {
		HeaderClick(ListView& sender, int columnIndex);

		/// クリックされた縦の行インデックス。
		int columnIndex() const;

	private:
		int _columnIndex;
	};


	/// 項目に関するイベント。
	struct ItemEvent : public Event {
		ItemEvent(ListView& sender, int itemIndex);

		/// イベントの発生した項目。
		ListView::Item item();

	private:
		int _itemIndex;
	};


	/// 項目をクリックしたイベント。
	struct ItemClick : public ItemEvent {
		ItemClick(ListView& sender, int itemIndex, int columnIndex, const Point& occurredPoint);

		/// 縦の行のインデックス。
		int columnIndex() const;
		/// イベントの発生した座標。
		const Point& occurredPoint() const;

	private:
		int _columnIndex;
		Point _occurredPoint;
	};

	typedef ItemClick ItemDoubleClick;
	typedef ItemClick ItemRightClick;


	/// 項目が変化したイベント。
	struct ItemChange : public ItemEvent {
		ItemChange(ListView& sender, int itemIndex, int oldStates, int newStates);

		/// 新しいフォーカス状態。
		bool newFocused() const;
		/// 新しい選択状態。
		bool newSelected() const;
		/// 新しい状態。
		int newState() const;
		/// 古いフォーカス状態。
		bool oldFocused() const;
		/// 古い選択状態。
		bool oldSelected() const;
		/// 古い状態。
		int oldState() const;

	private:
		int _oldStates;
		int _newStates;
	};


	/// 項目が変化しようとしているイベント。
	struct ItemChanging : public ItemChange {
		ItemChanging(ListView& sender, int itemIndex, int oldStates, int newStates);

		/// 変化をキャンセルするかどうか。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	/// 項目をマウス右ボタンまたは左ボタンでドラッグしたイベント。ドラッグを開始する範囲は選択できる範囲と同じ。
	struct ItemDrag : public ItemEvent {
		ItemDrag(ListView& sender, int itemIndex, bool rButton);

		/// 右ボタンドラッグかどうか。
		bool rButton() const;

	private:
		bool _rButton;
	};


	/// 項目を描画する直前のイベント。選択行の色を変えるには selected() を false に設定する。
	struct ItemPrePaint : public Event {
		ItemPrePaint(ListView& sender, ::tagNMLVCUSTOMDRAW* info);

		/// 項目の背景の色。ListView::brush 関数で画像ブラシを指定した場合は無効。selected() が true の場合も無効。
		Color backColor() const;
		void backColor(const Color& value);
		/// 項目のクライアント座標領域。
		Rectangle bounds() const;
		/// 縦の行のインデックス。
		int columnIndex() const;
		/// フォーカスがあるように描画するかどうか。
		bool focused() const;
		void focused(bool value);
		/// 項目の文字列のフォント。
		Font font() const;
		void font(HFONT value);
		/// イベントの発生した項目。
		ListView::Item item();
		/// 選択しているように描画するかどうか。選択項目の色を変えるにはこれを false にしなければならない。
		bool selected() const;
		void selected(bool value);
		/// 項目の文字列の色。
		Color textColor() const;
		void textColor(const Color& value);

	private:
		friend ListView;

		::tagNMLVCUSTOMDRAW* _info;
		bool _fontChanged;
	};


	/// 項目のツールチップが表示されるイベント。
	struct ItemTipPopup : public ItemEvent {
		ItemTipPopup(ListView& sender, int itemIndex, wchar_t* buffer, int bufferSize);

		/// 表示する文字列を設定する。文字列の長さには上限があって切り捨てられる。
		void setText(StringRange value);

	private:
		wchar_t* _buffer;
		int _bufferSize;
	};


	/// 項目の文字列編集が終わって反映する直前のイベント。
	struct TextEdit : public ItemEvent {
		TextEdit(ListView& sender, int itemIndex, String itemText);

		/// 編集結果を反映しないかどうか。
		bool cancel() const;
		void cancel(bool value);
		/// 編集テキスト。
		const String& itemText() const;
		void itemText(String value);

	private:
		String _itemText;
		bool _cancel;
	};


	/// 項目の文字列編集が始まる直前のイベント。
	struct TextEditing : public TextEdit {
		TextEditing(ListView& sender, int itemIndex, String itemText);

		/// 編集をするエディットコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。
		/// ある程度 ListView が制御しているので動作しない設定もある。
		Edit edit();
	};


	/// ヒットテストの結果。
	struct HitTestInfo {
		HitTestInfo(int itemIndex, int columnIndex, int flags);

		/// 縦の行のインデックス。当たってない場合は -1。
		int columnIndex() const;
		/// 項目インデックス。当たってない場合は -1。
		int itemIndex() const;
		/// アイコン画像の上。
		bool onImage() const;
		/// 状態画像の上。
		bool onStateImage() const;
		/// 文字列の上。
		bool onText() const;

	private:
		int _itemIndex;
		int _columnIndex;
		int _flags;
	};


public:
	/// ヌルポインタで作成。
	ListView();
	ListView(ListView&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	ListView(Control& parent, int x, int y, int width, int height, ListView::Style style = Style::details, ListView::Options options = Options::none);
	/// 親、位置、大きさ、項目情報配列から作成。
	ListView(Control& parent, int x, int y, int width, int height, ArrayRange<const ItemInfo> itemInfos, ListView::Style style = Style::details, ListView::Options options = Options::none);
	/// 親、位置、大きさ、縦の行情報配列、項目情報配列から作成。大きさを０にすると getPreferredSize 関数で求める。
	ListView(Control& parent, int x, int y, int width, int height, ArrayRange<const ColumnInfo> columnInfos, ArrayRange<const ItemInfo> itemInfos, ListView::Style style = Style::details, ListView::Options options = Options::none);
	virtual ~ListView();
	ListView& operator=(ListView&& value);

public:
	/// 項目を追加する。
	void add(const ItemInfo& itemInfo);
	/// フォーカスが外れても選択状態を表示するかどうか。初期値は false。
	bool alwaysSelected() const;
	void alwaysSelected(bool value);
	/// アイコンを整列する。style() が ListView::Style::largeIcon, smallIcon, tile の場合のみ有効。snapToGrid が true の場合は項目を一番近い一定間隔の升目の中に移動する。
	void arrange(bool snapToGrid = false);
	/// アイコンを自動整列するかどうか。初期値は true。
	bool autoArrange() const;
	void autoArrange(bool value);
	/// 項目の文字列の編集を始める。強制的にフォーカスを得る。
	void beginEdit(int index);
	/// 背景ブラシ。ブラシハンドルは参照されるので破棄しないよう注意。ハッチブラシは使えない。MSDN には COM の初期化が必要とあるが初期化しなくとも動いている。
	using Control::brush;
	virtual void brush(HBRUSH value);
	/// 背景ブラシの原点。設定には EnableVisualStyle.hpp のインクルードが必要。
	using Control::brushOrigin;
	virtual void brushOrigin(const Point& value);
	/// 項目を全て削除する。
	void clear();
	/// 縦の行の数。
	int columnCount() const;
	/// 縦の行の情報配列を一括で取得、設定する。ColumnInfo::width() が 0 の場合は Column::fitWidthToHeader 関数で幅を求める。
	std::vector<ListView::ColumnInfo> columnInfos() const;
	void columnInfos(ArrayRange<const ListView::ColumnInfo> value);
	/// 項目数。
	int count() const;
	/// リストビュー内に表示できる項目数。style() が ListView::Style::details か list の場合のみ有効。
	int countPerPage() const;
	/// コントロールの境界線の種類。初期値は Control::Edge::client。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 項目の文字列の編集を終了する。
	void endEdit();
	/// 項目を削除する。
	void erase(int index);
	/// 指定したインデックス以降で指定した文字列をもつ項目を見つける。サブ項目の文字列は検索されない。見つからない場合は -1。
	int findIndex(StringRange text, int startIndex = 0, bool prefixSearch = false) const;
	/// フォーカスされている項目インデックス。無い場合は -1。
	int focusedIndex() const;
	void focusedIndex(int value);
	/// 横の行を全て選択状態にするかどうか。初期値は false。
	bool fullRowSelect() const;
	void fullRowSelect(bool value);
	/// 縦の行を取得する。
	ListView::Column getColumn(int index);
	/// 指定したクライアント座標のヒットテストを行って情報を取得する。
	ListView::HitTestInfo getHitTestInfo(const Point& point) const;
	ListView::HitTestInfo getHitTestInfo(int x, int y) const;
	/// 指定したクライアント座標にある項目のインデックスを取得する。無い場合は -1。
	int getIndexAt(const Point& point) const;
	int getIndexAt(int x, int y) const;
	/// 適切なコントロールサイズ。font(), count() 等の値によって最適なサイズを返す。style() が ListView::Style::details の場合のみ正確なサイズを計算できる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 指定したインデックス以降で選択されている項目のインデックスを取得する。
	/// 引数に前回取得したインデックス + 1 を指定すれば次のインデックスを見つけられる。見つからない場合は -1 を返す。
	int getSelectedIndex(int startIndex = 0) const;
	/// 項目境界線を表示するかどうか。初期値は false。
	bool gridLines() const;
	void gridLines(bool value);
	/// ヘッダをドラッグ＆ドロップで移動できるかどうか。初期値は false。
	bool headerDragDrop() const;
	void headerDragDrop(bool value);
	/// ヘッダを表示するかどうか。初期値は true。
	bool headerVisible() const;
	void headerVisible(bool value);
	/// 項目にカーソルを合わせるとカーソルが Cursor::hand() になって文字列にアンダーラインが表示されるかどうか。初期値は false。
	bool hotTracking() const;
	void hotTracking(bool value);
	/// 項目にカーソルを合わせてしばらくすると項目が選択されるかどうか。初期値は false。
	bool hotTrackSelection() const;
	void hotTrackSelection(bool value);
	/// 項目同士の距離。Size(-1, -1) を指定するとデフォルトに戻る。style() が ListView::Style::largeIcon の場合のみ有効。縦幅が変わらない模様。
	Size iconSpacing() const;
	void iconSpacing(const Size& value);
	/// 項目を挿入する。
	void insert(int index, const ItemInfo& itemInfo);
	/// 項目の情報配列を一括で取得、設定する。
	std::vector<ListView::ItemInfo> itemInfos() const;
	void itemInfos(ArrayRange<const ListView::ItemInfo> value);
	/// 項目ごとに表示するツールチップコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。ある程度 ListView が制御しているので動作しない設定もある。
	ToolTip itemTip();
	/// 項目ごとにツールチップを表示するかどうか。onItemTipPopup() イベントで表示する文字列を指定する。初期値は false。
	bool itemTipEnabled() const;
	void itemTipEnabled(bool value);
	/// 大きいアイコンの画像リスト。ListView は画像リストのハンドルを破棄しない。
	ImageList largeImageList() const;
	void largeImageList(HIMAGELIST value);
	/// アイコンを左上から下に整列するかどうか。false の場合は左上から右に整列する。初期値は false。
	bool leftArrange() const;
	void leftArrange(bool value);
	/// アイテムを複数選択できるかどうか。初期値は true。
	bool multiselect() const;
	void multiselect(bool value);
	/// ヘッダをクリックしたイベント。
	Listener<ListView::HeaderClick&>& onHeaderClick();
	/// 項目が変化したイベント。
	Listener<ListView::ItemChange&>& onItemChange();
	/// 項目が変化しようとしているイベント。
	Listener<ListView::ItemChanging&>& onItemChanging();
	/// 項目をクリックしたイベント。
	Listener<ListView::ItemClick&>& onItemClick();
	/// 項目をダブルクリックしたイベント。
	Listener<ListView::ItemDoubleClick&>& onItemDoubleClick();
	/// 項目をマウス左ボタンまたは右ボタンでドラッグしたイベント。
	Listener<ListView::ItemDrag&>& onItemDrag();
	/// 項目を描画する直前のイベント。
	Listener<ListView::ItemPrePaint&>& onItemPrePaint();
	/// 項目を右クリックしたイベント。
	Listener<ListView::ItemRightClick&>& onItemRightClick();
	/// 項目のツールチップを表示するイベント。
	Listener<ListView::ItemTipPopup&>& onItemTipPopup();
	/// 項目の文字列編集が終わって反映する直前のイベント。
	Listener<ListView::TextEdit&>& onTextEdit();
	/// 項目の文字列編集が始まる直前のイベント。
	Listener<ListView::TextEditing&>& onTextEditing();
	///	コントロール作成後に変更できない設定。
	ListView::Options options() const;
	/// 項目を表示し切れない時にスクロールバーを表示するかどうか。初期値は true。
	/// 項目、位置、サイズや style を設定する前に設定しないとうまく動作しない。
	bool scrollable() const;
	void scrollable(bool value);
	/// 選択されている項目の数。
	int selectedCount() const;
	/// 小さいアイコンの画像リスト。ListView は画像リストのハンドルを破棄しない。
	ImageList smallImageList() const;
	void smallImageList(HIMAGELIST value);
	/// 項目を文字列順にソートする。
	void sort(bool ascending = true, int columnIndex = 0);
	/// 比較関数でソートする。比較関数の戻り値は String::compare と同様で、引数は比較する二つの項目インデックス。
	void sort(const std::tr1::function<int (int, int)>& compareFunction);
	/// 比較関数でソートする。比較関数の戻り値は String::compare と同様で、引数は比較する二つの項目文字列。
	void sort(const std::tr1::function<int (const String&, const String&)>& compareFunction, int columnIndex);
	/// 状態アイコンの画像リスト。ListView は画像リストのハンドルを破棄しない。
	/// ユーザ指定の画像リストはチェックボックスと共存できない。チェックボックスの表示を変えたい場合は checkBoxes() を true にした状態で stateImageList() 関数で取得した画像リストを操作する。
	ImageList stateImageList() const;
	void stateImageList(HIMAGELIST value);
	/// 表示スタイル。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。初期値は ListView::Style::details。
	ListView::Style style() const;
	void style(ListView::Style value);
	/// 文字色。
	Color textColor() const;
	void textColor(const Color& value);
	/// クリックで項目の文字列を編集できるかどうか。編集できるのは項目の最初の文字列のみ。初期値は false。
	bool textEditable() const;
	void textEditable(bool value);
	/// 項目文字列を表示するかどうか。非表示になるのは style() が ListView::Style::largeIcon または smallIcon の場合のみ。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。初期値は true。
	bool textVisible() const;
	void textVisible(bool value);
	/// アイコン表示の項目の文字列が長い場合に改行するかどうか。初期値は true。
	bool textWrap() const;
	void textWrap(bool value);
	/// 最初に表示されている項目のインデックス。style() が ListView::Style::details か ListView::Style::list の場合のみ有効。
	int topIndex() const;
	void topIndex(int value);

public:
	/// index 番目の項目。
	ListView::Item operator[](int index);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Listener<ListView::HeaderClick&> _onHeaderClick;
	Listener<ListView::ItemChange&> _onItemChange;
	Listener<ListView::ItemChanging&> _onItemChanging;
	Listener<ListView::ItemClick&> _onItemClick;
	Listener<ListView::ItemDoubleClick&> _onItemDoubleClick;
	Listener<ListView::ItemDrag&> _onItemDrag;
	Listener<ListView::ItemPrePaint&> _onItemPrePaint;
	Listener<ListView::ItemRightClick&> _onItemRightClick;
	Listener<ListView::ItemTipPopup&> _onItemTipPopup;
	Listener<ListView::TextEdit&> _onTextEdit;
	Listener<ListView::TextEditing&> _onTextEditing;
};



	}
}