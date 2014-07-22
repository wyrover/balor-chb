#include <balor/UniqueAny.hpp>

#include <balor/testUniqueAny.hpp> // testBalorDll

#include <typeinfo>
#include <utility>
#include <boost/any.hpp>

#include <balor/system/Module.hpp>
#include <balor/test/InstanceTracer.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/Listener.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace testUniqueAny {

using std::move;
using boost::any_cast;
using namespace balor::system;
using namespace balor::test;

namespace {
struct Noncopyable : public InstanceTracer {
	Noncopyable() {}
	Noncopyable(Noncopyable&& value) : InstanceTracer(move(value)) {}
	Noncopyable& operator=(Noncopyable&& value) {
		InstanceTracer::operator=(move(value));
		return *this;
	}

private:
	Noncopyable(const Noncopyable& ) {}
	Noncopyable& operator=(const Noncopyable& ) {}

	int i;
};

} // namespace



testCase(defaultConstruct) {
	UniqueAny any;
	testAssert(any.empty());
}


testCase(rvalueConstruct) {
	{// 빈 UniqueAny
		UniqueAny source;
		UniqueAny any = move(source);
		testAssert(any.empty());
		testAssert(source.empty());
	}
	{// 값이 있는 UniqueAny
		UniqueAny source = InstanceTracer();
		InstanceTracer::clearAllCount();
		UniqueAny any = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
		testAssert(!any.empty());
		testNoThrow(any_cast<InstanceTracer>(move(any)));
		testAssert(source.empty());
	}
}


testCase(anyTypeConstruct) {
	{// 우측값참조
		InstanceTracer source;
		InstanceTracer::clearAllCount();
		UniqueAny any = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(!any.empty());
		testNoThrow(any_cast<InstanceTracer&>(any));
	}
	{// 복사 금지 오브젝트 대입
		Noncopyable source;
		InstanceTracer::clearAllCount();
		UniqueAny any = move(source);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(!any.empty());
		testNoThrow(any_cast<Noncopyable&>(any));
	}
}


testCase(destruct) {
	{
		UniqueAny any;
	}
	{
		InstanceTracer source = InstanceTracer();
		{
			UniqueAny any = move(source);
			InstanceTracer::clearAllCount();
		}
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 1));
	}
}


testCase(assignmentOperator) {
	{// 우측값참조 대입
		UniqueAny source = InstanceTracer();
		UniqueAny any;
		InstanceTracer::clearAllCount();
		UniqueAny& result = any = move(source);
		testAssert(&result == &any);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0)); // 포인터 이동만
		testAssert(!any.empty());
		testNoThrow(any_cast<InstanceTracer>(move(any)));
		testAssert(source.empty());
		any = InstanceTracer();
		InstanceTracer::clearAllCount();
		any = move(any); // 자기 대입 조사
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
		testAssert(!any.empty());
		testNoThrow(any_cast<InstanceTracer>(move(any)));
	}
	{// 임의 형 우측값대입 
		InstanceTracer source;
		UniqueAny any;
		InstanceTracer::clearAllCount();
		UniqueAny& result = any = move(source);
		testAssert(&result == &any);
		testAssert(InstanceTracer::checkAllCount(0, 0, 1, 0));
		testAssert(!any.empty());
		testNoThrow(any_cast<InstanceTracer>(move(any)));
	}
}


testCase(empty) {
	UniqueAny any;
	testAssert(any.empty());
	any = InstanceTracer();
	testAssert(!any.empty());
	InstanceTracer::clearAllCount();
	any = UniqueAny();
	testAssert(InstanceTracer::checkAllCount(0, 0, 0, 1));
	testAssert(any.empty());
}


testCase(type) {
	UniqueAny any;
	testAssert(any.type() == typeid(void));
	any = InstanceTracer();
	testAssert(any.type() == typeid(InstanceTracer));
	any = int(1);
	testAssert(any.type() == typeid(int));
	any = UniqueAny();
	testAssert(any.type() == typeid(void));
}


#pragma warning(push)
#pragma warning(disable : 4189) // 'temp0' : 로컬 변수가 초기화 되었지만 참조 되지 않았다
testCase(any_castFunction) {
	{
		// 빈 UniqueAny
		UniqueAny any;
		testThrow(any_cast<InstanceTracer>(any), UniqueAny::BadCastException);
		testThrow(any_cast<InstanceTracer>(static_cast<const UniqueAny&>(any)), UniqueAny::BadCastException);
		/// 형(type) 미스 체크
		any = InstanceTracer();
		testThrow(any_cast<int>(any), UniqueAny::BadCastException);
		testThrow(any_cast<int>(static_cast<const UniqueAny&>(any)), UniqueAny::BadCastException);
		// 실제 값
		InstanceTracer::clearAllCount();
		InstanceTracer temp0 = any_cast<InstanceTracer>(any);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		InstanceTracer::clearAllCount();
		InstanceTracer temp1 = any_cast<InstanceTracer>(static_cast<const UniqueAny&>(any));
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		// const 실제 값
		InstanceTracer::clearAllCount();
		InstanceTracer temp2 = any_cast<const InstanceTracer>(any);
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		InstanceTracer::clearAllCount();
		InstanceTracer temp3 = any_cast<const InstanceTracer>(static_cast<const UniqueAny&>(any));
		testAssert(InstanceTracer::checkAllCount(0, 1, 0, 0));
		//  참조
		InstanceTracer::clearAllCount();
		InstanceTracer& temp4 = any_cast<InstanceTracer&>(any);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
		// const 참조
		InstanceTracer::clearAllCount();
		const InstanceTracer& temp5 = any_cast<const InstanceTracer&>(any);
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
		InstanceTracer::clearAllCount();
		const InstanceTracer& temp6 = any_cast<const InstanceTracer&>(static_cast<const UniqueAny&>(any));
		testAssert(InstanceTracer::checkAllCount(0, 0, 0, 0));
	}

	// boost 와 공존
	boost::any boostany;
	boostany = int(5);
	int i = any_cast<int>(boostany);
	testAssert(i == 5);
}
#pragma warning(pop)


testCase(dynamic_castInDll) {
	Module module(L"testBalorDll.dll");
	auto getStringAnyPtr = module.getFunction<decltype(getStringAny)>("getStringAny");
	auto getListenerAnyPtr = module.getFunction<decltype(getListenerAny)>("getListenerAny");
	auto setStringAnyPtr = module.getFunction<decltype(setStringAny)>("setStringAny");
	auto setListenerAnyPtr = module.getFunction<decltype(setListenerAny)>("setListenerAny");

	// DLL에서 만든 UniqueAny를 dynamic_cast
	testAssert(any_cast<String>((*getStringAnyPtr)()) == L"getStringAny");
	String string;
	any_cast<Listener<String&>>((*getListenerAnyPtr)())(string);
	testAssert(string == L"getListenerAny");

	// 여기에서 만든 Dll 중에서 dynamic_cast
	UniqueAny stringAny(String(L"setStringAny"));
	testNoThrow((*setStringAnyPtr)(stringAny));
	UniqueAny listenerAny(Listener<String&>([&] (String& value) { value = L"setListenerAny"; }));
	testNoThrow((*setListenerAnyPtr)(listenerAny));
}



	}
}