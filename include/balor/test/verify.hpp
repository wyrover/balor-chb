#pragma once

#include <cassert>



/// デバッグ時は assert だがリリース時も条件式だけは残るマクロ。
///
/// BOOST_VERIFY と同じ。（オリジナルの assert をかませている）
/// verify という単語は重複する可能性が高いので決してライブラリのヘッダファイルで使用してはならない。
#if defined(verify)
#	error ERROR: verify macro is already defined!
#endif

#if defined(NDEBUG)
#	define verify(expression) ((void)(expression))
#else
#	define verify(expression) assert(expression)
#endif