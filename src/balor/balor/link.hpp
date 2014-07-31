#pragma once

/// include �صθ� ���̺귯�� ��ũ�� �ڵ������� ������ �ش�. ��ü�� �����ϴ� all.hpp�� ����
/// ������ ���忡�� NDEBUG�� �������� ���� ��Ȳ�� beta�� �ΰ� �ִ�.

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