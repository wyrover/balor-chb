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
 * 문자열 포인터와 길이를 가지고 balor::String,balor::StringBuffer, std::wstring과 문자열 포인터로 변환 할 수 있다. 다양한 문자열 표현을 공통의 함수 인터페이스로 구현할 수 있도록 한다.
 * 
 * 이 클래스는 함수의 인수로만 사용하는 것을 상정하고 있으므로 로컬 변수나 클래스 변수로 사용하지 않는다.
 * 예를들면 생성자에 wstring 템플릿 오브젝트를 넘겨서 StringRange 로컬 변수를 만들면
 * StringRange는 기존에 파괴된 메모리 내용을 참고하고 있으므로 접근하면 프로그램은 죽는다.
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	String s(L"abc");

	// String::compareTo는 StringRange를 인수로 취하므로 문자열 포인터, String, StringBuffer, wstring을 돌려 줄수 있다
	s.compareTo(L"abc");
	s.compareTo(String(L"abc"));
	s.compareTo(StringBuffer(L"abc"));
	s.compareTo(std::wstring(L"abc"));
 * </code></pre>
 */
template<typename T>
class BasicStringRange {
public:
	/// 문자열 포인터로 초기화
	BasicStringRange(const T* string) : _c_str(string), _rawLength(-1) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(string != nullptr);
#endif
	}
	/// 문자열 포인터와 문자열 사이즈로 초기화
	/// 사이즈를 이미 알고 있어서 문자열 사이즈를 조사하는 처리를 생략하고 싶은 경우에 이 생성자를 사용한다
	/// StringRange 를 인수로 취하는 많은 함수는 length 위치의 문자열이 L'\0' 인 것을 기대하고 있으므로 부분 문자열을 지정하지 않는다.
	BasicStringRange(const T* string, int length) : _c_str(string), _rawLength(length) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(string != nullptr);
		::balor::test::noMacroAssert(-1 <= length);
#endif
	}
	/// std::basic_string 로 초기화
	template<typename Traits, typename Allocator> BasicStringRange(const std::basic_string<T, Traits, Allocator>& string) : _c_str(string.c_str()), _rawLength(string.length()) {}

public:
	/// 빈 문자열인지 어떤지. 길이가 0 이 아니어도 빈 문자열로 될 수 있음을 주의.
	bool empty() const { return *_c_str == 0; }
	/// 문자열 포인터
	const T* c_str() const { return _c_str; }
	/// 문자열의 길이
	int length() const {
		if (_rawLength == -1) {
			const_cast<BasicStringRange*>(this)->_rawLength = String::getLength(_c_str);
		}
		return _rawLength;
	}
	/// 문자열의 길이 길이가 아직 알수 없는 경우는 -1을 반환한다
	int rawLength() const { return _rawLength; }

private:
	const T* _c_str;
	int _rawLength;
};


/// 바이트 문자열 용
typedef BasicStringRange<char> ByteStringRange;


/// 와이드 문자열 용
typedef BasicStringRange<wchar_t> StringRange;



}