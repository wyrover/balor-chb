// testBalorDll.cpp : DLL 애플리케이션 용으로 익스포트 되는 함수를 정의한다
//


#pragma once

#if defined(TESTBALORDLL_EXPORTS)
#	define TESTBALORDLL_API __declspec(dllexport)
#else
#	define TESTBALORDLL_API __declspec(dllimport)
#endif