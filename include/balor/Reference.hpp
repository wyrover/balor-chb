#pragma once

#include <balor/NonCopyable.hpp>


namespace balor {

class Referenceable;



/// Reference クラスの型に依存しない実装部分。
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
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"Reference Sample");

	Reference<Button> r;
	assert(!r);
	{
		Button button(frame, 20, 10, 0, 0, L"ボタン");

		r = &button;
		assert(r);
		assert(r->text() == L"ボタン");

		button = Button(frame, 20, 10, 0, 0, L"ボタン2");
		assert(!r); // button は作り直されたので自動的に参照も nullptr になる。

		r = &button;
		assert(r);
		assert(r->text() == L"ボタン2");
	}
	assert(!r); // button は破棄されたので自動的に参照も nullptr になる。
 * </code></pre>
 */
template<typename T>
class Reference : public ReferenceBase {
public:
	/// ヌルポインタとして作成。
	Reference() {}
	Reference(const Reference& value) : ReferenceBase(value) {}
	/// 参照先のポインタから作成。
	Reference(T* pointer) : ReferenceBase(pointer) {}
	~Reference() {}

	Reference& operator=(const Reference& value) {
		if (this != &value) {
			_detach();
			_attach(value._pointer);
		}
		return *this;
	}
	/// 参照先のポインタを変更。
	Reference& operator=(T* pointer) {
		_detach();
		_attach(pointer);
		return *this;
	}

public:
	/// 参照先のポインタの取得。
	operator T*() const { return static_cast<T*>(_pointer); } // ここで reinterpret_cast を使ってはならない。仮想関数を持たない基底から持つ派生クラスへのキャストが正常に動かない。
	/// 参照先へのアクセス。
	T* operator->() const { return static_cast<T*>(_pointer); }
};



/**
 * Reference クラスで参照できるクラスの基底クラス。
 * このクラスを継承するクラスでは必ず右辺値参照コンストラクタと右辺値参照代入演算子を定義して
 * このクラスの右辺値参照コンストラクタと右辺値参照代入演算子を呼ぶ必要がある。
 * 実際の使い方は Reference クラスのドキュメントを参照。
 */
class Referenceable : private NonCopyable { // 一対多参照なのでコピー禁止
public:
	friend ReferenceBase;

protected:
	Referenceable();
	/// 派生クラスから呼ぶ必要がある。
	Referenceable(Referenceable&& value);
	~Referenceable();
	/// 派生クラスから呼ぶ必要がある。
	Referenceable& operator=(Referenceable&& value);

private:
	ReferenceBase* _first;
};



}