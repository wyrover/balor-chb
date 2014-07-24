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
 * StringRange 배열롭 보여주는 클래스
 * 
 * balor::String 또는 balor::StringBuffer, balor::io::File갂std::wstring, 문자열 포인터를 요소로 하는 고정 길이 배열 또는 포인터, std::vector, std::array 를 공통 인터페이스로 갖추고 있다
 * 그다지 효율적이지 않으므로 거대한 배열이라면 부하에 주의
 * 文字列と配列まわりのカオスさに挫けそうになるができるだけ単純化していきたい
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	String s(L"abc");

	// String::split는 StringRangeArray를 인수로 취하므로 String, wstring 등의 고정 길이 배열, vector 등을 반환 할 수 있다.
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
	/// 문자열 포인터의 고정 길이 배열로 초기화
	template<int Size> StringRangeArray(wchar_t* (&array)[Size]) : _begin(array), _length(Size), _accessor(_charPointerAccessor) {}
	template<int Size> StringRangeArray(const wchar_t* (&array)[Size]) : _begin(array), _length(Size), _accessor(_charPointerAccessor) {}
	/// 문자열 포인터의 포인터와 길이로 초기화
	StringRangeArray(const wchar_t* const * begin, int length) : _begin(begin), _length(length), _accessor(_charPointerAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// 문자열 포인터의 vector로 초기화
	template<typename Allocator> StringRangeArray(const std::vector<wchar_t*, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_charPointerAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const wchar_t*, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_charPointerAccessor) {}
	/// 문자열 포인터의 array로 초기화
	template<int Size> StringRangeArray(const std::tr1::array<wchar_t*, Size >& array) : _begin(array.data()), _length(Size), _accessor(_charPointerAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const wchar_t*, Size >& array) : _begin(array.data()), _length(Size), _accessor(_charPointerAccessor) {}


	/// File의 고정 길이 배열로 초기화
	template<int Size> StringRangeArray(const File (&array)[Size]) : _begin(array), _length(Size), _accessor(_fileAccessor) {}
	/// File의 포인터와 길이로 초기화
	StringRangeArray(const File* begin, int length) : _begin(begin), _length(length), _accessor(_fileAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// File의 vector로 초기화
	template<typename Allocator> StringRangeArray(const std::vector<File, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_fileAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const File, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_fileAccessor) {}
	/// File의 array로 초기화
	template<int Size> StringRangeArray(const std::tr1::array<File, Size >& array) : _begin(array.data()), _length(Size), _accessor(_fileAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const File, Size >& array) : _begin(array.data()), _length(Size), _accessor(_fileAccessor) {}


	/// String 의 고정 길이 배열로 초기화
	template<int Size> StringRangeArray(const String (&array)[Size]) : _begin(array), _length(Size), _accessor(_stringAccessor) {}
	/// String 의 포인터와 길이로 초기화
	StringRangeArray(const String* begin, int length) : _begin(begin), _length(length), _accessor(_stringAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// String 의 vector 로 초기화
	template<typename Allocator> StringRangeArray(const std::vector<String, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const String, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringAccessor) {}
	/// String 의 array 로 초기화
	template<int Size> StringRangeArray(const std::tr1::array<String, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const String, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringAccessor) {}


	/// StringBuffer 의 고정 길이 배열로 초기화
	template<int Size> StringRangeArray(const StringBuffer (&array)[Size]) : _begin(array), _length(Size), _accessor(_stringBufferAccessor) {}
	/// StringBuffer 의 포인터와 길이로 초기화
	StringRangeArray(const StringBuffer* begin, int length) : _begin(begin), _length(length), _accessor(_stringBufferAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// StringBuffer 의 vector 로 초기화
	template<typename Allocator> StringRangeArray(const std::vector<StringBuffer, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringBufferAccessor) {}
	template<typename Allocator> StringRangeArray(const std::vector<const StringBuffer, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stringBufferAccessor) {}
	/// StringBuffer 의 array 로 초기화
	template<int Size> StringRangeArray(const std::tr1::array<StringBuffer, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringBufferAccessor) {}
	template<int Size> StringRangeArray(const std::tr1::array<const StringBuffer, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stringBufferAccessor) {}


	/// wstring 의 고정 길이 배열로 초기화
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::basic_string<wchar_t, Traits, Allocator> (&array)[Size]) : _begin(array), _length(Size), _accessor(_stdStringAccessor) {}
	/// wstring 의 포인터와 길이로 초기화
	template<typename Traits, typename Allocator> StringRangeArray(const std::basic_string<wchar_t, Traits, Allocator>* begin, int length) : _begin(begin), _length(length), _accessor(_stdStringAccessor) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
#endif
	}
	/// wstring 의 vector 로 초기화
	template<typename Traits, typename Allocator, typename VectorAllocator> StringRangeArray(const std::vector<std::basic_string<wchar_t, Traits, Allocator>, VectorAllocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stdStringAccessor) {}
	template<typename Traits, typename Allocator, typename VectorAllocator> StringRangeArray(const std::vector<const std::basic_string<wchar_t, Traits, VectorAllocator>, Allocator>& array) : _begin(array.data()), _length(array.size()), _accessor(_stdStringAccessor) {}
	/// wstring 의 array 로 초기화
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::tr1::array<std::basic_string<wchar_t, Traits, Allocator>, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stdStringAccessor) {}
	template<typename Traits, typename Allocator, int Size> StringRangeArray(const std::tr1::array<const std::basic_string<wchar_t, Traits, Allocator>, Size >& array) : _begin(array.data()), _length(Size), _accessor(_stdStringAccessor) {}

public:
	/// 배열이 빈지 어떤지
	bool empty() const {
		return !_length;
	}
	/// 배열의 길이
	int length() const {
		return _length;
	}

public:
	/// 배열 요소로의 접근 
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