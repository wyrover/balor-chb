#pragma once

/// include しておけば balor ライブラリのリンクを自動的に設定してくれる。全ての all.hpp で include 済み。
/// リリースビルドで NDEBUG を定義しない状態を beta としている。

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