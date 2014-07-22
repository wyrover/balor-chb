// DllMain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

// 下で指定された定義の前に対象プラットフォームを指定しなければならない場合、以下の定義を変更してください。
// 異なるプラットフォームに対応する値に関する最新情報については、MSDN を参照してください。
#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE. の他のバージョン向けに適切な値に変更してください。
#endif

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>


//#ifdef _MANAGED
//#pragma managed(push, off)
//#endif


BOOL APIENTRY DllMain( HMODULE //hModule
                      ,DWORD ul_reason_for_call
                      ,LPVOID //lpReserved
					 ) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH : break;
		case DLL_THREAD_ATTACH  : break;
		case DLL_THREAD_DETACH  : break;
		case DLL_PROCESS_DETACH : break;
	}
    return TRUE;
}


//#ifdef _MANAGED
//#pragma managed(pop)
//#endif
