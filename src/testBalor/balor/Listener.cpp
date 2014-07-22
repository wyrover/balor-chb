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
	{// 빈 Listener
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
	{// 대입
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
	{// const 대입
		const Listener<int> empty;
		Listener<int> listener = [](int) {};
		Listener<int>& result = listener = empty;
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// 우측값대입
		Listener<int> empty;
		Listener<int> listener = [](int) {};
		Listener<int>& result = listener = move(empty);
		testAssert(&result == &listener);
		testAssert(!listener);
		testNoThrow(listener(0));
		testAssert(!empty);
		testNoThrow(empty(0));
	}
	{// const 우측값대입
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

	{// 널 함수 포인터로 초기화
#if !defined(NDEBUG)
		typedef Listener<int> ListenerType;
		testAssertionFailed(ListenerType listener = (FunctionPointer)nullptr);
#else
		testAssert(true); 
#endif
	}
	{// 함수 포인터로 초기화
		Listener<int> listener = function;
		testAssert(checkCallCount(listener));
	}
	{// 널 함수 포인터 대입
		Listener<int> listener;
#if !defined(NDEBUG)
		testAssertionFailed(listener = (FunctionPointer)nullptr);
#else
		testAssert(true); 
#endif
	}
	{// 함수 포인터 대입 
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
	{// 대입
		Listener<int> source = function;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 대입
		const Listener<int> source = function;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 우측값대입
		Listener<int> source = function;
		Listener<int> listener;
		listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 우측값대입
		const Listener<int> source = function;
		Listener<int> listener;
		listener = move(source);
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(functionObject) {
	{// 함수 오브젝트로 초기화
		TracedFunction tracedFunction;
		{
			InstanceTracer::clearAllCount();
			Listener<int> listener = tracedFunction;
			testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
			testAssert(checkCallCount(listener));
		}
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 1)); // 소멸자 호출도 체크
	}
	{// 함수 오브젝트의 우측값으로 초기화
		TracedFunction tracedFunction;
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트 대입
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트 우측값 대입
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 덮어쓰기로 소멸자가 호출 되는 것을 확인 
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
	{// 대입
		Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		InstanceTracer::clearAllCount();
		listener = listener; // 차기 대입 체크
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// const 대입
		const Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		const Listener<int>& constListener = listener;
		InstanceTracer::clearAllCount();
		listener = constListener; // 자기 대입 체크
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// 우측값대입
		Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		InstanceTracer::clearAllCount();
		listener = move(listener); // 자기 대입 체크 
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
	{// const 우측값대입
		const Listener<int> source = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
		const Listener<int>& constListener = listener;
		InstanceTracer::clearAllCount();
		listener = move(constListener); // 자기 대입 체크
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}
}


testCase(allocatedFunctionObject) {
	// AllocatedFunctionObject가 할당 되는 조건을 체크
	testAssert(sizeof(void*) < sizeof(BigTracedFunction));

	{// 함수 오브젝트로 초기화 
		BigTracedFunction tracedFunction;
		{
			InstanceTracer::clearAllCount();
			Listener<int> listener = tracedFunction;
			testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
			testAssert(checkCallCount(listener));
		}
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 1)); // 소멸자가 호출되는지도 체크 
	}
	{// 함수 오브젝트의 우측값으로 초기화
		BigTracedFunction tracedFunction;
		InstanceTracer::clearAllCount();
		Listener<int> listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트 대입
		BigTracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트의 우측값대입 
		BigTracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 덮어쓰기로 소멸자가 불러지는지 확인 
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
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // 포인터 이동만
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
	{// 대입
		Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// const 대입
		const Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
	{// 우측값대입
		Listener<int> source = BigTracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // 포인터 이동만
		testAssert(checkCallCount(listener));
		testAssert(!source);
	}
	{// const 우측값대입
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
	{// lambda로 초기화
		Listener<int> listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// lambda 대입
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
	{// 대입
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
	// AllocatedFunctionObject 할당 되어지는 조건 체크 
	testAssert(sizeof(void*) < sizeof(lambda));

	{// lambda로 초기화
		Listener<int> listener = lambda;
		testAssert(checkCallCount(listener));
	}
	{// lambda 대입
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
	{// 대입
		Listener<int> source = lambda;
		Listener<int> listener;
		listener = source;
		testAssert(checkCallCount(listener));
		testAssert(checkCallCount(source));
	}
}


testCase(listenerChain) {
	auto dummyFunction = [](int) {};

	{// 함수 오브젝트 추가
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트 추가(두 번째)
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 오브젝트 추가(세 번째)
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += tracedFunction;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 함수 오브젝트 추가 
		TracedFunction tracedFunction;
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 함수 오브젝트 추가(두 번째)
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 함수 오브젝트 추가(세 번째)
		TracedFunction tracedFunction;
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(tracedFunction);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 이벤트 추가
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 이벤트 추가(두 번째)
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 이벤트 추가(세 번째)
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += add;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 이벤트 추가
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 이벤트 추가(두 번째)
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 우측값 이벤트 추가(세 번째)
		Listener<int> add = TracedFunction();
		Listener<int> listener;
		listener += dummyFunction;
		listener += dummyFunction;
		InstanceTracer::clearAllCount();
		listener += move(add);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(checkCallCount(listener));
	}
	{// 함수 추가 시 처리
		Listener<int> listener = TracedFunction();;
		InstanceTracer::clearAllCount();
		listener += dummyFunction;
		testAssert(InstanceTracer::checkAllCount(0, 0, 2, 2));
		InstanceTracer::clearAllCount();
		listener += dummyFunction;
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 1));
		testAssert(checkCallCount(listener));
	}
	{// 덮어쓰기로 소멸자가 불러지는 것을 확인
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
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // 포인터만 이동
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
	{// 대입
		Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// const 대입
		const Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = source;
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		testAssert(checkCallChain(listener));
		testAssert(checkCallChain(source));
	}
	{// 우측값대입
		Listener<int> source = getTestListener();
		Listener<int> listener;
		InstanceTracer::clearAllCount();
		listener = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // 포인터만 이동
		testAssert(checkCallChain(listener));
		testAssert(!source);
	}
	{// const 우측값대입
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
	{// 빈 Listener
		Listener<int> listener;
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 0);
	}
	{// 함수 1개의 Listener
		Listener<int> listener;
		listener += [&] (int ) { ++callCount; };
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 0);
	}
	{// 복수 함수의 Listener
		Listener<int> listener;
		listener += [&] (int ) { ++callCount; };
		listener += [&] (int ) { ++callCount; };
		callCount = 0;
		listener.pop();
		listener(0);
		testAssert(callCount == 1);
	}
	{// 복수 함수를 비개 한 Listener
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