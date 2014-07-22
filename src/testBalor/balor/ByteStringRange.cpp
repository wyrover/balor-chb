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
	{ // assetテスト
		testAssertionFailed(const ByteStringRange range(nullptr));
		testNoThrow        (const ByteStringRange range(""));
		testAssertionFailed(const ByteStringRange range(nullptr, -1));
		testAssertionFailed(const ByteStringRange range("", -2));
		testNoThrow        (const ByteStringRange range("", -1));
	}
	{ // ポインタ一個バージョン
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // サイズ指定バージョン
		const char* string = "abc";
		const ByteStringRange range(string, 3);
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == 3);
		testAssert(range.length() == 3);
	}
	{ // 配列バージョン
		char string[5] = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 文字列リテラルバージョン
		const char string[] = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // stringバージョン
		const string string = "abc";
		const ByteStringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
}


testCase(empty) {
	{ // 空ではないリテラル
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(!range.empty());
	}
	{ // 空リテラル
		const char* string = "";
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
	{ // \0で始まるリテラル
		const char* string = "\0abc";
		const ByteStringRange range(string);
		testAssert(range.empty());
	}
	{ // \0で始まるリテラルをサイズ指定で初期化
		const char* string = "\0abc";
		const ByteStringRange range(string, 4);
		testAssert(range.empty());
	}
	{ // 空string
		const string string;
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
	{ // \0で始まる空ではないstring
		const string string("\0abc", 4);
		const ByteStringRange range = string;
		testAssert(range.empty());
	}
}


testCase(length) {
	{ // 空ではないリテラル
		const char* string = "abc";
		const ByteStringRange range = string;
		testAssert(range.length() == 3);
	}
	{ // 空リテラル
		const char* string = "";
		const ByteStringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0で始まるリテラル
		const char* string = "\0abc";
		const ByteStringRange range(string);
		testAssert(range.length() == 0);
	}
	{ // \0で始まるリテラルをサイズ指定で初期化
		const char* string = "\0abc";
		const ByteStringRange range(string, 4);
		testAssert(range.length() == 4);
	}
	{ // 空string
		const string string;
		const ByteStringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0で始まる空ではないstring
		const string string("\0abc", 4);
		const ByteStringRange range = string;
		testAssert(range.length() == 4);
	}
}


//testCase(rawLength) { // constructAndAccess でテスト済み
//}


testCase(functionArgument) {
	testAssert(rangeToString("abc") == "abc");
	testAssert(rangeToString(string("a") + "bc") == "abc");
}



	}
}