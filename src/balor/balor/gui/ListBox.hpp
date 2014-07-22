#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/gui/Control.hpp>
#include <balor/StringRangeArray.hpp>
#include <balor/UniqueAny.hpp>


namespace balor {
	namespace gui {



/**
 * リストボックスコントロール。
 * 
 * 水平スクロールバーは自動的には表示されないので、scrollWidth() に maxItemWidth() 等を設定する必要がある。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ListBox Sample");

	Label label(frame, 20, 10, 0, 0, L"ウインドウのアイコンを選択");
	Icon icons[] = {
		Icon::application(),
		Icon::exclamation(),
		Icon::question()
	};
	const wchar_t* items[] = {
		L"アプリケーションアイコン",
		L"エクスクラメーションアイコン",
		L"クエスチョンアイコン"
	};
	ListBox list(frame, 20, 50, 0, 0, items);
	list.onSelect() = [&] (ListBox::Select& e) {
		frame.icon(icons[list.selectedIndex()]);
	};
 
	frame.runMessageLoop();
 * </code></pre>
 */
class ListBox : public Control {
public:
	/// 項目の選択方法。
	struct SelectMode {
		enum _enum {
			none             = 0x4000L, /// 選択できない
			one              = 0x0000L, /// 一つだけ選択できる
			multiple         = 0x0008L, /// 複数選択できる
			multipleExtended = 0x0800L, /// CTRL キーを押しながらで複数選択、Shift キーを押しながらで範囲選択できる。
		};
		BALOR_NAMED_ENUM_MEMBERS(SelectMode);
	};


	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none             = 0          , 
			noHScroll        = 0x00100000L, /// 水平スクロールをしない。
			noVScroll        = 0x00200000L, /// 垂直スクロールをしない。
			scrollBarFixed   = 0x1000L    , /// noHScroll や noVScroll が設定されていないスクロールバーを常に表示する。
			multiColumn      = 0x0200L    , /// 縦の行を複数行にする。
			noIntegralHeight = 0x0100L    , /// コントロールの高さを自動的に項目の高さの倍数に調節しない。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// リストボックスのイベントの親クラス。
	typedef EventWithSubclassSender<ListBox, Control::Event> Event;

	typedef Event Select;


public:
	/// ヌルハンドルで作成。
	ListBox();
	ListBox(ListBox&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	ListBox(Control& parent, int x, int y, int width, int height, ListBox::SelectMode select = SelectMode::one, ListBox::Options options = Options::none);
	/// 親、位置、大きさ、項目配列から作成。大きさを０にすると getPreferredSize 関数で求める。
	ListBox(Control& parent, int x, int y, int width, int height, StringRangeArray items, ListBox::SelectMode select = SelectMode::one, ListBox::Options options = Options::none);
	virtual ~ListBox();
	ListBox& operator=(ListBox&& value);

public:
	/// 項目を追加する。
	void add(StringRange item);
	/// 範囲選択の開始点になる項目インデックス。設定されていない場合は -1。
	int anchorIndex() const;
	void anchorIndex(int value);
	/// 全ての項目を削除する。
	void clear();
	/// 全ての項目を未選択にする。
	void clearSelected();
	/// 一行の幅。options() に ListBox::Options::multiColumn を設定した場合のみ有効。
	void columnWidth(int value);
	/// 項目数。
	int count() const;
	/// １列に表示できる項目数？。
	int countPerPage() const;
	/// コントロールの境界線の種類。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 項目を削除する。
	void erase(int index);
	/// 最初に表示されている項目のインデックス。スクロールを制御できる。
	int firstVisibleIndex() const;
	void firstVisibleIndex(int value);
	/// フォーカスされている項目インデックス。
	int focusedIndex() const;
	void focusedIndex(int value);
	/// クライアント座標から最も近い項目のインデックスを求める。見つからない場合は -1 を返す。
	int getIndexAt(const Point& point) const;
	int getIndexAt(int x, int y) const;
	/// 指定したインデックスの項目。
	String getItem(int index) const;
	void getItemToBuffer(StringBuffer& buffer, int index) const;
	/// 項目のクライアント座標と大きさ。幅は現在のスクロール位置で表示されている大きさ。
	Rectangle getItemBounds(int index) const;
	/// 項目に紐つけたデータ。取得する場合は UniqueAny の中身の型を指定する。
	template<typename T> T getItemData(int index) {
		auto data = _getItemData(index);
		return any_cast<T>(*reinterpret_cast<UniqueAny*>(&data));
	}
	template<typename T> T getItemData(int index) const {
		auto data = _getItemData(index);
		return any_cast<T>(*reinterpret_cast<const UniqueAny*>(&data));
	}
	/// 項目に紐つけたデータがあるかどうか。
	bool getItemDataIsEmpty(int index) const;
	/// font(), items() の値によって最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 項目を挿入する。
	void insert(int index, StringRange item);
	/// 項目の選択状態の取得と設定。
	bool isSelected(int index) const;
	void isSelected(int index, bool selected);
	/// 項目の高さ。スクロールがおかしくなるので項目を追加する前に設定するべき。
	int itemHeight() const;
	void itemHeight(int value);
	/// 項目を一括で取得、設定する。
	std::vector<String, std::allocator<String> > items() const;
	void items(StringRangeArray value);
	/// 現在の項目の表示に必要な最大幅。
	int maxItemWidth() const;
	/// 選択項目が変更されたイベント。
	Listener<ListBox::Select&>& onSelect();
	///	コントロール作成後に変更できない設定。
	ListBox::Options options() const;
	/// first 番目から last 番目までの項目を選択する。selecting が false なら選択を解除する。複数選択可能な場合のみ有効。
	void select(int first, int last, bool selecting = true);
	/// 選択されている項目の数。
	int selectedCount() const;
	/// 選択されている項目のインデックス。選択されていない場合は -1 が返る。selectMode() が ListBox::SelectMode::one の場合のみ有効。
	int selectedIndex() const;
	void selectedIndex(int value);
	/// 選択されている項目のインデックス配列。
	std::vector<int, std::allocator<int> > selectedIndices() const;
	void selectedIndices(ArrayRange<int> value);
	/// 項目にデータを紐つける。
	void setItemData(int index, UniqueAny&& value);
	/// 水平スクロールできる幅。
	int scrollWidth() const;
	void scrollWidth(int value);
	/// 項目の選択方法。
	ListBox::SelectMode selectMode() const;
	/// tab 幅。初期値は 8。単位はフォントの平均幅。
	int tabWidth() const;
	void tabWidth(int value);
	/// 文字列の色。
	Color textColor() const;
	void textColor(const Color& value);

public:
	/// 項目を取得する。
	String operator[](int index) const;


protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	void* _getItemData(int index) const;

	int _tabWidth;
	Color _textColor;
	Listener<ListBox::Select&> _onSelect;
};



	}
}