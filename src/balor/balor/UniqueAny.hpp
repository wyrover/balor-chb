#pragma once

#include <typeinfo>
#include <utility>

#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>


namespace balor {



/**
 * 봀댰궻?궻뭠귩대입궳궖귡긏깋긚갃
 * 
 * boost::any 궴궬궋궫궋벏궣궬궕갂긓긯?귩뗕?궢궲궋귡궻궳긓긯?궳궖궶궋?귖대입궳궖귡갃
 * 뙸륉궳궼궵귪궶긖귽긛궻?궳궇귣궎궴대입궥귡궴긽긾깏뒆귟뱰궲귩뵼궎갃뜞뚣띍밙돸궻?뭤궼궇귡궔귖궢귢궶궋궕
 * 긏깋긚궻긖귽긛궼륂궸?귽깛?덇뙿빁궳궇귟갂balor::gui::ListBox 귘 balor::gui::ListView 귘 balor::gui::TreeView 뱳궻긏깋긚궼궞귢귩뿕뾭궢궲렳몧궢궲궋귡갃
 *
 * <h3>갋긖깛긵깑긓?긤</h3>
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
#pragma warning(disable : 4521) // '::balor::Any' : 븸릶궻긓긯? 긓깛긚긣깋긏??궕럚믦궠귢궲궋귏궥갃
#pragma warning(disable : 4522) // '::balor::Any' : 븸릶궻대입뎶럁럔궕럚믦궠귢궲궋귏궥갃
class UniqueAny : private NonCopyable {
public:
	/// any_cast 궢궫 Any 궻볙뾢궕뗴궬궯궫귟갂?궕듩댾궯궲궋궫뤾뜃궸뱤궛귞귢귡쀡둖갃
	class BadCastException : public Exception {};

public:
	/// 뗴궻륉뫴궳띿맟갃
	UniqueAny() : _holder(nullptr) {}
	UniqueAny(UniqueAny&& value) : _holder(value._holder) { value._holder = nullptr; }
	/// 봀댰궻?궻뭠궔귞띿맟갃
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
	/// 봀댰궻?궻뭠귩대입갃
	template<typename T>
	UniqueAny& operator=(T&& value) {
		UniqueAny temp(std::forward<T>(value));
		std::swap(_holder, temp._holder);
		return *this;
	}

public:
	/// 긡깛긵깒?긣덙릶궳럚믦궢궫?궳긢긲긅깑긣긓깛긚긣깋긏긣궢궲궩궻랷뤖귩뺅궥갃긓긯?귘댷벍궕믦?궠귢궶궋?뾭갃
	template<typename T>
	T& assign() {
		delete _holder;
		_holder = new ConcreteHolder<T>();
		return static_cast<ConcreteHolder<T>*>(_holder)->content;
	}
	///	뭠궕대입궠귢궲궋귡궔궵궎궔갃
	bool empty() const { return _holder == nullptr; }
	/// 대입궠귢궲궋귡뭠궻?륃뺪갃대입궠귢궲궋궶궋뤾뜃궼 void 궻?륃뺪귩뺅궥갃
	const type_info& type() const { return _holder ? _holder->type() : typeid(void); }

	/// 뭠귩?귩럚믦궢궲롦벦궥귡갃대입궠귢궲궋궶궋뤾뜃귘?궕듩댾궯궲궋귡뤾뜃궼 UniqueAny::BadCastException 귩뱤궛귡갃
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