#pragma once

// windows.h の include 用。


#ifndef WINVER
#define WINVER 0x0501 // Windows XP 以降
#endif

#ifndef _WIN32_WINNT                   
#define _WIN32_WINNT 0x0501 // Windows XP 以降
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410 // Windows 98 以降
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600 // IE 6.0 以降
#endif

#if !defined(ENABLE_WIN32_LEAN_AND_MEAN)
#if !defined(WIN32_LEAN_AND_MEAN)
#	define WIN32_LEAN_AND_MEAN // windows.h ヘッダーから使用されていない部分を除外する
#endif
#endif
#if !defined(ENABLE_MINMAX)
#if !defined(NOMINMAX)
#	define NOMINMAX // min,maxマクロを無効にする
#endif
#endif
#if !defined(OEMRESOURCE)
#	define OEMRESOURCE // OCRカーソルリソースを有効にする
#endif
#include <windows.h>

#if defined(MessageBox) // クラス名とかぶるので
#undef MessageBox
#endif