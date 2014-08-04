#pragma once

#include <vector>

#include <balor/gui/Control.hpp>
#include <balor/ArrayRange.hpp>

struct _IMAGELIST;
struct _TREEITEM;
struct tagNMTVCUSTOMDRAW;

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
* ツリービュー。
*
* マウス左ボタンを項目上で押した場合、onMouseDown イベントはボタンを離した時に発生し、onMouseUp イベントは発生しない。代わりに onClick イベントを使用できる。
* マウス右ボタンを押した場合、onMouseDonw イベントはボタンを離した時に発生し、onMouseUp イベントは発生しない。代わりに onRightClick イベントを使用できる。
* onDrag イベントはうまく拾えない。代わりに onItemDrag イベントを使用できる。
* 背景をブラシで塗る方法は判らなかった。WM_ERASEBKGND では項目の背景を塗れない。
*
* <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"TreeView Sample");

	typedef TreeView::ItemInfo Info;
	Info subInfos0[] = {
		Info(L"サブ項目00"),
		Info(L"サブ項目01")
	};
	Info subInfos1[] = {
		Info(L"サブ項目10"),
		Info(L"サブ項目11"),
		Info(L"サブ項目12")
	};
	Info infos[] = {
		Info(L"項目0", subInfos0),
		Info(L"項目1", subInfos1),
		Info(L"項目2")
	};
	TreeView tree(frame, 20, 10, 0, 0, infos);

	frame.runMessageLoop();
 * </code></pre>
 */
class TreeView : public Control {
public:
	typedef ::_IMAGELIST* HIMAGELIST;
	typedef ::_TREEITEM* HTREEITEM;
	typedef ::balor::graphics::ImageList ImageList;

	/// 項目の文字列の終端文字を含めた最大長。
	static const int maxTextLength = 260;


	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			checkBoxes = 0x0100L, // 項目ごとにチェックボックスを表示する。stateImageList() を操作してチェック画像を変更できる。画像を追加するとチェック状態が増える。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	class ItemInfo;
	class ItemDescendantsIterator;


	/// 項目。TreeView::root 関数等から取得できる。
	class Item {
	public:
		Item(HWND ownerHandle, HTREEITEM item);
		/// 項目情報を反映。
		Item& operator=(const ItemInfo& itemInfo);

		/// 子項目を末尾に追加する。
		Item add(const ItemInfo& itemInfo);
		/// 最後の子項目。無い場合はヌルの項目を返す。
		Item back();
		const Item back() const;
		/// 文字列の編集を始める。強制的にフォーカスを移す。
		void beginEdit();
		/// 文字列が太字かどうか。
		bool bold() const;
		void bold(bool value);
		/// ツりービュー内での項目の領域。
		Rectangle bounds() const;
		/// ツりービュー内での子項目も含む項目の領域。
		Rectangle boundsWithChildren() const;
		/// チェックボックスがチェックされているかどうか。チェックのオンとオフは state() の 1 と 0 に該当する。
		bool checked() const;
		void checked(bool value);
		/// 全ての子項目を削除する。
		void clear();
		/// 切り取りされた状態かどうか。アイコンが薄く表示される。
		bool cut() const;
		void cut(bool value);
		/// 項目の子孫を列挙するイテレータ。
		TreeView::ItemDescendantsIterator descendantsBegin();
		/// 文字列の編集を終える。
		void endEdit(bool cancel = false);
		/// 項目が見えるようにスクロールする。
		void ensureVisible();
		/// 子項目を削除する。
		void erase(Item& item);
		/// この項目の子項目が表示されているかどうか。
		bool expanded() const;
		void expanded(bool value);
		/// 全ての子孫項目を表示または非表示にする。
		void expandAll(bool value);
		/// 最初の子項目。無い場合はヌルの項目を返す。
		Item front();
		const Item front() const;
		/// ハイライト表示かどうか。選択状態と同じ表示。
		bool highlight() const;
		void highlight(bool value);
		/// アイコンの画像リスト（TreeView::imageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// 指定した子項目の前に子項目を追加する。
		Item insert(Item& nextItem, const ItemInfo& itemInfo);
		/// 子項目の情報配列を一括で取得、設定する。
		std::vector<TreeView::ItemInfo> itemInfos() const;
		void itemInfos(ArrayRange<const ItemInfo> value);
		/// 次の兄弟項目。無い場合はヌルの項目を返す。
		Item next();
		const Item next() const;
		/// 次の表示されている項目。無い場合はヌルの項目を返す。
		Item nextVisible();
		const Item nextVisible() const;
		/// ツリービューのハンドル
		HWND ownerHandle() const;
		/// 親項目。ルート項目かどうかは ListView::root() と == 比較して判断できる。
		Item parent();
		const Item parent() const;
		/// 前の兄弟項目。無い場合はヌルの項目を返す。
		Item prev();
		const Item prev() const;
		/// 前の表示されている項目。無い場合はヌルの項目を返す。
		Item prevVisible();
		const Item prevVisible() const;
		/// 選択状態アイコンの画像リスト（TreeView::imageList()）のインデックス。
		int selectedImageIndex() const;
		void selectedImageIndex(int value);
		/// 状態。状態画像リスト（TreeView::stateImageList()）のインデックス - 1 で、-1 なら表示しない。
		int state() const;
		void state(int value);
		/// 項目の文字列。
		String text() const;
		void text(StringRange value);
		void textToBuffer(StringBuffer& buffer) const;
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

	public:
		/// アイテムハンドルへの自動変換、 null チェック、同じ項目かどうかの == 比較用。
		operator HTREEITEM() const { return _item; }

	private:
		void* _userData() const;

		HWND _ownerHandle;
		HTREEITEM _item;
	};


	/// 項目情報構造体。情報を持つだけでツりービューは一切操作しない。TreeView::Item::itemInfos 関数等で使用する。
	class ItemInfo {
	public:
		ItemInfo(ItemInfo&& value);
		/// 項目情報の取得。
		ItemInfo(const Item& item);
		/// 文字列とアイコン画像インデックス等から作成。
		explicit ItemInfo(String text, int imageIndex = 0, int selectedImageIndex = -1, int state = -1);
		/// 文字列と子項目情報配列と画像インデックス等から作成。
		ItemInfo(String text, ArrayRange<const ItemInfo> itemInfos, int imageIndex = 0, int selectedImageIndex = -1, int state = -1);
		ItemInfo& operator=(ItemInfo&& value);

		/// 文字列が太字かどうか。
		bool bold() const;
		void bold(bool value);
		/// 切り取りされた状態かどうか。アイコンが薄く表示される。
		bool cut() const;
		void cut(bool value);
		/// ハイライト表示かどうか。選択状態と同じ表示。
		bool highlight() const;
		void highlight(bool value);
		/// アイコンの画像リスト（TreeView::imageList()）のインデックス。
		int imageIndex() const;
		void imageIndex(int value);
		/// 子項目の情報構造体配列。
		std::vector<TreeView::ItemInfo>& itemInfos();
		const std::vector<TreeView::ItemInfo>& itemInfos() const;
		/// 選択状態アイコンの画像リスト（TreeView::imageList()）のインデックス。-1 なら imageIndex() と同じ。
		int selectedImageIndex() const;
		void selectedImageIndex(int value);
		/// 状態。状態画像リスト（TreeView::stateImageList()）のインデックス - 1 で、-1 なら表示しない。
		int state() const;
		void state(int value);
		/// 項目の文字列。
		const String& text() const;
		void text(String value);

	private:
		friend Item;

		String _text;
		std::vector<ItemInfo> _itemInfos;
		int _imageIndex;
		int _selectedImageIndex;
		int _itemDataState;
	};


	/// 項目の子孫を列挙できるイテレータ。子項目も、子の子項目も全て再帰的に列挙する。
	class ItemDescendantsIterator {
	public:
		ItemDescendantsIterator(Item root);

		TreeView::ItemDescendantsIterator& operator++();
		TreeView::Item& operator*();
		TreeView::Item* operator->();
		operator bool() const;

	private:
		TreeView::Item _root;
		TreeView::Item _current;
	};



	/// ツりービューのイベントの親クラス。
	typedef EventWithSubclassSender<TreeView, Control::Event> Event;

	typedef Event Click;
	typedef Event RightClick;


	/// 項目に関するイベント。
	struct ItemEvent : public Event {
		ItemEvent(TreeView& sender, HTREEITEM item);

		/// 項目。
		TreeView::Item item();

	private:
		HTREEITEM _item;
	};


	/// 項目をマウス右ボタンまたは左ボタンでドラッグしたイベント。ドラッグを開始する範囲は選択できる範囲と同じ。
	struct ItemDrag : public ItemEvent {
		ItemDrag(TreeView& sender, HTREEITEM item, bool rButton);

		/// 右ボタンドラッグかどうか。
		bool rButton() const;

	private:
		bool _rButton;
	};


	/// 項目の子項目が表示されているかどうかが変化したイベント。
	struct ItemExpand : public ItemEvent {
		ItemExpand(TreeView& sender, HTREEITEM item, bool expanded);

		/// 項目の子項目が表示されているかどうか。
		bool expanded() const;

	private:
		bool _expanded;
	};


	/// 項目の子項目が表示されているかどうかが変化しようとしているイベント。
	struct ItemExpanding : public ItemExpand {
		ItemExpanding(TreeView& sender, HTREEITEM item, bool expanded);

		/// 変化をキャンセルするかどうか。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	/// 項目を描画する直前のイベント。
	struct ItemPrePaint : public Event {
		ItemPrePaint(TreeView& sender, ::tagNMTVCUSTOMDRAW* info);

		/// 項目の背景の色。selected() が true の場合は無効。TreeView::fullRowSelect() で範囲が変わる。
		Color backColor() const;
		void backColor(const Color& value);
		/// 項目のクライアント座標領域。
		Rectangle bounds() const;
		/// フォーカスがあるように描画するかどうか。
		bool focused() const;
		void focused(bool value);
		/// 項目の文字列のフォント。
		Font font() const;
		void font(HFONT value);
		/// イベントの発生した項目。
		TreeView::Item item();
		/// 項目のツリー階層の深さ。0 から始まる。
		int level() const;
		/// 選択しているように描画するかどうか。
		bool selected() const;
		void selected(bool value);
		/// 項目の文字列の色。
		Color textColor() const;
		void textColor(const Color& value);

	private:
		friend TreeView;

		::tagNMTVCUSTOMDRAW* _info;
		bool _fontChanged;
	};


	/// 選択項目が変化したイベント。
	struct ItemSelect : public Event {
		ItemSelect(TreeView& sender, HTREEITEM oldSelectedItem, HTREEITEM newSelectedItem);

		/// 新しい選択項目。
		TreeView::Item newSelectedItem() const;
		/// 古い選択項目。
		TreeView::Item oldSelectedItem() const;

	protected:
		HTREEITEM _oldSelectedItem;
		HTREEITEM _newSelectedItem;
	};


	/// 選択項目が変化しようとしているイベント。
	struct ItemSelecting : public ItemSelect {
		ItemSelecting(TreeView& sender, HTREEITEM oldSelectedItem, HTREEITEM newSelectedItem);

		/// 変化をキャンセルするかどうか。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	/// 項目の状態が変化したイベント。
	struct ItemStateChange : public ItemEvent {
		ItemStateChange(TreeView& sender, HTREEITEM item, int oldState, int newState);

		/// 新しい項目の状態。
		int newState() const;
		/// 古い項目の状態。
		int oldState() const;

	protected:
		int _oldState;
		int _newState;
	};


	/// 項目の状態が変化しようとしているイベント。
	struct ItemStateChanging : public ItemStateChange {
		ItemStateChanging(TreeView& sender, HTREEITEM item, int oldState, int newState);

		/// 新しい項目の状態。
		void newState(int value);
		using ItemStateChange::newState;
	};


	/// 項目のツールチップが表示されるイベント。
	struct ItemTipPopup : public ItemEvent {
		ItemTipPopup(TreeView& sender, HTREEITEM item, wchar_t* buffer, int bufferSize);

		/// 表示する文字列を設定する。文字列の長さには上限があって切り捨てられる。
		void setText(StringRange value);

	private:
		wchar_t* _buffer;
		int _bufferSize;
	};


	/// 項目の文字列を編集した直後のイベント。
	struct TextEdit : public ItemEvent {
		TextEdit(TreeView& sender, HTREEITEM item, String itemText);

		/// 編集を開始しないか、または編集結果を反映しないかどうか。
		bool cancel() const;
		void cancel(bool value);
		/// 編集される、または編集された文字列。
		const String& itemText() const;
		void itemText(String value);

	private:
		String _itemText;
		bool _cancel;
	};


	/// 項目の文字列を編集する直前のイベント。
	struct TextEditing : public TextEdit {
		TextEditing(TreeView& sender, HTREEITEM item, String itemText);

		/// 編集をするエディットコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。
		/// ある程度 TreeView が制御しているので動作しない設定もある。
		Edit edit();
	};


	/// ヒットテストの結果。
	struct HitTestInfo {
		HitTestInfo(HWND handle, HTREEITEM item, int flags);

		/// 当たっている項目。当たってない場合はヌル項目。
		TreeView::Item item();
		/// 子項目を表示/非表示にする[+][-]ボタンの上。
		bool onButton() const;
		/// アイコン画像の上。
		bool onImage() const;
		/// 項目のインデントの上。
		bool onIndent() const;
		/// 項目のアイコン画像、状態画像、または文字列の上。
		bool onItem() const;
		/// 項目の文字列より右の部分。
		bool onItemRight() const;
		/// 状態画像の上。
		bool onStateImage() const;
		/// 文字列の上。
		bool onText() const;

	private:
		HWND _handle;
		HTREEITEM _item;
		int _flags;
	};


public:
	/// ヌルハンドルで作成。
	TreeView();
	TreeView(TreeView&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	TreeView(Control& parent, int x, int y, int width, int height, TreeView::Options options = Options::none);
	/// 親、位置、大きさ、項目情報配列から作成。大きさを０にすると getPreferredSize 関数で求める。
	TreeView(Control& parent, int x, int y, int width, int height, ArrayRange<const ItemInfo> itemInfos, TreeView::Options options = Options::none);
	virtual ~TreeView();
	TreeView& operator=(TreeView&& value);

public:
	/// フォーカスが外れても選択状態を表示するかどうか。初期値は false。
	bool alwaysSelected() const;
	void alwaysSelected(bool value);
	/// 背景色。
	Color backColor() const;
	void backColor(const Color& value);
	/// 子項目を表示/非表示にする[+][-]ボタンを表示するかどうか。初期値は true。
	bool buttons() const;
	void buttons(bool value);
	/// 項目数。
	int count() const;
	/// ツりービュー内に表示できる項目数。
	int countPerPage() const;
	/// コントロールの境界線の種類。初期値は Control::Edge::client。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 横の行を全て選択状態にするかどうか。lines() が true の場合は true に設定できない。初期値は false。
	bool fullRowSelect() const;
	void fullRowSelect(bool value);
	/// 指定したクライアント座標のヒットテストを行って情報を取得する。
	TreeView::HitTestInfo getHitTestInfo(const Point& point) const;
	TreeView::HitTestInfo getHitTestInfo(int x, int y) const;
	/// 指定したクライアント座標にある項目を取得する。無い場合はヌル項目。
	TreeView::Item getItemAt(const Point& point);
	TreeView::Item getItemAt(int x, int y);
	/// 適切なコントロールサイズ。font(), count() 等の値によって最適なサイズを返す。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 項目にカーソルを合わせるとカーソルが Cursor::hand() になって文字列にアンダーラインが表示されるかどうか。初期値は false。
	bool hotTracking() const;
	void hotTracking(bool value);
	/// 項目を表示し切れない時に水平スクロールバーを表示するかどうか。項目がはみ出ていない時に設定しないとうまく動作しない。初期値は true。
	bool hScrollable() const;
	void hScrollable(bool value);
	/// アイコンの画像リスト。TreeView は画像リストのハンドルを破棄しない。
	ImageList imageList() const;
	void imageList(HIMAGELIST value);
	/// 子項目のインデントピクセル数。
	int indent() const;
	void indent(int value);
	/// 項目の高さ。
	int itemHeight() const;
	void itemHeight(int value);
	/// 項目ごとに表示するツールチップコントロールの参照。必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。ある程度 TreeView が制御しているので動作しない設定もある。
	ToolTip itemTip();
	/// 項目ごとにツールチップを表示するかどうか。onItemTipPopup イベントで表示する文字列を指定する。初期値は false。
	bool itemTipEnabled() const;
	void itemTipEnabled(bool value);
	/// 親と子項目を結ぶ線の色。
	Color lineColor() const;
	void lineColor(const Color& value);
	/// 親と子項目を結ぶ線を表示するかどうか。初期値は true。
	bool lines() const;
	void lines(bool value);
	/// 左クリックしたイベント。
	Listener<TreeView::Click&>& onClick();
	/// 項目をマウス右ボタンまたは左ボタンでドラッグしたイベント。ドラッグを開始する範囲は選択できる範囲と同じ。
	Listener<TreeView::ItemDrag&>& onItemDrag();
	/// 項目の子項目が表示されているかどうかが変化したイベント。
	Listener<TreeView::ItemExpand&>& onItemExpand();
	/// 項目の子項目が表示されているかどうかが変化しようとしているイベント。
	Listener<TreeView::ItemExpanding&>& onItemExpanding();
	/// 項目を描画する直前のイベント。
	Listener<TreeView::ItemPrePaint&>& onItemPrePaint();
	/// 選択項目が変化したイベント。
	Listener<TreeView::ItemSelect&>& onItemSelect();
	/// 選択項目が変化しようとしているイベント。
	Listener<TreeView::ItemSelecting&>& onItemSelecting();
	/// 項目の状態が変化したイベント。
	Listener<TreeView::ItemStateChange&>& onItemStateChange();
	/// 項目の状態が変化しようとしているイベント。
	Listener<TreeView::ItemStateChanging&>& onItemStateChanging();
	/// 項目のツールチップを表示するイベント。
	Listener<TreeView::ItemTipPopup&>& onItemTipPopup();
	/// 右クリックしたイベント。うまく拾えない onMouseUp イベントの代わりに使う。
	Listener<TreeView::RightClick&>& onRightClick();
	/// 項目の文字列編集が終わって反映する直前のイベント。
	Listener<TreeView::TextEdit&>& onTextEdit();
	/// 項目の文字列編集が始まる直前のイベント。
	Listener<TreeView::TextEditing&>& onTextEditing();
	///	コントロール作成後に変更できない設定。
	TreeView::Options options() const;
	/// 仮想的なルート項目を取得する。この項目は表示されないし子項目の操作以外は受け付けない。
	TreeView::Item root();
	const TreeView::Item root() const;
	/// ルート項目と子項目を結ぶ線を表示するかどうか。初期値は true。
	bool rootLines() const;
	void rootLines(bool value);
	/// 項目を表示し切れない時にスクロールバーを表示するかどうか。項目がはみ出ていない時に設定しないとうまく動作しない。初期値は true。
	bool scrollable() const;
	void scrollable(bool value);
	/// 選択されている項目。
	TreeView::Item selectedItem() const;
	void selectedItem(TreeView::Item value);
	/// 状態アイコンの画像リスト。0 番目の画像は使用されない。TreeView は画像リストのハンドルを破棄しない。
	/// ユーザ指定の画像リストはチェックボックスと共存できない。チェックボックスの表示を変えたい場合は stateImageList() 関数で取得した画像リストを操作する。
	ImageList stateImageList() const;
	void stateImageList(HIMAGELIST value);
	/// 文字色。
	Color textColor() const;
	void textColor(const Color& value);
	/// クリックで項目の文字列を編集できるかどうか。初期値は false。
	bool textEditable() const;
	void textEditable(bool value);
	/// 最初に表示されている項目。
	TreeView::Item topItem() const;
	void topItem(TreeView::Item value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Listener<TreeView::Click&> _onClick;
	Listener<TreeView::ItemDrag&> _onItemDrag;
	Listener<TreeView::ItemExpand&> _onItemExpand;
	Listener<TreeView::ItemExpanding&> _onItemExpanding;
	Listener<TreeView::ItemPrePaint&> _onItemPrePaint;
	Listener<TreeView::ItemSelect&> _onItemSelect;
	Listener<TreeView::ItemSelecting&> _onItemSelecting;
	Listener<TreeView::ItemStateChange&> _onItemStateChange;
	Listener<TreeView::ItemStateChanging&> _onItemStateChanging;
	Listener<TreeView::ItemTipPopup&> _onItemTipPopup;
	Listener<TreeView::RightClick&> _onRightClick;
	Listener<TreeView::TextEdit&> _onTextEdit;
	Listener<TreeView::TextEditing&> _onTextEditing;
};



	}
}