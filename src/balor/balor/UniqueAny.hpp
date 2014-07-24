#pragma once

#include <typeinfo>
#include <utility>

#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>


namespace balor {



/**
 * 다양한 타입의 값을 대입할 수 있는 클래스
 * 
 * boost::any 와 대체로 비슷하지만 복사를 금지하고 있으므로 복사할 수 없는 형을 대입할 수 있다
 * 현재는 어떤 사이즈의 형이라도 대입하면 메모리 할당을 한다. 이후 최적화 여지는 있을 듯
 * 클래스 사이즈는 보통 포인터 1개분이고, balor::gui::ListBox 나 balor::gui::ListView 나 balor::gui::TreeView 등의 클래스는 이것을 이용하여 구현하고 있다
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	UniqueAny any;

	any = int(128);
	int i = any_cast<int>(any);
	assert(i == 128);

	any = String(L"abc");
	String s = any_cast<String>(any);
	assert(s == L"abc");
 * </code></pre>
 */
#pragma warning(push)
#pragma warning(disable : 4521) // '::balor::Any' : 복수의 복사 생성자가 지정되어 있다
#pragma warning(disable : 4522) // '::balor::Any' : 복수의 대입 연산자가 지정되어 있다
class UniqueAny : private NonCopyable {
public:
	/// any_cast 한 Any의 내용이 비거나 형이 틀린 경우에 던져지는 예외
	class BadCastException : public Exception {};

public:
	/// 빈 상태로 만든다
	UniqueAny() : _holder(nullptr) {}
	UniqueAny(UniqueAny&& value) : _holder(value._holder) { value._holder = nullptr; }
	/// 임의의 형의 값에서 만든다
	template<typename T>
	UniqueAny(T&& value) : _holder(new ConcreteHolder<std::remove_const<std::remove_reference<T>::type>::type>(std::forward<T>(value))) {}
	~UniqueAny() {
		delete _holder;
		//_holder = nullptr;
	}

	UniqueAny& operator=(UniqueAny&& value) {
		std::swap(_holder, value._holder);
		return *this;
	}
	/// 임의의 형의 값을 대입
	template<typename T>
	UniqueAny& operator=(T&& value) {
		UniqueAny temp(std::forward<T>(value));
		std::swap(_holder, temp._holder);
		return *this;
	}

public:
	/// 템플릿 인수에서 지정한 형으로 기본 생성자로 하여 그 참조를 돌려준다. 복사나 이동이 정의 되지 않은 형용
	template<typename T>
	T& assign() {
		delete _holder;
		_holder = new ConcreteHolder<T>();
		return static_cast<ConcreteHolder<T>*>(_holder)->content;
	}
	///	값이 대입 되고 있는지 어떤지
	bool empty() const { return _holder == nullptr; }
	/// 대입 되고 있는 값의 형 정보. 대입 되고 있지 않은 경우는 void 형 정보를 반환한다
	const type_info& type() const { return _holder ? _holder->type() : typeid(void); }

	/// 값을 형을 지정하여 얻는다. 대입되고 있지 않은 경우나 형이 틀린 경우는 UniqueAny::BadCastException 예외를 던진다 
	template<typename T>
	friend T any_cast(UniqueAny& any) {
		typedef std::remove_const<std::remove_reference<T>::type>::type PureT;
		ConcreteHolder<PureT>* _holder = dynamic_cast<ConcreteHolder<PureT>*>(any._holder);
		if (!_holder) {
			throw BadCastException();
		}
		return _holder->content;
	}
	template<typename T>
	friend T any_cast(const UniqueAny& any) {
		typedef std::remove_const<std::remove_reference<T>::type>::type PureT;
		return any_cast<const PureT&>(const_cast<UniqueAny&>(any));
	}

private:
	class Holder {
	public:
		virtual ~Holder() {}
		virtual const type_info& type() const = 0;
	};

	template<typename T>
	class ConcreteHolder : public Holder {
	public:
		ConcreteHolder() {}
		ConcreteHolder(T&& value) : content(std::move(value)) {}
		virtual ~ConcreteHolder() {}

		virtual const type_info& type() const { return typeid(T); }

	private:
		ConcreteHolder(const ConcreteHolder& );
		ConcreteHolder& operator=(const ConcreteHolder& );

	public:
		T content;
	};

private:
	Holder* _holder;
};
#pragma warning(pop)



}