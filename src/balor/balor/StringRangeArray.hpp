#pragma once

#include <balor/test/noMacroAssert.hpp>
#include <balor/StringRange.hpp>

namespace std {
	namespace tr1 {
#ifdef  _WIN64
		template<typename T, unsigned __int64> class array;
#else
		template<typename T, __w64 unsigned int> class array;
#endif
	}
	template<typename T> class allocator;
	template<typename T> struct char_traits;
	template<typename T, typename Traits, typename Allocator> class basic_string;
	template<typename T, typename A> class vector;
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
 * 見かけ上 StringRange の配列に見えるクラス。
 * 
 * balor::String または balor::StringBuffer、balor::io::File、std::wstring、文字列ポインタを要素とする固定長配列またはポインター、std::vector、std::array を共通のインターフェースで扱えるようにする。
 * あまり効率的ではないので巨大配列なら負荷に注意する。
 * 文字列と配列まわりのカオスさに挫けそうになるができるだけ単純化していきたい。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	String s(L"abc");

	// String::split は StringRangeArray を引数に取るので 文字列ポインタ、String、wstring 等の固定長配列、vector 等を渡すことができる。
	const wchar_t* pointers[] = {L"abc", L"def"};
	s.split(pointers);

	std::vector<String> strings;
	strings.push_back(L"abc");
	strings.push_back(L"def");
	s.split(strings);

	std::wstring wstrings[] = {L"abc", L"def"};
	s.split(wstrings);
 * </code></pre>
 */
class StringRangeArray {
	typedef ::balor::io::File File;

public:
	/// 文字列ポインタの固定長配列で初期化。
	template<int Size> StringRangeArray(wchar_t* (&array)[Size]) : _begin(array), _length(Size), _accessor(_charPointerAccessor) {}
	template<int Size> StringRangeArray(const wchar_t* (&array)[Size]) : _begin(array), _length(Size), _accessor(_charPointerAccessor) {}
	/// 文字列ポインタのポインタと長さで初期化。
	StringRangeArray(const wchar_t* const * begin, int length) : _begin(begin), _length(length), _accessor(_charPointerAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// 文字列ポインタの vector で初期化。
	template<typename Allocator> StringRangeArray(const std::vector<wchar_t*, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_charPointerAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const wchar_t*, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_charPointerAccessor) {}
	/// 文字列ポインタの array で初期化。
	template<int Size> StringRangeArray(const std::tr1::array<wchar_t*, Size >& array) : _begin(array.data()), _length(Size), _accessor(_charPointerAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const wchar_t*, Size >& array) : _begin(array.data()), _length(Size), _accessor(_charPointerAccessor) {}


	/// File の固定長配列で初期化。
	template<int Size> StringRangeArray(const File (&array)[Size]) : _begin(array), _length(Size), _accessor(_fileAccessor) {}
	/// File のポインタと長さで初期化。
	StringRangeArray(const File* begin, int length) : _begin(begin), _length(length), _accessor(_fileAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// File の vector で初期化。
	template<typename Allocator> StringRangeArray(const std::vector<File, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_fileAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const File, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_fileAccessor) {}
	/// File の array で初期化。
	template<int Size> StringRangeArray(const std::tr1::array<File, Size >& array) : _begin(array.data()), _length(Size), _accessor(_fileAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const File, Size >& array) : _begin(array.data()), _length(Size), _accessor(_fileAccessor) {}


	/// String の固定長配列で初期化。
	template<int Size> StringRangeArray(const String (&array)[Size]) : _begin(array), _length(Size), _accessor(_stringAccessor) {}
	/// String のポインタと長さで初期化。
	StringRangeArray(const String* begin, int length) : _begin(begin), _length(length), _accessor(_stringAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// String の vector で初期化。
	template<typename Allocator> StringRangeArray(const std::vector<String, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const String, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringAccessor) {}
	/// String の array で初期化。
	template<int Size> StringRangeArray(const std::tr1::array<String, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const String, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringAccessor) {}


	/// StringBuffer の固定長配列で初期化。
	template<int Size> StringRangeArray(const StringBuffer (&array)[Size]) : _begin(array), _length(Size), _accessor(_stringBufferAccessor) {}
	/// StringBuffer のポインタと長さで初期化。
	StringRangeArray(const StringBuffer* begin, int length) : _begin(begin), _length(length), _accessor(_stringBufferAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// StringBuffer の vector で初期化。
	template<typename Allocator> StringRangeArray(const std::vector<StringBuffer, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringBufferAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const StringBuffer, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringBufferAccessor) {}
	/// StringBuffer の array で初期化。
	template<int Size> StringRangeArray(const std::tr1::array<StringBuffer, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringBufferAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const StringBuffer, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringBufferAccessor) {}


	/// wstring の固定長配列で初期化。
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::basic_string<wchar_t, Traits, Allocator> (&array)[Size]) : _begin(array), _length(Size), _accessor(_stdStringAccessor) {}
	/// wstring のポインタと長さで初期化。
	template<typename Traits, typename Allocator> StringRangeArray(const std::basic_string<wchar_t, Traits, Allocator>* begin, int length) : _begin(begin), _length(length), _accessor(_stdStringAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// wstring の vector で初期化。
	template<typename Traits, typename Allocator, typename VectorAllocator> StringRangeArray(const std::vector<std::basic_string<wchar_t, Traits, Allocator>, VectorAllocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stdStringAccessor) {}
	template<typename Traits, typename Allocator, typename VectorAllocator> StringRangeArray(const std::vector<const std::basic_string<wchar_t, Traits, VectorAllocator>, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stdStringAccessor) {}
	/// wstring の array で初期化。
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::tr1::array<std::basic_string<wchar_t, Traits, Allocator>, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stdStringAccessor) {}
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::tr1::array<const std::basic_string<wchar_t, Traits, Allocator>, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stdStringAccessor) {}

public:
	/// 配列が空かどうか。
	bool empty() const {
		return !_length;
	}
	/// 配列の長さ。
	int length() const {
		return _length;
	}

public:
	/// 配列要素へのアクセス
	StringRange operator[](int index) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= index);
		::balor::test::noMacroAssert(index < length());
#endif
		return (*_accessor)(_begin, index);
	}

private:
	static StringRange _charPointerAccessor(const void* begin, int index);
	static StringRange _fileAccessor(const void* begin, int index);
	static StringRange _stringAccessor(const void* begin, int index);
	static StringRange _stringBufferAccessor(const void* begin, int index);
	static StringRange _stdStringAccessor(const void* begin, int index);

	const void* _begin;
	StringRange (*_accessor)(const void* , int);
	int _length;
};



}