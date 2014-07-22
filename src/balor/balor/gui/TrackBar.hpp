#pragma once

#include <balor/gui/Control.hpp>
#include <balor/Enum.hpp>


namespace balor {
	namespace gui {

class ToolTip;



/**
 * トラックバー。
 * 
 * ValueChange は pageSize, minimum, maximum の変更で value が変化した場合には発生しない。
 * minimum を maximum より大きくしても maximum = minimum になるだけで増加量の符号は変わらない。
 * 以下は CTRL キーを押しながらドラッグで範囲選択を行うコードの例。
 * <pre><code>
 *	trackBar.thick(true);
 *	int start = 0;
 *	trackBar.onMouseDown() = [&] (Control::MouseDown& e) {
 *		if (e.lButton() && e.ctrl()) {
 *			start = trackBar.value();
 *			trackBar.selectionBegin(start);
 *			trackBar.selectionEnd(start);
 *		}
 *	};
 *	trackBar.onMouseMove() = [&] (Control::MouseMove& e) {
 *		if (e.lButton() && e.ctrl()) {
 *			auto value = trackBar.value();
 *			if (value <= start) {
 *				trackBar.selectionBegin(value);
 *				trackBar.selectionEnd(start);
 *			} else {
 *				trackBar.selectionBegin(start);
 *				trackBar.selectionEnd(value);
 *			}
 *		}
 *	};
 * </code></pre>
 *
 * TBM_SETTOOLTIPS については設定したツールチップが表示されなかった。要調査。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"TrackBar Sample");

	TrackBar track(frame, 20, 10, 0, 0, 0, 100, TrackBar::Options::valueTip);
	Edit edit(frame, track.bounds().right() + 10, 10, 0, 0, 4);
	edit.readOnly(true);
	edit.text(String() + track.value());
	track.onValueChange() = [&] (TrackBar::ValueChange& e) {
		edit.text(String() + e.newValue());
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class TrackBar : public Control {
public:
	/// コントロール作成後に変更できない設定。組み合わせで指定する。
	struct Options {
		enum _enum {
			none     = 0      ,
			valueTip = 0x0100L, /// スライダーを移動中に値をツールチップで表示する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// 目盛りをどこに表示するか。スライダーは目盛りのある方向が尖る。
	struct TickStyle {
		enum _enum {
			left   = 0x0004, /// 左に目盛りを表示する。※縦表示の場合。
			top    = 0x0004, /// 上に目盛りを表示する。※横表示の場合。
			right  = 0x0000, /// 右に目盛りを表示する。※縦表示の場合。
			bottom = 0x0000, /// 下に目盛りを表示する。※横表示の場合。
			both   = 0x0008, /// 上下または左右に目盛りを表示する。スライダーは長方形になる。
		};
		BALOR_NAMED_ENUM_MEMBERS(TickStyle);
	};

	/// スライダー移動中に表示するツールチップの位置。
	struct ValueTipSide {
		enum _enum {
			top    = 0, /// 上に表示する。※横表示の場合。
			left   = 1, /// 左に表示する。※縦表示の場合。
			bottom = 2, /// 下に表示する。※横表示の場合。
			right  = 3, /// 右に表示する。※縦表示の場合。
		};
		BALOR_NAMED_ENUM_MEMBERS(ValueTipSide);
	};


	/// トラックバーのイベントの親クラス。
	typedef EventWithSubclassSender<TrackBar, Control::Event> Event;


	/// value が変化したイベント。
	struct ValueChange : public Event {
		ValueChange(TrackBar& sender, int oldValue, int newValue, bool tracking);
		/// 新しい value。
		int newValue() const;
		/// 古い value。
		int oldValue() const;
		/// スライダーをトラッキング中かどうか。
		bool tracking() const;

	private:
		int _newValue;
		int _oldValue;
		bool _tracking;
	};


public:
	/// ヌルハンドルで作成。
	TrackBar();
	TrackBar(TrackBar&& value, bool checkSlicing = true);
	/// 親コントロール、位置、大きさ、最小位置、最大位置から作成。大きさを０にすると getPreferredSize 関数で求める。その他の引数は同名の関数を参照。
	TrackBar(Control& parent, int x, int y, int width, int height, int minimum = 0, int maximum = 100, TrackBar::Options options = Options::none);
	virtual ~TrackBar();
	TrackBar& operator=(TrackBar&& value);

public:
	/// 選択範囲をクリアする。
	void clearSelection();
	/// 目盛りを初期化する。最初と最後の目盛りだけ残る。
	void clearTick();
	/// 下矢印キーを押した時に左に移動するかどうか。初期値は false。
	bool downIsLeft() const;
	void downIsLeft(bool value);
	void endSelect();
	/// 現在の状態から最適な大きさを求める。
	virtual Size getPreferredSize(int width = 0, int height = 0) const;
	/// 矢印キーを押したときまたはマウスホイールのスライダーの移動量。初期値は１。
	int lineSize() const;
	void lineSize(int value);
	/// スライダー移動できる最大値。
	int maximum() const;
	void maximum(int value);
	/// スライダー移動できる最小値。
	int minimum() const;
	void minimum(int value);
	/// value が変化したイベントのリスナー。
	Listener<ValueChange&>& onValueChange();
	///	コントロール作成後に変更できない設定。
	TrackBar::Options options() const;
	/// ページ移動の大きさ。（スライダーの無い所をクリックしたり、PageUp、PageDown キーを押した時）
	int pageSize() const;
	void pageSize(int value);
	/// Scaler を使ってコントロールをスケーリングする。
	virtual void scale(const Scaler& scaler);
	/// 範囲選択の開始点。thick() が true である場合のみ有効。
	int selectionBegin() const;
	void selectionBegin(int value);
	/// 範囲選択の終了点。thick() が true である場合のみ有効。
	int selectionEnd() const;
	void selectionEnd(int value);
	/// 指定した値の位置に目盛りを追加する。
	void setTick(int value);
	/// スライダーの移動範囲の線が太いかどうか。または範囲選択を有効にするかどうか。初期値は false。
	bool thick() const;
	void thick(bool value);
	/// つまみの大きさ。目盛りと交差する方向のピクセルサイズ。tickStyle() が TrackBar::TickStyle::both の場合のみ有効。
	int thumbSize() const;
	void thumbSize(int value);
	/// つまみを表示するかどうか。初期値は true。
	bool thumbVisible() const;
	void thumbVisible(bool value);
	/// 目盛りの数。
	int tickCount() const;
	/// 目盛りの表示間隔。
	void tickFrequency(int value);
	/// 目盛りをどこに表示するか。スライダーの形にも影響する。初期値は TrackBar::TickStyle::both。
	TrackBar::TickStyle tickStyle() const;
	void tickStyle(TrackBar::TickStyle value);
	/// 目盛りを表示するかどうか。初期値は false。
	bool tickVisible() const;
	void tickVisible(bool value);
	/// 現在のスライダー位置の値。
	int value() const;
	void value(int value);
	/// スライダーを移動中に値を表示するツールチップコントロールの参照。options() に TrackBar::Options::valueTip を設定している場合のみ有効。
	/// 必要な設定を行ったら破棄しても良いが一部設定やイベントは破棄すると無効になる。ある程度 TrackBar が制御しているので動作しない設定もある。
	ToolTip valueTip();
	/// valueTip が表示される位置。
	void valueTipSide(TrackBar::ValueTipSide value);
	/// 垂直トラックバーかどうか。
	bool vertical() const;
	void vertical(bool value);

protected:
	/// メッセージを処理する。いわゆるウインドウプロシージャ。
	virtual void processMessage(Message& msg);

protected:
	int _value;
	Listener<ValueChange&> _onValueChange;
};



	}
}