#pragma once

#include <typeinfo>
#include <utility>

#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>


namespace balor {



/**
 * ”CˆÓ‚Ì?‚Ì’l‚ð´ëÀÔ‚Å‚«‚éƒNƒ‰ƒXB
 * 
 * boost::any ‚Æ‚¾‚¢‚½‚¢“¯‚¶‚¾‚ªAƒRƒs?‚ð‹Ö?‚µ‚Ä‚¢‚é‚Ì‚ÅƒRƒs?‚Å‚«‚È‚¢?‚à´ëÀÔ‚Å‚«‚éB
 * Œ»ó‚Å‚Í‚Ç‚ñ‚ÈƒTƒCƒY‚Ì?‚Å‚ ‚ë‚¤‚Æ´ëÀÔ‚·‚é‚Æƒƒ‚ƒŠŠ„‚è“–‚Ä‚ð”º‚¤B¡ŒãÅ“K‰»‚Ì?’n‚Í‚ ‚é‚©‚à‚µ‚ê‚È‚¢‚ª
 * ƒNƒ‰ƒX‚ÌƒTƒCƒY‚Íí‚É?ƒCƒ“?ˆêŒÂ•ª‚Å‚ ‚èAbalor::gui::ListBox ‚â balor::gui::ListView ‚â balor::gui::TreeView “™‚ÌƒNƒ‰ƒX‚Í‚±‚ê‚ð—˜—p‚µ‚ÄŽÀ‘•‚µ‚Ä‚¢‚éB
 *
 * <h3>EƒTƒ“ƒvƒ‹ƒR?ƒh</h3>
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
#pragma warning(disable : 4521) // '::balor::Any' : •¡”‚ÌƒRƒs? ƒRƒ“ƒXƒgƒ‰ƒN??‚ªŽw’è‚³‚ê‚Ä‚¢‚Ü‚·B
#pragma warning(disable : 4522) // '::balor::Any' : •¡”‚Ì´ëÀÔ‰‰ŽZŽq‚ªŽw’è‚³‚ê‚Ä‚¢‚Ü‚·B
class UniqueAny : private NonCopyable {
public:
	/// any_cast ‚µ‚½ Any ‚Ì“à—e‚ª‹ó‚¾‚Á‚½‚èA?‚ªŠÔˆá‚Á‚Ä‚¢‚½ê‡‚É“Š‚°‚ç‚ê‚é—áŠOB
	class BadCastException : public Exception {};

public:
	/// ‹ó‚Ìó‘Ô‚Åì¬B
	UniqueAny() : _holder(nullptr) {}
	UniqueAny(UniqueAny&& value) : _holder(value._holder) { value._holder = nullptr; }
	/// ”CˆÓ‚Ì?‚Ì’l‚©‚çì¬B
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
	/// ”CˆÓ‚Ì?‚Ì’l‚ð´ëÀÔB
	template<typename T>
	UniqueAny& operator=(T&& value) {
		UniqueAny temp(std::forward<T>(value));
		std::swap(_holder, temp._holder);
		return *this;
	}

public:
	/// ƒeƒ“ƒvƒŒ?ƒgˆø”‚ÅŽw’è‚µ‚½?‚ÅƒfƒtƒHƒ‹ƒgƒRƒ“ƒXƒgƒ‰ƒNƒg‚µ‚Ä‚»‚ÌŽQÆ‚ð•Ô‚·BƒRƒs?‚âˆÚ“®‚ª’è?‚³‚ê‚È‚¢?—pB
	template<typename T>
	T& assign() {
		delete _holder;
		_holder = new ConcreteHolder<T>();
		return static_cast<ConcreteHolder<T>*>(_holder)->content;
	}
	///	’l‚ª´ëÀÔ‚³‚ê‚Ä‚¢‚é‚©‚Ç‚¤‚©B
	bool empty() const { return _holder == nullptr; }
	/// ´ëÀÔ‚³‚ê‚Ä‚¢‚é’l‚Ì?î•ñB´ëÀÔ‚³‚ê‚Ä‚¢‚È‚¢ê‡‚Í void ‚Ì?î•ñ‚ð•Ô‚·B
	const type_info& type() const { return _holder ? _holder->type() : typeid(void); }

	/// ’l‚ð?‚ðŽw’è‚µ‚ÄŽæ“¾‚·‚éB´ëÀÔ‚³‚ê‚Ä‚¢‚È‚¢ê‡‚â?‚ªŠÔˆá‚Á‚Ä‚¢‚éê‡‚Í UniqueAny::BadCastException ‚ð“Š‚°‚éB
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