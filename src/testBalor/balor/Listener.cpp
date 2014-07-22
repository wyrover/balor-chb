#include <balor/Listener.hpp>

#include <string>
#include <utility>

#include <balor/test/InstanceTracer.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testListener {


using std::move;
using std::string;
using namespace balor::test;


namespace {
int callCount = 0;


void function(int) {
	++callCount;
}


struct TracedFunction {
	TracedFunction() {}
	TracedFunction(const TracedFunction& function) : tracer(function.tracer) {}
	TracedFunction(TracedFunction&& function) : tracer(move(function.tracer)) {}

	TracedFunction& operator=(const TracedFunction& function) {
		if (this != &function) {
			tracer = function.tracer;
		}
		return *this;
	}
	TracedFunction& operator=(TracedFunction&& function) {
		if (this != &function) {
			tracer = move(function.tracer);
		}
		return *this;
	}

	void operator()(int) {
		++callCount;
	}

	InstanceTracer tracer;
};


struct BigTracedFunction {
	BigTracedFunction() {}
	BigTracedFunction(const BigTracedFunction& function) : tracer(function.tracer) {}
	BigTracedFunction(BigTracedFunction&& function) : tracer(move(function.tracer)) {}

	BigTracedFunction& operator=(const BigTracedFunction& function) {
		if (this != &function) {
			tracer = function.tracer;
		}
		return *this;
	}
	BigTracedFunction& operator=(BigTracedFunction&& function) {
		if (this != &function) {
			tracer = move(function.tracer);
		}
		return *this;
	}

	void operator()(int) {
		dummy[0] = this;
		++callCount;
	}

	InstanceTracer tracer;
	void* dummy[10];
};


bool checkCallCount(const Listener<int>& listener) {
	if (!listener) {
		return false;
	}
	callCount = 0;
	listener(0);
	return callCount == 1;
}
} // namespace



testCase(nullFunction) {
	{// 空のListener
		Listener<int> empty;
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// copy construct
		Listener<int> empty;
		Listener<int> listener = empty;
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// const copy construct
		const Listener<int> empty;
		Listener<int> listener = empty;
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// move construct
		Listener<int> empty;
		Listener<int> listener = move(empty);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// const move construct
		const Listener<int> empty;
		Listener<int> listener = move(empty);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// 代入
		Listener<int> empty;
		Listener<int> listener = [](int) {};
		testAssert(listener);
		testNoThrow(listener(0));
		Listener<int>& result = listener = empty;
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// const 代入
		const Listener<int> empty;
		Listener<int> listener = [](int) {};
		Listener<int>& result = listener = empty;
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// 右辺値代入
		Listener<int> empty;
		Listener<int> listener = [](int) {};
		Listener<int>& result = listener = move(empty);
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// const 右辺値代入
		const Listener<int> empty;
		Listener<int> listener = [](int) {};
		Listener<int>& result = listener = move(empty);
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
}


testCase(functionPointer) {
	typedef void (*FunctionPointer)(int);

	{// ヌル関数ポインタで初期化
#if !defined(NDEBUG)
		typedef Listener<int> ListenerType;
		testAssertionFailed(ListenerType listener = (FunctionPointer)nullptr);
#else
		testAssert(true); // テスト件数あわせ
#endif
	}
	{// 関数ポインタで初期化
		Listener<int> listener = function;
		testAssert(checkCallCount(listener));
	}
	{// ヌル関数ポインタの代入
		Listener<int> listener;
#if !defined(NDEBUG)
		testAssertionFailed(listener = (FunctionPointer)nullptr);
#else
		testAssert(true); // テスト件数あわせ
#endif
	}
	{// 関数ポインタの代入
		Listener<int> listener;
		listener = function;
		testAssert(checkCallCount(listener));
	}
	{// copy construct
		Listener<int> source = function;
		Listener<int> listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const copy construct
		const Listener<int> source = function;
		Listener<int> listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// move construct
		Listener<int> source = function;
		Listener<int> listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const move construct
		const Listener<int> source = function;
		Listener<int> listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 代入
		Listener<int> source = function;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 代入
		const Listener<int> source = function;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 右辺値代入
		Listener<int> source = function;
		Listener<int> listener;
		listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 右辺値代入
		const Listener<int> source = function;
		Listener<int> listener;
		listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(functionObject) {
	{// 関数オブジェクトで初期化
		TracedFunction tracedFunction;
		{
			InstanceTracer::clearAllCount();
			Listener<int> listener = tracedFunction;
			testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
			testAssert(checkCallCount(listener));
		}
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 1)); // デストラクタ呼び出しもチェック
	}
	{// 関数オブジェクトの右辺値で初期化
		TracedFunction tracedFunction;
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの代入
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの右辺値の代入
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 上書きでデストラクタが呼ばれる事の確認
		Listener<int> listener = TracedFunction();
		InstanceTracer::clearAllCount();
		listener = Listener<int>();
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 1));
		testAssert(!listener);
	}
	{// copy construct
		Listener<int> source = TracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const copy construct
		const Listener<int> source = TracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// move construct
		Listener<int> source = TracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const move construct
		const Listener<int> source = TracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 代入
		Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		InstanceTracer::clearAllCount();
		listener = listener; // 自己代入のチェック
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// const 代入
		const Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		const Listener<int>& constListener = listener;
		InstanceTracer::clearAllCount();
		listener = constListener; // 自己代入のチェック
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// 右辺値代入
		Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		InstanceTracer::clearAllCount();
		listener = move(listener); // 自己代入のチェック
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// const 右辺値代入
		const Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		const Listener<int>& constListener = listener;
		InstanceTracer::clearAllCount();
		listener = move(constListener); // 自己代入のチェック
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
}


testCase(allocatedFunctionObject) {
	// AllocatedFunctionObjectが割り当てられる条件をチェック
	testAssert(sizeof(void*) < sizeof(BigTracedFunction));

	{// 関数オブジェクトで初期化
		BigTracedFunction tracedFunction;
		{
			InstanceTracer::clearAllCount();
			Listener<int> listener = tracedFunction;
			testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
			testAssert(checkCallCount(listener));
		}
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 1)); // デストラクタ呼び出しもチェック
	}
	{// 関数オブジェクトの右辺値で初期化
		BigTracedFunction tracedFunction;
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの代入
		BigTracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの右辺値の代入
		BigTracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 上書きでデストラクタが呼ばれる事の確認
		Listener<int> listener = BigTracedFunction();
		InstanceTracer::clearAllCount();
		listener = Listener<int>();
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 1));
		testAssert(!listener);
	}
	{// copy construct
		Listener<int> source = BigTracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const copy construct
		const Listener<int> source = BigTracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// move construct
		Listener<int> source = BigTracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // ポインタ移動のみ
		testAssert(checkCallCount(listener));
		testAssert(!source);
	}
	{// const move construct
		const Listener<int> source = BigTracedFunction();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 代入
		Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 代入
		const Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 右辺値代入
		Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // ポインタ移動のみ
		testAssert(checkCallCount(listener));
		testAssert(!source);
	}
	{// const 右辺値代入
		const Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(lambdaFunctionObject) {
	auto lambda = [](int) { ++callCount; };
	{// lambdaで初期化
		Listener<int> listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// lambdaの代入
		Listener<int> listener;
		listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// copy construct
		Listener<int> source = lambda;
		Listener<int> listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 代入
		Listener<int> source = lambda;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(bigLambdaFunctionObject) {
	int a = 11;
	int b = 21;
	auto lambda = [&](int) { callCount = callCount + (a * 2) - b; };
	// AllocatedFunctionObjectが割り当てられる条件のチェック
	testAssert(sizeof(void*) < sizeof(lambda));

	{// lambdaで初期化
		Listener<int> listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// lambdaの代入
		Listener<int> listener;
		listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// copy construct
		Listener<int> source = lambda;
		Listener<int> listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 代入
		Listener<int> source = lambda;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(listenerChain) {
	auto dummyFunction = [](int) {};

	{// 関数オブジェクトの追加
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの追加（二番目）
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数オブジェクトの追加（三番目）
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値関数オブジェクトの追加
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値関数オブジェクトの追加（二番目）
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値関数オブジェクトの追加（三番目）
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// イベントの追加
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// イベントの追加（二番目）
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// イベントの追加（三番目）
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値イベントの追加
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値イベントの追加（二番目）
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 右辺値イベントの追加（三番目）
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 関数追加時の処理
		Listener<int> listener = TracedFunction();;
		InstanceTracer::clearAllCount();
		listener += dummyFunction;
		testAssert(InstanceTracer::checkAllCount(0, 0, 2, 2));
		InstanceTracer::clearAllCount();
		listener += dummyFunction;
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 1));
		testAssert(checkCallCount(listener));
	}
	{// 上書きでデストラクタが呼ばれる事の確認
		Listener<int> listener = TracedFunction();
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener = Listener<int>();
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 1));
		testAssert(!listener);
	}

	string result;
	auto functionA = [&] (int) { result += "a"; };
	auto functionB = [&] (int) { result += "b"; };
	auto getTestListener = [&] () -> Listener<int> {
		Listener<int> listener;
		listener += functionA;
		listener += functionB;
		listener += TracedFunction();
		return move(listener);
	};
	auto checkCallChain = [&](const Listener<int>& listener) -> bool {
		result.clear();
		if (!checkCallCount(listener)) {
			return false;
		}
		return result == "ab";
	};

	{// copy construct
		Listener<int> source = getTestListener();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// const copy construct
		const Listener<int> source = getTestListener();
		InstanceTracer::clearAllCount();
		Listener<int> listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// move construct
		Listener<int> source = getTestListener();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // ポインタ移動のみ
		testAssert(checkCallChain(listener));
		testAssert(!source);
	}
	{// const move construct
		const Listener<int> source = getTestListener();
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// 代入
		Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// const 代入
		const Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// 右辺値代入
		Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // ポインタ移動のみ
		testAssert(checkCallChain(listener));
		testAssert(!source);
	}
	{// const 右辺値代入
		const Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
}


testCase(pop) {
	{// 空の Listener
		Listener<int> listener;
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 0);
	}
	{// 関数一個の Listener
		Listener<int> listener;
		listener += [&] (int ) { ++callCount; };
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 0);
	}
	{// 複数関数の Listener
		Listener<int> listener;
		listener += [&] (int ) { ++callCount; };
		listener += [&] (int ) { ++callCount; };
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 1);
	}
	{// 複数関数を空にした Listener
		Listener<int> listener;
		listener += [&] (int ) { ++callCount; };
		listener += [&] (int ) { ++callCount; };
		callCount = 0;
		listener.pop();
		listener.pop();
		listener(0);
		testAssert(callCount == 0);
	}
}



	}
}