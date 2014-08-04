#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Control.hpp>

namespace balor {
	namespace graphics {
		class Icon;
	}
}


namespace balor {
	namespace gui {



/**
  * ツールチップコントロール。
 *
 * set 関数でコントロールにツールチップを設定する。
 * TTM_SETMARGIN は Vista でビジュアルスタイルを使用すると動作しないのでサポートしない。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"ToolTip Sample");

	Button button(frame, 20, 10, 0, 0, L"ボタン");
	ToolTip toolTip(frame);
	toolTip.set(button, L"ボタンのツールチップ");
	toolTip.textColor(Color::blue());

	frame.runMessageLoop();
 * </code></pre>
 */
class ToolTip : protected Control {
public:
	typedef ::balor::graphics::Graphics Graphics;
	typedef ::balor::graphics::Icon Icon;


	/// ツールチップの表示オプション。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			center = 0x0002, /// コントロールの下中央に表示する。
			track = 0x0020, /// track 関数で表示する。自動的には表示されない。
			absolute = 0x0080, /// 画面からはみ出ないように位置を調整したりしない。show 関数で表示した時のみ有効。
			transparent = 0x0100, /// ツールチップ上にカーソルを移動してもツールチップを消さない。
			parseLinks = 0x1000, /// ツールチップ文字列のリンクタグ（<a />）をリンクとして表示する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// ツールチップで発生するイベント。
	struct Event : public EventWithSender<ToolTip> {
		Event(ToolTip& sender, Control& owner, int id);

		/// 表示領域を識別するID。表示領域を設定していない場合は 0 になる。
		int id() const;
		/// ツールチップを表示するオーナーコントロール。
		Control& owner();

	private:
		Control& _owner;
		int _id;
	};

	typedef Event LinkClick;
	typedef Event Pop;


	/// ツールチップを表示する前に文字列を設定するイベント。文字列を指定せずにツールチップを設定した場合のみ発生する。
	struct GetText : public ToolTip::Event {
		GetText(ToolTip& sender, Control& owner, int id);

		/// 表示する文字列。初期値は空文字列。
		String& text();
		void text(StringRange value);

	private:
		String _text;
	};


	/// ツールチップを表示する直前のイベント。
	struct Popup : public ToolTip::Event {
		Popup(ToolTip& sender, Control& owner, int id);

		/// 表示する位置。
		Point position() const;
		void position(const Point& value);
		void position(int x, int y);

	private:
		friend ToolTip;

		bool _positionChanged;
	};


public:
	/// ヌルハンドルで作成。
	ToolTip();
	ToolTip(ToolTip&& value, bool checkSlicing = true);
	/// 親コントロールから作成。ツールチップを表示するコントロールの属する Frame 等を指定する。
	explicit ToolTip(Control& parent);
	/// ハンドルを参照して作成。ハンドルは破棄しない。
	explicit ToolTip(HWND handle);
	~ToolTip();
	ToolTip& operator=(ToolTip&& value);

public:
	/// ツールチップを有効または無効にする。初期値は true。
	void activate(bool value);
	/// ウインドウが非アクティブでもツールチップを表示するかどうか。初期値は true。
	bool alwaysPopup() const;
	void alwaysPopup(bool value);
	/// ツールチップを表示する時にスライドアニメーションするかどうか。初期値は true。（TODO:効果が無いようだ）
	bool animate() const;
	void animate(bool value);
	/// 吹き出しの形をしたツールチップかどうか。初期値は false。
	bool balloon() const;
	void balloon(bool value);
	/// 文字列の位置と大きさからツールチップの位置と大きさを求める。
	Rectangle boundsFromTextBounds(const Rectangle& textBounds) const;
	Rectangle boundsFromTextBounds(int x, int y, int width, int height) const;
	/// set 関数で設定した全てのツールチップを削除する。
	void clear();
	/// 閉じるボタンを表示するかどうか。baloon() が true でタイトルを設定した場合のみ表示される。
	bool closeButton() const;
	void closeButton(bool value);
	/// ツールチップが現れたり消えるときにフェードインアウトするか否か。初期値は true。
	bool fade() const;
	void fade(bool value);
	/// フォント。ToolTip の初期フォントは Vista では「メイリオ」で height = 12。
	using Control::font;
	/// コントロールに設定されたツールチップを取得する。設定されていない場合は空文字列が返る。文字列に含まれる & を特殊処理した後の文字列が返る。
	/// タブコードが含まれるとそれ以降が返らない模様。
	String get(Control& control, int id = -1) const;
	/// ツールチップが表示されるまでの時間（ミリ秒）。初期値は 500。
	int initialDelay() const;
	void initialDelay(int value);
	/// ツールチップの最大文字列幅。この幅を超える場合は改行する。制限しない場合は -1。ただしタイトル文字列以下の幅では表示されない。
	int maxTextWidth() const;
	void maxTextWidth(int value);
	/// 表示する文字列の & の特殊処理を無効にするか否か。初期値は false。
	bool noPrefix() const;
	void noPrefix(bool value);
	/// 表示する前に文字列を設定するイベント。文字列を指定せずにツールチップを設定した場合のみ発生する。
	Listener<ToolTip::GetText&>& onGetText();
	/// ツールチップ上のリンクをクリックしたイベント。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
	Listener<ToolTip::LinkClick&>& onLinkClick();
	/// ツールチップが消える時のイベント。
	Listener<ToolTip::Pop&>& onPop();
	/// ツールチップを表示する直前のイベント。位置を変更できる。
	Listener<ToolTip::Popup&>& onPopup();
	/// 現在のツールチップの表示を消す。
	void pop();
	/// 現在のマウス位置でツールチップがあれば表示する。この関数の使用には EnableVisualStyle.hpp のインクルードが必要。
	void popup();
	/// ツールチップが表示されてから消えるまでの時間（ミリ秒）。初期値は 5000。
	int popDelay() const;
	void popDelay(int value);
	/// コントロールからツールチップを削除する。
	void reset(Control& control, int id = -1);
	/// 全てのディレイ（initialDelay、popDelay、reshowDelay）を初期値に戻す。
	void resetDelay();
	/// コントロールの外にカーソルが移動しても表示し続ける時間（ミリ秒）。初期値は 100。
	/// ただし同じツールチップコントロールを設定しているコントロールにカーソルを移動させると即座に消える。
	/// これを防ぐには違うツールチップコントロールを作って設定するしかない。 
	int reshowDelay() const;
	void reshowDelay(int value);
	/// コントロールにツールチップを表示するように設定する。文字列は onGetText イベントで設定する必要がある。reset 関数で無効にできる。
	void set(Control& control, ToolTip::Options options = Options::transparent);
	/// コントロールにツールチップを表示するように設定する。reset 関数で無効にできる。
	void set(Control& control, StringRange toolTip, ToolTip::Options options = Options::transparent);
	/// コントロールのクライアント座標領域にツールチップを表示するように設定する。id は領域を識別する番号。文字列は onGetText イベントで設定する必要がある。reset 関数で無効にできる。
	void set(Control& control, int id, const Rectangle& bounds, ToolTip::Options options = Options::transparent);
	void set(Control& control, int id, int x, int y, int width, int height, ToolTip::Options options = Options::transparent);
	/// コントロールのクライアント座標領域にツールチップを表示するように設定する。id は領域を識別する番号。reset 関数で無効にできる。
	void set(Control& control, int id, const Rectangle& bounds, StringRange toolTip, ToolTip::Options options = Options::transparent);
	void set(Control& control, int id, int x, int y, int width, int height, StringRange toolTip, ToolTip::Options options = Options::transparent);
	/// 指定した initialDelay から popDelay と reshowDelay を計算して全て設定する。
	void setDelay(int initialDelay);
	/// 文字列の色。タイトル文字列を設定した場合は無効。リンクの部分も固定色。
	Color textColor() const;
	void textColor(const Color& value);
	/// 文字列の表示フォーマット。ツールチップコントロールが用意するフォーマットに追加するフラグを設定する。初期値は Graphics::TextFormat::none。
	/// maxTextWidth() を -1 以外に設定することで Graphics::TextFormat::wordBreak 及び Graphics::TextFormat::expandTabs が強制的に追加される。
	Graphics::TextFormat textFormat() const;
	void textFormat(Graphics::TextFormat value);
	/// タイトル文字列。空文字列を指定すると消える。アイコンのみ表示はできない。タイトル文字列の取得は EnableVisualStyle.hpp のインクルードが必要。
	String title() const;
	void title(StringRange value, HICON icon = nullptr);
	/// 指定したスクリーン座標にツールチップを表示する。set 関数で options に ToolTip::Options::track を含める必要がある。pop 関数を呼ぶまで表示され続ける。
	void track(Control& control, const Point& position);
	void track(Control& control, int x, int y);
	void track(Control& control, int id, const Point& position);
	void track(Control& control, int id, int x, int y);
	/// track 関数で表示したツールチップのスクリーン座標位置を更新する。
	void trackPosition(const Point& value);
	void trackPosition(int x, int y);
	/// 再描画する。
	void update();
	/// 現在表示されているかどうか。
	bool visible() const;

public:
	/// HWND への自動変換 & null チェック用。
	using Control::operator HWND;

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	bool _owned;
	/// onGetText イベントで設定してから表示するまでの一時保存場所。
	String _temporaryText;
	Color _textColor;
	Graphics::TextFormat _textFormat;
	Listener<ToolTip::GetText&> _onGetText;
	Listener<ToolTip::LinkClick&> _onLinkClick;
	Listener<ToolTip::Pop&> _onPop;
	Listener<ToolTip::Popup&> _onPopup;
};



	}
}