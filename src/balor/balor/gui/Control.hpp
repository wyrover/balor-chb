#pragma once

#include <functional>

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

//namespace std {
//	namespace tr1 {
//template<typename T> class function;
//	}
//}

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
 * コントロ?ルの基底クラス。
 *
 * 全てのコントロ?ルクラスはこのクラスから派生する。
 * カ??ルを設定するには onCursorChange イベントを処理する。
 * ?ップアップメニュ?を?示するには onPopupMenu イベントを処理する。
 * コントロ?ルのニ?モニックやメニュ?のショ?トカットキ?の為に onShortcutKey イベントを利用できる。
 * ドラッグ＆ドロップを実装するのに DragDrop クラスを利用できる。
 * レイアウトを設定するのに DockLayout クラスや LineLayout クラス等を利用できる。
 *
 * <h3>・拡張のガイドライン</h3>
 *
 * Control クラスを継承した派生クラスはウインドウハンドルを作成して attachHandle 関数で Control に紐つけ、processMessage 関数をオ?バ?ライドしてウインドウプロシ?ジャを拡張する。
 * できるだけデストラク?で destoryHandle 関数を呼ぶ。さもなくば Control クラスのデストラク?から呼ばれるが、この時?で派生クラスのデストラク?は既に呼ばれているので
 * Control クラスの processMessage 関数が呼ばれてしまう。あるいは派生クラスに公開されている _handle, _defaultProcedure メンバ変数を?作して初期化、終了処理を行う。
 * Control はコピ?禁?クラスなので一時オブジェクトをｴ�ﾀﾔしたり、関数の戻り値としたり、STL コンテナに格?するためには右辺値コピ?コンストラク?、ｿ�ﾃｪｴ�ﾀﾔ演算子を定?する。
 * その定?は Control 共通の決め事にしたがって実装する必要がある。以下は Button クラスを継承したコ?ドの例。
 * <pre><code>
 * class MyButton : public Button {
 * public:
 *	MyButton() {}
 *	MyButton(MyButton&& value, bool checkSlicing = true)
 *		: Button(std::move(value), false) // Button クラスの派生クラスｴ�ﾀﾔ?ェックに必ずひっかかるので false を指定する。
 *		, myMember(std::move(value.myMember)) {
 *		if (checkSlicing) { // MyButton の派生クラスのｴ�ﾀﾔを?ェック
 *			assert("Object sliced" && typeid(*this) == typeid(value));
 *		}
 *	}
 *	MyButton(Control& parent, int x, int y) : Button(parent, x, y, 200, 100, L"myButton") {}
 *	MyButton& operator=(MyButton&& value) {
 *		if (this != &value) {
 *			this->~MyButton(); // メンバの移動や派生クラスのｴ�ﾀﾔ?ェックをコンストラク?にまとめる為にデストラク?と new を使う
 *			new (this) MyButton(std::move(value));
 *		}
 *		return *this;
 *	}
 *	std::vector<String> myMember;
 * };
 * </code></pre>
 * 
 * <h3>・メッセ?ジリフレクション</h3>
 *
 * WM_NOTIFY、WM_COMMAND、WM_CTLCOLORBTN 等のメッセ?ジは親コントロ?ルに通知されて親コントロ?ルで処理することになっているが
 * これを通知元の子コントロ?ルに送り返して子コントロ?ルで処理している。これはMFC や WIN32 API プログラ?ングではメッセ?ジリフレクションと呼ばれる。
 * こうすることで親コントロ?ルは子コントロ?ルに何が存在するか知らなくても良いし子コントロ?ルは自分のことは全て自分で処理できる。
 * この時送り返されるメッセ?ジ番号は Control::wmReflect + 元のメッセ?ジ番号 となる。
 *
 * <h3>・高ＤＰＩ環境への対応</h3>
 *
 * 高ＤＰＩ環境ではウインドウが見かけ上小さく、システ?フォントは大きくなっているのでレイアウトが崩れる。
 * 一部環境ではデフォルトで高ＤＰＩ設定の場合がある他、コントロ?ルパネルからもＤＰＩ設定を変更できる。
 * これに対応するにはまずプロジェクトの設定を以下のように変更する必要があるようだ。<br>
 * ?成プロパティ＞?ニフェストツ?ル＞入出力＞DPI 認識の有効化 = はい
 * <br>次にコ?ディングでどのように対応すべきかはアプリケ?ションによって異なるが以下のような対策が考えられる。
 *
 * <h6>１．フォントを固定する。</h6>
 *
 * フォントに固定の大きさを指定して作成し、全てのコントロ?ルに設定する。ただし高ＤＰＩ環境では小さく見える。
 *
 * <h6>２．Scaler クラスと scale 関数を利用してコントロ?ルをスケ?リングする。</h6>
 *
 * Scaler クラスを渡して Frame クラスの scale 関数を呼べば子コントロ?ルも含めてスケ?リングする。
 * ただし getPreferredSize 関数で大きさを決めている場合、この関数がフォントの大きさ等を考慮するのでこれをスケ?ルすると大きくなりすぎる。
 * この場合 scalable 関数で大きさをスケ?ルしないようにすればよい。これは多くのコントロ?ルのコンストラク?引数の大きさに０を指定した場合も同じ。
 * また高ＤＰＩを利用して精密な画像を見たいというような場合に画像までスケ?ルすると高ＤＰＩの意味が無くなるので
 * スケ?ルする対象はケ?スバイケ?スとなる。以下は Frame クラスとその子孫コントロ?ルをＤＰＩ比でスケ?ルするコ?ドの例。
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


	/// コントロ?ルの境界線の種類。
	struct Edge {
		enum _enum {
			none  , /// 境界線はない。
			line  , /// 一重線。
			sunken, /// へこんだ境界。ス?ティックコントロ?ルの境界。
			client, /// 立体的な境界。
		};
		BALOR_NAMED_ENUM_MEMBERS(Edge);
	};


	/// ウインドウハンドルを?す。ハンドルに対する?作をまとめて Control のインテリセンスを使いやすくする。
	class Handle : private ::balor::NonCopyable {
	public:
		/// ハンドルから作成。owned が true ならばデストラク?でハンドルを破棄する。
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
		/// 紐つけられたコントロ?ル。
		Control* control() const;
		void control(Control* value);
		/// 違うプロセスのハンドルかどうか。
		bool differentProcess() const;
		/// 違うスレッドのハンドルかどうか。
		bool differentThread() const;
		/// 境界線の種類。
		Control::Edge edge() const;
		void edge(Control::Edge value);
		/// 拡張ウインドウス?イル。
		int exStyle() const;
		void exStyle(int value);
		/// 指定したス?イルビットが全て有効かどうか。
		bool hasStyle(int bitFlags) const;
		/// 指定した拡張ス?イルビットが全て有効かどうか。
		bool hasExStyle(int bitFlags) const;
		/// 親ウインドウハンドル。
		Control::Handle parent() const;
		/// ウインドウプロシ?ジャ。
		Procedure procedure() const;
		Procedure procedure(Procedure value);
		/// 指定したス?イルビットを有効または無効にする。
		void setStyle(int bitFlags, bool on);
		/// 指定した拡張ス?イルビットを有効または無効にする。
		void setExStyle(int bitFlags, bool on);
		/// Control の processMessage を呼ぶ標?的なプロシ?ジャ関数。
		static Message::LRESULT __stdcall standardProcedure(HWND handle, unsigned int message, Message::WPARAM wparam, Message::LPARAM lparam);
		/// ウインドウス?イル。
		int style() const;
		void style(int value);
		/// 変更された style, exStyle を?示に反映する。
		void updateStyle();

	public:
		/// HWND への自動変換。
		operator HWND() const { return _handle; }

	private:
		HWND _handle;
	};


	/// 子コントロ?ルを列挙するイテレ??。?示優先度の高い順（＝追加した順）に列挙する。列挙し終えるとヌルを返し、以?は?作できない。
	class ControlsIterator {
	public:
		/// ?示優先度の最も高い子からイテレ?ト。
		ControlsIterator(const Control& parent);
		ControlsIterator(Control* current);

		/// ?示優先度の低い子へ。
		ControlsIterator& operator ++();
		/// ?示優先度の高い子へ。
		ControlsIterator& operator --();
		/// 列挙した Control ?イン?の取得。
		operator Control*() const;
		/// 列挙した Control ?イン?へのアクセス。
		Control* operator->();

		/// ?示優先度の最も低い子のイテレ??。
		static ControlsIterator getLast(const Control& parent);

	private:
		Control* _current;
	};


	/// 全ての子孫を列挙するイテレ??。列挙し終えるとヌルを返すようになり、以?は?作できない。
	class DescendantsIterator {
	public:
		/// 親クラスから作成。
		DescendantsIterator(const Control& parent);

		/// 次の子孫へ。
		DescendantsIterator& operator ++();
		/// 列挙した Control ?イン?の取得。
		operator Control*() const;
		/// 列挙した Control ?イン?へのアクセス。
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


	/// カ??ルを設定するイベント。設定しなければコントロ?ルのデフォルトになる。
	struct CursorChange : public Control::Event {
		CursorChange(Control& control);

		/// 設定するカ??ル。初期値は nullptr。
		HCURSOR cursor() const;
		void cursor(HCURSOR value);

	private:
		HCURSOR _cursor;
	};


	/// フォ?カスを失ったイベント。ComboBox のようにコントロ?ルによっては次または前のコントロ?ルを取得できないことがある。
	/// 多くのアプリケ?ションでは onDeactivate イベントのほうが使いやすい。
	struct Defocus : public Control::Event {
		Defocus(Control& sender, HWND nextFocusedHandle);

		/// フォ?カスを得たコントロ?ル。ヌルの場合もある。
		Control* nextFocused() const;
		/// フォ?カスを得たコントロ?ルのウインドウハンドル。ヌルの場合もある。
		Control::Handle nextFocusedHandle() const;

	private:
		HWND _nextFocusedHandle;
	};


	/// フォ?カスを得たイベント。ComboBox のようにコントロ?ルによっては次または前のコントロ?ルを取得できないことがある。
	/// 多くのアプリケ?ションでは onActivate イベントのほうが使いやすい。
	struct Focus : public Control::Event {
		Focus(Control& sender, HWND prevFocusedHandle);

		/// フォ?カスを失ったコントロ?ル。ヌルの場合もある。
		Control* prevFocused() const;
		/// フォ?カスを失ったコントロ?ルのウインドウハンドル。ヌルの場合もある。
		Control::Handle prevFocusedHandle() const;

	private:
		HWND _prevFocusedHandle;
	};


	/// ヘルプ情報を要求されたイベント。？??ンを押してコントロ?ルをクリックしたか、F1キ?を押した場合に発生する。
	/// ？??ンを押した場合、？??ンのある Frame をクリックしても発生せず子コントロ?ルでのみ発生する。メニュ?のヘルプは未対応。
	struct HelpRequest : public Control::Event {
		HelpRequest(Control& sender, Control& target, const Point& position);

		/// 処理をキャンセルして親コントロ?ルにまかせるかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// ?ウスカ??ルのクライアント座標。
		const Point& position() const;
		/// ヘルプ情報を要求されているコントロ?ル。
		Control& target();

	private:
		Control& _target;
		Point _position;
		bool _cancel;
	};


	/// キ?を押したか離したイベント。
	struct KeyEvent : public Control::Event {
		KeyEvent(Control& sender, Key key, int flags);

		/// ALTキ?が同時に押されているかどうか。
		bool alt() const;
		/// この後コントロ?ルの処理を呼ぶ必要が無い場合は true に設定する。初期値は false。
		bool handled() const;
		void handled(bool value);
		/// 押したか離したキ?。
		Key key() const;
		/// メッセ?ジが送られる前にキ?が押されていたかどうか。
		bool previousDown() const;
		/// キ?のリピ?ト回数。
		int repeatCount() const;

	private:
		Key _key;
		int _flags;
		bool _handled;
	};

	typedef KeyEvent KeyDown;
	typedef KeyEvent KeyUp;


	/// キ?入力で一文字入力があったイベント。カット＆ペ?ストは捕らえる事ができない。
	struct KeyPress : public Control::Event {
		KeyPress(Control& sender, wchar_t charCode);

		/// 入力された文字。変更することもできる。
		wchar_t charCode() const;
		void charCode(wchar_t value);
		/// この後コントロ?ルの処理を呼ぶ必要が無い場合は true に設定する。初期値は false。
		bool handled() const;
		void handled(bool value);

	private:
		wchar_t _charCode;
		bool _handled;
	};


	/// ?ウスの??ンをドラッグした、?ブルクリックした、カ??ルが移動した、??ンを離したイベント。
	struct MouseEvent : public Control::Event {
		MouseEvent(Control& sender, Mouse button, Message& message);

		/// イベントの対象となる??ン。
		Mouse button() const;
		/// CTRL キ?が押されているかどうか。
		bool ctrl() const;
		/// ?ウスの左??ンが押されているかどうか。
		bool lButton() const;
		/// ?ウスの中央??ンが押されているかどうか。
		bool mButton() const;
		/// ?ウスの右??ンが押されているかどうか。
		bool rButton() const;
		/// ?ウスのＸ１??ンが押されているかどうか。
		bool xButton1() const;
		/// ?ウスのＸ２??ンが押されているかどうか。
		bool xButton2() const;
		/// ?ウスカ??ルのクライアント座標。
		Point position() const;
		/// Shift キ?が押されているかどうか。
		bool shift() const;

	protected:
		Mouse _button;
		Message& _message;
	};
	typedef MouseEvent Drag;
	typedef MouseEvent MouseDoubleClick;
	typedef MouseEvent MouseMove;
	typedef MouseEvent MouseUp;


	/// ?ウスの??ンを押したイベント。
	struct MouseDown : public MouseEvent {
		MouseDown(Control& sender, Mouse button, Message& message, Rectangle& dragBox);

		///	中心?を?ウス位置として onDrag イベントを発生させる?ウスの移動範囲。初期値は DragDrop::defaultDragSize() 分の大きさ。
		/// 初期値はシステ?がモニ?解像度（DPI）に合わせて調整してくれるが独自に設定する場合は自分でスケ?リングを行うか DragDrop::defaultDragSize() の?数で設定すると良い。
		Rectangle dragBox() const;
		void dragBox(const Rectangle& value);
		void dragBox(int x, int y, int width, int height);

	private:
		Rectangle& _dragBox;
	};


	/// ?ウスホイ?ルを動かしたイベント。
	struct MouseWheel : public MouseEvent {
		MouseWheel(Control& sender, Message& _message);

		/// ?ウスカ??ルのクライアント座標。
		Point position() const;
		/// ?ウスホイ?ルの移動量。手前に?がすと負、奥に?がすと正の数で量は Mouse::wheelScrollDelta() で取得できる。
		int wheelDelta() const;
	};


	/// ?ップアップメニュ?を?示するイベント。
	struct PopupMenu : public Control::Event {
		PopupMenu(Control& sender, const Point& position, bool clicked);

		/// ?示をキャンセルしてコントロ?ルのデフォルト?ップアップメニュ?を?示するか、無ければ親コントロ?ルにまかせるかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);
		/// ?ウス右クリックをしたかどうか。Shift + F10 キ?を押した場合は false。
		bool clicked() const;
		/// クリックされたクライアント座標。Shift + F10 キ?を押した場合はクライアント領域の原?になる。
		const Point& position() const;

	private:
		Point _position;
		bool _clicked;
		bool _cancel;
	};


	/// キ?が押されたときにショ?トカットキ?を処理するイベント。
	/// キ?が押された時の処理の順番は、ショ?トカットキ?の処理→所属する Frame のショ?トカットキ?の処理→?イアログキ?の処理→コントロ?ルのメッセ?ジの処理となる。
	/// handled を true に設定すると処理できたと判断して以?の処理を飛ばして isInputKey が true ならばコントロ?ルのメッセ?ジ処理を行う。
	/// handled が false の場合は isInputKey で?イアログキ?の処理をしないかどうか判断する。
	/// isInputKey の初期値でコントロ?ルがキ?入力のメッセ?ジ処理を必要としているかどうかが判断できる。
	/// <br><br>
	/// ◎ おおまかな処理方針<br><br>
	/// １．ショ?トカットキ?を処理した場合??・・・ handled を true に、isInputKey を false に設定する。<br>
	/// ２．?イアログキ?を無効にする場合???・・・ isInputKey を true に設定する。<br>
	/// ２．?イアログキ?にする場合??????・・・ isInputKey を false に設定する。<br>
	/// ３．?イアログキ?の処理を上書きする場合・・・ handled を true に、処理できた場合は isInputKey を false に設定する。<br>
	/// ４．?イアログキ?の処理を入れ替える場合・・・ shortcut を書き換える。<br>
	struct ShortcutKey : public Control::Event {
		ShortcutKey(Control& sender, int shortcut, bool isInputKey);

		/// この後のショ?トカットキ?処理や?イアログキ?の処理を飛ばすなら true。初期値は false。
		bool handled() const;
		void handled(bool value);
		/// handled が false ならば?イアログキ?の処理を飛ばすかどうか。handled が true ならばコントロ?ルのメッセ?ジ処理を飛ばすかどうか。
		/// 初期値はコントロ?ルがこのキ?入力を必要としているかどうか。
		bool isInputKey() const;
		void isInputKey(bool value);
		/// Key と Key::Modifier を組み合わせたショ?トカットコ?ンド。変更もできる。
		int shortcut() const;
		void shortcut(int value);

	private:
		int _shortcut;
		bool _isInputKey;
		bool _handled;
	};


protected:
	/// ?画するイベントのテンプレ?ト。派生クラスで使用する。
	template<typename T, typename Base>
	struct PaintEvent : public Base {
		PaintEvent(T& sender, HDC hdc) : Base(sender), _hdc(hdc) {}

		/// ?画に使うグラフィクス。
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
	/// ラジオ??ン、?ェック?ックス、グル?プ?ックス、??ン等のように親コントロ?ルと地続きな子コントロ?ルの為に、親コントロ?ルは背景ブラシを公開する?務がある。
	virtual Brush brush() const;
	virtual void brush(HBRUSH value);
	/// 背景ブラシの原?。
	virtual Point brushOrigin() const;
	virtual void brushOrigin(const Point& value);
	void brushOrigin(int x, int y);
	/// ?ウスをキャプ?ャしているかどうか。
	bool captured() const;
	void captured(bool value);
	/// コントロ?ル原?から見たクライアント領域の位置。
	Point clientOrigin() const;
	/// クライアント領域の矩?。位置は常に Point(0, 0)。
	Rectangle clientRectangle() const;
	/// クライアント領域の大きさ。
	Size clientSize() const;
	virtual void clientSize(const Size& value);
	void clientSize(int width, int height);
	/// コントロ?ルの大きさからクライアント領域の大きさを求める。
	virtual Size clientSizeFromSize(const Size& size) const;
	/// クライアント座標系からスクリ?ン座標系に変換する。
	Point clientToScreen(const Point& point) const;
	Rectangle clientToScreen(const Rectangle& rect) const;
	/// control が子孫に含まれるかどうか。
	bool contains(HWND control) const;
	/// 自分または子孫がフォ?カスを得ているかどうか。
	bool containsFocus() const;
	/// 最前面の子コントロ?ルから（追加した順）列挙するイテレ??。
	Control::ControlsIterator controlsBegin() const;
	/// 最後面の子コントロ?ルから（追加した逆順）列挙するイテレ??。
	Control::ControlsIterator controlsLast() const;
	/// 既定のフォント。
	static Font defaultFont();
	/// 全ての子孫を列挙するイテレ??。
	Control::DescendantsIterator descendantsBegin() const;
	/// スクロ?ル位置やスクロ?ル範囲を考慮した仮想的なクライアント座標と大きさ。
	virtual Rectangle displayRectangle() const;
	/// ビット?ップにコントロ?ルを?画する。
	void drawTo(HBITMAP bitmap) const;
	/// 入力を受け付けるかどうか。
	virtual bool enabled() const;
	virtual void enabled(bool value);
	/// コントロ?ルの属する Frame を見つける。見つからない場合は nullptr を返す。
	virtual Frame* findFrame();
	/// controlsBegin() で得られる最初の子コントロ?ルの bounds()。
	Rectangle firstBounds() const;
	/// フォ?カスを得る。
	void focus();
	/// フォ?カスを得られるかどうか。TAB キ?や方向キ?で移動する対象にしたくない場合に false を返すように上書きする。tabStop だけでは方向キ?でフォ?カスを得る。
	virtual bool focusable() const;
	/// フォ?カスを得ているかどうか。
	virtual bool focused() const;
	/// ?示に使うフォント。初期値は defaultFont()。フォントハンドルは参照されるので破棄しないように注意。
	virtual Font font() const;
	virtual void font(HFONT value);
	/// ウインドウハンドルからコントロ?ルを取得する。コントロ?ルが紐ついていない場合は nullptr を返す。
	static Control* fromHandle(HWND handle);
	/// 指定したクライアント座標に子コントロ?ルがあれば返す。無ければ nullptr を返す。
	Control* getChildAt(const Point& clientPoint, bool skipInvisible = false, bool skipDisabled = false, bool skipTransparent = false) const;
	/// ?ブオ???順で start の次の子コントロ?ルを返す。forward が false ならば前のコントロ?ルを返す。無い場合は nullptr を返す。
	/// start は子コントロ?ルか自分自身でなければならない。start が自分自身ならば forward によって?ブオ???が最初または最後の子コントロ?ルを返す。
	Control* getNextControl(Control& start, bool forward) const;
	Control* getNextControl(Control& start, bool forward, bool tabStopOnly, bool nested, bool wrap);
	/// 引数で 0 を指定した部分のみ現在の状態に最適な大きさを求める。0 以外を指定した部分はそのまま返る。コントロ?ルの種類によって求め方が違う。
	/// コントロ?ルによっては width に 0 以外を渡して height に 0 を指定することで width を固定した場合の最適な height 等を求められる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// ウインドウハンドル。得られるハンドルは必ず作成済みで、現在のスレッドで使用できる。そうでなければ assert で警告を出す。
	HWND handle() const;
	/// ?ウスカ??ルがコントロ?ルの上にあるかどうか。
	bool hot() const;
	/// コントロ?ル ID。balor ライブラリではイベントに Listener を使うので使われない。アプリケ?ション固有の番号を付けたい場合に使う。
	int id() const;
	void id(int value);
	/// controlsLast() で得られる最初の子コントロ?ルの bounds()。
	Rectangle lastBounds() const;
	/// 全体または指定した領域を無効にして再?画されるようにする。
	void invalidate(bool invalidateChildren = false);
	void invalidate(const Rectangle& rect, bool invalidateChildren = false);
	void invalidate(HRGN region, bool invalidateChildren = false);
	/// コントロ?ルが属するメッセ?ジル?プのスレッドで function を実行する。他のスレッドからコントロ?ルを?作する場合に使う。
	/// synchronous は function の実行が終わるまで待つかどうか。引数や戻り値はラ??式を使って受け渡しすれば良い。
	void invoke(const std::function<void ()>& function, bool synchronous = true);
	/// コントロ?ルを現在のスレッドから?作すべきではないかどうか。true の場合は invoke() 関数を使う必要がある。
	bool invokeRequired() const;
	/// onMouseHover イベントが発生するまでの?ウス静?の時間（?リ秒）。初期値は 100。
	int mouseHoverTime() const;
	void mouseHoverTime(int value);
	/// インス?ンスを識別する名前。
	const String& name() const;
	void name(String value);
	/// 自分が属する Frame の activeControl になったイベント。onFocus との違いは activeControl は Frame 上のフォ?カス遷移だけを追跡するので
	/// ?ップアップメニュ?や?イアログ?示等でいちいちイベントが発生しないこと。ただし自分がトップレベルの Frame である場合はアクティブウインドウになった時のイベントになる。
	Listener<Control::Activate&>& onActivate();
	/// カ??ルを設定するイベント。
	Listener<Control::CursorChange&>& onCursorChange();
	/// 自分が属する Frame の activeControl ではなくなったイベント。onDefocus との違いは activeControl は Frame 上のフォ?カス遷移だけを追跡するので
	/// ?ップアップメニュ?や?イアロ?示グ等でいちいちイベントが発生しないこと。ただし自分がトップレベルの Frame である場合はアクティブウインドウではなくなった時のイベントになる。
	Listener<Control::Deactivate&>& onDeactivate();
	/// フォ?カスを失ったイベント。
	Listener<Control::Defocus&>& onDefocus();
	/// 左または中央または右?ウス??ンでドラッグを始めたイベント。onMouseDown イベントでドラッグを開始する移動範囲を指定できる。
	Listener<Control::Drag&>& onDrag();
	/// フォ?カスを得たイベント。
	Listener<Control::Focus&>& onFocus();
	/// ヘルプ情報を要求されたイベント。
	Listener<Control::HelpRequest&>& onHelpRequest();
	/// キ?を押したイベント。
	Listener<Control::KeyDown&>& onKeyDown();
	/// キ?入力で一文字入力したイベント。（WM_CHAR メッセ?ジ）
	Listener<Control::KeyPress&>& onKeyPress();
	/// キ?を離したイベント。
	Listener<Control::KeyUp&>& onKeyUp();
	/// ?ウス??ンを?ブルクリックしたイベント。
	Listener<Control::MouseDoubleClick&>& onMouseDoubleClick();
	/// ?ウス??ンを押したイベント。
	Listener<Control::MouseDown&>& onMouseDown();
	/// ?ウスカ??ルがコントロ?ル上に入ったイベント。
	Listener<Control::MouseEnter&>& onMouseEnter();
	/// ?ウスカ??ルを mouseHoverTime() 分静?したイベント。
	Listener<Control::MouseHover&>& onMouseHover();
	/// ?ウスカ??ルがコントロ?ル上から出たイベント。
	Listener<Control::MouseLeave&>& onMouseLeave();
	/// ?ウスカ??ルを動かしたイベント。
	Listener<Control::MouseMove&>& onMouseMove();
	/// ?ウス??ンを離したイベント。
	Listener<Control::MouseUp&>& onMouseUp();
	/// ?ウスホイ?ルを動かしたイベント。
	Listener<Control::MouseWheel&>& onMouseWheel();
	/// ?ップアップメニュ?を?示するイベント。処理しなかった場合は親ウインドウで処理する。
	Listener<Control::PopupMenu&>& onPopupMenu();
	/// キ?が押されたときにショ?トカットキ?を処理するイベント。
	Listener<Control::ShortcutKey&>& onShortcutKey();
	/// 親コントロ?ル。
	virtual Control* parent() const;
	virtual void parent(Control* value);
	/// 親コントロ?ルのクライアント座標系におけるこのコントロ?ルの位置。
	Point position() const;
	virtual void position(const Point& value);
	void position(int x, int y);
	/// コントロ?ルの?示領域。未指定の場合はヌルの領域が返る。設定後、領域ハンドルは参照されない。任意の?のコントロ?ルを作れる。
	Region region() const;
	void region(HRGN value);
	/// 大きさを getPreferredSize(0, 0) で求めて更新する。
	void resize();
	/// scale 関数で大きさをスケ?リングするかどうか。初期値は true。スケ?リングが必要ないか、独自に行う場合は falseに設定する。
	bool scalable() const;
	void scalable(bool value);
	/// Scaler を使ってコントロ?ルをスケ?リングする。
	virtual void scale(const Scaler& scaler);
	/// スクリ?ン座標系からクライアント座標系に変換する。
	Point screenToClient(const Point& point) const;
	Rectangle screenToClient(const Rectangle& rect) const;
	/// 大きさ。
	Size size() const;
	void size(const Size& value);
	void size(int width, int height);
	/// クライアント領域の大きさからコントロ?ルの大きさを求める。
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// ?ブキ?を押したときにフォ?カスの移る順番。初期値は 0。
	int tabIndex() const;
	void tabIndex(int value);
	/// ?ブキ?を押したときにこのコントロ?ルにフォ?カスを移せるかどうか。
	bool tabStop() const;
	void tabStop(bool value);
	/// 文字列。コントロ?ルによって?イトルだったりラベルだったりエディット内容だったり様々。
	String text() const;
	virtual void text(StringRange value);
	void textToBuffer(StringBuffer& buffer) const;
	/// ?画更新できるかどうか。?画で?示がちらつく場合に?画前に false に設定して?画後に true に設定し、その後 update 関数等を呼ぶ。
	void updatable(bool value);
	/// 無効な領域をその場で?画する。
	void update();
	/// ユ?ザが自由に使える任意のデ??。
	UniqueAny& userData();
	void userData(UniqueAny&& value);
	/// ?示しているかどうか。
	bool visible() const;
	virtual void visible(bool value);
	/// 親の影響をぬきにして?示されるかどうか。
	bool visibleExceptParent() const;
	/// このコントロ?ル個別でビジュアルス?イルが有効かどうか。システ?全体で有効かどうかは System::visualStyleEnabled() 関数で調べられる。
	/// Frame の外観は変わらない模様。
	bool visualStyle() const;
	void visualStyle(bool value);
	/// ?示優先度。小さいほど手前に?示される。0 を指定すると最前面に、-1 を指定すると最後面に移動する。
	int zOrder() const;
	virtual void zOrder(int value);

public:
	/// HWND への自動変換 & null ?ェック用。
	operator HWND() const;


protected:
	/// 作成されたウインドウハンドルをコントロ?ルに割り当てる。フォントが設定されてなければ defaultFont() に設定する。
	void attachHandle(HWND handle);
	//// ?ウストラッキング中でなければ?ウストラッキングを開始して onMouseEnter イベントを発生させる。
	void beginMouseTracking(int mouseHoverTime);
	/// ウインドウハンドルを破棄する。
	void destroyHandle();
	//// ?ウストラッキング中ならば?ウストラッキングを終了して onMouseLeave イベントを発生させる。
	void endMouseTracking();
	//// キ?入力が?イアログキ?ではなくコントロ?ルで処理する必要があるかどうか。
	virtual bool isInputKey(int shortcut) const;
	/// メッセ?ジル?プ内で TranslateMessage 関数の前に呼んでキ?入力を調べ、ショ?トカットキ?や?イアログキ?ならば処理して true を返す。
	/// 処理しなかったら false を返す。true が返った場合はメッセ?ジは処理済みなのでTranslateMessage 関数も DispatchMessageW 関数も呼ぶ必要は無い。
	static bool preTranslateMessage(Message& msg);
	/// ?イアログキ?を処理して処理できたかどうかを返す。
	virtual bool processDialogKey(int shortcut);
	/// メッセ?ジを処理する。いわゆるウインドウプロシ?ジャ。
	virtual void processMessage(Message& msg);
	/// 他のコントロ?ルのメッセ?ジを処理する。
	static void processMessage(Control& control, Message& msg);
	/// デフォルトウインドウプロシ?ジャでメッセ?ジを処理する。
	void processMessageByDefault(Message& msg);
	/// ?ウス??ンを押したときの共通処理。
	void processMouseDown(MouseDown&& event);
	/// ?ウス??ンを離したときの共通処理。
	void processMouseUp(MouseUp&& event);
	/// 指定したウインドウハンドルにメッセ?ジを?送する。メッセ?ジが処理されたかどうかを返す。
	static bool reflectMessage(HWND handle, Message& msg);
	//// ?ウストラッキング中ならば onMouseHover イベントの発生時刻を mouseHoverTime ?リ秒ほど延ばす。
	void resetMouseTracking(int mouseHoverTime);
	/// Frame や Panel 等で共通して使うウインドウクラス名。
	static const wchar_t* userClassName();


protected: // processMessage をオ?バ?ライドして「部分的に」書き換えたいと思ったときにこれらが private だと障害になりえる。
		   // 「部分的に」書き換えできるように処理を細切れにして protected の関数にする方が堅牢なのは間違いないがシンプルではない。

	/// メッセ?ジリフレクション用のメッセ?ジ番号。
	static const int wmReflect = 0x2000;

	/// ウインドウハンドル。
	Handle _handle;
	/// デフォルトウインドウプロシ?ジャ。processMessageByDefault 関数から呼ばれる。
	Procedure _defaultProcedure;
	/// ?ウストラッキング中かどうか。
	bool _mouseTracked;
	/// ?ウスがこの範囲から出たら onDrag イベントを発生させる。
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