#pragma once

#include <balor/test/noMacroAssert.hpp>


namespace std {
	namespace tr1 {
#ifdef  _WIN64
		template<typename T, unsigned __int64> class array;
#else
		template<typename T, __w64 unsigned int> class array;
#endif
	}
	template<typename T> class allocator;
	template<typename T, typename A> class vector;
}


namespace balor {



/**
 * 配列の始点と終点を持ち、std::vector、std::array、固定長配列、ポインタから変換できる。様々な配列表現を共通の関数インターフェースで実装できるようにする。
 *
 * このクラスは関数の引数に使う事を想定しているので、ローカル変数やクラス変数での使用はしないこと。
 * 例えばコンストラクタに vector のテンポラリオブジェクトを渡して ArrayRange のローカル変数を作成すると
 * ArrayRange は既に破壊されたメモリ内容を参照しているのでアクセスすればプログラムはクラッシュする。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Bitmap bitmap(100, 100, Bitmap::Format::palette8bpp);

	// ArrayRange 引数に固定長配列を渡す。
	Color staticArray[] = {Color::red(), Color::green(), Color::blue()};
	bitmap.palette(staticArray);

	// ArrayRange 引数に vector を渡す。
	std::vector<Color> stdVector;
	stdVector.push_back(Color::red());
	stdVector.push_back(Color::green());
	stdVector.push_back(Color::blue());
	bitmap.palette(stdVector);

	// ArrayRange 引数にポインターと配列サイズを渡す。
	Color* pointer = staticArray;
	bitmap.palette(ArrayRange<const Color>(pointer, 3));
 * </code></pre>
 */
template<typename T>
class ArrayRange {
private:
	template<typename T> struct removeConst { typedef T type; };
	template<typename T> struct removeConst<const T> { typedef T type; };
	typedef typename removeConst<T>::type PureT;

public:
	/// 固定長配列で初期化。
	template<int Size> ArrayRange(T (&array)[Size]) : _begin(array), _length(Size) {}
	/// ポインタと長さで初期化。
	ArrayRange(T* begin, int length) : _begin(begin), _length(length) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// vector で初期化。
	template<typename Allocator> ArrayRange(std::vector<PureT, Allocator>& array) : _begin(array.data()), _length(array.size()) {}
	/// const vector で初期化。
	template<typename Allocator> ArrayRange(const std::vector<PureT, Allocator>& array) : _begin(array.data()), _length(array.size()) {}
	/// array で初期化。
	template<int Size> ArrayRange(std::tr1::array<PureT, Size>& array) : _begin(array.data()), _length(Size) {}
	/// const array で初期化。
	template<int Size> ArrayRange(const std::tr1::array<PureT, Size >& array) : _begin(array.data()), _length(Size) {}

public:
	T* begin() { return _begin; }
	T* end() { return _begin + _length; }
	const T* begin() const { return _begin; }
	const T* end() const { return _begin + _length; }
	/// 配列サイズが０であるかどうか。
	bool empty() const { return !_length; }
	int length() const { return _length; }
	int size() const { return _length; }

public:
	/// 配列要素へのアクセス
	T& operator[](int index) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= index);
		::balor::test::noMacroAssert(index < length());
#endif
		return _begin[index];
	}

private:
	T* _begin;
	int _length;
};



}