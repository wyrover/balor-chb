#pragma once

#include <balor/Convert.hpp>
#include <balor/StringRange.hpp>


namespace balor {



/**
  * 문자열 연결, 삽입, 삭제나 교환을 할 수 있는 문자열 버퍼
 * 
 * String이 연결, 삽입, 삭제, 위치 교환 처리를 할 때 새로운 메모리 할당을 하는 것에 비해 StringBuffer는 버퍼 사이즈가 허락하는한 메모리 할당을 하지 않는다.
 * 버퍼에 외부의 고정 길이 배열을 지정할수도 있고 이 경우 고정 길이를 넘는 문자는 쓰여지지 않는다
 * String::refer나 String::literal과 조합하는 것으로 메모리 할당을 완전하게 배제할 수 있다
 *
 * <h3>샘플 코드</h3>
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
	/// 캐파시티 = 1 로 초기화
	StringBuffer();
	StringBuffer(const StringBuffer& value);
	StringBuffer(StringBuffer&& value);
	/// String 의 길이 + 1 의 캐파시티로 문자열을 복사하고 초기화
	explicit StringBuffer(const String& value);
	explicit StringBuffer(String&& value);
	/// 지정한 캐파시티로 초기화
	explicit StringBuffer(int capacity);
	/// 고정 길이 배열을 버퍼로서 초기화 
	template<int Capacity> explicit StringBuffer(wchar_t (&buffer)[Capacity], int length = 0) : _begin(buffer), _length(length), _capacity(Capacity), _allocatable(false) {
#if !defined(NDEBUG)
		::balor::test::noMacroAssert(0 <= length);
		::balor::test::noMacroAssert(length < Capacity);
#endif
		_begin[length] = L'\0';
	}
	/// 메모리 포인터로 초기화
	StringBuffer(wchar_t* buffer, int capacity, int length = 0);
	~StringBuffer();

	StringBuffer& operator=(const StringBuffer& value);
	StringBuffer& operator=(StringBuffer&& value);

public:
	/// 버퍼를 확장할 수 있는지 어떤지
	bool allocatable() const;
	/// 문자열의 선두
	wchar_t* begin() { return _begin; }
	const wchar_t* begin() const { return _begin; }
	/// 끝 문자를 포함한 버퍼 사이즈
	int capacity() const;
	void capacity(int value);
	/// 문자열의 길이를 0으로 한다
	void clear() { length(0); }
	/// 빈 문자열인지 어떤지
	bool empty() const;
	/// 문자열의 끝
	wchar_t* end() { return _begin + _length; }
	const wchar_t* end() const { return _begin + _length; }
	/// 지정한 부분을 삭제한다 
	void erase(int beginIndex);
	void erase(int beginIndex, int length);
	/// end() 에서 끝 문자을 검색하고 새로운 길이를 다시 설정한다
	void expandLength();
	/// 길이를 지정한 분만큼 크게한다. 끝 문자는 쓰지 않는다.
	void expandLength(int delta);
	/// 빒럻쀱귩?볺궥귡갃
	void insert(int beginIndex, wchar_t value, int count);
	void insert(int beginIndex, StringRange value);
	/// 문자열 포인터
	int length() const { return _length; }
	void length(int value);
	/// 지정한 문자나 문자열을 치환한다
	void replace(wchar_t oldValue, wchar_t newValue);
	void replace(StringRange oldValue, StringRange newValue);
	void replace(int beginIndex, int length, StringRange value);
	/// capacity 를 지정한 길이 이상으로 한다
	void reserve(int maxLength);
	/// capacity 를 현재의 길이 + 추가 길이 이상으로 한다
	void reserveAdditionally(int additionalLength);
	/// 선두와 마지막의 trimChars에 포함되는 문자를 빼낸다. trimChars는 빈 문자열의 경우는 whiteSpace로 본다
	void trim(StringRange trimChars = L"");

public:
	/// wstring으로 변환
	operator wstring() const;
	/// StringRange 으로 변환 
	operator StringRange() const { return StringRange(_begin, _length); }
	/// 문자 요소를 돌려준다
	wchar_t& operator[] (int index);
	const wchar_t& operator[] (int index) const;
	/// 문자열의 추가 연산자
	StringBuffer& operator+=(const StringBuffer& rhs);
	StringBuffer& operator+=(const String& rhs);
	StringBuffer& operator+=(const wstring& rhs);
	StringBuffer& operator+=(const wchar_t* rhs);
	StringBuffer& operator+=(wchar_t* rhs);
	StringBuffer& operator+=(wchar_t rhs);
	/// Convert::to<StringBuffer> 이 가능한 형의 += 연산
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