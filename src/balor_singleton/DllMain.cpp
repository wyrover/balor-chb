// DllMain.cpp : DLL 애플리케이션 엔트리 포인트 정의
//

// 아래에 정의된 정의 앞에 대상 플랫폼을 정의하지 않으면 안되는 경우 아래 정의 변경하세요
// 서로 다른 플랫폼에 대응하는 값 관한 최신 정보에 대해서는 MSDN을 참조하세요
#ifndef WINVER				
#define WINVER 0x0501		
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501	
#endif						

#ifndef _WIN32_WINDOWS		
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE			
#define _WIN32_IE 0x0600	
#endif

#define WIN32_LEAN_AND_MEAN		

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
