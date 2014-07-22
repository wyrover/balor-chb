#pragma once

#include <balor/graphics/Color.hpp>
#include <balor/gui/Control.hpp>

namespace balor {
	namespace graphics {
		class Bitmap;
		class Icon;
	}
}


namespace balor {
	namespace gui {



/**
 * スタティックコントロール。コントロール上に文字列やアイコン、ビットマップを表示する。
 * 
 * 名前を Static にすると変数名に困るので Label に変更した。描画イベントは無いが、オーナードローするぐらいなら Panel クラスで onPaint イベントを処理したほうが良い。
 * 文字列、ビットマップ、アイコンの３種別のどれかを表示し、途中から種別を変更することはできない。
 * ビットマップを貼って DockLayout で拡縮させるのはちらつきが酷すぎるのでやめたほうがよい。Panel で onPaint イベントを処理すべき。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Label Sample");

	Label textLabel(frame, 20, 10, 0, 0, L"文字列ラベル:");
	Label iconLabel(frame, 20, 50, 0, 0, Icon::asterisk());

	frame.runMessageLoop();
 * </code></pre>
 */
class Label : public Control {
public:
	typedef ::balor::graphics::Bitmap Bitmap;
	typedef ::balor::graphics::Icon Icon;


	/// 文字列の配置。
	struct TextAlign {
		enum _enum {
			left   = 0x00000000, /// 左揃え。
			center = 0x00000001, /// 中央揃え。
			right  = 0x00000002, /// 右揃え。
		};
		BALOR_NAMED_ENUM_MEMBERS(TextAlign);
	};


	/// Label のイベントの親クラス。
	typedef EventWithSubclassSender<Label, Control::Event> Event;


public:
	/// ヌルハンドルで作成。
	Label();
	Label(Label&& value, bool checkSlicing = true);
	/// 親、位置、大きさ、文字列から作成。大きさを０にすると getPreferredSize 関数で求める。
	Label(Control& parent, int x, int y, int width, int height, StringRange text);
	/// 親、位置、大きさ、ビットマップから作成。大きさを０にすると getPreferredSize 関数で求める。
	Label(Control& parent, int x, int y, int width, int height, HBITMAP bitmap);
	/// 親、位置、大きさ、アイコンから作成。大きさを０にすると getPreferredSize 関数で求める。
	Label(Control& parent, int x, int y, int width, int height, HICON icon);
	virtual ~Label();
	Label& operator=(Label&& value);

public:
	/// 表示するビットマップ。無い場合はヌルハンドルのビットマップが返る。
	Bitmap bitmap() const;
	void bitmap(HBITMAP value);
	/// ビットマップやアイコンを拡縮せずに原寸で中央に描画する。初期値は false。
	bool centerImage() const;
	void centerImage(bool value);
	/// コントロールの境界線の種類。初期値は Control::Edge::none。
	Control::Edge edge() const;
	void edge(Control::Edge value);
	/// 複数行エディットコントロールのように文字列を表示する。例えば単語区切りや改行コードがなくともはみ出れば改行する。初期値は true。
	bool editLike() const;
	void editLike(bool value);
	/// 文字列が一行に収まらない場合に文末に省略文字「...」を表示するかどうか。true の場合改行しなくなる。初期値は false。
	bool endEllipsis() const;
	void endEllipsis(bool value);
	/// フォーカスを得られるかどうか。
	virtual bool focusable() const;
	/// 現在の文字列やフォント、画像等から最適な大きさを求める。width を固定すると最適な height が求まる。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 表示するアイコン。無い場合はヌルハンドルのアイコンが返る。
	Icon icon() const;
	void icon(HICON value);
	/// '&' を特殊文字として処理しないかどうか。初期値は false。
	bool noPrefix() const;
	void noPrefix(bool value);
	/// パス文字列が一行に収まらない場合にファイル名は表示しつつ途中で省略文字「...」を表示するかどうか。true の場合改行しなくなる。初期値は false。
	bool pathEllipsis() const;
	void pathEllipsis(bool value);
	/// 文字列の配置。初期値は Label::TextAlign::left。
	Label::TextAlign textAlign() const;
	void textAlign(Label::TextAlign value);
	/// 文字列の色。
	Color textColor() const;
	void textColor(const Color& value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	Color _textColor;
};



	}
}