#pragma once

#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Key.hpp>
#include <balor/gui/Mouse.hpp>
#include <balor/gui/Message.hpp>
#include <balor/Enum.hpp>
#include <balor/Event.hpp>
#include <balor/Listener.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/Reference.hpp>
#include <balor/Size.hpp>
#include <balor/String.hpp>
#include <balor/UniqueAny.hpp>

struct HWND__;

namespace std {
	namespace tr1 {
template<typename T> class function;
	}
}

namespace balor {
	namespace graphics {
		class Brush;
		class Color;
		class Font;
		class Region;
	}
}


namespace balor {
	namespace gui {

class Frame;
class Scaler;



/**
 * コントロールの基底クラス。
 *
 * 全てのコントロールクラスはこのクラスから派生する。
 * カーソルを設定するには onCursorChange イベントを処理する。
 * ポップアップメニューを表示するには onPopupMenu イベントを処理する。
 * コントロールのニーモニックやメニューのショートカットキーの為に onShortcutKey イベントを利用できる。
 * ドラッグ＆ドロップを実装するのに DragDrop クラスを利用できる。
 * レイアウトを設定するのに DockLayout クラスや LineLayout クラス等を利用できる。
 *
 * <h3>・拡張のガイドライン</h3>
 *
 * Control クラスを継承した派生クラスはウインドウハンドルを作成して attachHandle 関数で Control に紐つけ、processMessage 関数をオーバーライドしてウインドウプロシージャを拡張する。
 * できるだけデストラクタで destoryHandle 関数を呼ぶ。さもなくば Control クラスのデストラクタから呼ばれるが、この時点で派生クラスのデストラクタは既に呼ばれているので
 * Control クラスの processMessage 関数が呼ばれてしまう。あるいは派生クラスに公開されている _handle, _defaultProcedure メンバ変数を操作して初期化、終了処理を行う。
 * Control はコピー禁止クラスなので一時オブジェクトを代入したり、関数の戻り値としたり、STL コンテナに格納するためには右辺値コピーコンストラクタ、右辺値代入演算子を定義する。
 * その定義は Control 共通の決め事にしたがって実装する必要がある。以下は Button クラスを継承したコードの例。
 * <pre><code>
 * class MyButton : public Button {
 * public:
 *	MyButton() {}
 *	MyButton(MyButton&& value, bool checkSlicing = true)
 *		: Button(std::move(value), false) // Button クラスの派生クラス代入チェックに必ずひっかかるので false を指定する。
 *		, myMember(std::move(value.myMember)) {
 *		if (checkSlicing) { // MyButton の派生クラスの代入をチェック
 *			assert("Object sliced" && typeid(*this) == typeid(value));
 *		}
 *	}
 *	MyButton(Control& parent, int x, int y) : Button(parent, x, y, 200, 100, L"myButton") {}
 *	MyButton& operator=(MyButton&& value) {
 *		if (this != &value) {
 *			this->~MyButton(); // メンバの移動や派生クラスの代入チェックをコンストラクタにまとめる為にデストラクタと new を使う
 *			new (this) MyButton(std::move(value));
 *		}
 *		return *this;
 *	}
 *	std::vector<String> myMember;
 * };
 * </code></pre>
 * 
 * <h3>・メッセージリフレクション</h3>
 *
 * WM_NOTIFY、WM_COMMAND、WM_CTLCOLORBTN 等のメッセージは親コントロールに通知されて親コントロールで処理することになっているが
 * これを通知元の子コントロールに送り返して子コントロールで処理している。これはMFC や WIN32 API プログラミングではメッセージリフレクションと呼ばれる。
 * こうすることで親コントロールは子コントロールに何が存在するか知らなくても良いし子コントロールは自分のことは全て自分で処理できる。
 * この時送り返されるメッセージ番号は Control::wmReflect + 元のメッセージ番号 となる。
 *
 * <h3>・高ＤＰＩ環境への対応</h3>
 *
 * 高ＤＰＩ環境ではウインドウが見かけ上小さく、システムフォントは大きくなっているのでレイアウトが崩れる。
 * 一部環境ではデフォルトで高ＤＰＩ設定の場合がある他、コントロールパネルからもＤＰＩ設定を変更できる。
 * これに対応するにはまずプロジェクトの設定を以下のように変更する必要があるようだ。<br>
 * 構成プロパティ＞マニフェストツール＞入出力＞DPI 認識の有効化 = はい
 * <br>次にコーディングでどのように対応すべきかはアプリケーションによって異なるが以下のような対策が考えられる。
 *
 * <h6>１．フォントを固定する。</h6>
 *
 * フォントに固定の大きさを指定して作成し、全てのコントロールに設定する。ただし高ＤＰＩ環境では小さく見える。
 *
 * <h6>２．Scaler クラスと scale 関数を利用してコントロールをスケーリングする。</h6>
 *
 * Scaler クラスを渡して Frame クラスの scale 関数を呼べば子コントロールも含めてスケーリングする。
 * ただし getPreferredSize 関数で大きさを決めている場合、この関数がフォントの大きさ等を考慮するのでこれをスケールすると大きくなりすぎる。
 * この場合 scalable 関数で大きさをスケールしないようにすればよい。これは多くのコントロールのコンストラクタ引数の大きさに０を指定した場合も同じ。
 * また高ＤＰＩを利用して精密な画像を見たいというような場合に画像までスケールすると高ＤＰＩの意味が無くなるので
 * スケールする対象はケースバイケースとなる。以下は Frame クラスとその子孫コントロールをＤＰＩ比でスケールするコードの例。
 * <pre><code>
 *	frame.scale(Scaler());
 * </code></pre>
 *
 */
class Control : public Referenceable {
public:
	typedef ::HWND__* HWND;
	typedef Message::LRESULT (__stdcall *Procedure)(HWND handle, unsigned int message, Message::WPARAM wparam, Message::LPARAM lparam);
	typedef ::balor::graphics::Brush Brush;
	typedef ::balor::graphics::Color Color;
	typedef ::balor::graphics::Font Font;
	typedef ::balor::graphics::Graphics Graphics;
	typedef ::balor::graphics::Region Region;
	typedef Graphics::HBITMAP HBITMAP;
	typedef Graphics::HBRUSH HBRUSH;
	typedef Graphics::HDC HDC;
	typedef Graphics::HFONT HFONT;
	typedef Graphics::HICON HICON;
	typedef Graphics::HICON HCURSOR;
	typedef Graphics::HRGN HRGN;


	/// コントロールの境界線の種類。
	struct Edge {
		enum _enum {
			none  , /// 境界線はない。
			line  , /// 一重線。
			sunken, /// へこんだ境界。スタティックコントロールの境界。
			client, /// 立体的な境界。
		};
		BALOR_NAMED_ENUM_MEMBERS(Edge);
	};


	/// ウインドウハンドルを表す。ハンドルに対する操作をまとめて Control のインテリセンスを使いやすくする。
	class Handle : private ::balor::NonCopyable {
	public:
		/// ハンドルから作成。owned が true ならばデストラクタでハンドルを破棄する。
		Handle(HWND handle = nullptr);
		Handle(Handle&& value);
		~Handle();
		Handle& operator=(Handle&& value);

	public:
		/// 親のクライアント座標系での位置と大きさ。
		Rectangle bounds() const;
		void bounds(const Rectangle& value);
		void bounds(int x, int y, int width, int height);
		/// parent を親に持つ事ができるかどうか。
		bool checkParentingCycle(HWND parent) const;
		/// 紐つけられたコントロール。
		Control* control() const;
		void control(Control* value);
		/// 違うプロセスのハンドルかどうか。
		bool differentProcess() const;
		/// 違うスレッドのハンドルかどうか。
		bool differentThread() const;
		/// 境界線の種類。
		Control::Edge edge() const;
		void edge(Control::Edge value);
		/// 拡張ウインドウスタイル。
		int exStyle() const;
		void exStyle(int value);
		/// 指定したスタイルビットが全て有効かどうか。
		bool hasStyle(int bitFlags) const;
		/// 指定した拡張スタイルビットが全て有効かどうか。
		bool hasExStyle(int bitFlags) const;
		/// 親ウインドウハンドル。
		Control::Handle parent() const;
		/// ウインドウプロシージャ。
		Procedure procedure() const;
		Procedure procedure(Procedure value);
		/// 指定したスタイルビットを有効または無効にする。
		void setStyle(int bitFlags, bool on);
		/// 指定した拡張スタイルビットを有効または無効にする。
		void setExStyle(int bitFlags, bool on);
		/// Control の processMessage を呼ぶ標準的なプロシージャ関数。
		static Message::LRESULT __stdcall standardProcedure(HWND handle, unsigned int message, Message::WPARAM wparam, Message::LPARAM lparam);
		/// ウインドウスタイル。
		int style() const;
		void style(int value);
		/// 変更された style, exStyle を表示に反映する。
		void updateStyle();

	public:
		/// HWND への自動変換。
		operator HWND() const { return _handle; }

	private:
		HWND _handle;
	};


	/// 子コントロールを列挙するイテレータ。表示優先度の高い順（＝追加した順）に列挙する。列挙し終えるとヌルを返し、以降は操作できない。
	class ControlsIterator {
	public:
		/// 表示優先度の最も高い子からイテレート。
		ControlsIterator(const Control& parent);
		ControlsIterator(Control* current);

		/// 表示優先度の低い子へ。
		ControlsIterator& operator ++();
		/// 表示優先度の高い子へ。
		ControlsIterator& operator --();
		/// 列挙した Control ポインタの取得。
		operator Control*() const;
		/// 列挙した Control ポインタへのアクセス。
		Control* operator->();

		/// 表示優先度の最も低い子のイテレータ。
		static ControlsIterator getLast(const Control& parent);

	private:
		Control* _current;
	};


	/// 全ての子孫を列挙するイテレータ。列挙し終えるとヌルを返すようになり、以降は操作できない。
	class DescendantsIterator {
	public:
		/// 親クラスから作成。
		DescendantsIterator(const Control& parent);

		/// 次の子孫へ。
		DescendantsIterator& operator ++();
		/// 列挙した Control ポインタの取得。
		operator Control*() const;
		/// 列挙した Control ポインタへのアクセス。
		Control* operator->();

	private:
		Control* _parent;
		Control* _current;
	};



	/// Control のイベントの親クラス。
	typedef EventWithSender<Control> Event;

	typedef Event Activate;
	typedef Event Deactivate;
	typedef Event MouseEnter;
	typedef Event MouseHover;
	typedef Event MouseLeave;


	/// カーソルを設定するイベント。設定しなければコントロールのデフォルトになる。
	struct CursorChange : public Control::Event {
		CursorChange(Control& control);

		/// 設定するカーソル。初期値は nullptr。
		HCURSOR cursor() const;
		void cursor(HCURSOR value);

	private:
		HCURSOR _cursor;
	};


	/// フォーカスを失ったイベント。ComboBox のようにコントロールによっては次または前のコントロールを取得できないことがある。
	/// 多くのアプリケーションでは onDeactivate イベントのほうが使いやすい。
	struct Defocus : public Control::Event {
		Defocus(Control& sender, HWND nextFocusedHandle);

		/// フォーカスを得たコントロール。ヌルの場合もある。
		Control* nextFocused() const;
		/// フォーカスを得たコントロールのウインドウハンドル。ヌルの場合もある。
		Control::Handle nextFocusedHandle() const;

	private:
		HWND _nextFocusedHandle;
	};


	/// フォーカスを得たイベント。ComboBox のようにコントロールによっては次または前のコントロールを取得できないことがある。
	/// 多くのアプリケーションでは onActivate イベントのほうが使いやすい。
	struct Focus : public Control::Event {
		Focus(Control& sender, HWND prevFocusedHandle);

		/// フォーカスを失ったコントロール。ヌルの場合もある。
		Control* prevFocused() const;
		/// フォーカスを失ったコントロールのウインドウハンドル。ヌルの場合もある。
		Control::Handle prevFocusedHandle() const;

	private:
		HWND _prevFocusedHandle;
	};


	/// ヘルプ情報を要求されたイベント。？ボタンを押してコントロールをクリックしたか、F1キーを押した場合に発生する。
	/// ？ボタンを押した場合、？ボタンのある Frame をクリックしても発生せず子コントロールでのみ発生する。メニューのヘルプは未対応。
	struct HelpRequest : public Control::Event {
		HelpRequest(Control& sender, Control& target, const Point& position);

		/// 処理をキャンセルして親コントロールにまかせるかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// マウスカーソルのクライアント座標。
		const Point& position() const;
		/// ヘルプ情報を要求されているコントロール。
		Control& target();

	private:
		Control& _target;
		Point _position;
		bool _cancel;
	};


	/// キーを押したか離したイベント。
	struct KeyEvent : public Control::Event {
		KeyEvent(Control& sender, Key key, int flags);

		/// ALTキーが同時に押されているかどうか。
		bool alt() const;
		/// この後コントロールの処理を呼ぶ必要が無い場合は true に設定する。初期値は false。
		bool handled() const;
		void handled(bool value);
		/// 押したか離したキー。
		Key key() const;
		/// メッセージが送られる前にキーが押されていたかどうか。
		bool previousDown() const;
		/// キーのリピート回数。
		int repeatCount() const;

	private:
		Key _key;
		int _flags;
		bool _handled;
	};

	typedef KeyEvent KeyDown;
	typedef KeyEvent KeyUp;


	/// キー入力で一文字入力があったイベント。カット＆ペーストは捕らえる事ができない。
	struct KeyPress : public Control::Event {
		KeyPress(Control& sender, wchar_t charCode);

		/// 入力された文字。変更することもできる。
		wchar_t charCode() const;
		void charCode(wchar_t value);
		/// この後コントロールの処理を呼ぶ必要が無い場合は true に設定する。初期値は false。
		bool handled() const;
		void handled(bool value);

	private:
		wchar_t _charCode;
		bool _handled;
	};


	/// マウスのボタンをドラッグした、ダブルクリックした、カーソルが移動した、ボタンを離したイベント。
	struct MouseEvent : public Control::Event {
		MouseEvent(Control& sender, Mouse button, Message& message);

		/// イベントの対象となるボタン。
		Mouse button() const;
		/// CTRL キーが押されているかどうか。
		bool ctrl() const;
		/// マウスの左ボタンが押されているかどうか。
		bool lButton() const;
		/// マウスの中央ボタンが押されているかどうか。
		bool mButton() const;
		/// マウスの右ボタンが押されているかどうか。
		bool rButton() const;
		/// マウスのＸ１ボタンが押されているかどうか。
		bool xButton1() const;
		/// マウスのＸ２ボタンが押されているかどうか。
		bool xButton2() const;
		/// マウスカーソルのクライアント座標。
		Point position() const;
		/// Shift キーが押されているかどうか。
		bool shift() const;

	protected:
		Mouse _button;
		Message& _message;
	};
	typedef MouseEvent Drag;
	typedef MouseEvent MouseDoubleClick;
	typedef MouseEvent MouseMove;
	typedef MouseEvent MouseUp;


	/// マウスのボタンを押したイベント。
	struct MouseDown : public MouseEvent {
		MouseDown(Control& sender, Mouse button, Message& message, Rectangle& dragBox);

		///	中心点をマウス位置として onDrag イベントを発生させるマウスの移動範囲。初期値は DragDrop::defaultDragSize() 分の大きさ。
		/// 初期値はシステムがモニタ解像度（DPI）に合わせて調整してくれるが独自に設定する場合は自分でスケーリングを行うか DragDrop::defaultDragSize() の倍数で設定すると良い。
		Rectangle dragBox() const;
		void dragBox(const Rectangle& value);
		void dragBox(int x, int y, int width, int height);

	private:
		Rectangle& _dragBox;
	};


	/// マウスホイールを動かしたイベント。
	struct MouseWheel : public MouseEvent {
		MouseWheel(Control& sender, Message& _message);

		/// マウスカーソルのクライアント座標。
		Point position() const;
		/// マウスホイールの移動量。手前に転がすと負、奥に転がすと正の数で量は Mouse::wheelScrollDelta() で取得できる。
		int wheelDelta() const;
	};


	/// ポップアップメニューを表示するイベント。
	struct PopupMenu : public Control::Event {
		PopupMenu(Control& sender, const Point& position, bool clicked);

		/// 表示をキャンセルしてコントロールのデフォルトポップアップメニューを表示するか、無ければ親コントロールにまかせるかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// マウス右クリックをしたかどうか。Shift + F10 キーを押した場合は false。
		bool clicked() const;
		/// クリックされたクライアント座標。Shift + F10 キーを押した場合はクライアント領域の原点になる。
		const Point& position() const;

	private:
		Point _position;
		bool _clicked;
		bool _cancel;
	};


	/// キーが押されたときにショートカットキーを処理するイベント。
	/// キーが押された時の処理の順番は、ショートカットキーの処理→所属する Frame のショートカットキーの処理→ダイアログキーの処理→コントロールのメッセージの処理となる。
	/// handled を true に設定すると処理できたと判断して以降の処理を飛ばして isInputKey が true ならばコントロールのメッセージ処理を行う。
	/// handled が false の場合は isInputKey でダイアログキーの処理をしないかどうか判断する。
	/// isInputKey の初期値でコントロールがキー入力のメッセージ処理を必要としているかどうかが判断できる。
	/// <br><br>
	/// ◎ おおまかな処理方針<br><br>
	/// １．ショートカットキーを処理した場合　　・・・ handled を true に、isInputKey を false に設定する。<br>
	/// ２．ダイアログキーを無効にする場合　　　・・・ isInputKey を true に設定する。<br>
	/// ２．ダイアログキーにする場合　　　　　　・・・ isInputKey を false に設定する。<br>
	/// ３．ダイアログキーの処理を上書きする場合・・・ handled を true に、処理できた場合は isInputKey を false に設定する。<br>
	/// ４．ダイアログキーの処理を入れ替える場合・・・ shortcut を書き換える。<br>
	struct ShortcutKey : public Control::Event {
		ShortcutKey(Control& sender, int shortcut, bool isInputKey);

		/// この後のショートカットキー処理やダイアログキーの処理を飛ばすなら true。初期値は false。
		bool handled() const;
		void handled(bool value);
		/// handled が false ならばダイアログキーの処理を飛ばすかどうか。handled が true ならばコントロールのメッセージ処理を飛ばすかどうか。
		/// 初期値はコントロールがこのキー入力を必要としているかどうか。
		bool isInputKey() const;
		void isInputKey(bool value);
		/// Key と Key::Modifier を組み合わせたショートカットコマンド。変更もできる。
		int shortcut() const;
		void shortcut(int value);

	private:
		int _shortcut;
		bool _isInputKey;
		bool _handled;
	};


protected:
	/// 描画するイベントのテンプレート。派生クラスで使用する。
	template<typename T, typename Base>
	struct PaintEvent : public Base {
		PaintEvent(T& sender, HDC hdc) : Base(sender), _hdc(hdc) {}

		/// 描画に使うグラフィクス。
		Graphics graphics() const { return Graphics(_hdc); }

	private:
		HDC _hdc;
	};


protected:
	/// ヌルハンドルで作成。
	Control();
	Control(Control&& value);
	virtual ~Control() = 0;

public:
	/// 親のクライアント座標系での位置と大きさ。
	virtual Rectangle bounds() const;
	virtual void bounds(const Rectangle& value);
	void bounds(int x, int y, int width, int height);
	/// 背景ブラシ。ブラシハンドルは参照されるので破棄しないよう注意。
	/// ラジオボタン、チェックボックス、グループボックス、ボタン等のように親コントロールと地続きな子コントロールの為に、親コントロールは背景ブラシを公開する義務がある。
	virtual Brush brush() const;
	virtual void brush(HBRUSH value);
	/// 背景ブラシの原点。
	virtual Point brushOrigin() const;
	virtual void brushOrigin(const Point& value);
	void brushOrigin(int x, int y);
	/// マウスをキャプチャしているかどうか。
	bool captured() const;
	void captured(bool value);
	/// コントロール原点から見たクライアント領域の位置。
	Point clientOrigin() const;
	/// クライアント領域の矩形。位置は常に Point(0, 0)。
	Rectangle clientRectangle() const;
	/// クライアント領域の大きさ。
	Size clientSize() const;
	virtual void clientSize(const Size& value);
	void clientSize(int width, int height);
	/// コントロールの大きさからクライアント領域の大きさを求める。
	virtual Size clientSizeFromSize(const Size& size) const;
	/// クライアント座標系からスクリーン座標系に変換する。
	Point clientToScreen(const Point& point) const;
	Rectangle clientToScreen(const Rectangle& rect) const;
	/// control が子孫に含まれるかどうか。
	bool contains(HWND control) const;
	/// 自分または子孫がフォーカスを得ているかどうか。
	bool containsFocus() const;
	/// 最前面の子コントロールから（追加した順）列挙するイテレータ。
	Control::ControlsIterator controlsBegin() const;
	/// 最後面の子コントロールから（追加した逆順）列挙するイテレータ。
	Control::ControlsIterator controlsLast() const;
	/// 既定のフォント。
	static Font defaultFont();
	/// 全ての子孫を列挙するイテレータ。
	Control::DescendantsIterator descendantsBegin() const;
	/// スクロール位置やスクロール範囲を考慮した仮想的なクライアント座標と大きさ。
	virtual Rectangle displayRectangle() const;
	/// ビットマップにコントロールを描画する。
	void drawTo(HBITMAP bitmap) const;
	/// 入力を受け付けるかどうか。
	virtual bool enabled() const;
	virtual void enabled(bool value);
	/// コントロールの属する Frame を見つける。見つからない場合は nullptr を返す。
	virtual Frame* findFrame();
	/// controlsBegin() で得られる最初の子コントロールの bounds()。
	Rectangle firstBounds() const;
	/// フォーカスを得る。
	void focus();
	/// フォーカスを得られるかどうか。TAB キーや方向キーで移動する対象にしたくない場合に false を返すように上書きする。tabStop だけでは方向キーでフォーカスを得る。
	virtual bool focusable() const;
	/// フォーカスを得ているかどうか。
	virtual bool focused() const;
	/// 表示に使うフォント。初期値は defaultFont()。フォントハンドルは参照されるので破棄しないように注意。
	virtual Font font() const;
	virtual void font(HFONT value);
	/// ウインドウハンドルからコントロールを取得する。コントロールが紐ついていない場合は nullptr を返す。
	static Control* fromHandle(HWND handle);
	/// 指定したクライアント座標に子コントロールがあれば返す。無ければ nullptr を返す。
	Control* getChildAt(const Point& clientPoint, bool skipInvisible = false, bool skipDisabled = false, bool skipTransparent = false) const;
	/// タブオーダー順で start の次の子コントロールを返す。forward が false ならば前のコントロールを返す。無い場合は nullptr を返す。
	/// start は子コントロールか自分自身でなければならない。start が自分自身ならば forward によってタブオーダーが最初または最後の子コントロールを返す。
	Control* getNextControl(Control& start, bool forward) const;
	Control* getNextControl(Control& start, bool forward, bool tabStopOnly, bool nested, bool wrap);
	/// 引数で 0 を指定した部分のみ現在の状態に最適な大きさを求める。0 以外を指定した部分はそのまま返る。コントロールの種類によって求め方が違う。
	/// コントロールによっては width に 0 以外を渡して height に 0 を指定することで width を固定した場合の最適な height 等を求められる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// ウインドウハンドル。得られるハンドルは必ず作成済みで、現在のスレッドで使用できる。そうでなければ assert で警告を出す。
	HWND handle() const;
	/// マウスカーソルがコントロールの上にあるかどうか。
	bool hot() const;
	/// コントロール ID。balor ライブラリではイベントに Listener を使うので使われない。アプリケーション固有の番号を付けたい場合に使う。
	int id() const;
	void id(int value);
	/// controlsLast() で得られる最初の子コントロールの bounds()。
	Rectangle lastBounds() const;
	/// 全体または指定した領域を無効にして再描画されるようにする。
	void invalidate(bool invalidateChildren = false);
	void invalidate(const Rectangle& rect, bool invalidateChildren = false);
	void invalidate(HRGN region, bool invalidateChildren = false);
	/// コントロールが属するメッセージループのスレッドで function を実行する。他のスレッドからコントロールを操作する場合に使う。
	/// synchronous は function の実行が終わるまで待つかどうか。引数や戻り値はラムダ式を使って受け渡しすれば良い。
	void invoke(const std::tr1::function<void ()>& function, bool synchronous = true);
	/// コントロールを現在のスレッドから操作すべきではないかどうか。true の場合は invoke() 関数を使う必要がある。
	bool invokeRequired() const;
	/// onMouseHover イベントが発生するまでのマウス静止の時間（ミリ秒）。初期値は 100。
	int mouseHoverTime() const;
	void mouseHoverTime(int value);
	/// インスタンスを識別する名前。
	const String& name() const;
	void name(String value);
	/// 自分が属する Frame の activeControl になったイベント。onFocus との違いは activeControl は Frame 上のフォーカス遷移だけを追跡するので
	/// ポップアップメニューやダイアログ表示等でいちいちイベントが発生しないこと。ただし自分がトップレベルの Frame である場合はアクティブウインドウになった時のイベントになる。
	Listener<Control::Activate&>& onActivate();
	/// カーソルを設定するイベント。
	Listener<Control::CursorChange&>& onCursorChange();
	/// 自分が属する Frame の activeControl ではなくなったイベント。onDefocus との違いは activeControl は Frame 上のフォーカス遷移だけを追跡するので
	/// ポップアップメニューやダイアロ表示グ等でいちいちイベントが発生しないこと。ただし自分がトップレベルの Frame である場合はアクティブウインドウではなくなった時のイベントになる。
	Listener<Control::Deactivate&>& onDeactivate();
	/// フォーカスを失ったイベント。
	Listener<Control::Defocus&>& onDefocus();
	/// 左または中央または右マウスボタンでドラッグを始めたイベント。onMouseDown イベントでドラッグを開始する移動範囲を指定できる。
	Listener<Control::Drag&>& onDrag();
	/// フォーカスを得たイベント。
	Listener<Control::Focus&>& onFocus();
	/// ヘルプ情報を要求されたイベント。
	Listener<Control::HelpRequest&>& onHelpRequest();
	/// キーを押したイベント。
	Listener<Control::KeyDown&>& onKeyDown();
	/// キー入力で一文字入力したイベント。（WM_CHAR メッセージ）
	Listener<Control::KeyPress&>& onKeyPress();
	/// キーを離したイベント。
	Listener<Control::KeyUp&>& onKeyUp();
	/// マウスボタンをダブルクリックしたイベント。
	Listener<Control::MouseDoubleClick&>& onMouseDoubleClick();
	/// マウスボタンを押したイベント。
	Listener<Control::MouseDown&>& onMouseDown();
	/// マウスカーソルがコントロール上に入ったイベント。
	Listener<Control::MouseEnter&>& onMouseEnter();
	/// マウスカーソルを mouseHoverTime() 分静止したイベント。
	Listener<Control::MouseHover&>& onMouseHover();
	/// マウスカーソルがコントロール上から出たイベント。
	Listener<Control::MouseLeave&>& onMouseLeave();
	/// マウスカーソルを動かしたイベント。
	Listener<Control::MouseMove&>& onMouseMove();
	/// マウスボタンを離したイベント。
	Listener<Control::MouseUp&>& onMouseUp();
	/// マウスホイールを動かしたイベント。
	Listener<Control::MouseWheel&>& onMouseWheel();
	/// ポップアップメニューを表示するイベント。処理しなかった場合は親ウインドウで処理する。
	Listener<Control::PopupMenu&>& onPopupMenu();
	/// キーが押されたときにショートカットキーを処理するイベント。
	Listener<Control::ShortcutKey&>& onShortcutKey();
	/// 親コントロール。
	virtual Control* parent() const;
	virtual void parent(Control* value);
	/// 親コントロールのクライアント座標系におけるこのコントロールの位置。
	Point position() const;
	virtual void position(const Point& value);
	void position(int x, int y);
	/// コントロールの表示領域。未指定の場合はヌルの領域が返る。設定後、領域ハンドルは参照されない。任意の形のコントロールを作れる。
	Region region() const;
	void region(HRGN value);
	/// 大きさを getPreferredSize(0, 0) で求めて更新する。
	void resize();
	/// scale 関数で大きさをスケーリングするかどうか。初期値は true。スケーリングが必要ないか、独自に行う場合は falseに設定する。
	bool scalable() const;
	void scalable(bool value);
	/// Scaler を使ってコントロールをスケーリングする。
	virtual void scale(const Scaler& scaler);
	/// スクリーン座標系からクライアント座標系に変換する。
	Point screenToClient(const Point& point) const;
	Rectangle screenToClient(const Rectangle& rect) const;
	/// 大きさ。
	Size size() const;
	void size(const Size& value);
	void size(int width, int height);
	/// クライアント領域の大きさからコントロールの大きさを求める。
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// タブキーを押したときにフォーカスの移る順番。初期値は 0。
	int tabIndex() const;
	void tabIndex(int value);
	/// タブキーを押したときにこのコントロールにフォーカスを移せるかどうか。
	bool tabStop() const;
	void tabStop(bool value);
	/// 文字列。コントロールによってタイトルだったりラベルだったりエディット内容だったり様々。
	String text() const;
	virtual void text(StringRange value);
	void textToBuffer(StringBuffer& buffer) const;
	/// 描画更新できるかどうか。描画で表示がちらつく場合に描画前に false に設定して描画後に true に設定し、その後 update 関数等を呼ぶ。
	void updatable(bool value);
	/// 無効な領域をその場で描画する。
	void update();
	/// ユーザが自由に使える任意のデータ。
	UniqueAny& userData();
	void userData(UniqueAny&& value);
	/// 表示しているかどうか。
	bool visible() const;
	virtual void visible(bool value);
	/// 親の影響をぬきにして表示されるかどうか。
	bool visibleExceptParent() const;
	/// このコントロール個別でビジュアルスタイルが有効かどうか。システム全体で有効かどうかは System::visualStyleEnabled() 関数で調べられる。
	/// Frame の外観は変わらない模様。
	bool visualStyle() const;
	void visualStyle(bool value);
	/// 表示優先度。小さいほど手前に表示される。0 を指定すると最前面に、-1 を指定すると最後面に移動する。
	int zOrder() const;
	virtual void zOrder(int value);

public:
	/// HWND への自動変換 & null チェック用。
	operator HWND() const;


protected:
	/// 作成されたウインドウハンドルをコントロールに割り当てる。フォントが設定されてなければ defaultFont() に設定する。
	void attachHandle(HWND handle);
	//// マウストラッキング中でなければマウストラッキングを開始して onMouseEnter イベントを発生させる。
	void beginMouseTracking(int mouseHoverTime);
	/// ウインドウハンドルを破棄する。
	void destroyHandle();
	//// マウストラッキング中ならばマウストラッキングを終了して onMouseLeave イベントを発生させる。
	void endMouseTracking();
	//// キー入力がダイアログキーではなくコントロールで処理する必要があるかどうか。
	virtual bool isInputKey(int shortcut) const;
	/// メッセージループ内で TranslateMessage 関数の前に呼んでキー入力を調べ、ショートカットキーやダイアログキーならば処理して true を返す。
	/// 処理しなかったら false を返す。true が返った場合はメッセージは処理済みなのでTranslateMessage 関数も DispatchMessageW 関数も呼ぶ必要は無い。
	static bool preTranslateMessage(Message& msg);
	/// ダイアログキーを処理して処理できたかどうかを返す。
	virtual bool processDialogKey(int shortcut);
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);
	/// 他のコントロールのメッセージを処理する。
	static void processMessage(Control& control, Message& msg);
	/// デフォルトウインドウプロシージャでメッセージを処理する。
	void processMessageByDefault(Message& msg);
	/// マウスボタンを押したときの共通処理。
	void processMouseDown(MouseDown&& event);
	/// マウスボタンを離したときの共通処理。
	void processMouseUp(MouseUp&& event);
	/// 指定したウインドウハンドルにメッセージを転送する。メッセージが処理されたかどうかを返す。
	static bool reflectMessage(HWND handle, Message& msg);
	//// マウストラッキング中ならば onMouseHover イベントの発生時刻を mouseHoverTime ミリ秒ほど延ばす。
	void resetMouseTracking(int mouseHoverTime);
	/// Frame や Panel 等で共通して使うウインドウクラス名。
	static const wchar_t* userClassName();


protected: // processMessage をオーバーライドして「部分的に」書き換えたいと思ったときにこれらが private だと障害になりえる。
		   // 「部分的に」書き換えできるように処理を細切れにして protected の関数にする方が堅牢なのは間違いないがシンプルではない。

	/// メッセージリフレクション用のメッセージ番号。
	static const int wmReflect = 0x2000;

	/// ウインドウハンドル。
	Handle _handle;
	/// デフォルトウインドウプロシージャ。processMessageByDefault 関数から呼ばれる。
	Procedure _defaultProcedure;
	/// マウストラッキング中かどうか。
	bool _mouseTracked;
	/// マウスがこの範囲から出たら onDrag イベントを発生させる。
	Rectangle _dragBox;
	HBRUSH _brush;
	Point _brushOrigin;
	int _mouseHoverTime;
	String _name;
	int _tabIndex;
	bool _scalable;
	UniqueAny _userData;
	Listener<Control::Activate&> _onActivate;
	Listener<Control::CursorChange&> _onCursorChange;
	Listener<Control::Deactivate&> _onDeactivate;
	Listener<Control::Defocus&> _onDefocus;
	Listener<Control::Drag&> _onDrag;
	Listener<Control::Focus&> _onFocus;
	Listener<Control::HelpRequest&> _onHelpRequest;
	Listener<Control::KeyDown&> _onKeyDown;
	Listener<Control::KeyPress&> _onKeyPress;
	Listener<Control::KeyUp&> _onKeyUp;
	Listener<Control::MouseDoubleClick&> _onMouseDoubleClick;
	Listener<Control::MouseDown&> _onMouseDown;
	Listener<Control::MouseEnter&> _onMouseEnter;
	Listener<Control::MouseHover&> _onMouseHover;
	Listener<Control::MouseLeave&> _onMouseLeave;
	Listener<Control::MouseMove&> _onMouseMove;
	Listener<Control::MouseUp&> _onMouseUp;
	Listener<Control::MouseWheel&> _onMouseWheel;
	Listener<Control::PopupMenu&> _onPopupMenu;
	Listener<Control::ShortcutKey&> _onShortcutKey;
};



	}
}