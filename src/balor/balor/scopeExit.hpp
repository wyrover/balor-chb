#pragma once


namespace balor {


namespace detail {
/// scopeExit で使う関数実行クラス。
template<typename Function>
class ScopeExitFunctionHolder {
public:
	ScopeExitFunctionHolder(const Function& function) : function(function) {
	}
	ScopeExitFunctionHolder(const ScopeExitFunctionHolder& value) : function(value.function) {
		static_assert(false, "RVO (Return Value Optimization) is indispensable!"); // 戻り値最適化されなかったら二回関数が呼ばれてしまうのでエラーにする
	}
	~ScopeExitFunctionHolder() {
		function();
	}

private:
	ScopeExitFunctionHolder& operator=(const ScopeExitFunctionHolder& );

	Function function;
};
} // detail


template<typename Function>
::balor::detail::ScopeExitFunctionHolder<Function> makeScopeExit(const Function& function) {
	return ::balor::detail::ScopeExitFunctionHolder<Function>(function);
}

#if defined BALOR_DETAIL_CAT_STRING
#error BALOR_DETAIL_CAT_STRING macro is already defined
#endif
/// マクロ定数も含めて文字列を連結するマクロ関数
#define BALOR_DETAIL_CAT_STRING(a, b) BALOR_DETAIL_CAT_STRING_IMPL(a, b)
#define BALOR_DETAIL_CAT_STRING_IMPL(a, b) a ## b


/**
 * 関数オブジェクトをデストラクト時に実行するオブジェクトを自動生成するマクロ関数。
 *
 * boost/scope_exit.hpp とだいたい同じ。いわゆる RAII イディオムに使用する。
 * この恐ろしいマクロはプログラムの scopeExit という単語を全て書き換えてしまうので決してヘッダファイルに include してはならない。
 * 実装はコンパイラの戻り値最適化の挙動に依存しており、戻り値最適化されなかった場合を static_assert で検出している。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Debug::enableMemoryLeakCheck();
	try {
		int* buffer = new int[64];
		scopeExit([&] () {
			delete [] buffer;
		});
		throw 0;
	} catch (...) { // ここでメモリ開放されている。
	}
 * </code></pre>
 */
#define scopeExit auto BALOR_DETAIL_CAT_STRING(balor_scopeExit, __COUNTER__) = ::balor::makeScopeExit



}