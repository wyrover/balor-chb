#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Convert.hpp>
#include <balor/Enum.hpp>
#include <balor/StringRange.hpp>
#include <balor/StringRangeArray.hpp>

namespace balor {
	namespace locale {
class Charset;
class Locale;
	}
}


namespace balor {

class StringBuffer;



/**
* 문자열 나타낸다.
*
* Java 나 .NET의 String과 비슷. Unicode만을 다루는 문자열 포인터의 내용을 변경하는 것을 허락하지 않는다
* std::wstring과 호환 변환 가능. 단 basic_string을 인수로 취하는 템플릿 함수에는 넘겨야 하는 것이 많으므로 이 경우는 begin, end 함수를 사용한다.
* 함수 인수에 balor::locale::Locale을 넘기지 않는 경우는 보통 서수 처리를 한다.(지역차를 무시하고 문자 코드를 수치로서 비교한다)
* 특히 operator < 등의 연산자 처리에서는 Locale을 지정할 수 없으므로 모두 서수 처리가 되는 것을 주의.
* 서수 처리에서는 전각 알파벳에 대한 ignoreCase는 지원하지 않는다.
* Convert::to<String> 함수로 변환 가능한 형과 String은 + 연산자로 연결할 수 있다.
*
* Unicode 정규화를 위해 isNormalized, normalize 함수는 Vista 이후나 IE7 이후가 설치된 XP나 또는 normaliz.dll을 설치한 XP에서만 지원한다.
*
* Vista 이후라면 ::FindNLSString 함수를 사용할 수 있지만 balor는 XP 이후 지원이므로
* indexOf, lastIndexOf, startsWith 및 endsWith 함수는 자력으로 구현하였다
* API 가 대응하고 있지 않으므로 서로게이트 페어면서 결합문자인 문자 코드라고 판단할 수 없다.
*
* 내용을 변경하지 않는다는 사양을 살리기 위해 refer, literal 함수를 사용하여 문자열 포인터를 참조하는 것만으로 할 수 있지만 포인터 참조의 위험성을 충분히 고려해야 한다.
*
* 문자열 클래스 자체는 바퀴의 재발명이고 가능하면 피하고 싶었지만 
* std::wstring 및 Boost.string algo 에서는 로케일이나 Unicode 관련 처리를 완전하게 무시하고 있으므로 대응한 문자열 클래스를 만들었다.
*
* ※서로게이트 페어, 정규화, 결합문자는 Unicode 사양
 *
 * <h3>샘플 코드</h3>
 * <pre><code>
	String s(L"abc");

	// std::wstring과 상호호환
	std::wstring ws = s;
	s = ws;

	// 로케일을 고려한 비교
	bool result = String::compare(s, L"가나다", Locale::current());

	// 문자열 끼리 또는 수치와 연결
	s = s + L"def";
	s = String(L"(") + 128 + L", " + 256 + L")";
 * </code></pre>
 */
class String {
public:
	typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> > string;
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > wstring;
	typedef ::balor::locale::Locale Locale;
#ifdef  _WIN64
	typedef unsigned __int64 SizeType;
#else
	typedef __w64 unsigned int SizeType;
#endif

	friend StringBuffer;

	/// 문자열 비교 옵션. 조합하여 지정한다.
	/// 문자열을 검색하는 함수(indexOf, lastIndexOf, startsWith, endsWith등) ignoreNonSpace, ignoreSymbols은 지정할 수 없다.
	struct CompareOptions {
		enum _enum {
			none           = 0         , 
			ignoreCase     = 0x00000001, /// 대문자와 소문자를 구별하지 않는다
			ignoreNonSpace = 0x00000002, /// 장소를 취하지 않는 결합문자를 무시한다. 무시한 결과가 옳은지, 합성한 결과가 옳은지 경우를 평가한다
			ignoreSymbols  = 0x00000004, /// 기호를 무시한다.
			ignoreKanaType = 0x00010000, /// 히라가나 가타가나를 구별하지 않는다
			ignoreWidth    = 0x00020000, /// 전각과 반각을 구별하지 않는다
			stringSort     = 0x00001000, /// 「-」 나 「'」를 특별 취급하지 않는다.지정하지 않은 경우는 이것들을 없는 것으로 비교한다.
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(CompareOptions);
	};

	/// Unicode 의 정규화 형식
	struct NormalizationForm {
		enum _enum {
			c  = 0x1, /// 정규화 형식 C
			d  = 0x2, /// 정규화 형식 D
			kc = 0x5, /// 정규화 형식 KC
			kd = 0x6, /// 정규화 형식 KD
		};
		BALOR_NAMED_ENUM_MEMBERS(NormalizationForm);
	};

	/// normaliz.dll 이 발견되지 않은 경우 예외가 던져진다
	class NormalizeDllNotFoundException : public Exception {};

	/// 빠진 서로게이트 등의 정규화 불가능한 문자를 정규화 하려고 할 때 예외 발생
	class NormalizeFailedException : public Exception {};

public:
	/// 빈 문자열 L""로 초기화
	String();
	String(const String& value);
	String(String&& value);
	/// StringBuffer에서 변환 
	String(const StringBuffer& value);
	String(StringBuffer&& value);
	/// std::wstring 로 초기화
	String(const wstring& value);
	/// 문자열 포인터로 초기화
	String(const wchar_t* value);
	/// 캐릭터와 반복 수로 초기화
	String(wchar_t c, int count);
	/// 문자열 포인터와 문자열의 길이로 초기화
	String(const wchar_t* value, int length);
	/// 바이트 문자열을 캐릭터셋으로 디코드하고 초기화 
	String(ByteStringRange value, const ::balor::locale::Charset& charset);
	~String();

	String& operator=(const String& value);
	String& operator=(String&& value);
	/// StringBuffer를 대입
	String& operator=(const StringBuffer& value);
	String& operator=(StringBuffer&& value);
	/// std::wstring 를 대입
	String& operator=(const wstring& value);
	/// 문자열 포인터를 대입
	String& operator=(const wchar_t* value);

public:
	/// 문자열로의 포인터
	const wchar_t* begin() const { return _c_str; }
	/// 문자열로의 포인터
	const wchar_t* c_str() const { return _c_str; }
	/// 문자열 비교. 반환 값은 C 언어의 strcmp 계 함수와 같다
	static int compare(StringRange lhs, StringRange rhs, bool ignoreCase = false);
	static int compare(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	static int compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, bool ignoreCase = false);
	static int compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	/// 문자열 비교. 반환 값은 C 언어의 strcmp 계 함수와 같다
	int compareTo(StringRange rhs) const;
	/// 문자열 내에서 value 를 찾았는지
	bool contains(StringRange value) const;
	/// 문자배열에 null 종단을 포함하는 문자열 전체를 복사한다.
	template<int Size> void copyTo(wchar_t (&array)[Size]) const { copyTo(array, Size); }
	void copyTo(wchar_t* array, int arraySize) const;
	/// 문자 배열에 포함 문자열을 복사한다
	template<int Size> void copyTo(int beginIndex, wchar_t (&array)[Size], int arrayIndex, int length) const { copyTo(beginIndex, array, Size, arrayIndex, length); }
	void copyTo(int beginIndex, wchar_t* array, int arraySize, int arrayIndex, int length) const;
	/// 빈 문자열인지. 길이가 0이 아니라도 빈 문자열에 보일 수 있으므로 주의
	bool empty() const;
	/// 문자열 종단의 포인터
	const wchar_t* end() const { return _c_str + length(); }
	/// 지정한 문자열로 끝나는지
	bool endsWith(StringRange value, bool ignoreCase = false) const;
	bool endsWith(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 문자열이 같은지 
	static bool equals(ByteStringRange lhs, ByteStringRange rhs, bool ignoreCase = false);
	static bool equals(StringRange lhs, StringRange rhs, bool ignoreCase = false);
	static bool equals(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	/// 문자열이 같은지
	bool equalTo(StringRange rhs, bool ignoreCase = false) const;
	bool equalTo(StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 지정한 부분을 제거한 문자열을 반환
	String erase(int beginIndex) const;
	String erase(int beginIndex, int length) const;
	/// 바이트 문자열 포인터의 문자열 길이를 반환
	static int getLength(const char* value);
	/// 문자열 포인터의 문자열 길이를 반환
	static int getLength(const wchar_t* value);
	/// 결합 문자나 서로게이트 페어를 고려한 beginIndex 의 다음 문자 인덱스를 반환
	int getNextLetterIndex(int beginIndex = 0) const;
	/// 문자열 해시 값
	SizeType hashCode() const;
	/// 지정한 문자나 문자열이 처음에 발견된 위치 반환
	int indexOf(StringRange value, bool ignoreCase = false) const;
	int indexOf(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int indexOf(wchar_t value, int beginIndex = 0) const;
	int indexOf(StringRange value, int beginIndex, bool ignoreCase = false) const;
	int indexOf(StringRange value, int beginIndex, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int indexOf(wchar_t value, int beginIndex, int length) const;
	int indexOf(StringRange value, int beginIndex, int length, bool ignoreCase = false) const;
	int indexOf(StringRange value, int beginIndex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 지정한 문자열 중의 문자의 어딘가가 처음 발견된 위치를 반환
	int indexOfAny(StringRange anyOf, int beginIndex = 0) const;
	int indexOfAny(StringRange anyOf, int beginIndex, int length) const;
	/// 문자열 삽입하고 반환
	String insert(int beginIndex, StringRange value) const;
	/// 지정한 정규화 형식으로 정규화 되어 있는지
	bool isNormalized(String::NormalizationForm frame = NormalizationForm::c) const;
	/// 빈 문자열만 포함되었는지
	bool isWhiteSpace() const;
	/// 복수의 문자열을 separator로 결합하여 반환.
	static String join(StringRange separator, StringRangeArray values);
	/// 지정한 문자나 문자열이 최후에 발견된 위치를 반환 
	int lastIndexOf(StringRange value, bool ignoreCase = false) const;
	int lastIndexOf(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int lastIndexOf(wchar_t value, int beginIndex = 0) const;
	int lastIndexOf(StringRange value, int beginIndex, bool ignoreCase = false) const;
	int lastIndexOf(StringRange  value, int beginIndex, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int lastIndexOf(wchar_t value, int beginIndex, int length) const;
	int lastIndexOf(StringRange value, int beginIndex, int length, bool ignoreCase = false) const;
	int lastIndexOf(StringRange  value, int beginIndex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 지정한 문자열 중의 문자 어딘가가 마지막에 발견된 위치 반환
	int lastIndexOfAny(StringRange anyOf, int beginIndex = 0) const;
	int lastIndexOfAny(StringRange anyOf, int beginIndex, int length) const;
	/// 문자열의 길이
	int length() const;
	/// 결합 문자나 서로게이트 페어를 고려하여 표시되는 문자열의 길이
	int lengthInLetter() const;
	/// 문자열 리터럴을 참조하는 String을 만든다
	template<int Size> static String literal(const wchar_t (&string)[Size]) { return String(1 - Size, string); }
	/// 지정한 정규화 형식으로 정규화를 반환
	String normalize(String::NormalizationForm frame = NormalizationForm::c) const;
	/// 지정한 길이가 될때까지 선두에 문자를 박아서 반환
	String padLeft(int length, wchar_t paddingChar = L' ') const;
	/// 지정한 길이가 될때까지 마지막에 문자를 박아서 반환
	String padRight(int length, wchar_t paddingChar = L' ') const;
	/// 문자열 포인터를 고려하는 String을 만든다. String은 내용 변경을 허락하지 않지만 참조처가 먼저 파괴되면 위험 
	static String refer(StringRange value);
	static String refer(const wchar_t* value, int length);
	/// 문자열 포인터을 참조하고 있는지
	bool referred() const { return _length <= 0; }
	/// 지정한 문자나 문자열 치환해서 반환
	String replace(wchar_t oldValue, wchar_t newValue) const;
	String replace(StringRange oldValue, StringRange newValue) const;
	/// 지정한 복수의 구별 문자로 분활한 문자열 배열을 반환. separators가 빈 경우는 구별 문자를 whiteSpace로 본다
	std::vector<String, std::allocator<String> > split(StringRange separators = L"") const;
	/// 지정한 복수의 구별 문자열에서 분할한 문자열 배열을 반환.
	std::vector<String, std::allocator<String> > split(StringRangeArray separators) const;
	/// 지정한 문자열로 시작하는지
	bool startsWith(StringRange value, bool ignoreCase = false) const;
	bool startsWith(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 부분 문자열을 반환
	String substring(int beginIndex) const;
	String substring(int beginIndex, int length) const;
	/// 문자열을 소문자로 반환
	String toLower() const;
	String toLower(const Locale& locale) const;
	/// 소문자를 대문자로 해서 반환
	String toUpper() const;
	String toUpper(const Locale& locale) const;
	/// 선두와 마지막의 trimChars에 포함되는 문자을 모두 제거하고 반환. trimChars가 빈 문자열인 경우는 whiteSpace로 본다.
	String trim(StringRange trimChars = String()) const;

public:
	/// wstring 으로 변환
	operator wstring() const;
	/// StringRange 으로 변환
	operator StringRange() const { return StringRange(_c_str, length()); }
	/// 문자 요소 반환
	wchar_t operator[] (int index) const;
	/// 문자열 추가
	String& operator+=(const String& rhs);
	String& operator+=(const wstring& rhs);
	String& operator+=(const wchar_t* rhs);
	String& operator+=(wchar_t* rhs);
	String& operator+=(wchar_t rhs);
	/// Convert::to<String> 이 가능한 형의 += 연산.
	template<typename T> String& operator+=(const T& rhs) { return *this += Convert::to<String>(rhs); }
	/// 문자열 가산.
	friend String operator+(const String& lhs, const String& rhs);
	friend String operator+(const String& lhs, const wstring& rhs);
	friend String operator+(const String& lhs, const wchar_t* rhs);
	friend String operator+(const String& lhs, wchar_t* rhs);
	friend String operator+(const String& lhs, wchar_t rhs);
	friend String operator+(const wstring& lhs, const String& rhs);
	friend String operator+(const wchar_t* lhs, const String& rhs);
	friend String operator+(wchar_t* lhs, const String& rhs);
	friend String operator+(wchar_t lhs, const String& rhs);
	/// Convert::to<String> 이 가능한 형과의 + 연산.
	template<typename T> friend String operator+(const String& lhs, const T& rhs) { return lhs + Convert::to<String>(rhs); }
	template<typename T> friend String operator+(const T& lhs, const String& rhs) { return Convert::to<String>(lhs) + rhs; }
	/// 비교 연산 
	friend bool operator==(StringRange lhs, StringRange rhs);
	friend bool operator!=(StringRange lhs, StringRange rhs);
	friend bool operator< (StringRange lhs, StringRange rhs);
	friend bool operator<=(StringRange lhs, StringRange rhs);
	friend bool operator> (StringRange lhs, StringRange rhs);
	friend bool operator>=(StringRange lhs, StringRange rhs);

private:
	String(int length, const wchar_t* value);

	const wchar_t* _c_str;
	int _length;
};



}


namespace stdext {
	inline ::balor::String::SizeType hash_value(const ::balor::String& string) {
		return string.hashCode();
	}
}