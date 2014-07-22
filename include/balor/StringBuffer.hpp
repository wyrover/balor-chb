#pragma once

#include <balor/Convert.hpp>
#include <balor/StringRange.hpp>


namespace balor {



/**
 * 文字列の連結、挿入、削除や置換を行える文字列バッファ。
 * 
 * String が連結、挿入、削除、置換処理を行うたびに新たにメモリ割り当てを行うのに対し、StringBuffer はバッファサイズが許す限りメモリ割り当てを行わない。
 * バッファに外部の固定長配列を指定することもでき、この場合は固定長を超えて文字を書き込めない。
 * String::refer や String::literal と組み合わせることでメモリ割り当てを完全に排除できる。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	wchar_t buffer[256];
	StringBuffer s(buffer);
	s += L"abc";
	s += L"def";
	assert(String::equals(buffer, L"abcdef"));
 * </code></pre>
 */
class StringBuffer {
public:
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > wstring;
	friend String;

public:
	/// キャパシティ = 1 で初期化。
	StringBuffer();
	StringBuffer(const StringBuffer& value);
	StringBuffer(StringBuffer&& value);
	/// String の長さ + 1 のキャパシティで文字列をコピーして初期化。
	explicit StringBuffer(const String& value);
	explicit StringBuffer(String&& value);
	/// 指定したキャパシティで初期化。
	explicit StringBuffer(int capacity);
	/// 固定長配列をバッファとして初期化。
	template<int Capacity> explicit StringBuffer(wchar_t (&buffer)[Capacity], int length = 0) : _begin(buffer), _length(length), _capacity(Capacity), _allocatable(false) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
		::balor::test::noMacroAssert(length < Capacity);
#endif
		_begin[length] = L'\0';
	}
	/// メモリポインタで初期化。
	StringBuffer(wchar_t* buffer, int capacity, int length = 0);
	~StringBuffer();

	StringBuffer& operator=(const StringBuffer& value);
	StringBuffer& operator=(StringBuffer&& value);

public:
	/// バッファを拡張できるかどうか。
	bool allocatable() const;
	/// 文字列の先頭。
	wchar_t* begin() { return _begin; }
	const wchar_t* begin() const { return _begin; }
	/// 終端文字を含んだバッファのサイズ。
	int capacity() const;
	void capacity(int value);
	/// 文字列の長さを０にする。
	void clear() { length(0); }
	/// 空文字列かどうか。
	bool empty() const;
	/// 文字列の終端。
	wchar_t* end() { return _begin + _length; }
	const wchar_t* end() const { return _begin + _length; }
	/// 指定した部分を削除する。
	void erase(int beginIndex);
	void erase(int beginIndex, int length);
	/// end() から終端文字を検索して新しく長さを設定し直す。
	void expandLength();
	/// 長さを指定した分だけ大きくする。終端文字は書き込まない。
	void expandLength(int delta);
	/// 文字列を挿入する。
	void insert(int beginIndex, wchar_t value, int count);
	void insert(int beginIndex, StringRange value);
	/// 文字列の長さ。
	int length() const { return _length; }
	void length(int value);
	/// 指定した文字や文字列を置換する。
	void replace(wchar_t oldValue, wchar_t newValue);
	void replace(StringRange oldValue, StringRange newValue);
	void replace(int beginIndex, int length, StringRange value);
	/// capacity を指定した長さ以上にする。
	void reserve(int maxLength);
	/// capacity を現在の長さ + 追加の長さ以上にする。
	void reserveAdditionally(int additionalLength);
	/// 先頭と末尾の trimChars に含まれる文字を取り除く。trimChars が空文字列の場合は whiteSpace とみなす。
	void trim(StringRange trimChars = L"");

public:
	/// wstring への変換。
	operator wstring() const;
	/// StringRange への変換。
	operator StringRange() const { return StringRange(_begin, _length); }
	/// 文字要素を返す。
	wchar_t& operator[] (int index);
	const wchar_t& operator[] (int index) const;
	/// 文字列の追加演算子。
	StringBuffer& operator+=(const StringBuffer& rhs);
	StringBuffer& operator+=(const String& rhs);
	StringBuffer& operator+=(const wstring& rhs);
	StringBuffer& operator+=(const wchar_t* rhs);
	StringBuffer& operator+=(wchar_t* rhs);
	StringBuffer& operator+=(wchar_t rhs);
	/// Convert::to<StringBuffer> が可能な型の += 演算。
	template<typename T> StringBuffer& operator+=(const T& rhs) {
		Convert::to<StringBuffer>(*this, rhs);
		return *this;
	}

private:
	int _getNewCapacity(int newLength) const;

	wchar_t* _begin;
	int _length;
	int _capacity;
	bool _allocatable;
};



}