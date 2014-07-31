#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/gui/Control.hpp>
#include <balor/gui/Padding.hpp>
#include <balor/Enum.hpp>
#include <balor/StringBuffer.hpp>

struct HBITMAP__;


namespace balor {
	namespace gui {


/**
* エディットコントロール。
*
* options() に Edit::Options::multiline を設定した場合のみ有効な機能があるが、
* options() に Edit::Options::multiline | Edit::Options::noVScroll を設定すれば一行エディットでありながら複数行の場合の機能を使える。
* オートコンプリート機能を使いたい場合は AutoComplete クラスを使用する。
* Tab キーや Enter キーについては onShortcutKey イベントで挙動を変更できる。Control::ShortcutKey クラスのドキュメントを参照。
* onTextChange イベントを処理することで入力制限や置換に対応できる。（onKeyPress イベントでは貼り付けや切り取りに対応できない）
* 以下はひらがなをカタカナに強制変換する例。
*<pre><code>
*	Edit edit(frame, 10, 10, 100, 20);
*	edit.onTextChange() = [&] (Edit::TextChange& e) {
*		StringBuffer buffer(e.newText());
*		for (auto i = buffer.begin(), end = buffer.end(); i != end; ++i) {
*			if (L'ぁ' <= *i && *i <= L'ん') {
*				*i += L'ァ' - L'ぁ';
*			}
*		}
*		e.newText(buffer);
*	};
*</code></pre>
* 上記の例のように常に文字列を設定し直すとオートコンプリート機能を使う場合に不具合が出るので
* 問題であれば onKeyPress イベントと onPaste イベントを併用するのが良い。
*
* IME 関連は日本語環境の場合のみ動作する。それ以外では何を設定しても無視する。
* 現状の実装では imeFixed() が true の場合でも、マウスで IME 言語バーを操作すると次の１文字の入力は固定された設定に戻る前の入力になる。
* 機会を見て修正したいがどの道ペースト等に対応する為に別の入力制限は必須。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"Edit Sample");

Edit edit(frame, 20, 10, 0, 0, 10);
Button button(frame, 20, 50, 0, 0, L"数値決定", [&] (Button::Click& ) {
MsgBox::show(L"入力した数値は\n\n" + edit.text());
});
frame.acceptButton(&button); // エディットコントロール上で Enter キーを押すと反応するようにする。

// 小数点を含む数値のみ入力できるようにする。C++ 標準正規表現ライブラリは #include <regex>
std::wregex editRegex(L"-?[0-9]*\\.?[0-9]*");
edit.onTextChange() = [&] (Edit::TextChange& e) {
auto text = e.newText();
if (!std::regex_match(text.begin(), text.end(), editRegex)) {
e.cancel(true);
}
};
edit.imeMode(Edit::ImeMode::alphaHalf);

frame.runMessageLoop();
* </code></pre>
*/
class Edit : public Control {
public:
	typedef ::HBITMAP__* HBITMAP;

	/// 編集文字列の配置。
	struct Align {
		enum _enum {
			left = 0x0000, /// 左揃え。
			center = 0x0001, /// 中央揃え。
			right = 0x0002, /// 右揃え。
		};
		BALOR_NAMED_ENUM_MEMBERS(Align);
	};


	/// IME の変換モード。
	struct ImeConvert {
		enum _enum {
			default = 0xFFFF, /// OS に任せる。
			general = 0x0008, /// 一般。
			personAndPlaceName = 0x0001, /// 人名/地名。
			informal = 0x0010, /// 話し言葉。
			none = 0x0000, /// 無変換。
		};
		BALOR_NAMED_ENUM_MEMBERS(ImeConvert);
	};


	/// IME の動作。
	struct ImeMode {
		enum _enum {
			default = 0x1000, /// OS に任せる。
			on = 0x2000, /// IMEを有効にする。
			off = 0x3000, /// IMEを無効にする。
			disabled = 0x4000, /// IMEを無効にしてユーザが有効化できなくする。
			hiragana = 0x0009, /// ひらがな。
			katakana = 0x000b, /// 全角カタカナ。
			katakanaHalf = 0x0003, /// 半角カタカナ。
			alpha = 0x0008, /// 全角英数。
			alphaHalf = 0x0000, /// 半角英数。
		};
		BALOR_NAMED_ENUM_MEMBERS(ImeMode);
	};


	/// IME の挙動を表すフラグ。組み合わせで指定する。
	struct ImeStatus {
		enum _enum {
			none = 0,
			noKeyPress = 0x0001, /// IMEの入力結果について onKeyPress イベント（WM_CHAR メッセージ）を発生させない。
			cancelOnGotFocus = 0x0002, /// フォーカスを得たとき IME の入力途中の内容を破棄する。
			completeOnLostFocus = 0x0004, /// フォーカスを失うとき IME の入力途中の内容を入力完了する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(ImeStatus);
	};


	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			multiline = 0x0004L, /// 複数行入力できるようにする。Tab キー、Enterキー、Esc キーがダイアログキーとみなされなくなる。
			noHScroll = 0x0080L, /// 入力文字が右にはみ出る場合に右にスクロールせずに自動改行する。
			noVScroll = 0x0040L, /// 入力文字が下にはみ出る場合に下にスクロールせずにそれ以上入力できなくなる。
			alwaysSelected = 0x0100L, /// フォーカスが無い時でも文字列の選択状態を表示する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// 文字列の選択状態。
	struct Selection {
		Selection() {}
		Selection(int index, int length) : index(index), length(length) {}

		/// 選択を開始する文字インデックス。
		int index;
		/// 選択している文字数。
		int length;
	};


	/// Label のイベントの親クラス。
	typedef EventWithSubclassSender<Edit, Control::Event> Event;


	/// クリップボードの文字列を貼り付けるイベント。
	struct Paste : public Edit::Event {
		Paste(Edit& sender);

		/// 貼り付けをキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// 貼り付ける文字列。
		String text() const;

	private:
		bool _cancel;
	};


	/// 文字列を変更して表示に反映する直前のイベント。
	struct TextChange : public Edit::Event {
		TextChange(Edit& sender);

		/// 変更をキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// 変更後の選択状態。
		Edit::Selection newSelection() const;
		void newSelection(const Edit::Selection& value);
		/// 変更後の文字列。
		String newText() const;
		void newText(StringRange value);
		/// 変更前の選択状態。
		Edit::Selection oldSelection() const;
		/// 変更前の文字列。
		String oldText() const;

	private:
		bool _cancel;
	};


public:
	/// ヌルハンドルで作成。
	Edit();
	Edit(Edit&& value, bool checkSlicing = true);
	/// 親、位置、大きさから作成。
	Edit(Control& parent, int x, int y, int width, int height, Edit::Options options = Options::none);
	/// 親、位置、大きさ、一行の文字数、行数から作成。大きさを０にすると一行の文字数と行数から大きさを求める。
	Edit(Control& parent, int x, int y, int width, int height, int lineLength, int lineCount = 1, Edit::Options options = Options::none);
	/// ハンドルを参照して作成。ハンドルは破棄しない。
	explicit Edit(HWND handle);
	~Edit();
	Edit& operator=(Edit&& value);


public:
	/// 編集文字列の配置。初期値は Edit::Align::left。
	Edit::Align align() const;
	void align(Edit::Align value);
	/// やり直し情報があるかどうか。
	bool canUndo() const;
	/// キャレットの点滅時間（ミリ秒）。
	static int caretBlinkTime();
	/// キャレットのある文字インデックス。
	int caretIndex() const;
	void caretIndex(int value);
	/// キャレットを表示するかどうか。
	bool caretVisible() const;
	void caretVisible(bool value);
	/// キャレットの大きさや、灰色にするかどうかを設定する。例えば半角数字だけを入力する場合に四角いキャレットにするには下記のようなコードを書く。
	/// <pre><code>
	/// Graphics graphics(edit);
	/// graphics.font(edit.font());
	/// edit.changeCaret(graphics.measureText(L"0"));
	/// </code></pre>
	void changeCaret(const Size& size, bool grayed = false);
	void changeCaret(int width, int height, bool grayed = false);
	/// キャレットの画像を変更する。画像は文字に対して XOR 演算を行う。キャレットの大きさを変更すると無効になる。
	void changeCaret(HBITMAP caret);
	/// やり直し情報を消す。
	void clearUndo();
	/// 選択している文字列をクリップボードにコピーする。文字列が選択されていない場合は何もしない。
	void copy() const;
	/// ユーザが初めて入力するまで表示するガイド文字列。optins() に Edit::Options::multiline を設定していない場合のみ有効。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
	String cueBanner() const;
	void cueBanner(StringRange value);
	/// 選択している文字列を切り取ってクリップボードに貼り付ける。文字列が選択されていない場合はなにもしない。
	void cut();
	/// システムデフォルトのキャレットの幅。
	static int defaultCaretWidth();
	/// コントロールの境界線の種類。初期値は Control::Edge::client。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 最初に表示されている行。
	int firstVisibleLine() const;
	/// クライアント座標から最も近い文字インデックスを求める。クライアント領域上ではない場合は -1 を返す。
	int getIndexAt(const Point& position) const;
	int getIndexAt(int x, int y) const;
	/// 行の先頭の文字インデックスを求める。
	int getIndexFromLine(int line) const;
	/// 文字インデックスから行を求める。
	int getLineFromIndex(int index) const;
	/// 行の文字数。
	int getLineLength(int line) const;
	/// 行の文字列を返す。
	String getLineText(int line) const;
	/// 文字インデックスからクライアント座標を求める。キャレットが移動できる文末はなぜか位置が取得できないので注意。
	Point getPositionFromIndex(int index) const;
	/// 引数で 0 を指定した部分のみ現在の状態に最適な大きさを求める。0 以外を指定した部分はそのまま返る。
	/// Edit は素のままでは最適な大きさが推測できないので preferredCharCount() の一行の文字数、行数から最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 水平スクロールバーを表示するかどうか。optins() に Edit::Options::multiline を設定した場合のみ有効。初期値は false。
	bool hScrollBar() const;
	void hScrollBar(bool value);
	/// IME の変換モード。初期値は Edit::ImeConvert::default。
	Edit::ImeConvert imeConvert() const;
	void imeConvert(Edit::ImeConvert value);
	/// imeMode() と imeConvert() を現在の状態からユーザが変更できなくするかどうか。初期値は false。
	bool imeFixed() const;
	void imeFixed(bool value);
	/// IME の動作。初期値は Edit::ImeConvert::default。
	Edit::ImeMode imeMode() const;
	void imeMode(Edit::ImeMode value);
	/// IME の挙動を表すフラグ。初期値は Edit::ImeStatus::none。
	Edit::ImeStatus imeStatus() const;
	void imeStatus(Edit::ImeStatus value);
	/// 編集文字列の長さ。
	int length() const;
	/// 編集文字列の行数。
	int lineCount() const;
	/// 大文字を全て小文字として入力するどうか。初期値は false。
	bool lowercaseOnly() const;
	void lowercaseOnly(bool value);
	/// 文字列とコントロールの間の余白。multiline() が false の場合は left と right しか指定できない。初期値は Padding(2, 0, 2, 0)。
	/// 複数行ではないが上下の余白が欲しい場合は options() に Edit::Options::mutiline | Edit::Options::noVScroll を設定すると良い。
	/// 複数行ではなく options() に Edit::Options::noHScroll を設定した場合、余白の部分にはみ出た文字が表示されてしまうので上記のように複数行にすると良い。
	Padding margin() const;
	void margin(const Padding& value);
	void margin(int left, int top, int right, int bottom);
	/// 入力できる最大文字数。
	int maxLength() const;
	void maxLength(int value);
	/// 文字列が編集されたかどうか。
	bool modified() const;
	void modified(bool value);
	/// 複数行入力できるかどうか。コンストラクタで options 引数に Edit::Options::multiline を設定したかどうか。
	bool multiline() const;
	/// 数字のみ許可するかどうか。ただし貼り付けした文字列はチェックされない。初期値は false。
	bool numberOnly() const;
	void numberOnly(bool value);
	/// クリップボードの文字列を貼り付けるイベント。
	Listener<Edit::Paste&>& onPaste();
	/// 文字列を変更して表示に反映する直前のイベント。
	Listener<Edit::TextChange&>& onTextChange();
	///	コントロール作成後に変更できない設定。
	Edit::Options options() const;
	/// パスワード入力でパスワードを隠す文字。0 なら隠さない。初期値は 0。
	wchar_t passwordChar() const;
	void passwordChar(wchar_t value);
	/// クリップボードの文字列を貼り付ける。
	void paste();
	/// この Edit で想定される一行の文字数と行数。width は一行の文字数、height は行数を表す。getPreferredSize 関数に 0 を指定した時、この数値から大きさを求める。初期値は Size(10, 1)。
	Size preferredCharCount() const;
	void preferredCharCount(const Size& value);
	void preferredCharCount(int lineLength, int lineCount);
	/// 読み取り専用かどうか。初期値は false。
	bool readOnly() const;
	void readOnly(bool value);
	/// Scaler を使ってコントロールをスケーリングする。
	virtual void scale(const Scaler& scaler);
	/// 水平方向に x 文字数分、垂直方向に y 行分スクロールする。x は実際の文字数ではなく半角文字の平均文字幅のスクロール量。
	void scroll(int x, int y);
	/// y行の x 文字目にスクロールする。x は実際の文字数ではなく半角文字の平均文字幅のスクロール量。
	void scrollInto(int x, int y);
	/// 一行下にスクロールする。
	void scrollLineDown();
	/// 一行上にスクロールする。
	void scrollLineUp();
	/// １ページ下にスクロールする。
	void scrollPageDown();
	/// １ページ上にスクロールする。
	void scrollPageUp();
	/// キャレットが見える位置にスクロールする。
	void scrollToCaret();
	/// 選択されている文字列。無い場合は空文字列を返す。
	String selectedText() const;
	void selectedText(StringRange value);
	/// 文字列の選択状態。
	Edit::Selection selection() const;
	void selection(const Edit::Selection& value);
	void selection(int index, int length);
	/// クライアント領域の大きさからコントロールの大きさを求める。
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// tab 幅。初期値は取得できないがおそらく 8。
	void tabWidth(int value);
	/// 編集文字列。
	using Control::text;
	virtual void text(StringRange value);
	/// 文字列の色。
	Color textColor() const;
	void textColor(const Color& value);
	/// 直前の編集をやり直す。
	void undo();
	/// 小文字を全て大文字として入力するかどうか。初期値は false。
	bool uppercaseOnly() const;
	void uppercaseOnly(bool value);
	/// 垂直スクロールバーを表示するかどうか。multiline() が true の場合のみ有効。初期値は false。
	bool vScrollBar() const;
	void vScrollBar(bool value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	void _initialize();

	bool _owned;
	HBITMAP _caret;
	Size _caretSize;
	int _caretIndex;
	bool _caretVisible;
	ImeMode _imeMode;
	ImeMode _imeModeBackup;
	bool _imeModeChanging;
	ImeConvert _imeConvert;
	ImeConvert _imeConvertBackup;
	bool _imeFixed;
	Padding _margin;
	Size _preferredCharCount;
	bool _textChanging;
	Color _textColor;
	StringBuffer _oldText;
	Selection _oldSelection;
	Listener<Edit::Paste&> _onPaste;
	Listener<Edit::TextChange&> _onTextChange;
};



	}
}