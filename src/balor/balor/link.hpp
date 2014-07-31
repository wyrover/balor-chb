#pragma once

/// include 해두면 라이브러리 링크를 자동적으로 설정해 준다. 전체를 포함하는 all.hpp로 끝남
/// 릴리스 빌드에서 NDEBUG를 정의하지 않은 상황을 beta로 두고 있다.

#if defined(_DEBUG)
#if defined(_DLL)
#pragma comment(lib, "balor_debug.lib")
#else
#pragma comment(lib, "balor_debug_static.lib")
#endif
#elif !defined(NDEBUG)
#if defined(_DLL)
#pragma comment(lib, "balor_beta.lib")
#else
#pragma comment(lib, "balor_beta_static.lib")
#endif
#else
#if defined(_DLL)
#pragma comment(lib, "balor.lib")
#else
#pragma comment(lib, "balor_static.lib")
#endif
#endif