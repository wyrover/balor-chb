#pragma once

#include <balor/test/noMacroAssert.hpp>


namespace std {
template<typename T> struct char_traits;
template<typename T> class allocator;
template<typename T, typename Traits, typename Allocator> class basic_string;
}

namespace balor {
	namespace io {
class File;
	}
}


namespace balor {

class String;
class StringBuffer;


/**
 * 文字列のポインタと長さを持ち、balor::String、balor::StringBuffer、std::wstring と文字列ポインタから変換できる。様々な文字列表現を共通の関数インターフェースで実装できるようにする。
 * 
 * このクラスは関数の引数にのみ使う事を想定しているので、ローカル変数やクラス変数で使用しないこと。
 * 例えばコンストラクタに wstring のテンポラリオブジェクトを渡して StringRange のローカル変数を作成すると、
 * StringRange は既に破壊されたメモリ内容を参照しているのでアクセスすればプログラムはクラッシュする。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	String s(L"abc");

	// String::compareTo は StringRange を引数に取るので文字列ポインタ, String, StringBuffer, wstring を渡すことができる。
	s.compareTo(L"abc");
	s.compareTo(String(L"abc"));
	s.compareTo(StringBuffer(L"abc"));
	s.compareTo(std::wstring(L"abc"));
 * </code></pre>
 */
template<typename T>
class BasicStringRange {
public:
	/// 文字列ポインタで初期化。
	BasicStringRange(const T* string) : _c_str(string), _rawLength(-1) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(string != nullptr);
#endif
	}
	/// 文字列ポインタと文字列サイズで初期化。
	/// サイズが既にわかっていて文字列のサイズを調べる処理を省きたい場合にこのコンストラクタを使う。
	/// StringRange を引数に取る多くの関数は length 位置の文字が L'\0' であることを期待しているので部分文字列を指定しないこと。
	BasicStringRange(const T* string, int length) : _c_str(string), _rawLength(length) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(string != nullptr);
		::balor::test::noMacroAssert(-1 <= length);
#endif
	}
	/// std::basic_string で初期化。
	template<typename Traits, typename Allocator> BasicStringRange(const std::basic_string<T, Traits, Allocator>& string) : _c_str(string.c_str()), _rawLength(string.length()) {}

public:
	/// 空文字列かどうか。長さが０ではなくても空文字列になりえることに注意。
	bool empty() const { return *_c_str == 0; }
	/// 文字列ポインタ。
	const T* c_str() const { return _c_str; }
	/// 文字列の長さ。
	int length() const {
		if (_rawLength == -1) {
			const_cast<BasicStringRange*>(this)->_rawLength = String::getLength(_c_str);
		}
		return _rawLength;
	}
	/// 文字列の長さ。長さがまだ判っていないときは -1を返す。
	int rawLength() const { return _rawLength; }

private:
	const T* _c_str;
	int _rawLength;
};


/// バイト文字列用。
typedef BasicStringRange<char> ByteStringRange;


/// ワイド文字列用。
typedef BasicStringRange<wchar_t> StringRange;



}