#pragma once

#include <vector>

#include <balor/gui/Control.hpp>
#include <balor/gui/Padding.hpp>
#include <balor/Enum.hpp>


namespace balor {
	namespace gui {



/**
 * 子コントロールを親の枠にドッキングさせるレイアウト。
 * 
 * レイアウト処理は自動で行われないので onResized イベント等で手動で更新する必要がある。
 * 子コントロールを親に追加した順（表示優先度）はレイアウトに影響を与える。
 * DockLayout::Style::fill のコントロールは必ず最後に追加されなければならない。
 * 四辺にドッキングする場合は追加した順に内側にむかって配置される。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"DockLayout Sample");

	Button left(frame, 0, 0, 200, 200, L"左側");
	Button top(frame, 0, 0, 200, 200, L"上側");
	Button bottom(frame, 0, 0, 200, 200, L"下側");
	Button right(frame, 0, 0, 200, 200, L"右側");
	Button center(frame, 0, 0, 0, 0, L"中央");

	DockLayout layout(frame);
	layout.setStyle(left, DockLayout::Style::left);
	layout.setStyle(top, DockLayout::Style::top);
	layout.setStyle(bottom, DockLayout::Style::bottom);
	layout.setStyle(right, DockLayout::Style::right);
	layout.setStyle(center, DockLayout::Style::fill);
	layout.perform();
	frame.onResized() = [&] (Frame::Resized& ) {
		layout.perform();
	};

	// フレームを小さくしすぎてコントロールが潰れないようにする。
	layout.setMinimumSize(center, center.getPreferredSize());
	frame.onResizing() = [&] (Frame::Resizing& e) {
		e.minTrackSize(layout.preferredSize());
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class DockLayout : private NonCopyable {
public:
	/// どこにドッキングするか。
	struct Style {
		enum _enum {
			none  , /// 何もしない。
			left  , /// 左にドッキング。
			top   , /// 上部にドッキング。
			right , /// 右にドッキング。
			bottom, /// 下部にドッキング。
			fill  , /// 空いている中心部の空間を占める。
		};
		BALOR_NAMED_ENUM_MEMBERS(Style);
	};

public:
	DockLayout();
	DockLayout(DockLayout&& value);
	DockLayout(Control& target);
	~DockLayout();
	DockLayout& operator=(DockLayout&& value);

public:
	/// 子の最小の大きさ。初期値は Size(0, 0)。
	Size getMinimumSize(Control& child) const;
	/// 子のドッキング位置。初期値は DockLayout::Style::none。
	DockLayout::Style getStyle(Control& child) const;
	/// 親の枠と子コントロールの間の隙間の大きさ。初期値は Padding(0, 0, 0, 0)。
	Padding padding() const;
	void padding(const Padding& value);
	void padding(int left, int top, int right, int bottom);
	/// レイアウトを更新する。
	void perform();
	/// 全ての子コントロールが表示でき、ドッキングする子コントロールとその他のコントロールが重ならない最低限の大きさ。
	Size preferredSize() const;
	/// Scaler を使ってレイアウト設定をスケーリングする。コントロールは別途スケーリングが必要。
	void scale(const Scaler& scaler);
	/// 子の最小の大きさを設定する。
	void setMinimumSize(Control& child, const Size value);
	void setMinimumSize(Control& child, int width, int height);
	/// 子のドッキング位置を設定する。
	void setStyle(Control& child, DockLayout::Style value);
	/// レイアウト対象の親コントロール。
	Control* target();

private:
	struct ChildInfo {
		ChildInfo();
		Reference<Control> child;
		Style style;
		Size minimumSize;
	};

	const ChildInfo* _findInfo(Control& child) const;
	ChildInfo& _getInfo(Control& child);

	Reference<Control> _target;
	Padding _padding;
	std::vector<ChildInfo> _childInfos;
};



	}
}