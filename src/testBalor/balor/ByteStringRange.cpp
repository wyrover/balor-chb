#include <balor/StringRange.hpp>

#include <string>

#include <balor/String.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testByteStringRange {


using std::string;


namespace {
string rangeToString(ByteStringRange range) {
	return range.c_str();
}
} // namespace



testCase(constructAndAccess) {
	{ // asset테스트
		testAssertionFailed(const ByteStringRange range(nullptr));
		testNoThrow        (const ByteStringRange range(""));
		testAssertionFailed(const ByteStringRange range(nullptr, -1));
		testAssertionFailed(const ByteStringRange range("", -2));
		testNoThrow        (const ByteStringRange range("", -1));
	}
	{ // 포인터 1개 버전 
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 사이즈 지정 버전 
		const char* string = "abc";
		const ByteStringRange range(string, 3);
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == 3);
		testAssert(range.length() == 3);
	}
	{ // 배열 버전 
		char string[5] = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 문자열 리터럴 버전 
		const char string[] = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // string 버전 
		const string string = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
}


testCase(empty) {
	{ // 비지 않은 리터럴
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(!range.empty());
	}
	{ // 빈 리터럴
		const char* string = "";
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 리터럴
		const char* string = "\0abc";
		const ByteStringRange range(string);
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 리터럴을 사이즈로 초기화
		const char* string = "\0abc";
		const ByteStringRange range(string, 4);
		testAssert(range.empty());
	}
	{ // 빈 string
		const string string;
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
	{ // \0으로 시작하는 비지 않은 string
		const string string("\0abc", 4);
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
}


testCase(length) {
	{ // 비지 않은 리터럴
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(range.length() == 3);
	}
	{ // 빈 리터럴
		const char* string = "";
		const ByteStringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0으로 시작하는 리터럴
		const char* string = "\0abc";
		const ByteStringRange range(string);
		testAssert(range.length() == 0);
	}
	{ // \0으로 시작하는 리터럴을 사이즈로 지정하여 초기화
		const char* string = "\0abc";
		const ByteStringRange range(string, 4);
		testAssert(range.length() == 4);
	}
	{ // 빈 string
		const string string;
		const ByteStringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0으로 시작하는 비지 않은 string
		const string string("\0abc", 4);
		const ByteStringRange range = string;
		testAssert(range.length() == 4);
	}
}


//testCase(rawLength) { // constructAndAccess 테스트 끝
//}


testCase(functionArgument) {
	testAssert(rangeToString("abc") == "abc");
	testAssert(rangeToString(string("a") + "bc") == "abc");
}



	}
}