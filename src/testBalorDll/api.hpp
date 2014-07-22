// testBalorDll.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//


#pragma once

#if defined(TESTBALORDLL_EXPORTS)
#	define TESTBALORDLL_API __declspec(dllexport)
#else
#	define TESTBALORDLL_API __declspec(dllimport)
#endif