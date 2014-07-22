#pragma once

#include <new>
#include <type_traits>
#include <utility>

#include <balor/test/noMacroAssert.hpp>


namespace balor {



/**
 * Event を引数に取る返値の無い関数や関数オブジェクトを代入、追加できる関数オブジェクト。
 *
 * std::function<void (Event)> とだいたい同じだが .NET のデリゲートのように operator+= による複数の関数追加をサポートする。
 * operator-= による関数の削除は関数オブジェクトの比較方法が無いため未実装で operator= で上書きするか pop 関数で末尾から削除するしかない。
 * std::function 同様、引数が変換可能であれば引数が違う型の関数も登録できる。（例えば継承関係のある参照型同士など）
 * ラムダ式を代入する場合はポインタ一個分までのキャプチャならばメモリ割り当ては発生しない。
 * 
 * <h3>・関数ポインタの比較に関する問題</h3>
 * 関数ポインタやメンバ関数ポインタの比較をする場合、DLL プロジェクトをまたいで使うと同じ関数でもアドレスが異なる場合がある。
 * SymGetSymFromAddr関数によるシンボル名の取得は、必ず pdb ファイルを添付しなければならないのであまり実用的ではない。
 * この問題は balor をスタティックライブラリではなく DLL にしてしまえば少なくとも balor 内部では解決するが
 * ユーザ関数については依然として関数をDLL内で定義しない限り問題が発生する。
 */
#pragma warning(push)
#pragma warning(disable : 4521) // '::balor::Listener<Sender, Event>' : 複数のコピー コンストラクターが指定されています。
#pragma warning(disable : 4522) // '::balor::Listener<Sender, Event>' : 複数の代入演算子が指定されています。
template<typename Event>
class Listener {
public:
	/// 関数登録無しで初期化。
	Listener() { new (function()) Function(); }
	/// 関数ポインタで初期化。
	template<typename EventType>
	Listener(void (*pointer)(EventType)) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(pointer != nullptr);
#endif
		new (function()) FunctionPointer<EventType>(pointer);
	}
	/// 関数オブジェクトで初期化。
	template<typename T>
	Listener(T&& functionObject) {
#pragma warning(push)
#pragma warning(disable : 4127) // 条件式が定数です。
		if (sizeof(FunctionObject<std::remove_reference<T>::type>) <= sizeof(data)) {
#pragma warning(pop)
			new (function()) FunctionObject<std::remove_reference<T>::type>(std::forward<T>(functionObject));
		} else {
			new (function()) AllocatedFunctionObject<std::remove_reference<T>::type>(std::forward<T>(functionObject));
		}
	}
	Listener(Listener& value) { value.function()->clone(*function()); } // T&&版の呼び出し防止
	Listener(const Listener& value) { value.function()->clone(*function()); }
	Listener(Listener&& value) { value.function()->moveTo(*function()); }
	Listener(const Listener&& value) { value.function()->clone(*function()); } // T&&版の呼び出し防止
	~Listener() { function()->~Function(); }

	/// 関数オブジェクトの代入。
	template<typename T>
	Listener& operator=(T&& functionObject) {
		this->~Listener();
		new (this) Listener(std::forward<T>(functionObject));
		return *this;
	}
	Listener& operator=(Listener& value) { // T&&版の呼び出し防止
		if (&value != this) {
			function()->~Function();
			value.function()->clone(*function());
		}
		return *this;
	}
	Listener& operator=(const Listener& value) {
		if (&value != this) {
			function()->~Function();
			value.function()->clone(*function());
		}
		return *this;
	}
	Listener& operator=(Listener&& value) {
		if (&value != this) {
			function()->~Function();
			value.function()->moveTo(*function());
		}
		return *this;
	}
	Listener& operator=(const Listener&& value) { // T&&版の呼び出し防止
		if (&value != this) {
			function()->~Function();
			value.function()->clone(*function());
		}
		return *this;
	}

public:
	/// 最後に追加した関数を削除する。
	void pop() {
		FunctionType type = function()->type();
		if (type == otherFunctionType) {
			this->~Listener();
			new (this) Listener();
		} else if (type == listenerChainFunctionType) {
			auto chain = static_cast<ListenerChain*>(function());
			auto size = chain->size();
			if (size == 1) {
				this->~Listener();
				new (this) Listener();
			} else {
				ListenerChain newChain(std::move(*chain), size - 1);
				function()->~Function();
				new (function()) ListenerChain(std::move(newChain));
			}
		}
	}

private:
	typedef void (Listener::*SafeBool)();
	void safeBoolTrueValue() {}
public:
	/// 関数が登録されているかどうか。
	operator SafeBool() const { return function()->type() != nullFunctionType ? &Listener::safeBoolTrueValue : nullptr; }
	/// 関数呼び出し。関数が登録されていない場合は何もしない。
	void operator() (Event event) const { (*function())(event); }
	/// 関数の追加。
	template<typename T>
	void operator+=(T&& anyFunction) {
		FunctionType type = function()->type();
		if (type == nullFunctionType) {
			*this = std::forward<T>(anyFunction);
		} else if (type == otherFunctionType) {
			Listener backup = std::move(*this);
			function()->~Function();
			new (function()) ListenerChain(std::move(backup), std::forward<T>(anyFunction));
		} else {
			static_cast<ListenerChain*>(function())->add(std::forward<T>(anyFunction));
		}
	}


private:
	enum FunctionType {
		 nullFunctionType
		,listenerChainFunctionType
		,otherFunctionType
	};

	struct Function {
		virtual ~Function() {}
		virtual void operator() (Event ) {}
		virtual void clone(Function& dest) const { new (&dest) Function(*this); }
		virtual void moveTo(Function& dest)  { new (&dest) Function(*this); }
		virtual FunctionType type() const { return nullFunctionType; }
	};

	template<typename T>
	struct FunctionPointer : public Function {
		explicit FunctionPointer(void (*pointer)(T )) : pointer(pointer) {}
		virtual void operator() (Event event) { (*pointer)(event); }
		virtual void clone(Function& dest) const { new (&dest) FunctionPointer(pointer); }
		virtual void moveTo(Function& dest)  { new (&dest) FunctionPointer(pointer); }
		virtual FunctionType type() const { return otherFunctionType; }
	private:
		void (*pointer)(T);
	};

	template<typename T>
	struct FunctionObject : public Function {
		explicit FunctionObject(const T& object) : object(object) {}
		explicit FunctionObject(T&& object) : object(std::move(object)) {}
		virtual void operator() (Event event) { object(event); }
		virtual void clone(Function& dest) const { new (&dest) FunctionObject(object); }
		virtual void moveTo(Function& dest)  { new (&dest) FunctionObject(std::move(object)); }
		virtual FunctionType type() const { return otherFunctionType; }
	private:
		FunctionObject& operator=(const FunctionObject& );
		T object;
	};

	template<typename T>
	struct AllocatedFunctionObject : public Function {
		explicit AllocatedFunctionObject(const T& object) : pointer(new T(object)) {}
		explicit AllocatedFunctionObject(T&& object) : pointer(new T(std::move(object))) {}
		AllocatedFunctionObject(const AllocatedFunctionObject& object) : pointer(new T(*object.pointer)) {}
		AllocatedFunctionObject(AllocatedFunctionObject&& object) : pointer(object.pointer) {
			object.pointer = nullptr;
			object.~AllocatedFunctionObject();
		}
		virtual ~AllocatedFunctionObject() { delete pointer; pointer = nullptr; }
		virtual void clone(Function& dest) const { new (&dest) AllocatedFunctionObject(*this); }
		virtual void moveTo(Function& dest)  { new (&dest) AllocatedFunctionObject(std::move(*this)); }
		virtual void operator() (Event event) { (*pointer)(event); }
		virtual FunctionType type() const { return otherFunctionType; }
	private:
		T* pointer;
	};

	struct ListenerChain : public Function {
		struct Listeners {
			Listeners() : pointer(nullptr) {}
			~Listeners() {
				delete [] pointer;
				pointer = nullptr;
			}
			Listener* pointer; // vector を使うと ListenerChain が大きすぎてメモリ割り当てが発生する
		};
		template<typename T>
		explicit ListenerChain(Listener&& listener, T&& function) {
			listeners.pointer = new Listener[3];
			listeners.pointer[0] = std::move(listener);
			listeners.pointer[1] = std::forward<T>(function);
		}
		ListenerChain(const ListenerChain& chain) {
			int size = chain.size();
			listeners.pointer = new Listener[size + 1];
			for (int i = 0; i < size; ++i) {
				listeners.pointer[i] = chain.listeners.pointer[i];
			}
		}
		ListenerChain(ListenerChain&& chain) {
			listeners.pointer = chain.listeners.pointer;
			chain.listeners.pointer = nullptr;
			chain.~ListenerChain();
		}
		ListenerChain(ListenerChain&& chain, int size) {
			listeners.pointer = new Listener[size + 1];
			for (int i = 0; i < size; ++i) {
				listeners.pointer[i] = std::move(chain.listeners.pointer[i]);
			}
		}
		virtual void clone(Function& dest) const { new (&dest) ListenerChain(*this); }
		virtual void moveTo(Function& dest)  { new (&dest) ListenerChain(std::move(*this)); }
		virtual void operator() (Event event) {
			for (Listener* i = listeners.pointer; *i; ++i) {
				(*i)(event);
			}
		}
		virtual FunctionType type() const { return listenerChainFunctionType; }
		template<typename T>
		void add(T&& function) {
			Listeners newListeners;
			int oldSize = size();
			newListeners.pointer = new Listener[oldSize + 2];
			for (int i = 0; i < oldSize; ++i) {
				newListeners.pointer[i] = std::move(listeners.pointer[i]);
			}
			newListeners.pointer[oldSize] = std::forward<T>(function);
			Listener* tempPointer = listeners.pointer;
			listeners.pointer = newListeners.pointer;
			newListeners.pointer = tempPointer;
		}
		int size() const {
			int i = 0;
			for (; listeners.pointer[i]; ++i) {} // 末尾の nullFunctionType が番人。
			return i;
		}
		Listeners listeners;
	};

	Function* function() const { return const_cast<Function*>(reinterpret_cast<const Function*>(&data)); }

	typename std::aligned_storage<sizeof(FunctionPointer<Event>), std::alignment_of<FunctionPointer<Event> >::value>::type data;
};
#pragma warning(pop)



}