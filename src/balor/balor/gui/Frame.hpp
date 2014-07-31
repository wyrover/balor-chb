#pragma once

#include <balor/gui/ScrollableControl.hpp>

struct tagMINMAXINFO;

namespace balor {
	namespace graphics {
		class Icon;
	}
}


namespace balor {
	namespace gui {

class Button;
class MenuBar;



/**
* トップレベルで他のコントロールのフレームとなるウインドウ。
*
* TODO: parent に他のコントロールを指定して子フレームになると WS_CAPTION スタイルを持つ限りフレーム上のエディットコントロールがマウスに反応しなくなる。原因不明。.NET でも同じ現象が起きるので WIN32 の仕様かもしれない。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"Frame Sample");

Edit edit(frame, 20, 10, 0, 0, 20);
Button accept(frame, 20, 40, 0, 0, L"決定", [&] (Button::Click& ) {
MsgBox::show(L"決定しました。");
});
Button cancel(frame, 20, 70, 0, 0, L"キャンセル", [&] (Button::Click& ) {
MsgBox::show(L"キャンセルしました。");
});
frame.acceptButton(&accept); // Enter キーで反応。
frame.cancelButton(&cancel); // ESC キーで反応。
frame.onClosing() = [&] (Frame::Closing& e) {
if (MsgBox::show(frame, L"閉じますか？", L"終了", MsgBox::Buttons::okCancel) != MsgBox::Result::ok) {
e.cancel(true);
}
};
frame.icon(Icon::question());

frame.runMessageLoop();
* </code></pre>
*/
class Frame : public ScrollableControl {
public:
	typedef ::balor::graphics::Icon Icon;


	/// ウインドウを閉じる理由。
	struct CloseReason {
		enum _enum {
			none,
			taskManagerClosing, /// タスクマネージャーから閉じた。
			userClosing, /// close() メソッドか閉じるボタンで閉じた。
			windowShutDown, /// ウインドウズのシャットダウン。
		};
		BALOR_NAMED_ENUM_MEMBERS(CloseReason);
	};


	/// ウインドウのスタイル。
	struct Style {
		enum _enum {
			none, /// タイトルバーも枠も無いウインドウ。
			singleLine, /// 一重線の枠を持つウインドウ。
			threeDimensional, /// 立体的な枠を持つウインドウ。
			dialog, /// ダイアログ風。
			sizable, /// リサイズできるウインドウ。
			toolWindow, /// ツールウインドウ。アイコン、最大化ボタン、最小化ボタン、ヘルプボタンが表示されない。
			sizableToolWindow, /// リサイズできるツールウインドウ。アイコン、最大化ボタン、最小化ボタン、ヘルプボタンが表示されない。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};


	/// Frame のイベントの親クラス。
	typedef EventWithSubclassSender<Frame, ScrollableControl::Event> Event;

	typedef Event DisplayChange;
	typedef Event MenuLoopBegin;
	typedef Event MenuLoopEnd;
	typedef Event ResizeMoveBegin;
	typedef Event ResizeMoveEnd;
	typedef PaintEvent<Frame, Event> Paint;


	/// ウインドウを閉じたイベント。
	struct Close : public Frame::Event {
		Close(Frame& sender, CloseReason reason);

		/// 閉じた理由。
		CloseReason closeReason() const;

	private:
		CloseReason _closeReason;
	};


	/// ウインドウを閉じるイベント。
	struct Closing : public Close {
		Closing(Frame& sender, CloseReason reason);

		/// 閉じるのをキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	///	ヘルプボタンを押したイベント。
	struct HelpButtonClick : public Frame::Event {
		HelpButtonClick(Frame& sender);

		/// デフォルトの処理をキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	///	移動したイベント。
	struct Move : public Frame::Event {
		Move(Frame& sender, const Point& position);

		/// 移動したスクリーン位置。
		const Point& position() const;

	protected:
		Point _position;
	};


	///	移動するイベント。
	struct Moving : public Move {
		Moving(Frame& sender, const Point& position);

		/// 移動するスクリーン位置。
		using Move::position;
		void position(const Point& value);
		void position(int x, int y);
	};


	///	大きさを変更するイベント。
	struct Resizing : public Frame::Event {
		Resizing(Frame& sender, ::tagMINMAXINFO* info);

		/// 最大化した時のスクリーン位置。※イベント名のミスマッチ！
		Point maximizedPosition() const;
		void maximizedPosition(const Point& value);
		void maximizedPosition(int x, int y);
		/// 最大化した時の大きさ。
		Size maximizedSize() const;
		void maximizedSize(const Size& value);
		void maximizedSize(int width, int height);
		/// ウインドウをドラッグして大きさ変更する時の最大の大きさ。
		Size maxTrackSize() const;
		void maxTrackSize(const Size& value);
		void maxTrackSize(int width, int height);
		/// ウインドウをドラッグして大きさ変更する時の最小の大きさ。
		Size minTrackSize() const;
		void minTrackSize(const Size& value);
		void minTrackSize(int width, int height);

	private:
		::tagMINMAXINFO* _info;
	};


	///	大きさを変更したイベント。
	struct Resize : public Frame::Event {
		Resize(Frame& sender, int flag);

		/// 最大化されたかどうか。
		bool maximized() const;
		/// 最小化されたかどうか。
		bool minimized() const;

	private:
		int _flag;
	};


public:
	/// ヌルハンドルで作成。
	Frame();
	/// タイトル文字列、クライアントサイズ、スタイルから作成。クライアントサイズを０にすると既定の大きさで作成する。
	Frame(StringRange text, int clientWidth = 0, int clientHeight = 0, Frame::Style style = Style::sizable);
	Frame(Frame&& value, bool checkSlicing = true);
	virtual ~Frame();
	Frame& operator=(Frame&& value);

public:
	/// ボタン以外にフォーカスがある時に Enter キーを押すと押した事になるボタン。
	Button* acceptButton() const;
	void acceptButton(Button* value);
	/// アクティブにする。
	void activate();
	/// ウインドウ上でフォーカスを得ているコントロール。ウインドウがアクティブでなくなっても保存され、ウインドウが再びアクティブになった時にフォーカスが復元される。
	virtual Control* activeControl() const;
	virtual void activeControl(Control* value);
	/// 現在アクティブなウインドウ。
	static Frame* activeFrame();
	/// ESC キーを押した時に押した事になるボタン。
	Button* cancelButton() const;
	void cancelButton(Button* value);
	/// タイトルバーの文字列のフォント。
	static Font captionFont();
	/// オーナーウインドウの中心に移動する。
	void centerToOwner();
	/// スクリーンの中心に移動する。
	void centerToScreen();
	/// ウインドウを閉じる。ウインドウは非表示になるが破壊はされない。
	void close();
	/// デスクトップ上での座標。
	Point desktopPosition() const;
	void desktopPosition(const Point& value);
	void desktopPosition(int x, int y);
	/// メッセージループから抜ける。
	static void exitMessageLoop();
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// ？ボタンを有効にするかどうか。有効にした場合、最大化ボタンと最小化ボタンは消える。初期値は false。
	bool helpButton() const;
	void helpButton(bool value);
	/// タイトルバーに表示するアイコン。大きいアイコンを指定する。初期値は nullptr。
	Icon icon() const;
	void icon(HICON value);
	/*bool keyPreview() const;  onShortcutKey イベントあればいらない？ *//**/
	/*void keyPreview(bool value); *//**/
	/// 最大化ボタンを有効にするかどうか。初期値は true。
	bool maximizeButton() const;
	void maximizeButton(bool value);
	/// 最大化しているかどうか。visible が true の場合のみ変更できる。初期値は false。
	bool maximized() const;
	void maximized(bool value);
	/// メニューバーを設定する。
	void menuBar(MenuBar* value);
	/// メニューバーの占める領域。無い場合は Rectangle(0, 0, 0, 0) を返す。
	Rectangle menuBarBounds() const;
	/// メニューバーがフォーカスを得ているかどうか。
	bool menuBarFocused() const;
	/// 最小化ボタンを有効にするかどうか。初期値は true。
	bool minimizeButton() const;
	void minimizeButton(bool value);
	/// 最小化しているかどうか。初期値は false。
	bool minimized() const;
	void minimized(bool value);
	/// ウインドウを閉じたイベント。
	Listener<Frame::Close&>& onClose();
	/// ウインドウを閉じるイベント。
	Listener<Frame::Closing&>& onClosing();
	/// モニタの解像度または色数が変更されたイベント。
	Listener<Frame::DisplayChange&>& onDisplayChange();
	/// ヘルプボタンを押したイベント。
	Listener<Frame::HelpButtonClick&>& onHelpButtonClick();
	/// メニューの表示を開始したイベント。
	Listener<Frame::MenuLoopBegin&>& onMenuLoopBegin();
	/// メニューの表示を終了したイベント。
	Listener<Frame::MenuLoopEnd&>& onMenuLoopEnd();
	/// 移動したイベント。
	Listener<Frame::Move&>& onMove();
	/// 移動するイベント。
	Listener<Frame::Moving&>& onMoving();
	/// クライアント領域を描画するイベント。
	Listener<Frame::Paint&>& onPaint();
	/// 大きさが変更されたイベント。
	Listener<Frame::Resize&>& onResize();
	/// 大きさが変更されるイベント。
	Listener<Frame::Resizing&>& onResizing();
	/// サイズ変更や移動を開始したイベント。
	Listener<Frame::ResizeMoveBegin&>& onResizeMoveBegin();
	/// サイズ変更や移動を終了したイベント。
	Listener<Frame::ResizeMoveEnd&>& onResizeMoveEnd();
	/// オーナー。オーナーを持つウインドウは常にオーナーより前に表示されてオーナーが最小化や閉じたりすると追従する。
	Frame* owner() const;
	void owner(Frame* value);
	/// オーナーウインドウハンドル。オーナーを持つウインドウは常にオーナーより前に表示されてオーナーが最小化や閉じたりすると追従する。
	HWND ownerHandle() const;
	void ownerHandle(HWND value);
	/// 親コントロール。
	using ScrollableControl::parent;
	virtual void parent(Control* value);
	/// setLayer 関数で設定したレイヤーを無効にする。
	void resetLayer();
	/// 最大化も最小化もしていない状態の bounds。
	Rectangle restoreBounds() const;
	/// メッセージループを実行する。exitMessageLoop 関数を呼ぶか、この関数を呼んだウインドウが閉じられればループを抜ける。
	void runMessageLoop();
	/// 同じスレッドの他のウインドウを使用不可にしてメッセージループを実行する。いわゆるダイアログの表示。オーナーを設定しておく必要がある。
	void runModalMessageLoop();
	/// 溜まっているメッセージを処理してメッセージループを終了すべきかどうかを返す。
	bool runPostedMessages();
	/// ウインドウを半透明にする。alpha は不透明度(0～1) で 1 で不透明。resetLayer 関数で無効化できる。
	void setLayer(float alpha);
	/// ウインドウ上で colorKey と同じ色の部分を透明にする。resetLayer 関数で無効化できる。
	void setLayer(const Color& colorKey);
	/// graphicsWithAlphaChannel で指定したアルファチャンネルを持った３２ビット画像でウインドウと背景を半透明合成する。画像はあらかじめ Bitmap::premultiplyAlpha 関数を実行しておく必要がある。
	/// alpha 引数でさらに全体に半透明をかけることもできる。style() を強制的に Frame::Style::none に設定する。resetLayer 関数で無効化できる。他の setLayer 関数が既に適用されている場合、この関数は失敗する。
	/// WM_PAINT が処理されなくなり、画像のみ表示されるのでこの Frame の上にコントロールを表示したい場合はこの Frame をオーナーに持ち、colorKey で透過した Frame を上に乗せて位置を追従させると良い。
	void setLayer(HDC graphicsWithAlphaChannel, float alpha = 1.0f);
	/// クライアント領域の大きさからコントロールの大きさを求める。
	virtual Size sizeFromClientSize(const Size& clientSize) const;
	/// タスクバーに表示するかどうか。表示を消せるのは style() が Frame::Style::toolWindow か Frame::Style::sizableToolWindow の時かオーナーを持つウインドウだけ。
	/// 上記の style() 以外で表示を消すには上記の style() を持った非表示のオーナーを持てば良い。初期値は true。
	bool showInTaskbar() const;
	void showInTaskbar(bool value);
	/// ウインドウのスタイル。初期値は Frame::Style::sizable。
	Frame::Style style() const;
	void style(Frame::Style value);
	/// 閉じる、最大化、最小化ボタンとシステムメニューを有効にするかどうか。初期値は true。
	bool systemMenu() const;
	void systemMenu(bool value);
	/// タイトル文字列。
	using ScrollableControl::text;
	virtual void text(StringRange value);
	/// 常に最前面に表示されるかどうか。初期値は false。
	bool topMost() const;
	void topMost(bool value);
	/// メニューバーの表示を更新する。ポップアップではないメニュー項目を変更したら更新する必要がある。
	void updateMenubar();
	/// 表示されるかどうか。初期値は false。
	using ScrollableControl::visible;
	virtual void visible(bool value);


public: // 本当は protected にしたいが Control とその派生クラスから呼ぶ必要があるので公開。
	/// 子孫コントロールが子孫ではなくなるときの処理。。
	virtual void processDescendantErased(Control& descendant);
	///	子孫コントロールがフォーカスを得たときの処理。
	virtual void processDescendantFocused(Control& descendant);

protected:
	/// ダイアログキーを処理して処理できたかどうかを返す。
	virtual bool processDialogKey(int shortcut);
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);
	/// ウインドウハンドルのスタイルを更新する。
	void updateHandleStyle(int style, int exStyle, HICON icon);


protected:
	bool _canPostQuitMessage;
	CloseReason _closeReason;
	Reference<Button> _acceptButton;
	Reference<Control> _activeControl;
	Reference<Button> _cancelButton;
	Style _style;
	Listener<Frame::Close&> _onClose;
	Listener<Frame::Closing&> _onClosing;
	Listener<Frame::DisplayChange&> _onDisplayChange;
	Listener<Frame::HelpButtonClick&> _onHelpButtonClick;
	Listener<Frame::MenuLoopBegin&> _onMenuLoopBegin;
	Listener<Frame::MenuLoopEnd&> _onMenuLoopEnd;
	Listener<Frame::Move&> _onMove;
	Listener<Frame::Moving&> _onMoving;
	Listener<Frame::Paint&> _onPaint;
	Listener<Frame::Resize&> _onResize;
	Listener<Frame::Resizing&> _onResizing;
	Listener<Frame::ResizeMoveBegin&> _onResizeMoveBegin;
	Listener<Frame::ResizeMoveEnd&> _onResizeMoveEnd;
};



	}
}