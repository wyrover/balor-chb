#pragma once

#include <balor/Point.hpp>

struct HWND__;


namespace balor {
	namespace gui {

class Control;



/**
 * ウインドウメッセージ構造体。MSG 構造体のコピー＆拡張。
 */
class Message {
public:
	typedef HWND__* HWND;
#if defined(_WIN64)
	typedef unsigned __int64 UINT_PTR;
	typedef __int64 LONG_PTR;
#else
	typedef __w64 unsigned int UINT_PTR;
	typedef __w64 long LONG_PTR;
#endif
	typedef UINT_PTR WPARAM;
	typedef LONG_PTR LPARAM;
	typedef LONG_PTR LRESULT;

public:
	Message();
	Message(HWND handle, unsigned int message, WPARAM wparam = 0, LPARAM lparam = 0);

public:
	HWND handle;
	unsigned int message;
	WPARAM wparam;
	LPARAM lparam;
	unsigned long time;
	Point point;
	/// ウインドウプロシージャの戻り値。
	LRESULT result;
};



	}
}