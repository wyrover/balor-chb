#pragma once

#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>

struct HMONITOR__;
struct HWND__;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
	class Point;
	class Rectangle;
	class String;
}


namespace balor {
	namespace gui {



/**
 * ＰＣのモニターを表す。
 * 
 * 仮想画面とはマルチモニタ環境で複数のモニタを一つのモニタとして扱った仮想的な巨大モニタのこと。
 * 仮想画面座標はプライマリモニタの左上を原点とし、プライマリモニタの左にあるモニタでは位置が負の数に、右にあるモニタでは位置は正の数になる。
 */
class Monitor : private NonCopyable {
public:
	typedef ::HMONITOR__* HMONITOR;
	typedef ::HWND__* HWND;

	/// モニタの設置方向。
	struct Orientation {
		enum _enum {
			angle0   = 0, /// 普通に置いている。
			angle90  = 1, /// 時計回りに９０度回転している。
			angle180 = 2, /// 時計回りに１８０度回転している。
			angle270 = 3, /// 時計回りに２７０度回転している。
		};
		BALOR_NAMED_ENUM_MEMBERS(Orientation);
	};


public:
	/// ハンドルから作成する。nullptr の場合はプライマリモニタ。
	explicit Monitor(HMONITOR handle = nullptr);
	Monitor(Monitor&& value);
	Monitor& operator=(Monitor&& value);

public:
	/// ビット深度。
	int bitsPerPixel() const;
	/// 仮想画面上での位置とモニタの解像度。
	Rectangle bounds() const;
	/// デバイス名。
	String deviceName() const;
	/// 指定したコントロールがもっとも大きな面積を占めているモニタを返す。
	static Monitor fromControl(HWND control);
	/// 指定した仮想画面座標にあるモニタを返す。
	static Monitor fromPoint(const Point& point);
	//// 指定した仮想画面上の矩形が最も大きな面積を占めるモニタを返す。
	static Monitor fromRectangle(const Rectangle& rect);
	/// 全てのモニタを列挙する。仮想的なデバイスも列挙する。
	static std::vector<Monitor, std::allocator<Monitor> > monitors();
	/// 全てのモニタが同じビット深度かどうか。（ただしフォーマットは違う可能性がある）
	static bool monitorsHasSameBitsPerPixel();
	/// モニタの設置方向。
	Monitor::Orientation orientation() const;
	/// プライマリモニタかどうか。
	bool primary() const;
	/// プライマリモニタを取得する。
	static Monitor primaryMonitor();
	/// リフレッシュレート（垂直走査周波数）。システムデフォルトの場合は０または１。
	int refreshRate() const;
	static Rectangle virtualMonitorBounds();
	/// モニタの数。monitors().size() とは必ずしも一致しない。
	static int visibleMonitorsCount();
	/// デスクトップの作業領域。bounds() からタスクバー領域を除いたもの。
	Rectangle workingArea() const;

private:
	HMONITOR _handle;
};



	}
}