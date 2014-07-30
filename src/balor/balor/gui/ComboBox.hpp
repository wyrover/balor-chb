#pragma once

#include <balor/gui/Control.hpp>
#include <balor/gui/Edit.hpp>
#include <balor/gui/ListBox.hpp>
#include <balor/StringRangeArray.hpp>


namespace balor {
	namespace gui {



/**
* コンボボックス。
*
* コンボボックスはエディットコントロールとリストボックスを含んでおり、それぞれ edit、list 関数でコントロールを取得できる。
* ComboBox クラスでできない操作はこの関数で得られたコントロールに対して行う。コンボボックスはドロップダウン表示をするボタンの部分になる。
* あるいは style() が ComboBox::Style::dropDownList の場合はエディットコントロールがなく、その部分はコンボボックスになる。
* onFocus 及び onDefocus イベントでは前のフォーカス、次のフォーカスコントロールを得られない。
* edit() と list() の tabWidth() は設定できない模様。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"ComboBox Sample");

const wchar_t* items[] = {
L"りんご",
L"バナナ",
L"パイナップル",
L"スイカ"
};
ComboBox combo(frame, 20, 10, 0, 0, items);
combo.edit().textColor(Color::red());

frame.runMessageLoop();
* </code></pre>
*/
class ComboBox : public Control {
public:
	/// コンボボックスのスタイル。
	struct Style {
		enum _enum {
			simple = 0x0001L, /// エディットの下にリストボックスを常に表示する。
			dropDown = 0x0002L, /// エディットの隣のボタンを押すとリストボックスが表示される。
			dropDownList = 0x0003L, /// エディットは入力不能で隣のボタンを押すとリストボックスが表示される。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};


	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			noHScroll = 0x0040L, /// 入力文字列が右にはみ出る場合にスクロールせず、それ以上入力できなくする。
			noIntegralHeight = 0x0400L, /// リストボックスの高さを自動的に項目の高さの倍数に調節するのをやめる。style() が ComboBox::Style::simple でリストボックスの背景の色を変える場合に影響する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// コンボボックス上のエディットコントロール。
	class Edit : public ::balor::gui::Edit {
		friend ComboBox;

		Edit();
		Edit(Edit&& value);
		Edit(HWND handle);
		~Edit();
		Edit& operator=(Edit&& value);

	private:
		/// フォーカスイベントは ComboBox で。
		Listener<Control::Defocus&>& onDefocus();
		/// フォーカスイベントは ComboBox で。
		Listener<Control::Focus&>& onFocus();

	public:
		/// 親変更を禁止。
		using Control::parent;
		virtual void parent(Control*);

	protected:
		//// キー入力がダイアログキーではなくコントロールで処理する必要があるかどうか。
		virtual bool isInputKey(int shortcut) const;
		/// メッセージを処理する。いわゆるウインドウプロシージャ。
		virtual void processMessage(Message& msg);
	};


	/// コンボボックス上のリストボックス。
	class List : public ::balor::gui::ListBox {
		friend ComboBox;

		List();
		List(List&& value);
		List(HWND handle);
		~List();
		List& operator=(List&& value);

	private:
		/// フォーカスイベントは ComboBox で。
		Listener<Control::Defocus&>& onDefocus();
		/// フォーカスイベントは ComboBox で。
		Listener<Control::Focus&>& onFocus();

	public:
		/// 親変更を禁止。
		using Control::parent;
		virtual void parent(Control*);
	};


	/// コンボボックスのイベントの親クラス。
	typedef EventWithSubclassSender<ComboBox, Control::Event> Event;

	typedef Event CloseUp;
	typedef Event DropDown;
	typedef Event Select;
	typedef Event SelectCancel;
	typedef Event SelectEnd;


public:
	/// ヌルハンドルで作成。
	ComboBox();
	ComboBox(ComboBox&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、スタイルから作成。
	ComboBox(Control& parent, int x, int y, int width, int height, ComboBox::Style style = Style::dropDown, ComboBox::Options options = Options::none);
	/// 親、位置、大きさ、項目配列、スタイルから作成。大きさを０にすると getPreferredSize 関数で求める。
	ComboBox(Control& parent, int x, int y, int width, int height, StringRangeArray items, ComboBox::Style style = Style::dropDown, ComboBox::Options options = Options::none);
	virtual ~ComboBox();
	ComboBox& operator=(ComboBox&& value);

public:
	/// ドロップダウンを表示した時の位置と大きさ。
	Rectangle boundsWithDropDown() const;
	/// ドロップダウンを開始するボタンの矩形。
	Rectangle dropDownButtonBounds() const;
	/// ドロップダウンを開始するボタンが押されているかどうか。
	bool dropDownButtonIsPushed() const;
	/// ドロップダウンが表示されているかどうか。
	bool dropDowned() const;
	void dropDowned(bool value);
	/// ドロップダウンのスクロール可能な幅。
	int dropDownScrollWidth() const;
	void dropDownScrollWidth(int value);
	/// ドロップダウンの幅。コンボボックスの幅より小さくはならない。style() が ComboBox::Style::simple の場合は常にコンボボックスの幅と同じ。
	int dropDownWidth() const;
	void dropDownWidth(int value);
	/// コンボボックス上のエディットコントロール。style() が ComboBox::Style::DropDownList の場合は作成されない。
	ComboBox::Edit& edit();
	const ComboBox::Edit& edit() const;
	/// エディットの部分の高さ。style() が ComboBox::Style::simple でかつ options() に ComboBox::Options::noIntegralHeight を設定しなかった場合はうまく変更できない。
	int editHeight() const;
	void editHeight(int value);
	/// 下矢印キーでドロップダウン開くかどうか。true の場合はマウスホイールで選択できなくなる。false の場合は F4 キーで開く。初期値は false。
	bool extendedUI() const;
	void extendedUI(bool value);
	/// フォーカスを得られるかどうか。
	virtual bool focused() const;
	/// 適切なコントロールサイズ。font, style, items の値によって最適な大きさを返す。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// コンボボックス上のリストボックス。
	ComboBox::List& list();
	const ComboBox::List& list() const;
	/// ドロップダウンリストが項目を何個まで表示できるか。現在の項目数より小さい場合はスクロールバーが表示される。
	/// options() に ComboBox::Options::noIntegralHeight を設定したか、style() が ComboBox::Style::simple の場合は無効。
	/// 初期値は 30。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
	int maxVisibleItemCount() const;
	void maxVisibleItemCount(int value);
	/// ドロップダウンを閉じた時のイベント。
	Listener<ComboBox::CloseUp&>& onCloseUp();
	/// ドロップダウンを表示した時のイベント。
	Listener<ComboBox::DropDown&>& onDropDown();
	/// 選択項目を変更した時のイベント。ドロップダウン表示中に矢印キーを押した時は起きるがマウス移動では起きない。
	Listener<ComboBox::Select&>& onSelect();
	/// 項目を選択せずにドロップダウンを閉じた時のイベント。ESC キーや他のコントロールにフォーカスを移した場合。
	Listener<ComboBox::SelectCancel&>& onSelectCancel();
	/// 項目を選択してドロップダウンを閉じた時のイベント。
	Listener<ComboBox::SelectEnd&>& onSelectEnd();
	///	コントロール作成後に変更できない設定。
	ComboBox::Options options() const;
	/// 現在選択している項目のインデックス。選択していないか、選択後に一度でも文字を編集したら -1 が返る。
	int selectedIndex() const;
	void selectedIndex(int value);
	/// コンボボックスのスタイル。
	ComboBox::Style style() const;

protected:
	//// キー入力がダイアログキーではなくコントロールで処理する必要があるかどうか。
	virtual bool isInputKey(int shortcut) const;
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

	Edit _edit;
	List _list;
	Listener<ComboBox::CloseUp&> _onCloseUp;
	Listener<ComboBox::DropDown&> _onDropDown;
	Listener<ComboBox::Select&> _onSelect;
	Listener<ComboBox::SelectCancel&> _onSelectCancel;
	Listener<ComboBox::SelectEnd&> _onSelectEnd;
};



	}
}