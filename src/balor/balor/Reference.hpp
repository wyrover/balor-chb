#pragma once

#include <balor/NonCopyable.hpp>


namespace balor {

class Referenceable;



/// Reference 클래스 형에 의존하지 않는 구현 부분
class ReferenceBase {
protected:
	friend Referenceable;

protected:
	ReferenceBase();
	ReferenceBase(const ReferenceBase& value);
	ReferenceBase(Referenceable* pointer);
	~ReferenceBase();

protected:
	void _attach(Referenceable* pointer);
	void _detach();

protected:
	Referenceable* _pointer;
	ReferenceBase* _next;
};



/**
* クラスの一対多の参照ポインタを表す。
*
* Referenceable クラスの派生クラスのポインタを代入することができ、そのポインタのようにふるまう。
* 参照先のデストラクタが呼ばれるとポインタは自動的にヌルになる。
* また参照先がムーブセマンティクスによって移動した時、ポインタも移動先に更新される。
*
* C++ ではクラス同士の参照は shared_ptr を使う事が多いがその為にはクラスをヒープメモリに割り当てする必要がある。
* 右辺値参照のサポートによってコピー禁止クラスを無理やりスマートポインタで管理する必要性が薄れたので
* クラス同士の参照についてもメモリ割り当てをすることなく解決する仕組みを作った。ただし実値なのでライフサイクルの管理はあくまで手動。
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	Frame frame(L"Reference Sample");

	Reference<Button> r;
	assert(!r);
	{
		Button button(frame, 20, 10, 0, 0, L"버튼");

		r = &button;
		assert(r);
		assert(r->text() == L"버튼");

		button = Button(frame, 20, 10, 0, 0, L"버튼2");
		assert(!r); // button은 다시 만들어서 자동적으로 참조도 nullptr 이 된다.

		r = &button;
		assert(r);
		assert(r->text() == L"??깛2");
	}
	assert(!r); // button은 파괴되었으므로 자동적으로 참조도 nullptr이 된다
 * </code></pre>
 */
template<typename T>
class Reference : public ReferenceBase {
public:
	/// 널포인터로 만든다
	Reference() {}
	Reference(const Reference& value) : ReferenceBase(value) {}
	/// 참조처의 포인트로 만든다
	Reference(T* pointer) : ReferenceBase(pointer) {}
	~Reference() {}

	Reference& operator=(const Reference& value) {
		if (this != &value) {
			_detach();
			_attach(value._pointer);
		}
		return *this;
	}
	/// 참조처의 포인터를 변경
	Reference& operator=(T* pointer) {
		_detach();
		_attach(pointer);
		return *this;
	}

public:
	/// 참조처의 포인터 취즉
	operator T*() const { return static_cast<T*>(_pointer); } // 여기서 reinterpret_cast를 사용해서는 안된다. 가상 함수를 가지지 않는 기저에서 가진 파생 클래스로 캐스트가 정상으로 동작하지 않는다
	/// 참조처에 접근
	T* operator->() const { return static_cast<T*>(_pointer); }
};



/**
* Reference クラスで参照できるクラスの基底クラス。
* このクラスを継承するクラスでは必ず右辺値参照コンストラクタと右辺値参照代入演算子を定義して
* このクラスの右辺値参照コンストラクタと右辺値参照代入演算子を呼ぶ必要がある。
* 実際の使い方は Reference クラスのドキュメントを参照。
 */
class Referenceable : private NonCopyable { // 1대다 참조이므로 복사 금지
public:
	friend ReferenceBase;

protected:
	Referenceable();
	/// 파생 클래스에서 호출 필요가 있다
	Referenceable(Referenceable&& value);
	~Referenceable();
	/// 파생 클래스에서 호출 필요가 있다
	Referenceable& operator=(Referenceable&& value);

private:
	ReferenceBase* _first;
};



}