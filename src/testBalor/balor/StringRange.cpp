#include <balor/StringRange.hpp>

#include <string>

#include <balor/String.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testStringRange {


using std::wstring;


namespace {
wstring rangeToString(StringRange range) {
	return range.c_str();
}
} // namespace



testCase(constructAndAccess) {
	{ // asset 테스트
		testAssertionFailed(const StringRange range(nullptr));
		testNoThrow        (const StringRange range(L""));
		testAssertionFailed(const StringRange range(nullptr, -1));
		testAssertionFailed(const StringRange range(L"", -2));
		testNoThrow        (const StringRange range(L"", -1));
	}
	{ // 포인터 1개 버전
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(range == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 사이즈 지정 버전
		const wchar_t* string = L"abc";
		const StringRange range(string, 3);
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == 3);
		testAssert(range.length() == 3);
	}
	{ // 배열 버전
		wchar_t string[5] = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 문자열 리터럴 버전
		const wchar_t string[] = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // wstring 버전
		const wstring string = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
	{ // String 버전
		const String string = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
}


testCase(empty) {
	{ // 비지 않은 리터럴
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(!range.empty());
	}
	{ // 빈 리터럴
		const wchar_t* string = L"";
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 리터럴
		const wchar_t* string = L"\0abc";
		const StringRange range(string);
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 리터럴를 사이즈 지정으로 초기화
		const wchar_t* string = L"\0abc";
		const StringRange range(string, 4);
		testAssert(range.empty());
	}
	{ // 빈 wstring
		const wstring string;
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 비지 않은 wstring
		const wstring string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // 빈 String
		const String string;
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 비지 않은 String
		const String string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.empty());
	}
}


testCase(length) {
	{ // 비지 않은 리터럴
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(range.length() == 3);
	}
	{ // 빈 리터럴
		const wchar_t* string = L"";
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0 으로 시작하는 리터럴
		const wchar_t* string = L"\0abc";
		const StringRange range(string);
		testAssert(range.length() == 0);
	}
	{ // \0 으로 시작하는 리터럴를 포인터 2개로 초기화
		const wchar_t* string = L"\0abc";
		const StringRange range(string, 4);
		testAssert(range.length() == 4);
	}
	{ // 빈 wstring
		const wstring string;
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0 으로 시작하는 비지 않은 wstring
		const wstring string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.length() == 4);
	}
	{ // 빈 String
		const String string;
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0 으로 시작하는 미지 않은 String
		const String string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.length() == 4);
	}
}


//testCase(rawLength) { // constructAndAccess 로 데스트 끝
//}


testCase(functionArgument) {
	testAssert(rangeToString(L"abc") == L"abc");
	testAssert(rangeToString(wstring(L"a") + L"bc") == L"abc");
}



	}
}