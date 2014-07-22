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
	{ // assetテスト
		testAssertionFailed(const StringRange range(nullptr));
		testNoThrow        (const StringRange range(L""));
		testAssertionFailed(const StringRange range(nullptr, -1));
		testAssertionFailed(const StringRange range(L"", -2));
		testNoThrow        (const StringRange range(L"", -1));
	}
	{ // ポインタ一個バージョン
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(range == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // サイズ指定バージョン
		const wchar_t* string = L"abc";
		const StringRange range(string, 3);
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == 3);
		testAssert(range.length() == 3);
	}
	{ // 配列バージョン
		wchar_t string[5] = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // 文字列リテラルバージョン
		const wchar_t string[] = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string);
		testAssert(range.rawLength() == -1);
		testAssert(range.length() == 3);
	}
	{ // wstringバージョン
		const wstring string = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
	{ // Stringバージョン
		const String string = L"abc";
		const StringRange range = string;
		testAssert(range.c_str() == string.c_str());
		testAssert(range.rawLength() == static_cast<int>(string.length()));
		testAssert(range.length() == static_cast<int>(string.length()));
	}
}


testCase(empty) {
	{ // 空ではないリテラル
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(!range.empty());
	}
	{ // 空リテラル
		const wchar_t* string = L"";
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0で始まるリテラル
		const wchar_t* string = L"\0abc";
		const StringRange range(string);
		testAssert(range.empty());
	}
	{ // \0で始まるリテラルをサイズ指定で初期化
		const wchar_t* string = L"\0abc";
		const StringRange range(string, 4);
		testAssert(range.empty());
	}
	{ // 空wstring
		const wstring string;
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0で始まる空ではないwstring
		const wstring string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // 空String
		const String string;
		const StringRange range = string;
		testAssert(range.empty());
	}
	{ // \0で始まる空ではないString
		const String string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.empty());
	}
}


testCase(length) {
	{ // 空ではないリテラル
		const wchar_t* string = L"abc";
		const StringRange range = string;
		testAssert(range.length() == 3);
	}
	{ // 空リテラル
		const wchar_t* string = L"";
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0で始まるリテラル
		const wchar_t* string = L"\0abc";
		const StringRange range(string);
		testAssert(range.length() == 0);
	}
	{ // \0で始まるリテラルをポインタ二個で初期化
		const wchar_t* string = L"\0abc";
		const StringRange range(string, 4);
		testAssert(range.length() == 4);
	}
	{ // 空wstring
		const wstring string;
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0で始まる空ではないwstring
		const wstring string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.length() == 4);
	}
	{ // 空String
		const String string;
		const StringRange range = string;
		testAssert(range.length() == 0);
	}
	{ // \0で始まる空ではないString
		const String string(L"\0abc", 4);
		const StringRange range = string;
		testAssert(range.length() == 4);
	}
}


//testCase(rawLength) { // constructAndAccess でテスト済み
//}


testCase(functionArgument) {
	testAssert(rangeToString(L"abc") == L"abc");
	testAssert(rangeToString(wstring(L"a") + L"bc") == L"abc");
}



	}
}