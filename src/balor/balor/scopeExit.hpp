#pragma once


namespace balor {


namespace detail {
/// scopeExit 를 사용하는 함수 실행 클래스
template<typename Function>
class ScopeExitFunctionHolder {
public:
	ScopeExitFunctionHolder(const Function& function) : function(function) {
	}
	ScopeExitFunctionHolder(const ScopeExitFunctionHolder& value) : function(value.function) {
		static_assert(false, "RVO (Return Value Optimization) is indispensable!"); // 반환값 최적화 되지 않는다면 이중 함수 호출이되므로 에러가 된다.
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
/// 메크로 정수도 포함하여 문자열을 연결하는 매크로 함수
#define BALOR_DETAIL_CAT_STRING(a, b) BALOR_DETAIL_CAT_STRING_IMPL(a, b)
#define BALOR_DETAIL_CAT_STRING_IMPL(a, b) a ## b


/**
 * 함수 오브젝트를 소멸할 때 실행하는 오브젝트를 자동생성하는 매크로 함수
 *
 * boost/scope_exit.hpp 와 대부분 비슷하다. 이를테면 RAII 이데움을 사용한다.
 * 이 무서운 매크로는 프로그램의 scopeExit 라는 단어를 모두 다시 적어서 절대 헤더 파일에 include 해서는 안된다.
 * 사실 컴파일러 반환값 최적화 행동에 의존하여 반환값 최적화가 되지 않는 경우는 static_assert를 검출하고 있다.
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	Debug::enableMemoryLeakCheck();
	try {
		int* buffer = new int[64];
		scopeExit([&] () {
			delete [] buffer;
		});
		throw 0;
	} catch (...) { // 여기에서 메모리 해제된다
	}
 * </code></pre>
 */
#define scopeExit auto BALOR_DETAIL_CAT_STRING(balor_scopeExit, __COUNTER__) = ::balor::makeScopeExit



}