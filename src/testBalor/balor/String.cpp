#include <balor/String.hpp>

#include <string>
#include <vector>
#pragma warning(push, 3)
#include <boost/assign.hpp>
#pragma warning(pop)

#include <balor/system/windows.hpp>

#include <balor/locale/Charset.hpp>
#include <balor/locale/Locale.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testString {


using std::move;
using std::string;
using std::vector;
using std::wstring;
using namespace boost::assign;
using namespace balor::locale;



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	Locale::locales(); // EnumSystemLocalesW 関数は最初の呼び出しでだけハンドルを複数開いて保持する
}


testCase(defaultConstruct) {
	String string;
	testAssert(string.empty());
	testAssert(string.length() == 0);
	testAssert(string == L"");
}


testCase(copyConstruct) {
	{// 空文字列のコピー
		String source;
		String string(source);
		testAssert(string.c_str() == source.c_str());
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
		testAssert(string == source);
	}
	{// 文字列のコピー
		String source(L"abcd");
		String string(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
	{// 参照文字列のコピー
		String source = String::literal(L"abcd");
		String string(source);
		testAssert(string.c_str() == source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
}


testCase(rvalueConstruct) {
	{// 空文字列の移動
		String source;
		String string = move(source);
		testAssert(string.c_str() == source.c_str());
		testAssert(source.empty());
		testAssert(string.empty());
		testAssert(source.length() == 0);
		testAssert(string.length() == 0);
		testAssert(string == L"");
	}
	{// 文字列の移動
		String source(L"abcd");
		String string = move(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(source.empty());
		testAssert(!string.empty());
		testAssert(source.length() == 0);
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
	}
	{// 参照文字列の移動
		String source = String::literal(L"abcd");
		String string = move(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(source.empty());
		testAssert(!string.empty());
		testAssert(source.length() == 0);
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
	}
}


testCase(wstringValueConstruct) {
	{// 空の wstring で初期化
		wstring source;
		String string(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
		testAssert(string == source);
	}
	{// wstring のコピー
		wstring source(L"abcd");
		String string(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
}


testCase(wchar_tPointerValueConstruct) {
	{// 無効な引数
		testAssertionFailed(String string(nullptr));
	}
	{// 空の文字列で初期化
		const wchar_t* source = L"";
		String string(source);
		testAssert(string.c_str() != source);
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == source);
	}
	{// 文字列で初期化
		const wchar_t* source = L"abcd";
		String string(source);
		testAssert(string.c_str() != source);
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == source);
	}
}


testCase(cAndCountConstruct) {
	{// 無効な引数
		testAssertionFailed(String string(L'a', -1));
		testNoThrow        (String string(L'a',  0));
	}
	{// ０個のキャラクターで初期化
		String string(L'a', 0);
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
	}
	{// 複数のキャラクターで初期化
		String string(L'B', 5);
		testAssert(!string.empty());
		testAssert(string.length() == 5);
		testAssert(string == L"BBBBB");
	}
}


testCase(valueAndSizeConstruct) {
	{// 無効な引数
		testAssertionFailed(String string(nullptr,  0));
		testAssertionFailed(String string(L""    , -1));
		testNoThrow        (String string(L""    ,  0));
	}
	{// ０サイズで初期化
		const wchar_t* source = L"abcd";
		String string(source, 0);
		testAssert(string.c_str() != source);
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
		testAssert(string != source);
	}
	{// 文字列とサイズで初期化
		const wchar_t* source = L"abcd";
		String string(source, 4);
		testAssert(string.c_str() != source);
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
	{// 部分文字列で初期化
		const wchar_t* source = L"abcd";
		String string(source, 3);
		testAssert(string.c_str() != source);
		testAssert(!string.empty());
		testAssert(string.length() == 3);
		testAssert(string == L"abc");
		testAssert(string != source);
	}
}


testCase(valueAndEncodingConstruct) {
	{// 空文字列で初期化
		String string("", Charset(932, true));
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
	}
	{// バイト文字列で初期化
		String string("abcａｂｃあいうえお漢字", Charset(932, true));
		testAssert(!string.empty());
		testAssert(string.length() == 13);
		testAssert(string == L"abcａｂｃあいうえお漢字");
	}
}


testCase(assignment) {
	{// 空Stringの代入
		String source;
		String string = L"efg";
		string = source;
		testAssert(string.c_str() == source.c_str());
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
	}
	{// Stringの代入
		String source(L"abcd");
		String string;
		string = source;
		testAssert(string.c_str() != source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
	{// 参照Stringの代入
		String source = String::literal(L"abcd");
		String string = String::literal(L"efg");
		string = source;
		testAssert(string.c_str() == source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
}


testCase(rvalueAssignment) {
	{// 空Stringの代入
		String source;
		String string = L"efg";
		string = move(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(!source.empty());
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
	}
	{// Stringの代入
		String source(L"abcd");
		String string;
		string = move(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(source.empty());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
	}
	{// 参照Stringの代入
		String source = String::literal(L"abcd");
		String string = String::literal(L"efg");
		string = move(source);
		testAssert(string.c_str() != source.c_str());
		testAssert(!source.empty());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
	}
}


testCase(wstringValueAssignment) {
	{// 空wstringの代入
		wstring source;
		String string = L"efg";
		string = source;
		testAssert(string.c_str() != source.c_str());
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
		testAssert(string == source);
	}
	{// wstringの代入
		wstring source(L"abcd");
		String string;
		string = source;
		testAssert(string.c_str() != source.c_str());
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
}


testCase(wchar_tPointerValueAssignment) {
	{// 無効なパラメータ
		String string;
		testAssertionFailed(string = nullptr);
	}
	{// 空文字列ポインタの代入
		const wchar_t* source = L"";
		String string = L"efg";
		string = source;
		testAssert(string.c_str() != source);
		testAssert(string.empty());
		testAssert(string.length() == 0);
		testAssert(string == L"");
		testAssert(string == source);
	}
	{// 文字列ポインタの代入
		const wchar_t* source = L"abcd";
		String string;
		string = source;
		testAssert(string.c_str() != source);
		testAssert(!string.empty());
		testAssert(string.length() == 4);
		testAssert(string == L"abcd");
		testAssert(string == source);
	}
}


testCase(compareOrdinal) {
	{// 空文字列の比較
		testAssert(String::compare(L"" , L"" ) == 0);
		testAssert(String::compare(L"a", L"" ) >  0);
		testAssert(String::compare(L"" , L"a") <  0);
		testAssert(String::compare(L"" , L"" , true) == 0);
		testAssert(String::compare(L"a", L"" , true) >  0);
		testAssert(String::compare(L"" , L"a", true) <  0);
	}

	{// 序数比較であることの検証
		testAssert(L'B' < L'a');
		testAssert(String::compare(L"a", L"a") == 0);
		testAssert(String::compare(L"a", L"B") >  0);
		testAssert(String::compare(L"B", L"a") <  0);
	}

	{// ignoreCaseの検証
		const wchar_t string0[] = L"abcABC";
		const wchar_t string1[] = L"ABCabc";
		testAssert(String::compare(string0, string1) > 0);
		testAssert(String::compare(string0, string1, true) == 0);
	}

	{// ignoreCaseの検証（全角文字は考慮されない）
		const wchar_t string0[] = L"ａｂｃＡＢＣ";
		const wchar_t string1[] = L"ＡＢＣａｂｃ";
		testAssert(String::compare(string0, string1) > 0);
		testAssert(String::compare(string0, string1, true) > 0);
	}
}


testCase(compareWithLocale) {
	{// 無効なパラメータ
		testAssertionFailed(String::compare(L"abc", L"def", Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の比較
		testAssert(String::compare(L"" , L"" , Locale::current()) == 0);
		testAssert(String::compare(L"a", L"" , Locale::current()) >  0);
		testAssert(String::compare(L"" , L"a", Locale::current()) <  0);
	}

	{// StringRange::ranLength が-1の場合とそうでない場合
		const wchar_t* string0 = L"abc";
		const wchar_t* string1 = L"abc";
		String string2 = L"abc";
		String string3 = L"abc";
		testAssert(String::compare(string0, string1, Locale::current()) == 0);
		testAssert(String::compare(string0, string2, Locale::current()) == 0);
		testAssert(String::compare(string2, string0, Locale::current()) == 0);
		testAssert(String::compare(string2, string3, Locale::current()) == 0);
	}

	{// 序数比較ではないことの検証
		testAssert(L'B' < L'a');
		testAssert(String::compare(L"a", L"a", Locale::invariant()) == 0);
		testAssert(String::compare(L"a", L"B", Locale::invariant()) <  0);
		testAssert(String::compare(L"B", L"a", Locale::invariant()) >  0);
	}

	{// サロゲートペアの比較（CharNextのように文頭か文中かで挙動が変わるかどうかもチェック）
		const wchar_t string0[] = L"\xd840\xdc0b";
		const wchar_t string1[] = L"\xd840\xdc0b";
		const wchar_t string2[] = L"ab\xd840\xdc0b";
		const wchar_t string3[] = L"ab\xd840\xdc0b";
		const wchar_t string4[] = L"\xd840\xdc0b" L"ab";
		const wchar_t string5[] = {0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string6[] = {L'a', L'b', 0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string7[] = {L'a', L'b', 0xd840, 0xdc0b, L'a', L'b', 0};
		testAssert(String::compare(string0, string1, Locale::invariant()) == 0);
		testAssert(String::compare(string2, string3, Locale::invariant()) == 0);
		testAssert(String::compare(string4, string5, Locale::invariant()) == 0);
		testAssert(String::compare(string6, string7, Locale::invariant()) == 0);
	}

	{// 結合文字列の比較
		const wchar_t string0[] = {0x0041, 0x0301, 0};
		const wchar_t string1[] = {0x0041, 0x0301, 0};
		const wchar_t string2[] = {0x00C1, 0};
		const wchar_t string3[] = {0x00C1, 0};
		testAssert(String::compare(string0, string1, Locale::invariant()) == 0);
		testAssert(String::compare(string0, string2, Locale::invariant()) == 0);
		testAssert(String::compare(string2, string3, Locale::invariant()) == 0);
	}

	{// カルチャの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		const wchar_t string0[] = L"LogIn";
		const wchar_t string1[] = L"LOGIN";
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreCase) == 0);
		testAssert(String::compare(string0, string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) != 0);
	}

	// StringComparison の検証
	{// ignoreCase
		const wchar_t string0[] = L"abcａｂｃABCＡＢＣ";
		const wchar_t string1[] = L"ABCＡＢＣabcａｂｃ";
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreCase) == 0);
	}
	{// ignoreNonSpace
		const wchar_t string0[] = {0x0041, 0x0301, 0};
		const wchar_t string1[] = {0x0041, 0};
		const wchar_t string2[] = {0x00C1, 0};
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreNonSpace) == 0);
		testAssert(String::compare(string0, string2, Locale::invariant()) == 0);
		testAssert(String::compare(string0, string2, Locale::invariant(), String::CompareOptions::ignoreNonSpace) == 0);
	}
	{// ignoreSymbols
		const wchar_t string0[] = L" \t\r\n+-\\()'&%abc";
		const wchar_t string1[] = L"abc";
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreSymbols) == 0);
	}
	{// ignoreKanaType
		const wchar_t string0[] = L"アイウ";
		const wchar_t string1[] = L"あいう";
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreKanaType) == 0);
	}
	{// ignoreWidth
		const wchar_t string0[] = L"abcｱｲｳ +-@";
		const wchar_t string1[] = L"ａｂｃアイウ　＋－＠";
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreWidth) == 0);
	}
	{// ignoreWidth + ignoreKanaType
		const wchar_t string0[] = L"ｱｲｳ";
		const wchar_t string1[] = L"あいう";
		testAssert(String::compare(string0, string1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::ignoreWidth | String::CompareOptions::ignoreKanaType) == 0);
	}
	{// stringSort
		const wchar_t string0[] = L"a-b";
		const wchar_t string1[] = L"aa";
		testAssert(String::compare(string0, string1, Locale::invariant()) > 0);
		testAssert(String::compare(string0, string1, Locale::invariant(), String::CompareOptions::stringSort) < 0);
	}
}


testCase(compareOrdinalWithIndexAndLength) {
	{// 無効なパラメータ
		testAssertionFailed(String::compare(L"a", -1, L"b",  0,  0));
		testNoThrow        (String::compare(L"a",  0, L"b",  0,  0));
		testAssertionFailed(String::compare(L"a",  2, L"b",  0,  0));
		testNoThrow        (String::compare(L"a",  1, L"b",  0,  0));
		testAssertionFailed(String::compare(L"a",  0, L"b", -1,  0));
		testNoThrow        (String::compare(L"a",  0, L"b",  0,  0));
		testAssertionFailed(String::compare(L"a",  0, L"b",  2,  0));
		testNoThrow        (String::compare(L"a",  0, L"b",  1,  0));
		testAssertionFailed(String::compare(L"a",  0, L"b",  0, -1));
	}

	{// 空文字列の比較
		testAssert(String::compare(L"" , 0, L"" , 0, 0) == 0);
		testAssert(String::compare(L"a", 0, L"" , 0, 1) >  0);
		testAssert(String::compare(L"" , 0, L"a", 0, 1) <  0);
		testAssert(String::compare(L"" , 0, L"" , 0, 0, true) == 0);
		testAssert(String::compare(L"a", 0, L"" , 0, 1, true) >  0);
		testAssert(String::compare(L"" , 0, L"a", 0, 1, true) <  0);
	}

	{// 序数比較であることの検証
		testAssert(L'B' < L'a');
		testAssert(String::compare(L"a", 0, L"a", 0, 1) == 0);
		testAssert(String::compare(L"a", 0, L"B", 0, 1) >  0);
		testAssert(String::compare(L"B", 0, L"a", 0, 1) <  0);
	}

	{// lindexの検証
		const wchar_t string0[] = L"abcd";
		const wchar_t string1[] = L"bcde";
		testAssert(String::compare(string0, 0, string1, 0, 3) != 0);
		testAssert(String::compare(string0, 1, string1, 0, 3) == 0);
	}

	{// rindexの検証
		const wchar_t string0[] = L"bcde";
		const wchar_t string1[] = L"abcd";
		testAssert(String::compare(string0, 0, string1, 0, 3) != 0);
		testAssert(String::compare(string0, 0, string1, 1, 3) == 0);
	}

	{// lengthの検証
		const wchar_t string0[] = L"abcd";
		const wchar_t string1[] = L"abce";
		testAssert(String::compare(string0, 0, string1, 0, 3) == 0);
		testAssert(String::compare(string0, 0, string1, 0, 4) <  0);
	}

	{// lengthの検証（文字列長のうち最小を採用）
		const wchar_t string0[] = L"abc";
		const wchar_t string1[] = L"abc";
		testAssert(String::compare(string0, 0, string1, 0, 3) == 0);
		testAssert(String::compare(string0, 0, string1, 0, 6) == 0);
	}

	{// ignoreCaseの検証
		const wchar_t string0[] = L"abcABC";
		const wchar_t string1[] = L"ABCabc";
		testAssert(String::compare(string0, 0, string1, 0, 3) > 0);
		testAssert(String::compare(string0, 0, string1, 0, 3, true) == 0);
	}
}


testCase(compareWithIndexAndLengthAndLocale) {
	{// 無効なパラメータ
		testAssertionFailed(String::compare(L"a", -1, L"b",  0,  0, Locale::current()));
		testNoThrow        (String::compare(L"a",  0, L"b",  0,  0, Locale::current()));
		testAssertionFailed(String::compare(L"a",  2, L"b",  0,  0, Locale::current()));
		testNoThrow        (String::compare(L"a",  1, L"b",  0,  0, Locale::current()));
		testAssertionFailed(String::compare(L"a",  0, L"b", -1,  0, Locale::current()));
		testNoThrow        (String::compare(L"a",  0, L"b",  0,  0, Locale::current()));
		testAssertionFailed(String::compare(L"a",  0, L"b",  2,  0, Locale::current()));
		testNoThrow        (String::compare(L"a",  0, L"b",  1,  0, Locale::current()));
		testAssertionFailed(String::compare(L"a",  0, L"b",  0, -1, Locale::current()));
		testAssertionFailed(String::compare(L"a",  0, L"b",  0,  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の比較
		testAssert(String::compare(L"" , 0, L"" , 0, 0, Locale::current()) == 0);
		testAssert(String::compare(L"a", 0, L"" , 0, 1, Locale::current()) >  0);
		testAssert(String::compare(L"" , 0, L"a", 0, 1, Locale::current()) <  0);
	}

	{// 序数比較ではないことの検証
		testAssert(L'B' < L'a');
		testAssert(String::compare(L"a", 0, L"B", 0, 1, Locale::invariant()) < 0);
	}

	{// lindexの検証
		const wchar_t string0[] = L"abcd";
		const wchar_t string1[] = L"bcde";
		testAssert(String::compare(string0, 0, string1, 0, 3, Locale::invariant()) != 0);
		testAssert(String::compare(string0, 1, string1, 0, 3, Locale::invariant()) == 0);
	}

	{// rindexの検証
		const wchar_t string0[] = L"bcde";
		const wchar_t string1[] = L"abcd";
		testAssert(String::compare(string0, 0, string1, 0, 3, Locale::invariant()) != 0);
		testAssert(String::compare(string0, 0, string1, 1, 3, Locale::invariant()) == 0);
	}

	{// lengthの検証
		const wchar_t string0[] = L"abcd";
		const wchar_t string1[] = L"abce";
		testAssert(String::compare(string0, 0, string1, 0, 3, Locale::invariant()) == 0);
		testAssert(String::compare(string0, 0, string1, 0, 4, Locale::invariant()) <  0);
	}

	{// lengthの検証（文字列長のうち最小を採用）
		const wchar_t string0[] = L"abc";
		const wchar_t string1[] = L"abc";
		testAssert(String::compare(string0, 0, string1, 0, 3, Locale::invariant()) == 0);
		testAssert(String::compare(string0, 0, string1, 0, 6, Locale::invariant()) == 0);
	}

	{// lengthの検証（結合文字列）
		const wchar_t string0[] = {0x0041, 0x0301, 0};
		const wchar_t string1[] = {0x00C1, 0};
		const wchar_t string2[] = {0x00C1, 'a', 0};
		testAssert(String::compare(string0, 0, string1, 0, 2, Locale::invariant()) == 0);
		testAssert(String::compare(string0, 0, string1, 0, 1, Locale::invariant()) != 0);
		testAssert(String::compare(string0, 0, string2, 0, 2, Locale::invariant()) <  0);
	}

	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		const wchar_t string0[] = L"aLogInc";
		const wchar_t string1[] = L"bLOGINd";
		testAssert(String::compare(string0, 1, string1, 1, 5, Locale::invariant()) != 0);
		testAssert(String::compare(string0, 1, string1, 1, 5, Locale::invariant(), String::CompareOptions::ignoreCase) == 0);
		testAssert(String::compare(string0, 1, string1, 1, 5, Locale(L"vi-VN")   , String::CompareOptions::ignoreCase) != 0);
	}
}


testCase(compareTo) {
	{// 空文字列の比較
		testAssert(String(L"" ).compareTo(L"" ) == 0);
		testAssert(String(L"a").compareTo(L"" ) >  0);
		testAssert(String(L"" ).compareTo(L"a") <  0);
	}

	{// StringRange::ranLength が-1の場合とそうでない場合
		String string0 = L"abc";
		const wchar_t* string1 = L"abc";
		String string2 = L"abc";
		testAssert(string0.compareTo(string1) == 0);
		testAssert(string0.compareTo(string2) == 0);
	}

	{// 序数比較ではないことの検証
		testAssert(L'B' < L'a');
		testAssert(String(L"a").compareTo(L"a") == 0);
		testAssert(String(L"a").compareTo(L"B") <  0);
		testAssert(String(L"B").compareTo(L"a") >  0);
	}
}


testCase(contains) {
	{// 空文字列の比較
		testAssert(String(L"" ).contains(L"" ) == true );
		testAssert(String(L"a").contains(L"" ) == true );
		testAssert(String(L"" ).contains(L"a") == false);
	}

	{// 正常ケース
		testAssert(String(L"abcdef").contains(L"abc") == true);
		testAssert(String(L"abcdef").contains(L"bcd") == true);
		testAssert(String(L"abcdef").contains(L"def") == true);
		testAssert(String(L"abcdef").contains(L"abd") == false);
	}
}


testCase(copyTo) {
	{// 無効な引数
		wchar_t buffer[2] = L"1";
		testAssertionFailed(String(L"01").copyTo(buffer));
		testNoThrow        (String(L"0" ).copyTo(buffer));
		testAssertionFailed(String::refer(buffer).copyTo(buffer));
		testAssertionFailed(String(L"0").copyTo(const_cast<wchar_t (&)[2]>(L"1")));
	}
	{// 空文字列のコピー
		wchar_t buffer[2] = L"a";
		String(L"").copyTo(buffer);
		testAssert(buffer == String());
	}

	{// 正常ケース
		wchar_t buffer[4];
		String string0 = L"ab";
		String string1 = L"abc";
		string0.copyTo(buffer);
		testAssert(buffer == string0);
		string1.copyTo(buffer);
		testAssert(buffer == string1);
	}
}


testCase(copyToWithDestinationSize) {
	{// 無効な引数
		wchar_t buffer[2] = L"1";
		testAssertionFailed(String(L"0").copyTo(nullptr, 2));
		testAssertionFailed(String(L"01").copyTo(buffer, 2));
		testNoThrow        (String(L"0" ).copyTo(buffer, 2));
		testAssertionFailed(String::refer(buffer).copyTo(buffer, 2));
		testAssertionFailed(String(L"0").copyTo(const_cast<wchar_t (&)[2]>(L"1"), 2));
	}
	{// 空文字列のコピー
		wchar_t buffer[2] = L"a";
		String(L"" ).copyTo(buffer, 2);
		testAssert(buffer == String());
	}

	{// 正常ケース
		wchar_t buffer[4];
		String string0 = L"ab";
		String string1 = L"abc";
		string0.copyTo(buffer, 3);
		testAssert(buffer == string0);
		string1.copyTo(buffer, 4);
		testAssert(buffer == string1);
	}
}


testCase(copyToWithIndexAndCount) {
	{// 無効な引数
		wchar_t buffer[3] = L"1";
		String string0 = L"0";
		String string1 = L"01";
		String string2 = String::refer(buffer);
		// 0 <= sourceIndex
		testAssertionFailed(string0.copyTo(-1, buffer,  0,  0));
		testNoThrow        (string0.copyTo( 0, buffer,  0,  0));
		// sourceIndex <= length()
		testAssertionFailed(string0.copyTo( 2, buffer,  0,  0));
		testNoThrow        (string0.copyTo( 1, buffer,  0,  0));
		// 0 <= destinationIndex
		testAssertionFailed(string0.copyTo( 0, buffer, -1,  0));
		// destinationIndex < destinationSize
		testAssertionFailed(string0.copyTo( 0, buffer,  3,  0));
		testNoThrow        (string0.copyTo( 0, buffer,  2,  0));
		// 0 <= count
		testAssertionFailed(string0.copyTo( 0, buffer,  0, -1));
		// count <= length() - soureIndex + 1
		testAssertionFailed(string0.copyTo( 1, buffer,  0,  2));
		testNoThrow        (string0.copyTo( 0, buffer,  0,  2));
		// count < destinationSize - destinationIndex
		testAssertionFailed(string1.copyTo( 0, buffer,  1,  3));
		testNoThrow        (string1.copyTo( 0, buffer,  1,  2));
		// isOverlapped
		testAssertionFailed(string2.copyTo( 0, buffer,  0,  1));
		// IsBadWritePtr(destination, destinationSize)
		testAssertionFailed(string0.copyTo( 0, const_cast<wchar_t (&)[2]>(L"1"),  0,  1));
	}
	{// 空文字列のコピー
		wchar_t buffer[2] = L"a";
		String(L"").copyTo(0, buffer, 0, 0);
		testAssert(buffer == StringRange(L"a"));
		String(L"").copyTo(0, buffer, 0, 1);
		testAssert(buffer == StringRange(L""));
	}

	// 正常ケース
	{// sourceIndexの検証
		wchar_t buffer[3] = L"ab";
		String string0 = L"0123";
		string0.copyTo(0, buffer, 0, 2);
		testAssert(buffer == StringRange(L"01"));
		string0.copyTo(2, buffer, 0, 2);
		testAssert(buffer == StringRange(L"23"));
	}
	{// destinationIndexの検証
		wchar_t buffer[4] = L"abc";
		String string0 = L"0123";
		string0.copyTo(0, buffer, 1, 2);
		testAssert(buffer == StringRange(L"a01"));
		string0.copyTo(2, buffer, 0, 2);
		testAssert(buffer == StringRange(L"231"));
	}
	{// countの検証
		wchar_t buffer[4] = L"abc";
		String string0 = L"0123";
		string0.copyTo(0, buffer, 0, 1);
		testAssert(buffer == StringRange(L"0bc"));
		string0.copyTo(1, buffer, 0, 3);
		testAssert(buffer == StringRange(L"123"));
	}
}


testCase(copyToWithIndexAndDestinationSizeAndCount) {
	{// 無効な引数
		wchar_t buffer[3] = L"1";
		String string0 = L"0";
		String string1 = L"01";
		String string2 = String::refer(buffer);
		// destination != nullptr
		testAssertionFailed(string0.copyTo( 0, nullptr, 3, 0,  0));
		testNoThrow        (string0.copyTo( 0, buffer , 3,  0,  0));
	}

	{// 正常ケース
		wchar_t buffer[3] = L"ab";
		String string0 = L"0123";
		string0.copyTo(0, buffer, 3, 0, 2);
		testAssert(buffer == StringRange(L"01"));
		string0.copyTo(2, buffer, 3, 0, 2);
		testAssert(buffer == StringRange(L"23"));
	}
}


testCase(empty) {
	String string0;
	String string1 = L"a";
	String string2 = L"\0abc";
	testAssert(string0.empty() == true );
	testAssert(string1.empty() == false);
	testAssert(string2.empty() == true );
}


testCase(endsWithOrdinal) {
	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.endsWith(L"" ) == true );
		testAssert(string0.endsWith(L"a") == false);
		testAssert(string1.endsWith(L"" ) == true );
		testAssert(string0.endsWith(L"" , true) == true );
		testAssert(string0.endsWith(L"a", true) == false);
		testAssert(string1.endsWith(L"" , true) == true );
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.endsWith(string1) == false);
	}

	{// 正常ケース
		String        string0   = L"あいうab01ａｂ０１漢字";
		const wchar_t string1[] = L"ab01ａｂ０１漢字";
		const wchar_t string2[] = L"ab01ａｂ０１漢字 ";
		const wchar_t string3[] = L" ab01ａｂ０１漢字";
		testAssert(string0.endsWith(string1) == true );
		testAssert(string0.endsWith(string2) == false);
		testAssert(string0.endsWith(string3) == false);
	}

	{// ignoreCase
		String        string0   = L"あいうabcABC";
		const wchar_t string1[] = L"ABCabc";
		testAssert(string0.endsWith(string1, false) == false);
		testAssert(string0.endsWith(string1, true ) == true );
	}
	{// ignoreCase（全角アルファベットはサポートしない）
		String        string0   = L"あいうａｂｃＡＢＣ";
		const wchar_t string1[] = L"ＡＢＣａｂｃ";
		testAssert(string0.endsWith(string1, false) == false);
		testAssert(string0.endsWith(string1, true ) == false);
	}
}


testCase(endsWithWithLocale) {
	{// 無効な引数
		String string0 = L"";
		testAssertionFailed(string0.endsWith(L"", Locale::invariant(), String::CompareOptions::_enum(-1)));
		testNoThrow        (string0.endsWith(L"", Locale::invariant()));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.endsWith(L"" , Locale::current()) == true );
		testAssert(string0.endsWith(L"a", Locale::current()) == false);
		testAssert(string1.endsWith(L"" , Locale::current()) == true );
	}

	{// サロゲートペアの比較（CharNextのように文頭か文中かで挙動が変わるかどうかもチェック）
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2Src[] = {L'a', L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string4Src[] = {0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string5   [] = {0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string6Src[] = {L'a', L'b', 0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string7   [] = {L'b', 0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string8Src[] = {L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string9   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t stringa   [] = {0xdc0b, 0};
		const wchar_t stringb   [] = {0xdc0b, 0xd840, 0xdc0b, 0};
		String string0 = string0Src;
		String string2 = string2Src;
		String string4 = string4Src;
		String string6 = string6Src;
		String string8 = string8Src;
		testAssert(string0.endsWith(string1, Locale::invariant()) == true );
		testAssert(string0.endsWith(string3, Locale::invariant()) == false);
		testAssert(string2.endsWith(string1, Locale::invariant()) == true );
		testAssert(string2.endsWith(string3, Locale::invariant()) == true );
		testAssert(string2.endsWith(string4, Locale::invariant()) == false);
		testAssert(string4.endsWith(string5, Locale::invariant()) == true );
		testAssert(string4.endsWith(string3, Locale::invariant()) == false);
		testAssert(string6.endsWith(string5, Locale::invariant()) == true );
		testAssert(string6.endsWith(string7, Locale::invariant()) == true );
		testAssert(string6.endsWith(string2, Locale::invariant()) == false);
		testAssert(string8.endsWith(string9, Locale::invariant()) == true );
		testAssert(string8.endsWith(stringa, Locale::invariant()) == false);
		testAssert(string8.endsWith(stringb, Locale::invariant()) == false);
	}

	{// 結合文字列の比較
		const wchar_t string0Src[] = {L'a', L'b', L'c', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {L'b', L'c', 0x0041, 0x0301, 0};
		const wchar_t string3Src[] = {L'a', L'b', L'c', 0x00C1, 0};
		const wchar_t string4   [] = {0x00C1, 0};
		const wchar_t string5   [] = {L'b', L'c', 0x00C1, 0};
		const wchar_t string6   [] = {0x0301, 0};
		String string0 = string0Src;
		String string3 = string3Src;
		testAssert(string0.endsWith(string1, Locale::invariant()) == true );
		testAssert(string0.endsWith(string2, Locale::invariant()) == true );
		testAssert(string0.endsWith(string3, Locale::invariant()) == true );
		testAssert(string0.endsWith(string4, Locale::invariant()) == true );
		testAssert(string0.endsWith(string5, Locale::invariant()) == true );
		// 結合文字での検索は出来ない
		testAssert(string0.endsWith(string6, Locale::invariant()) == false);
		testAssert(string3.endsWith(string0, Locale::invariant()) == true );
		testAssert(string3.endsWith(string1, Locale::invariant()) == true );
		testAssert(string3.endsWith(string2, Locale::invariant()) == true );
		testAssert(string3.endsWith(string4, Locale::invariant()) == true );
		testAssert(string3.endsWith(string5, Locale::invariant()) == true );
		// 結合文字での検索は出来ない
		testAssert(string3.endsWith(string6, Locale::invariant()) == false);
	}

	{// カルチャの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String        string0   = L"abcLogIn";
		const wchar_t string1[] = L"LOGIN";
		testAssert(string0.endsWith(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true );
		testAssert(string0.endsWith(string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == false);
	}

	// StringComparison の検証
	{ // ignoreCase
		String        string0   =  L"あいうabcａｂｃABCＡＢＣ";
		const wchar_t string1[] =  L"ABCＡＢＣabcａｂｃ";
		testAssert(string0.endsWith(string1, Locale::invariant()) == false);
		testAssert(string0.endsWith(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true);
	}
	{// ignoreKanaType
		String        string0   = L"abcアイウ";
		const wchar_t string1[] = L"あいう";
		testAssert(string0.endsWith(string1, Locale::invariant()) == false);
		testAssert(string0.endsWith(string1, Locale::invariant(), String::CompareOptions::ignoreKanaType) == true);
	}
	{// ignoreWidth
		String        string0   = L"あいうabcｱｲｳ +-@";
		const wchar_t string1[] = L"ａｂｃアイウ　＋－＠";
		testAssert(string0.endsWith(string1, Locale::invariant()) == false);
		testAssert(string0.endsWith(string1, Locale::invariant(), String::CompareOptions::ignoreWidth) == true);
	}
	{// ignoreWidth + ignoreKanaType
		String        string0   = L"abcｱｲｳ";
		const wchar_t string1[] = L"あいう";
		testAssert(string0.endsWith(string1, Locale::invariant()) == false);
		testAssert(string0.endsWith(string1, Locale::invariant(), String::CompareOptions::ignoreWidth | String::CompareOptions::ignoreKanaType) == true);
	}
}


testCase(equalsWithByteCharPointer) {
	{// 空文字列の比較
		const char* string0 = "";
		const char* string1 = "";
		const char* string2 = "a";
		testAssert(String::equals(string0, string1) == true );
		testAssert(String::equals(string0, string2) == false);
		testAssert(String::equals(string2, string0) == false);
		testAssert(String::equals(string0, string1, true) == true );
		testAssert(String::equals(string0, string2, true) == false);
		testAssert(String::equals(string2, string0, true) == false);
	}

	{// 正常ケース
		string string0 = "abcde";
		string string1 = "abcde";
		string string2 = "abcdef";
		string string3 = "abcd";
		string string4 = "abcdf";
		testAssert(String::equals(string0, string1) == true );
		testAssert(String::equals(string0, string2) == false);
		testAssert(String::equals(string0, string3) == false);
		testAssert(String::equals(string0, string4) == false);
	}
	{// ignoreCase
		string string0 = "abcABC";
		string string1 = "ABCabc";
		testAssert(String::equals(string0, string1) == false);
		testAssert(String::equals(string0, string1, true) == true);
	}
	{// ignoreCase（全角はサポートしない）
		string string0 = "ａｂｃＡＢＣ";
		string string1 = "ＡＢＣａｂｃ";
		testAssert(String::equals(string0, string1) == false);
		testAssert(String::equals(string0, string1, true) == false);
	}
}


testCase(equalsWithCharPointer) {
	{// 空文字列の比較
		const wchar_t* string0 = L"";
		const wchar_t* string1 = L"";
		const wchar_t* string2 = L"a";
		testAssert(String::equals(string0, string1) == true );
		testAssert(String::equals(string0, string2) == false);
		testAssert(String::equals(string2, string0) == false);
		testAssert(String::equals(string0, string1, true) == true );
		testAssert(String::equals(string0, string2, true) == false);
		testAssert(String::equals(string2, string0, true) == false);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(String::equals(string0, string1) == false);
	}

	{// 正常ケース
		String string0 = L"abcde";
		String string1 = L"abcde";
		String string2 = L"abcdef";
		String string3 = L"abcd";
		String string4 = L"abcdf";
		testAssert(String::equals(string0, string1) == true );
		testAssert(String::equals(string0, string2) == false);
		testAssert(String::equals(string0, string3) == false);
		testAssert(String::equals(string0, string4) == false);
	}
	{// ignoreCase
		String string0 = L"abcABC";
		String string1 = L"ABCabc";
		testAssert(String::equals(string0, string1) == false);
		testAssert(String::equals(string0, string1, true) == true);
	}
	{// ignoreCase（全角はサポートしない）
		String string0 = L"ａｂｃＡＢＣ";
		String string1 = L"ＡＢＣａｂｃ";
		testAssert(String::equals(string0, string1) == false);
		testAssert(String::equals(string0, string1, true) == false);
	}
}


testCase(equalsWithLhsAndRhsAndLocaleAndComparison) {
	{// 無効なパラメータ
		testAssertionFailed(String::equals(L"abc", L"def", Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の比較
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(String::equals(string0, string1, Locale::current()) == true );
		testAssert(String::equals(string0, string2, Locale::current()) == false);
		testAssert(String::equals(string2, string0, Locale::current()) == false);
	}

	{// StringRange::ranLength が-1の場合とそうでない場合
		String         string0 = L"abc";
		const wchar_t* string1 = L"abc";
		String         string2 = L"abc";
		testAssert(String::equals(string0, string1, Locale::current()) == true );
		testAssert(String::equals(string0, string2, Locale::current()) == true );
	}

	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"LogIn";
		String string1 = L"LOGIN";
		testAssert(String::equals(string0, string1, Locale::invariant()) == false);
		testAssert(String::equals(string0, string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true );
		testAssert(String::equals(string0, string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == false);
	}
}


testCase(equalTo) {
	{// 空文字列の比較
		const String string0 = L"";
		const String string1 = L"";
		const String string2 = L"a";
		testAssert(string0.equalTo(string1) == true );
		testAssert(string0.equalTo(string2) == false);
		testAssert(string2.equalTo(string0) == false);
		testAssert(string0.equalTo(string1, true) == true );
		testAssert(string0.equalTo(string2, true) == false);
		testAssert(string2.equalTo(string0, true) == false);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.equalTo(string1) == false);
	}

	{// 正常ケース
		String string0 = L"abcde";
		String string1 = L"abcde";
		String string2 = L"abcdef";
		String string3 = L"abcd";
		String string4 = L"abcdf";
		testAssert(string0.equalTo(string1) == true );
		testAssert(string0.equalTo(string2) == false);
		testAssert(string0.equalTo(string3) == false);
		testAssert(string0.equalTo(string4) == false);
	}
	{// ignoreCase
		String string0 = L"abcABC";
		String string1 = L"ABCabc";
		testAssert(string0.equalTo(string1) == false);
		testAssert(string0.equalTo(string1, true) == true);
	}
	{// ignoreCase（全角はサポートしない）
		String string0 = L"ａｂｃＡＢＣ";
		String string1 = L"ＡＢＣａｂｃ";
		testAssert(string0.equalTo(string1) == false);
		testAssert(string0.equalTo(string1, true) == false);
	}
}


testCase(equalToWithLocale) {
	{// 無効なパラメータ
		testAssertionFailed(String(L"abc").equalTo(L"def", Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の比較
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.equalTo(string1, Locale::current()) == true );
		testAssert(string0.equalTo(string2, Locale::current()) == false);
		testAssert(string2.equalTo(string0, Locale::current()) == false);
	}

	{// StringRange::ranLength が-1の場合とそうでない場合
		String         string0 = L"abc";
		const wchar_t* string1 = L"abc";
		String         string2 = L"abc";
		testAssert(string0.equalTo(string1, Locale::current()) == true );
		testAssert(string0.equalTo(string2, Locale::current()) == true );
	}

	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"LogIn";
		String string1 = L"LOGIN";
		testAssert(string0.equalTo(string1, Locale::invariant()) == false);
		testAssert(string0.equalTo(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true );
		testAssert(string0.equalTo(string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == false);
	}
}


testCase(erase) {
	{// 無効なパラメータ
		String string0 = L"abc";
		testAssertionFailed(string0.erase(-1));
		testNoThrow        (string0.erase( 0));
		testAssertionFailed(string0.erase( 4));
		testNoThrow        (string0.erase( 3));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.erase(0) == L"");
	}

	{// 正常ケース
		String string0 = L"abc";
		testAssert(string0.erase(0) == L"");
		testAssert(string0.erase(1) == L"a");
		testAssert(string0.erase(2) == L"ab");
		testAssert(string0.erase(3) == L"abc");
	}
}


testCase(eraseWithStartIndexAndCount) {
	{// 無効なパラメータ
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.erase(-1,  0));
		testNoThrow        (string0.erase( 0,  0));
		testAssertionFailed(string0.erase( 4,  0));
		testNoThrow        (string0.erase( 3,  0));
		// count out of range
		testAssertionFailed(string0.erase( 0, -1));
		testNoThrow        (string0.erase( 0,  0));
		testAssertionFailed(string0.erase( 0,  4));
		testNoThrow        (string0.erase( 0,  3));
		testAssertionFailed(string0.erase( 1,  3));
		testNoThrow        (string0.erase( 1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.erase(0, 0) == L"");
	}

	{// startIndex の検証
		String string0 = L"abc";
		testAssert(string0.erase(0, 3) == L"");
		testAssert(string0.erase(1, 2) == L"a");
		testAssert(string0.erase(2, 1) == L"ab");
		testAssert(string0.erase(3, 0) == L"abc");
	}
	{// count の検証
		String string0 = L"abc";
		testAssert(string0.erase(0, 0) == L"abc");
		testAssert(string0.erase(0, 1) == L"bc");
		testAssert(string0.erase(0, 2) == L"c");
		testAssert(string0.erase(0, 3) == L"");
	}
}


testCase(getLengthWithByteCharPointer) {
	testAssertionFailed(String::getLength(static_cast<const char*>(nullptr)));
	testAssert(String::getLength("") == 0);
	testAssert(String::getLength("abc") == 3);
}


testCase(getLengthWithCharPointer) {
	testAssertionFailed(String::getLength(static_cast<const wchar_t*>(nullptr)));
	testAssert(String::getLength(L"") == 0);
	testAssert(String::getLength(L"abc") == 3);
}


testCase(getNextLetterIndex) {
	{// 無効なパラメータ
		String string0 = L"abc";
		testAssertionFailed(string0.getNextLetterIndex(-1));
		testNoThrow        (string0.getNextLetterIndex( 0));
		testAssertionFailed(string0.getNextLetterIndex( 4));
		testNoThrow        (string0.getNextLetterIndex( 3));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.getNextLetterIndex(0) == 0);
	}

	// 正常ケース
	{// サロゲートペア
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.getNextLetterIndex(0) == 2);
		testAssert(string0.getNextLetterIndex(1) == 2);
		testAssert(string0.getNextLetterIndex(2) == 3);
		testAssert(string0.getNextLetterIndex(3) == 5);
		testAssert(string0.getNextLetterIndex(4) == 5);
		testAssert(string0.getNextLetterIndex(5) == 7);
		testAssert(string0.getNextLetterIndex(6) == 7);
		testAssert(string0.getNextLetterIndex(7) == 8);
		testAssert(string0.getNextLetterIndex(8) == 10);
		testAssert(string0.getNextLetterIndex(9) == 10);
	}
	{// 結合文字列
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		String string0 = string0Src;
		testAssert(string0.getNextLetterIndex(0) == 2);
		testAssert(string0.getNextLetterIndex(1) == 2);
		testAssert(string0.getNextLetterIndex(2) == 3);
		testAssert(string0.getNextLetterIndex(3) == 5);
		testAssert(string0.getNextLetterIndex(4) == 5);
		testAssert(string0.getNextLetterIndex(5) == 7);
		testAssert(string0.getNextLetterIndex(6) == 7);
		testAssert(string0.getNextLetterIndex(7) == 8);
		testAssert(string0.getNextLetterIndex(8) == 10);
		testAssert(string0.getNextLetterIndex(9) == 10);
	}
	{// 混合ケース
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0x0041, 0x0301, 0xd840, 0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.getNextLetterIndex(0) == 2);
		testAssert(string0.getNextLetterIndex(1) == 2);
		testAssert(string0.getNextLetterIndex(2) == 4);
		testAssert(string0.getNextLetterIndex(3) == 4);
		testAssert(string0.getNextLetterIndex(4) == 6);
		testAssert(string0.getNextLetterIndex(5) == 6);
		testAssert(string0.getNextLetterIndex(6) == 6);
	}
}


testCase(indexOfOrdinalWithValueAndIgnoreCase) {
	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.indexOf(string1) ==  0);
		testAssert(string0.indexOf(string2) == -1);
		testAssert(string2.indexOf(string0) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.indexOf(string1) ==  0);
		testAssert(string0.indexOf(string2) ==  0);
		testAssert(string0.indexOf(string3) == -1);
		testAssert(string0.indexOf(string4) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.indexOf(string1) ==  0);
		testAssert(string0.indexOf(string2) ==  0);
		testAssert(string0.indexOf(string3) == -1);
		testAssert(string0.indexOf(string4) ==  2);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.indexOf(string1, true) ==  0);
		testAssert(string0.indexOf(string2, true) ==  0);
		testAssert(string0.indexOf(string3, true) == -1);
		testAssert(string0.indexOf(string4, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.indexOf(string1, true) ==  0);
		testAssert(string0.indexOf(string2, true) ==  0);
		testAssert(string0.indexOf(string3, true) == -1);
		testAssert(string0.indexOf(string4, true) ==  2);
	}
}


testCase(indexOfWithStringValueAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// invalid comparison
		testAssertionFailed(string0.indexOf(L"a", Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string1.indexOf(L"" , Locale::current()) == 0);
		testAssert(string1.indexOf(L"a", Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexとcountの検証
		String string0 = L"bcdab";
		testAssert(string0.indexOf(L"ab", Locale::current()) == 3);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.indexOf(string1, Locale::invariant()) == -1);
		testAssert(string0.indexOf(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.indexOf(string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, Locale::current()) ==  0);
		testAssert(string0.indexOf(string2, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.indexOf(string3, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, Locale::current()) ==  0);
		testAssert(string0.indexOf(string2, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.indexOf(string5, Locale::current()) ==  0);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, Locale::current()) ==  2);
		testAssert(string0.indexOf(string2, Locale::current()) ==  2);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.indexOf(string5, Locale::current()) ==  2);
	}
}


testCase(indexOfWithCharValueAndStartIndex) {
	{// 無効な引数
		String string0 = L"abc";
		testAssertionFailed(string0.indexOf(L'a', -1));
		testNoThrow        (string0.indexOf(L'a',  0));
		testAssertionFailed(string0.indexOf(L'a',  4));
		testNoThrow        (string0.indexOf(L'a',  3));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.indexOf(L'\0') == 0); // 終端の処理が.NETと微妙に違うがまぁいいだろう
		testAssert(string0.indexOf(L'a') == -1);
	}

	{// 正常ケース
		String string0 = L"abcd";
		testAssert(string0.indexOf(L'\0') ==  4);
		testAssert(string0.indexOf(L'a' ) ==  0);
		testAssert(string0.indexOf(L'b' ) ==  1);
		testAssert(string0.indexOf(L'd' ) ==  3);
		testAssert(string0.indexOf(L'e' ) == -1);
	}
	{// 正常ケース（startIndexの検証）
		String string0 = L"abcd";
		testAssert(string0.indexOf(L'\0', 4) ==  4);
		testAssert(string0.indexOf(L'a' , 0) ==  0);
		testAssert(string0.indexOf(L'a' , 1) == -1);
		testAssert(string0.indexOf(L'b' , 1) ==  1);
		testAssert(string0.indexOf(L'b' , 2) == -1);
		testAssert(string0.indexOf(L'd' , 3) ==  3);
		testAssert(string0.indexOf(L'd' , 4) == -1);
		testAssert(string0.indexOf(L'e' , 0) == -1);
	}
}


testCase(indexOfOrdinalWithValueAndStartIndexAndIgnoreCase) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.indexOf(string1, -1));
		testNoThrow        (string0.indexOf(string1,  0));
		testAssertionFailed(string0.indexOf(string1,  3));
		testNoThrow        (string0.indexOf(string1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.indexOf(string1, 0) ==  0);
		testAssert(string0.indexOf(string2, 0) == -1);
		testAssert(string2.indexOf(string0, 0) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.indexOf(string1, 0) ==  0);
		testAssert(string0.indexOf(string2, 0) ==  0);
		testAssert(string0.indexOf(string3, 0) == -1);
		testAssert(string0.indexOf(string4, 0) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.indexOf(string1, 0) ==  0);
		testAssert(string0.indexOf(string2, 0) ==  0);
		testAssert(string0.indexOf(string3, 0) == -1);
		testAssert(string0.indexOf(string4, 0) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcdefcd";
		String string1 = L"cd";
		testAssert(string0.indexOf(string1, 0) ==  2);
		testAssert(string0.indexOf(string1, 2) ==  2);
		testAssert(string0.indexOf(string1, 3) ==  6);
		testAssert(string0.indexOf(string1, 7) == -1);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.indexOf(string1, 0, true) ==  0);
		testAssert(string0.indexOf(string2, 0, true) ==  0);
		testAssert(string0.indexOf(string3, 0, true) == -1);
		testAssert(string0.indexOf(string4, 0, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.indexOf(string1, 0, true) ==  0);
		testAssert(string0.indexOf(string2, 0, true) ==  0);
		testAssert(string0.indexOf(string3, 0, true) == -1);
		testAssert(string0.indexOf(string4, 0, true) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcDefcd";
		String string1 = L"Cd";
		testAssert(string0.indexOf(string1, 0, true) ==  2);
		testAssert(string0.indexOf(string1, 2, true) ==  2);
		testAssert(string0.indexOf(string1, 3, true) ==  6);
		testAssert(string0.indexOf(string1, 7, true) == -1);
	}
}


testCase(indexOfWithStringValueAndStartIndexAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.indexOf(L"a", -1, Locale::current()));
		testNoThrow        (string0.indexOf(L"a",  0, Locale::current()));
		testAssertionFailed(string0.indexOf(L"a",  4, Locale::current()));
		testNoThrow        (string0.indexOf(L"a",  3, Locale::current()));
		// invalid comparison
		testAssertionFailed(string0.indexOf(L"a",  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string1.indexOf(L"" , 0, Locale::current()) == 0);
		testAssert(string1.indexOf(L"a", 0, Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexとcountの検証
		String string0 = L"abcdab";
		testAssert(string0.indexOf(L"ab", 0, Locale::current()) ==  0);
		testAssert(string0.indexOf(L"ab", 1, Locale::current()) ==  4);
		testAssert(string0.indexOf(L"bc", 0, Locale::current()) ==  1);
		testAssert(string0.indexOf(L"bc", 2, Locale::current()) == -1);
		testAssert(string0.indexOf(L"ac", 0, Locale::current()) == -1);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.indexOf(string1, 0, Locale::invariant()) == -1);
		testAssert(string0.indexOf(string1, 0, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.indexOf(string1, 0, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, Locale::current()) ==  0);
		testAssert(string0.indexOf(string1, 1, Locale::current()) ==  3);
		testAssert(string0.indexOf(string1, 4, Locale::current()) ==  5);
		testAssert(string0.indexOf(string1, 6, Locale::current()) ==  8);
		testAssert(string0.indexOf(string1, 9, Locale::current()) == -1);
		testAssert(string0.indexOf(string2, 0, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.indexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, Locale::current()) ==  0);
		testAssert(string0.indexOf(string1, 1, Locale::current()) ==  3);
		testAssert(string0.indexOf(string1, 4, Locale::current()) ==  5);
		testAssert(string0.indexOf(string1, 6, Locale::current()) ==  8);
		testAssert(string0.indexOf(string1, 9, Locale::current()) == -1);
		testAssert(string0.indexOf(string2, 0, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.indexOf(string5, 0, Locale::current()) ==  0);
		testAssert(string0.indexOf(string5, 1, Locale::current()) ==  3);
		testAssert(string0.indexOf(string5, 4, Locale::current()) ==  5);
		testAssert(string0.indexOf(string5, 6, Locale::current()) ==  8);
		testAssert(string0.indexOf(string5, 9, Locale::current()) == -1);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, Locale::current()) ==  2);
		testAssert(string0.indexOf(string1, 2, Locale::current()) ==  2);
		testAssert(string0.indexOf(string1, 3, Locale::current()) ==  3);
		testAssert(string0.indexOf(string2, 0, Locale::current()) ==  2);
		testAssert(string0.indexOf(string2, 3, Locale::current()) == -1);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.indexOf(string5, 0, Locale::current()) ==  2);
		testAssert(string0.indexOf(string5, 2, Locale::current()) ==  2);
		testAssert(string0.indexOf(string5, 3, Locale::current()) ==  3);
	}
}


testCase(indexOfWithCharValueAndStartIndexAndCount) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.indexOf(L'a', -1,  0));
		testNoThrow        (string0.indexOf(L'a',  0,  0));
		testAssertionFailed(string0.indexOf(L'a',  4,  0));
		testNoThrow        (string0.indexOf(L'a',  3,  0));
		// count out of range
		testAssertionFailed(string0.indexOf(L'a',  0, -1));
		testAssertionFailed(string0.indexOf(L'a',  1,  3));
		testNoThrow        (string0.indexOf(L'a',  1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.indexOf(L'\0', 0, 0) == -1);
		testAssert(string0.indexOf(L'a' , 0, 0) == -1);
	}

	{// 正常ケース（startIndexとcountの検証）
		String string0 = L"abcd";
		testAssert(string0.indexOf(L'a', 0, 0) == -1);
		testAssert(string0.indexOf(L'a', 0, 1) ==  0);
		testAssert(string0.indexOf(L'a', 1, 1) == -1);
		testAssert(string0.indexOf(L'b', 1, 0) == -1);
		testAssert(string0.indexOf(L'b', 1, 1) ==  1);
		testAssert(string0.indexOf(L'b', 2, 1) == -1);
		testAssert(string0.indexOf(L'd', 3, 0) == -1);
		testAssert(string0.indexOf(L'd', 3, 1) ==  3);
		testAssert(string0.indexOf(L'd', 2, 1) == -1);
		testAssert(string0.indexOf(L'e', 0, 4) == -1);
	}
}


testCase(indexOfOrdinalWithValueAndStartIndexAndCountAndIgnoreCase) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.indexOf(string1, -1,  0));
		testNoThrow        (string0.indexOf(string1,  0,  0));
		testAssertionFailed(string0.indexOf(string1,  3,  0));
		testNoThrow        (string0.indexOf(string1,  2,  0));
		// count out of range
		testAssertionFailed(string0.indexOf(string1,  0, -1));
		testAssertionFailed(string0.indexOf(string1,  0,  3));
		testNoThrow        (string0.indexOf(string1,  0,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.indexOf(string1, 0, 0) ==  0);
		testAssert(string0.indexOf(string2, 0, 0) == -1);
		testAssert(string2.indexOf(string0, 0, 1) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, 2) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.indexOf(string1, 0, 6) ==  0);
		testAssert(string0.indexOf(string2, 0, 6) ==  0);
		testAssert(string0.indexOf(string3, 0, 6) == -1);
		testAssert(string0.indexOf(string4, 0, 6) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.indexOf(string1, 0, 6) ==  0);
		testAssert(string0.indexOf(string2, 0, 6) ==  0);
		testAssert(string0.indexOf(string3, 0, 6) == -1);
		testAssert(string0.indexOf(string4, 0, 6) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcdefcd";
		String string1 = L"cd";
		testAssert(string0.indexOf(string1, 0, 8) ==  2);
		testAssert(string0.indexOf(string1, 2, 6) ==  2);
		testAssert(string0.indexOf(string1, 3, 5) ==  6);
		testAssert(string0.indexOf(string1, 7, 1) == -1);
	}
	{// countの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.indexOf(string1, 0, 0) == -1);
		testAssert(string0.indexOf(string1, 0, 3) == -1);
		testAssert(string0.indexOf(string1, 0, 4) ==  2);
		testAssert(string0.indexOf(string1, 2, 1) == -1);
		testAssert(string0.indexOf(string1, 2, 2) ==  2);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.indexOf(string1, 0, 6, true) ==  0);
		testAssert(string0.indexOf(string2, 0, 6, true) ==  0);
		testAssert(string0.indexOf(string3, 0, 6, true) == -1);
		testAssert(string0.indexOf(string4, 0, 6, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.indexOf(string1, 0, 6, true) ==  0);
		testAssert(string0.indexOf(string2, 0, 6, true) ==  0);
		testAssert(string0.indexOf(string3, 0, 6, true) == -1);
		testAssert(string0.indexOf(string4, 0, 6, true) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcDefcd";
		String string1 = L"Cd";
		testAssert(string0.indexOf(string1, 0, 8, true) ==  2);
		testAssert(string0.indexOf(string1, 2, 6, true) ==  2);
		testAssert(string0.indexOf(string1, 3, 5, true) ==  6);
		testAssert(string0.indexOf(string1, 7, 1, true) == -1);
	}
	{// countの検証
		String string0 = L"abCdef";
		String string1 = L"cD";
		testAssert(string0.indexOf(string1, 0, 0, true) == -1);
		testAssert(string0.indexOf(string1, 0, 3, true) == -1);
		testAssert(string0.indexOf(string1, 0, 4, true) ==  2);
		testAssert(string0.indexOf(string1, 2, 1, true) == -1);
		testAssert(string0.indexOf(string1, 2, 2, true) ==  2);
	}
}


testCase(indexOfWithStringValueAndStartIndexAndCountAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.indexOf(L"a", -1,  0, Locale::current()));
		testNoThrow        (string0.indexOf(L"a",  0,  0, Locale::current()));
		testAssertionFailed(string0.indexOf(L"a",  4,  0, Locale::current()));
		testNoThrow        (string0.indexOf(L"a",  3,  0, Locale::current()));
		// count out of range
		testAssertionFailed(string0.indexOf(L"a",  0, -1, Locale::current()));
		testAssertionFailed(string0.indexOf(L"a",  1,  3, Locale::current()));
		testNoThrow        (string0.indexOf(L"a",  1,  2, Locale::current()));
		// invalid comparison
		testAssertionFailed(string0.indexOf(L"a",  0,  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.indexOf(L"" , 0, 0, Locale::current()) == 0);
		testAssert(string0.indexOf(L"a", 0, 0, Locale::current()) == -1);
		testAssert(string1.indexOf(L"" , 0, 1, Locale::current()) == 0);
		testAssert(string1.indexOf(L"a", 0, 1, Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexとcountの検証
		String string0 = L"abcdab";
		testAssert(string0.indexOf(L"ab", 0, 0, Locale::current()) == -1);
		testAssert(string0.indexOf(L"ab", 0, 1, Locale::current()) == -1);
		testAssert(string0.indexOf(L"ab", 0, 2, Locale::current()) ==  0);
		testAssert(string0.indexOf(L"ab", 1, 2, Locale::current()) == -1);
		testAssert(string0.indexOf(L"ab", 1, 5, Locale::current()) ==  4);
		testAssert(string0.indexOf(L"bc", 0, 0, Locale::current()) == -1);
		testAssert(string0.indexOf(L"bc", 0, 2, Locale::current()) == -1);
		testAssert(string0.indexOf(L"bc", 0, 3, Locale::current()) ==  1);
		testAssert(string0.indexOf(L"bc", 2, 2, Locale::current()) == -1);
		testAssert(string0.indexOf(L"ac", 0, 6, Locale::current()) == -1);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.indexOf(string1, 0, 10, Locale::invariant()) == -1);
		testAssert(string0.indexOf(string1, 0, 10, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.indexOf(string1, 0, 10, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, 10, Locale::current()) ==  0);
		testAssert(string0.indexOf(string1, 1,  9, Locale::current()) ==  3);
		testAssert(string0.indexOf(string1, 4,  6, Locale::current()) ==  5);
		testAssert(string0.indexOf(string1, 6,  4, Locale::current()) ==  8);
		testAssert(string0.indexOf(string1, 9,  1, Locale::current()) == -1);
		testAssert(string0.indexOf(string2, 0, 10, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.indexOf(string3, 0, 10, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0, 10, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0, 10, Locale::current()) ==  0);
		testAssert(string0.indexOf(string1, 1,  9, Locale::current()) ==  3);
		testAssert(string0.indexOf(string1, 4,  6, Locale::current()) ==  5);
		testAssert(string0.indexOf(string1, 6,  4, Locale::current()) ==  8);
		testAssert(string0.indexOf(string1, 9,  1, Locale::current()) == -1);
		testAssert(string0.indexOf(string2, 0, 10, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, 0, 10, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0, 10, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.indexOf(string5, 0, 10, Locale::current()) ==  0);
		testAssert(string0.indexOf(string5, 1,  9, Locale::current()) ==  3);
		testAssert(string0.indexOf(string5, 4,  6, Locale::current()) ==  5);
		testAssert(string0.indexOf(string5, 6,  4, Locale::current()) ==  8);
		testAssert(string0.indexOf(string5, 9,  1, Locale::current()) == -1);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.indexOf(string1, 0,  4, Locale::current()) ==  2);
		testAssert(string0.indexOf(string1, 2,  2, Locale::current()) ==  2);
		testAssert(string0.indexOf(string1, 3,  1, Locale::current()) ==  3);
		testAssert(string0.indexOf(string2, 0,  4, Locale::current()) ==  2);
		testAssert(string0.indexOf(string2, 3,  1, Locale::current()) == -1);
		// 基底文字や結合文字では検索できない
		testAssert(string0.indexOf(string3, 0,  4, Locale::current()) == -1);
		testAssert(string0.indexOf(string4, 0,  4, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.indexOf(string5, 0,  4, Locale::current()) ==  2);
		testAssert(string0.indexOf(string5, 2,  2, Locale::current()) ==  2);
		testAssert(string0.indexOf(string5, 3,  1, Locale::current()) ==  3);
	}
}


testCase(indexOfAnyWithAnyOfAndStartIndex) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.indexOfAny(string1, -1));
		testNoThrow        (string0.indexOfAny(string1,  0));
		testAssertionFailed(string0.indexOfAny(string1,  3));
		testNoThrow        (string0.indexOfAny(string1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.indexOfAny(string1, 0) == -1);
		testAssert(string0.indexOfAny(string2, 0) == -1);
		testAssert(string2.indexOfAny(string0, 0) == -1);
	}

	// 正常ケース
	{// anyOfの検証
		String string0 = L"abcdef";
		String string1 = L"fb";
		String string2 = L"ghijklmne";
		String string3 = L"a";
		String string4 = L"fghi";
		String string5 = L"ghijklmn";
		testAssert(string0.indexOfAny(string1) ==  1);
		testAssert(string0.indexOfAny(string2) ==  4);
		testAssert(string0.indexOfAny(string3) ==  0);
		testAssert(string0.indexOfAny(string4) ==  5);
		testAssert(string0.indexOfAny(string5) == -1);
	}
	{// startIndexの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.indexOfAny(string1   ) ==  2);
		testAssert(string0.indexOfAny(string1, 2) ==  2);
		testAssert(string0.indexOfAny(string1, 3) ==  3);
		testAssert(string0.indexOfAny(string1, 4) == -1);
	}
}


testCase(indexOfAnyWithAnyOfAndStartIndexAndCount) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.indexOfAny(string1, -1,  0));
		testNoThrow        (string0.indexOfAny(string1,  0,  0));
		testAssertionFailed(string0.indexOfAny(string1,  3,  0));
		testNoThrow        (string0.indexOfAny(string1,  2,  0));
		// count out of range
		testAssertionFailed(string0.indexOfAny(string1,  0, -1));
		testAssertionFailed(string0.indexOfAny(string1,  0,  3));
		testNoThrow        (string0.indexOfAny(string1,  0,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.indexOfAny(string1, 0, 0) == -1);
		testAssert(string0.indexOfAny(string2, 0, 0) == -1);
		testAssert(string2.indexOfAny(string0, 0, 1) == -1);
	}

	// 正常ケース
	{// anyOfの検証
		String string0 = L"abcdef";
		String string1 = L"fb";
		String string2 = L"ghijklmne";
		String string3 = L"a";
		String string4 = L"fghi";
		String string5 = L"ghijklmn";
		testAssert(string0.indexOfAny(string1, 0, 6) ==  1);
		testAssert(string0.indexOfAny(string2, 0, 6) ==  4);
		testAssert(string0.indexOfAny(string3, 0, 6) ==  0);
		testAssert(string0.indexOfAny(string4, 0, 6) ==  5);
		testAssert(string0.indexOfAny(string5, 0, 6) == -1);
	}
	{// startIndexの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.indexOfAny(string1, 0, 6) ==  2);
		testAssert(string0.indexOfAny(string1, 2, 4) ==  2);
		testAssert(string0.indexOfAny(string1, 3, 3) ==  3);
		testAssert(string0.indexOfAny(string1, 4, 2) == -1);
	}
	{// countの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.indexOfAny(string1, 0, 0) == -1);
		testAssert(string0.indexOfAny(string1, 0, 2) == -1);
		testAssert(string0.indexOfAny(string1, 0, 3) ==  2);
		testAssert(string0.indexOfAny(string1, 3, 0) == -1);
		testAssert(string0.indexOfAny(string1, 3, 1) ==  3);
	}
}


testCase(insert) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"c";
		// startIndex out of range
		testAssertionFailed(string0.insert( -1, string1));
		testNoThrow        (string0.insert(  0, string1));
		testAssertionFailed(string0.insert(  3, string1));
		testNoThrow        (string0.insert(  2, string1));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.insert(0, string1) == L"");
		testAssert(string0.insert(0, string2) == L"a");
		testAssert(string2.insert(0, string0) == L"a");
		testAssert(string2.insert(1, string0) == L"a");
	}

	{// 正常ケース
		String string0 = L"abcd";
		String string1 = L"01";
		testAssert(string0.insert(0, string1) == L"01abcd");
		testAssert(string0.insert(4, string1) == L"abcd01");
		testAssert(string0.insert(2, string1) == L"ab01cd");
	}
}


testCase(isNormalized) {
	{// 無効な引数
		testAssertionFailed(String().isNormalized(String::NormalizationForm::_enum(-1)));
	}
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.isNormalized() == true);
		testAssert(string0.isNormalized(String::NormalizationForm::d) == true);
		testAssert(string0.isNormalized(String::NormalizationForm::kc) == true);
		testAssert(string0.isNormalized(String::NormalizationForm::kd) == true);
	}

	{// 正常ケース
		const wchar_t string0Src[] = {0x00C1, 0}; // アキュート付き大文字A NFC
		const wchar_t string1Src[] = {0x0041, 0x0301, 0}; // アキュート付き大文字A NFD & NFKD
		const wchar_t string2Src[] = {0x3326, 0}; // ㌦ NFC
		const wchar_t string3Src[] = {0x30C8, 0x3099, 0x30EB, 0}; // ㌦ NFKD
		const wchar_t string4Src[] = {0x30C9, 0x30EB, 0}; // ㌦ NFKC
		const wchar_t string6Src[] = {0x0301, 0}; // 結合文字単体
		String string0 = string0Src;
		String string1 = string1Src;
		String string2 = string2Src;
		String string3 = string3Src;
		String string4 = string4Src;
		String string5 = L"abc";
		String string6 = string6Src;

		// formC
		testAssert(string0.isNormalized() == true );
		testAssert(string1.isNormalized() == false);
		testAssert(string2.isNormalized() == true );
		testAssert(string3.isNormalized() == false);
		testAssert(string4.isNormalized() == true );
		testAssert(string5.isNormalized() == true );
		testAssert(string6.isNormalized() == true );
		// formD
		testAssert(string0.isNormalized(String::NormalizationForm::d) == false);
		testAssert(string1.isNormalized(String::NormalizationForm::d) == true );
		testAssert(string2.isNormalized(String::NormalizationForm::d) == true );
		testAssert(string3.isNormalized(String::NormalizationForm::d) == true );
		testAssert(string4.isNormalized(String::NormalizationForm::d) == false);
		testAssert(string5.isNormalized(String::NormalizationForm::d) == true );
		testAssert(string6.isNormalized(String::NormalizationForm::d) == true );
		// formKC
		testAssert(string0.isNormalized(String::NormalizationForm::kc) == true );
		testAssert(string1.isNormalized(String::NormalizationForm::kc) == false);
		testAssert(string2.isNormalized(String::NormalizationForm::kc) == false);
		testAssert(string3.isNormalized(String::NormalizationForm::kc) == false);
		testAssert(string4.isNormalized(String::NormalizationForm::kc) == true );
		testAssert(string5.isNormalized(String::NormalizationForm::kc) == true );
		testAssert(string6.isNormalized(String::NormalizationForm::kc) == true );
		// formKD
		testAssert(string0.isNormalized(String::NormalizationForm::kd) == false);
		testAssert(string1.isNormalized(String::NormalizationForm::kd) == true );
		testAssert(string2.isNormalized(String::NormalizationForm::kd) == false);
		testAssert(string3.isNormalized(String::NormalizationForm::kd) == true );
		testAssert(string4.isNormalized(String::NormalizationForm::kd) == false);
		testAssert(string5.isNormalized(String::NormalizationForm::kd) == true );
		testAssert(string6.isNormalized(String::NormalizationForm::kd) == true );
	}
	{// サロゲートペアの検証
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0};
		const wchar_t string1Src[] = {0xd840, 0};
		const wchar_t string2Src[] = {0xdc0b, 0};
		String string0 = string0Src;
		String string1 = string1Src;
		String string2 = string2Src;
		testAssert(string0.isNormalized() == true );
		testAssert(string1.isNormalized() == false);
		testAssert(string2.isNormalized() == false);
	}
}


testCase(isWhiteSpace) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.isWhiteSpace() == true );
	}
	{// 正常ケース
		String string0 = L"\t\r\n　";
		String string1 = L"a \t\r\n　";
		String string2 = L" \t\r\n　a";
		testAssert(string0.isWhiteSpace() == true );
		testAssert(string1.isWhiteSpace() == false);
		testAssert(string2.isWhiteSpace() == false);
	}
}


testCase(join) {
	{// 空文字、空配列の検証
		String string0 = L"";
		String string1 = L"a";
		vector<String> values0;
		String values1[] = {L""};
		String values2[] = {L"", L""};
		String values3[] = {L"b", L"c"};
		testAssert(String::join(string0, values0) == L"");
		testAssert(String::join(string0, values1) == L"");
		testAssert(String::join(string0, values2) == L"");
		testAssert(String::join(string0, values3) == L"bc");
		testAssert(String::join(string1, values0) == L"");
		testAssert(String::join(string1, values1) == L"");
		testAssert(String::join(string1, values2) == L"a");
		testAssert(String::join(string1, values3) == L"bac");
	}

	{// 正常ケース
		String string0 = L"=+=";
		String values0[] = {L"a", L"bc", L"def"};
		testAssert(String::join(string0, values0) == L"a=+=bc=+=def");
	}
}


testCase(lastIndexOfOrdinalWithValueAndIgnoreCase) {
	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.lastIndexOf(string1) ==  0);
		testAssert(string0.lastIndexOf(string2) == -1);
		testAssert(string2.lastIndexOf(string0) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.lastIndexOf(string1) ==  0);
		testAssert(string0.lastIndexOf(string2) ==  0);
		testAssert(string0.lastIndexOf(string3) == -1);
		testAssert(string0.lastIndexOf(string4) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.lastIndexOf(string1) ==  0);
		testAssert(string0.lastIndexOf(string2) ==  0);
		testAssert(string0.lastIndexOf(string3) == -1);
		testAssert(string0.lastIndexOf(string4) ==  2);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, true) ==  0);
		testAssert(string0.lastIndexOf(string2, true) ==  0);
		testAssert(string0.lastIndexOf(string3, true) == -1);
		testAssert(string0.lastIndexOf(string4, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, true) ==  0);
		testAssert(string0.lastIndexOf(string2, true) ==  0);
		testAssert(string0.lastIndexOf(string3, true) == -1);
		testAssert(string0.lastIndexOf(string4, true) ==  2);
	}
}


testCase(lastIndexOfWithStringValueAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// invalid comparison
		testAssertionFailed(string0.lastIndexOf(L"a",  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.lastIndexOf(L"" , Locale::current()) == 0);
		testAssert(string0.lastIndexOf(L"a", Locale::current()) == -1);
		testAssert(string1.lastIndexOf(L"" , Locale::current()) == 0);
		testAssert(string1.lastIndexOf(L"a", Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexの検証
		String string0 = L"abcdab";
		testAssert(string0.lastIndexOf(L"ab", Locale::current()) ==  4);
		testAssert(string0.lastIndexOf(L"bc", Locale::current()) ==  1);
		testAssert(string0.lastIndexOf(L"ac", Locale::current()) == -1);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.lastIndexOf(string1, Locale::invariant()) == -1);
		testAssert(string0.lastIndexOf(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.lastIndexOf(string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string2, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.lastIndexOf(string3, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string2, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.lastIndexOf(string5, Locale::current()) ==  8);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string2, Locale::current()) ==  2);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.lastIndexOf(string5, Locale::current()) ==  3);
	}
}


testCase(lastIndexOfWithCharValueAndStartIndex) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(L'a', -1));
		testNoThrow        (string0.lastIndexOf(L'a',  0));
		testAssertionFailed(string0.lastIndexOf(L'a',  4));
		testNoThrow        (string0.lastIndexOf(L'a',  3));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.lastIndexOf(L'\0', 0) == -1);
		testAssert(string0.lastIndexOf(L'a' , 0) == -1);
	}

	{// 正常ケース（startIndexとcountの検証）
		String string0 = L"abcdabcd";
		testAssert(string0.lastIndexOf(L'a', 0) ==  4);
		testAssert(string0.lastIndexOf(L'a', 5) == -1);
		testAssert(string0.lastIndexOf(L'b', 0) ==  5);
		testAssert(string0.lastIndexOf(L'd', 0) ==  7);
		testAssert(string0.lastIndexOf(L'e', 0) == -1);
	}
}


testCase(lastIndexOfOrdinalWithValueAndStartIndexAndIgnoreCase) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(string1, -1));
		testNoThrow        (string0.lastIndexOf(string1,  0));
		testAssertionFailed(string0.lastIndexOf(string1,  3));
		testNoThrow        (string0.lastIndexOf(string1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.lastIndexOf(string1, 0) ==  0);
		testAssert(string0.lastIndexOf(string2, 0) == -1);
		testAssert(string2.lastIndexOf(string0, 0) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0) ==  0);
		testAssert(string0.lastIndexOf(string2, 0) ==  0);
		testAssert(string0.lastIndexOf(string3, 0) == -1);
		testAssert(string0.lastIndexOf(string4, 0) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0) ==  0);
		testAssert(string0.lastIndexOf(string2, 0) ==  0);
		testAssert(string0.lastIndexOf(string3, 0) == -1);
		testAssert(string0.lastIndexOf(string4, 0) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcdefcd";
		String string1 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0) ==  6);
		testAssert(string0.lastIndexOf(string1, 7) == -1);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, true) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, true) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, true) == -1);
		testAssert(string0.lastIndexOf(string4, 0, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, true) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, true) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, true) == -1);
		testAssert(string0.lastIndexOf(string4, 0, true) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcDefcd";
		String string1 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, true) ==  6);
		testAssert(string0.lastIndexOf(string1, 7, true) == -1);
	}
}


testCase(lastIndexOfWithStringValueAndStartIndexAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(L"a", -1, Locale::current()));
		testNoThrow        (string0.lastIndexOf(L"a",  0, Locale::current()));
		testAssertionFailed(string0.lastIndexOf(L"a",  4, Locale::current()));
		testNoThrow        (string0.lastIndexOf(L"a",  3, Locale::current()));
		// invalid comparison
		testAssertionFailed(string0.lastIndexOf(L"a",  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.lastIndexOf(L"" , 0, Locale::current()) == 0);
		testAssert(string0.lastIndexOf(L"a", 0, Locale::current()) == -1);
		testAssert(string1.lastIndexOf(L"" , 0, Locale::current()) == 0);
		testAssert(string1.lastIndexOf(L"a", 0, Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexの検証
		String string0 = L"abcdab";
		testAssert(string0.lastIndexOf(L"ab", 0, Locale::current()) ==  4);
		testAssert(string0.lastIndexOf(L"ab", 5, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"bc", 0, Locale::current()) ==  1);
		testAssert(string0.lastIndexOf(L"ac", 0, Locale::current()) == -1);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.lastIndexOf(string1, 0, Locale::invariant()) == -1);
		testAssert(string0.lastIndexOf(string1, 0, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.lastIndexOf(string1, 0, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string2, 0, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.lastIndexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string2, 0, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.lastIndexOf(string5, 0, Locale::current()) ==  8);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string2, 0, Locale::current()) ==  2);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, 0, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.lastIndexOf(string5, 0, Locale::current()) ==  3);
	}
}


testCase(lastIndexOfWithCharValueAndStartIndexAndCount) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(L'a', -1,  0));
		testNoThrow        (string0.lastIndexOf(L'a',  0,  0));
		testAssertionFailed(string0.lastIndexOf(L'a',  4,  0));
		testNoThrow        (string0.lastIndexOf(L'a',  3,  0));
		// count out of range
		testAssertionFailed(string0.lastIndexOf(L'a',  0, -1));
		testAssertionFailed(string0.lastIndexOf(L'a',  1,  3));
		testNoThrow        (string0.lastIndexOf(L'a',  1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.lastIndexOf(L'\0', 0, 0) == -1);
		testAssert(string0.lastIndexOf(L'a' , 0, 0) == -1);
	}

	{// 正常ケース（startIndexとcountの検証）
		String string0 = L"abcdabcd";
		testAssert(string0.lastIndexOf(L'a', 0, 0) == -1);
		testAssert(string0.lastIndexOf(L'a', 0, 1) ==  0);
		testAssert(string0.lastIndexOf(L'a', 0, 5) ==  4);
		testAssert(string0.lastIndexOf(L'a', 5, 3) == -1);
		testAssert(string0.lastIndexOf(L'b', 1, 0) == -1);
		testAssert(string0.lastIndexOf(L'b', 1, 1) ==  1);
		testAssert(string0.lastIndexOf(L'b', 0, 8) ==  5);
		testAssert(string0.lastIndexOf(L'd', 0, 8) ==  7);
		testAssert(string0.lastIndexOf(L'e', 0, 8) == -1);
	}
}


testCase(lastIndexOfOrdinalWithValueAndStartIndexAndCountAndIgnoreCase) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(string1, -1,  0));
		testNoThrow        (string0.lastIndexOf(string1,  0,  0));
		testAssertionFailed(string0.lastIndexOf(string1,  3,  0));
		testNoThrow        (string0.lastIndexOf(string1,  2,  0));
		// count out of range
		testAssertionFailed(string0.lastIndexOf(string1,  0, -1));
		testAssertionFailed(string0.lastIndexOf(string1,  0,  3));
		testNoThrow        (string0.lastIndexOf(string1,  0,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.lastIndexOf(string1, 0, 0) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, 0) == -1);
		testAssert(string2.lastIndexOf(string0, 0, 1) ==  0);
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, 2) == -1);
	}

	// 正常ケース
	{// valueの検証
		String string0 = L"abcdef";
		String string1 = L"a";
		String string2 = L"abcdef";
		String string3 = L"abcdefg";
		String string4 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0, 6) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, 6) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, 6) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 6) ==  2);
	}
	{// valueの検証（value.rawLength が -1の場合）
		String         string0 = L"abcdef";
		const wchar_t* string1 = L"a";
		const wchar_t* string2 = L"abcdef";
		const wchar_t* string3 = L"abcdefg";
		const wchar_t* string4 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0, 6) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, 6) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, 6) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 6) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcdefcd";
		String string1 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0, 8) ==  6);
		testAssert(string0.lastIndexOf(string1, 7, 1) == -1);
	}
	{// countの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.lastIndexOf(string1, 0, 0) == -1);
		testAssert(string0.lastIndexOf(string1, 0, 3) == -1);
		testAssert(string0.lastIndexOf(string1, 0, 4) ==  2);
		testAssert(string0.lastIndexOf(string1, 2, 1) == -1);
		testAssert(string0.lastIndexOf(string1, 2, 2) ==  2);
	}
	{// valueの検証（ignoreCase）
		String string0 = L"aBcDeF";
		String string1 = L"A";
		String string2 = L"AbCdEf";
		String string3 = L"AbCdEfG";
		String string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, 6, true) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, 6, true) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, 6, true) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 6, true) ==  2);
	}
	{// valueの検証（ignoreCase）（value.rawLength が -1の場合）
		String         string0 = L"aBcDeF";
		const wchar_t* string1 = L"A";
		const wchar_t* string2 = L"AbCdEf";
		const wchar_t* string3 = L"AbCdEfG";
		const wchar_t* string4 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, 6, true) ==  0);
		testAssert(string0.lastIndexOf(string2, 0, 6, true) ==  0);
		testAssert(string0.lastIndexOf(string3, 0, 6, true) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 6, true) ==  2);
	}
	{// startIndexの検証
		String string0 = L"abcDefcd";
		String string1 = L"Cd";
		testAssert(string0.lastIndexOf(string1, 0, 8, true) ==  6);
		testAssert(string0.lastIndexOf(string1, 7, 1, true) == -1);
	}
	{// countの検証
		String string0 = L"abCdef";
		String string1 = L"cD";
		testAssert(string0.lastIndexOf(string1, 0, 0, true) == -1);
		testAssert(string0.lastIndexOf(string1, 0, 3, true) == -1);
		testAssert(string0.lastIndexOf(string1, 0, 4, true) ==  2);
		testAssert(string0.lastIndexOf(string1, 2, 1, true) == -1);
		testAssert(string0.lastIndexOf(string1, 2, 2, true) ==  2);
	}
}


testCase(lastIndexOfWithStringValueAndStartIndexAndCountAndLocaleAndComparison) {
	{// 無効な引数
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOf(L"a", -1,  0, Locale::current()));
		testNoThrow        (string0.lastIndexOf(L"a",  0,  0, Locale::current()));
		testAssertionFailed(string0.lastIndexOf(L"a",  4,  0, Locale::current()));
		testNoThrow        (string0.lastIndexOf(L"a",  3,  0, Locale::current()));
		// count out of range
		testAssertionFailed(string0.lastIndexOf(L"a",  0, -1, Locale::current()));
		testAssertionFailed(string0.lastIndexOf(L"a",  1,  3, Locale::current()));
		testNoThrow        (string0.lastIndexOf(L"a",  1,  2, Locale::current()));
		// invalid comparison
		testAssertionFailed(string0.lastIndexOf(L"a",  0,  0, Locale::current(), String::CompareOptions::_enum(-1)));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.lastIndexOf(L"" , 0, 0, Locale::current()) == 0);
		testAssert(string0.lastIndexOf(L"a", 0, 0, Locale::current()) == -1);
		testAssert(string1.lastIndexOf(L"" , 0, 1, Locale::current()) == 0);
		testAssert(string1.lastIndexOf(L"a", 0, 1, Locale::current()) == 0);
	}

	// 正常ケース
	{// startIndexとcountの検証
		String string0 = L"abcdab";
		testAssert(string0.lastIndexOf(L"ab", 0, 0, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"ab", 0, 1, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"ab", 0, 2, Locale::current()) ==  0);
		testAssert(string0.lastIndexOf(L"ab", 1, 4, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"ab", 1, 5, Locale::current()) ==  4);
		testAssert(string0.lastIndexOf(L"bc", 0, 0, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"bc", 0, 2, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"bc", 0, 3, Locale::current()) ==  1);
		testAssert(string0.lastIndexOf(L"bc", 2, 2, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(L"ac", 0, 6, Locale::current()) == -1);
	}
	{// カルチャとStringComparisonの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String string0 = L"abcLogInde";
		String string1 = L"LOGIN";
		testAssert(string0.lastIndexOf(string1, 0, 10, Locale::invariant()) == -1);
		testAssert(string0.lastIndexOf(string1, 0, 10, Locale::invariant(), String::CompareOptions::ignoreCase) == 3);
		testAssert(string0.lastIndexOf(string1, 0, 10, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == -1);
	}
	{// サロゲートペアの検索
		const wchar_t string0Src[] = {0xd840, 0xdc0b, L'a', 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t string3   [] = {0xd840, 0};
		const wchar_t string4   [] = {0xdc0b, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, 10, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string1, 0,  9, Locale::current()) ==  5);
		testAssert(string0.lastIndexOf(string1, 0,  6, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string1, 0,  4, Locale::current()) ==  0);
		testAssert(string0.lastIndexOf(string1, 1,  2, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string2, 0, 10, Locale::current()) ==  3);
		// 欠けたサロゲートコードでは検索できない
		testAssert(string0.lastIndexOf(string3, 0, 10, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 10, Locale::current()) == -1);
	}
	{// 結合文字列の検索
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', 0x0041, 0x0301, 0x0041, 0x0301, L'b', 0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0, 10, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string1, 0,  9, Locale::current()) ==  5);
		testAssert(string0.lastIndexOf(string1, 0,  6, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string1, 0,  4, Locale::current()) ==  0);
		testAssert(string0.lastIndexOf(string1, 1,  2, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string2, 0, 10, Locale::current()) ==  3);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, 0, 10, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0, 10, Locale::current()) == -1);
		// 合成文字による検索
		testAssert(string0.lastIndexOf(string5, 0, 10, Locale::current()) ==  8);
		testAssert(string0.lastIndexOf(string5, 0,  9, Locale::current()) ==  5);
		testAssert(string0.lastIndexOf(string5, 0,  6, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string5, 0,  4, Locale::current()) ==  0);
		testAssert(string0.lastIndexOf(string5, 1,  2, Locale::current()) == -1);
	}
	{// 合成文字の検索
		const wchar_t string0Src[] = {L'a', L'b', 0x00C1, 0x00C1, 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, 0x0041, 0x0301, 0};
		const wchar_t string3   [] = {0x0041, 0};
		const wchar_t string4   [] = {0x0301, 0};
		const wchar_t string5   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.lastIndexOf(string1, 0,  4, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string1, 2,  2, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string1, 0,  3, Locale::current()) ==  2);
		testAssert(string0.lastIndexOf(string2, 0,  4, Locale::current()) ==  2);
		testAssert(string0.lastIndexOf(string2, 0,  3, Locale::current()) == -1);
		// 基底文字や結合文字では検索できない
		testAssert(string0.lastIndexOf(string3, 0,  4, Locale::current()) == -1);
		testAssert(string0.lastIndexOf(string4, 0,  4, Locale::current()) == -1);
		// 合成文字同士による検索
		testAssert(string0.lastIndexOf(string5, 0,  4, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string5, 2,  2, Locale::current()) ==  3);
		testAssert(string0.lastIndexOf(string5, 0,  3, Locale::current()) ==  2);
	}
}


testCase(lastIndexOfAnyWithAnyOfAndStartIndex) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOfAny(string1, -1));
		testNoThrow        (string0.lastIndexOfAny(string1,  0));
		testAssertionFailed(string0.lastIndexOfAny(string1,  3));
		testNoThrow        (string0.lastIndexOfAny(string1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.lastIndexOfAny(string1, 0) == -1);
		testAssert(string0.lastIndexOfAny(string2, 0) == -1);
		testAssert(string2.lastIndexOfAny(string0, 0) == -1);
	}

	// 正常ケース
	{// anyOfの検証
		String string0 = L"abcdef";
		String string1 = L"fb";
		String string2 = L"ghijklmne";
		String string3 = L"a";
		String string4 = L"fghi";
		String string5 = L"ghijklmn";
		testAssert(string0.lastIndexOfAny(string1) ==  5);
		testAssert(string0.lastIndexOfAny(string2) ==  4);
		testAssert(string0.lastIndexOfAny(string3) ==  0);
		testAssert(string0.lastIndexOfAny(string4) ==  5);
		testAssert(string0.lastIndexOfAny(string5) == -1);
	}
	{// startIndexの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.lastIndexOfAny(string1   ) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 2) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 3) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 4) == -1);
	}
}


testCase(lastIndexOfAnyWithAnyOfAndStartIndexAndCount) {
	{// 無効な引数
		String string0 = L"ab";
		String string1 = L"a";
		// startIndex out of range
		testAssertionFailed(string0.lastIndexOfAny(string1, -1,  0));
		testNoThrow        (string0.lastIndexOfAny(string1,  0,  0));
		testAssertionFailed(string0.lastIndexOfAny(string1,  3,  0));
		testNoThrow        (string0.lastIndexOfAny(string1,  2,  0));
		// count out of range
		testAssertionFailed(string0.lastIndexOfAny(string1,  0, -1));
		testAssertionFailed(string0.lastIndexOfAny(string1,  0,  3));
		testNoThrow        (string0.lastIndexOfAny(string1,  0,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"";
		String string2 = L"a";
		testAssert(string0.lastIndexOfAny(string1, 0, 0) == -1);
		testAssert(string0.lastIndexOfAny(string2, 0, 0) == -1);
		testAssert(string2.lastIndexOfAny(string0, 0, 1) == -1);
	}

	// 正常ケース
	{// anyOfの検証
		String string0 = L"abcdef";
		String string1 = L"fb";
		String string2 = L"ghijklmne";
		String string3 = L"a";
		String string4 = L"fghi";
		String string5 = L"ghijklmn";
		testAssert(string0.lastIndexOfAny(string1, 0, 6) ==  5);
		testAssert(string0.lastIndexOfAny(string2, 0, 6) ==  4);
		testAssert(string0.lastIndexOfAny(string3, 0, 6) ==  0);
		testAssert(string0.lastIndexOfAny(string4, 0, 6) ==  5);
		testAssert(string0.lastIndexOfAny(string5, 0, 6) == -1);
	}
	{// startIndexの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.lastIndexOfAny(string1, 1, 5) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 2, 4) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 3, 3) ==  3);
		testAssert(string0.lastIndexOfAny(string1, 4, 2) == -1);
	}
	{// countの検証
		String string0 = L"abcdef";
		String string1 = L"cd";
		testAssert(string0.lastIndexOfAny(string1, 0, 0) == -1);
		testAssert(string0.lastIndexOfAny(string1, 0, 2) == -1);
		testAssert(string0.lastIndexOfAny(string1, 0, 3) ==  2);
		testAssert(string0.lastIndexOfAny(string1, 0, 4) ==  3);
	}
}


testCase(length) {
	testAssert(String(L"").length() == 0);
	testAssert(String(L"abc").length() == 3);
}


testCase(lengthInLetter) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.lengthInLetter() == 0);
	}

	{// 正常ケース（サロゲートペアと結合文字列入り）
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0x0041, 0x0301, L'a', L'b', 0};
		String string0 = string0Src;
		testAssert(string0.lengthInLetter() == 4);
	}

	// より詳細なケースは testCase(getNextLetterIndex) にてテスト済み
}


testCase(literal) {
	{// 空文字列の検証
		const wchar_t string0[] = L"";
		testAssert(String::literal(string0) == L"");
		testAssert(String::literal(string0).c_str() == string0);
	}

	{// 正常ケース
		const wchar_t string0[] = L"abc";
		testAssert(String::literal(string0) == L"abc");
		testAssert(String::literal(string0).c_str() == string0);
	}
}


testCase(normalize) {
	{// 無効な引数
		testAssertionFailed(String().normalize(String::NormalizationForm::_enum(-1)));
	}
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.normalize() == L"");
		testAssert(string0.normalize(String::NormalizationForm::d ) == L"");
		testAssert(string0.normalize(String::NormalizationForm::kc) == L"");
		testAssert(string0.normalize(String::NormalizationForm::kd) == L"");
	}

	{// 正常ケース
		const wchar_t string0Src[] = {0x00C1, 0}; // アキュート付き大文字A NFC & NFKC
		const wchar_t string1Src[] = {0x0041, 0x0301, 0}; // アキュート付き大文字A NFD & NFKD
		const wchar_t string2Src[] = {0x3326, 0}; // ㌦ NFC
		const wchar_t string3Src[] = {0x30C8, 0x3099, 0x30EB, 0}; // ㌦ NFKD
		const wchar_t string4Src[] = {0x30C9, 0x30EB, 0}; // ㌦ NFKC
		const wchar_t string6Src[] = {0x0301, 0}; // 結合文字単体
		String string0 = string0Src;
		String string1 = string1Src;
		String string2 = string2Src;
		String string3 = string3Src;
		String string4 = string4Src;
		String string5 = L"abc";
		String string6 = string6Src;

		// formC
		testAssert(string0.normalize() == string0);
		testAssert(string1.normalize() == string0);
		testAssert(string2.normalize() == string2);
		testAssert(string3.normalize() == string4);
		testAssert(string4.normalize() == string4);
		testAssert(string5.normalize() == string5);
		testAssert(string6.normalize() == string6);
		// formD
		testAssert(string0.normalize(String::NormalizationForm::d) == string1);
		testAssert(string1.normalize(String::NormalizationForm::d) == string1);
		testAssert(string2.normalize(String::NormalizationForm::d) == string2);
		testAssert(string3.normalize(String::NormalizationForm::d) == string3);
		testAssert(string4.normalize(String::NormalizationForm::d) == string3);
		testAssert(string5.normalize(String::NormalizationForm::d) == string5);
		testAssert(string6.normalize(String::NormalizationForm::d) == string6);
		// formKC
		testAssert(string0.normalize(String::NormalizationForm::kc) == string0);
		testAssert(string1.normalize(String::NormalizationForm::kc) == string0);
		testAssert(string2.normalize(String::NormalizationForm::kc) == string4);
		testAssert(string3.normalize(String::NormalizationForm::kc) == string4);
		testAssert(string4.normalize(String::NormalizationForm::kc) == string4);
		testAssert(string5.normalize(String::NormalizationForm::kc) == string5);
		testAssert(string6.normalize(String::NormalizationForm::kc) == string6);
		// formKD
		testAssert(string0.normalize(String::NormalizationForm::kd) == string1);
		testAssert(string1.normalize(String::NormalizationForm::kd) == string1);
		testAssert(string2.normalize(String::NormalizationForm::kd) == string3);
		testAssert(string3.normalize(String::NormalizationForm::kd) == string3);
		testAssert(string4.normalize(String::NormalizationForm::kd) == string3);
		testAssert(string5.normalize(String::NormalizationForm::kd) == string5);
		testAssert(string6.normalize(String::NormalizationForm::kd) == string6);
	}
	{// サロゲートペアの検証
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0};
		const wchar_t string1Src[] = {0xd840, 0};
		const wchar_t string2Src[] = {0xdc0b, 0};
		String string0 = string0Src;
		String string1 = string1Src;
		String string2 = string2Src;
		testAssert(string0.normalize() == string0);
		testThrow(string1.normalize(), String::NormalizeFailedException);
		testThrow(string2.normalize(), String::NormalizeFailedException);
	}
}


testCase(padLeft) {
	{// 無効なパラメータ
		testAssertionFailed(String().padLeft(-1));
		testNoThrow        (String().padLeft( 0));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.padLeft(0) == L"");
		testAssert(string0.padLeft(1) == L" ");
		testAssert(string1.padLeft(0) == L"a");
	}

	{// 正常ケース
		String string0 = L"abc";
		testAssert(string0.padLeft(3) == L"abc");
		testAssert(string0.padLeft(4) == L" abc");
		testAssert(string0.padLeft(5, L'0') == L"00abc");
	}
}


testCase(padRight) {
	{// 無効なパラメータ
		testAssertionFailed(String().padRight(-1));
		testNoThrow        (String().padRight( 0));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.padRight(0) == L"");
		testAssert(string0.padRight(1) == L" ");
		testAssert(string1.padRight(0) == L"a");
	}

	{// 正常ケース
		String string0 = L"abc";
		testAssert(string0.padRight(3) == L"abc");
		testAssert(string0.padRight(4) == L"abc ");
		testAssert(string0.padRight(5, L'0') == L"abc00");
	}
}


testCase(refer) {
	{// 無効なパラメータ
		testAssertionFailed(String::refer(nullptr));
	}

	{// 空文字列の検証
		const wchar_t* string0 = L"";
		testAssert(String::refer(string0) == L"");
		testAssert(String::refer(string0).c_str() == string0);
	}

	{// 正常ケース
		const wchar_t* string0 = L"abc";
		String         string1 = L"def";
		testAssert(String::refer(string0) == L"abc");
		testAssert(String::refer(string0).c_str() == string0);
		testAssert(String::refer(string1) == L"def");
		testAssert(String::refer(string1).c_str() == string1.c_str());
	}
}


testCase(referWithValueAndLength) {
	{// 無効なパラメータ
		const wchar_t* string0 = L"";
		const wchar_t* string1 = L"a";
		testAssertionFailed(String::refer(nullptr,  0));
		testAssertionFailed(String::refer(string0, -1));
		testNoThrow        (String::refer(string0,  0));
		testAssertionFailed(String::refer(string1,  0));
		testNoThrow        (String::refer(string1,  1));
	}

	{// 空文字列の検証
		const wchar_t* string0 = L"";
		testAssert(String::refer(string0, 0) == L"");
		testAssert(String::refer(string0, 0).c_str() == string0);
	}

	{// 正常ケース
		const wchar_t* string0 = L"abc";
		testAssert(String::refer(string0, 3) == L"abc");
		testAssert(String::refer(string0, 3).c_str() == string0);
	}
}


testCase(referred) {
	testAssert(String().referred());
	testAssert(String(L"").referred());
	testAssert(!String(L"a").referred());
	testAssert(String::literal(L"a").referred());
	testAssert(String::refer(L"a").referred());
}


testCase(replace) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.replace(L'a', L'b') == L"");
	}

	{// 正常ケース
		String string0 = L"abcaabbccabc";
		testAssert(string0.replace(L'a', L'0') == L"0bc00bbcc0bc");
		testAssert(string0.replace(L'b', L'1') == L"a1caa11cca1c");
	}
}


testCase(replaceWithOldValueAndNewValue) {
	{// 無効なパラメータ
		String string0 = L"";
		testAssertionFailed(string0.replace(L"" , L""));
		testNoThrow        (string0.replace(L"a", L""));
	}


	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.replace(L"a", L"ab") == L"");
	}

	// 正常ケース
	{// ヒット無し
		String string0 = L"abc";
		testAssert(string0.replace(L"d", L"e") == L"abc");
	}
	{// 空文字列化
		String string0 = L"abc";
		String string1 = L"abcabcabc";
		testAssert(string0.replace(L"abc", L"") == L"");
		testAssert(string1.replace(L"abc", L"") == L"");
	}
	{// ヒット文字列交差
		String string0 = L"abababab";
		testAssert(string0.replace(L"abab", L"0") == L"00");
	}
	{// oldValue.length() < newValue.length()
		String string0 = L"a0a1a2a";
		testAssert(string0.replace(L"a", L"abcd") == L"abcd0abcd1abcd2abcd");
	}
	{// newValue.length() < oldValue.length()
		String string0 = L"abcd0abcd1abcd2abcd";
		testAssert(string0.replace(L"abcd", L"a") == L"a0a1a2a");
	}
}


testCase(split) {
	{// 空文字の検証
		String string0 = L"";
		vector<String> vector0;
		vector0 += L"";
		testAssert(string0.split(L"" ) == vector0);
		testAssert(string0.split(L"a") == vector0);
	}

	// 正常ケース
	// 区切り文字未設定
	{// 区切り文字０
		String string0 = L"abcd";
		vector<String> vector0;
		vector0 += string0;
		testAssert(string0.split(L"") == vector0);
	}
	{// 区切り文字のみ
		String string0 = L" ";
		String string1 = L"\t\n";
		vector<String> vector1;
		vector1 += L"", L"";
		vector<String> vector2;
		vector2 += L"", L"", L"";
		testAssert(string0.split(L"") == vector1);
		testAssert(string1.split(L"") == vector2);
	}
	{// 区切り文字混合
		String string0 = L" ab　cd\t";
		vector<String> vector0;
		vector0 += L"", L"ab", L"cd", L"";
		testAssert(string0.split(L"") == vector0);
	}
	// 区切り文字設定
	{// 区切り文字０
		String string0 = L"abcd";
		vector<String> vector0;
		vector0 += string0;
		testAssert(string0.split(L"012") == vector0);
	}
	{// 区切り文字のみ
		String string0 = L"0";
		String string1 = L"01";
		vector<String> vector1;
		vector1 += L"", L"";
		vector<String> vector2;
		vector2 += L"", L"", L"";
		testAssert(string0.split(L"012") == vector1);
		testAssert(string1.split(L"012") == vector2);
	}
	{// 区切り文字混合
		String string0 = L"0ab1cd2";
		vector<String> vector0;
		vector0 += L"", L"ab", L"cd", L"";
		testAssert(string0.split(L"012") == vector0);
	}
}


testCase(splitWithStringRangeSeparators) {
	{// 空文字の検証
		String string0 = L"";
		vector<String> separators0;
		vector<String> separators1;
		separators1 += L"abc";
		vector<String> vector1;
		vector1 += L"";
		testAssertionFailed(string0.split(separators0));
		testAssert(string0.split(separators1) == vector1);
	}

	// 正常ケース
	{// 区切り文字０
		String string0 = L"abcd";
		vector<String> separators0;
		separators0 += L"012", L"345", L"678";
		vector<String> vector0;
		vector0 += string0;
		testAssert(string0.split(separators0) == vector0);
	}
	{// 区切り文字のみ
		String string0 = L"012";
		String string1 = L"012345";
		vector<String> separators0;
		separators0 += L"012", L"345", L"678";
		vector<String> vector1;
		vector1 += L"", L"";
		vector<String> vector2;
		vector2 += L"", L"", L"";
		testAssert(string0.split(separators0) == vector1);
		testAssert(string1.split(separators0) == vector2);
	}
	{// 区切り文字混合
		String string0 = L"012ab345cd678";
		vector<String> separators0;
		separators0 += L"012", L"345", L"678";
		vector<String> vector0;
		vector0 += L"", L"ab", L"cd", L"";
		testAssert(string0.split(separators0) == vector0);
	}
}


testCase(startsWithOrdinal) {
	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.startsWith(L"" ) == true );
		testAssert(string0.startsWith(L"a") == false);
		testAssert(string1.startsWith(L"" ) == true );
		testAssert(string0.startsWith(L"" , true) == true );
		testAssert(string0.startsWith(L"a", true) == false);
		testAssert(string1.startsWith(L"" , true) == true );
	}

	{// 序数比較であることの検証
		const wchar_t string0Src[] = {0x0041, 0x0301, 0};
		const wchar_t string1   [] = {0x00C1, 0};
		String string0 = string0Src;
		testAssert(string0.startsWith(string1) == false);
	}

	{// 正常ケース
		String        string0   = L"ab01ａｂ０１漢字あいう";
		const wchar_t string1[] = L"ab01ａｂ０１漢字";
		const wchar_t string2[] = L" ab01ａｂ０１漢字";
		const wchar_t string3[] = L"ab01ａｂ０１漢字 ";
		testAssert(string0.startsWith(string1) == true );
		testAssert(string0.startsWith(string2) == false);
		testAssert(string0.startsWith(string3) == false);
	}

	{// ignoreCase
		String        string0   = L"abcABCあいう";
		const wchar_t string1[] = L"ABCabc";
		testAssert(string0.startsWith(string1, false) == false);
		testAssert(string0.startsWith(string1, true ) == true );
	}
	{// ignoreCase（全角アルファベットはサポートしない）
		String        string0   = L"ａｂｃＡＢＣあいう";
		const wchar_t string1[] = L"ＡＢＣａｂｃ";
		testAssert(string0.startsWith(string1, false) == false);
		testAssert(string0.startsWith(string1, true ) == false);
	}
}


testCase(startsWithWithLocale) {
	{// 無効な引数
		String string0 = L"";
		testAssertionFailed(string0.startsWith(L"", Locale::invariant(), String::CompareOptions::_enum(-1)));
		testNoThrow        (string0.startsWith(L"", Locale::invariant()));
	}

	{// 空文字列の検証
		String string0 = L"";
		String string1 = L"a";
		testAssert(string0.startsWith(L"" , Locale::current()) == true );
		testAssert(string0.startsWith(L"a", Locale::current()) == false);
		testAssert(string1.startsWith(L"" , Locale::current()) == true );
	}

	{// サロゲートペアの比較（CharNextのように文頭か文中かで挙動が変わるかどうかもチェック）
		const wchar_t string0Src[] = {0xd840, 0xdc0b, 0};
		const wchar_t string1   [] = {0xd840, 0xdc0b, 0};
		const wchar_t string2Src[] = {0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string3   [] = {0xd840, 0xdc0b, L'a', 0};
		const wchar_t string4Src[] = {L'a', L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string5   [] = {L'a', L'b', 0xd840, 0xdc0b, 0};
		const wchar_t string6Src[] = {L'a', L'b', 0xd840, 0xdc0b, L'a', L'b', 0};
		const wchar_t string7   [] = {L'a', L'b', 0xd840, 0xdc0b, L'a', 0};
		const wchar_t string8Src[] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0xd840, 0xdc0b, L'a', 0};
		const wchar_t string9   [] = {0xd840, 0xdc0b, 0xd840, 0xdc0b, 0};
		const wchar_t stringa   [] = {0xdc0b, 0};
		const wchar_t stringb   [] = {0xdc0b, 0xd840, 0xdc0b, 0};
		String string0 = string0Src;
		String string2 = string2Src;
		String string4 = string4Src;
		String string6 = string6Src;
		String string8 = string8Src;
		testAssert(string0.startsWith(string1, Locale::invariant()) == true );
		testAssert(string0.startsWith(string3, Locale::invariant()) == false);
		testAssert(string2.startsWith(string1, Locale::invariant()) == true );
		testAssert(string2.startsWith(string3, Locale::invariant()) == true );
		testAssert(string2.startsWith(string4, Locale::invariant()) == false);
		testAssert(string4.startsWith(string5, Locale::invariant()) == true );
		testAssert(string4.startsWith(string3, Locale::invariant()) == false);
		testAssert(string6.startsWith(string5, Locale::invariant()) == true );
		testAssert(string6.startsWith(string7, Locale::invariant()) == true );
		testAssert(string6.startsWith(string2, Locale::invariant()) == false);
		testAssert(string8.startsWith(string9, Locale::invariant()) == true );
		testAssert(string8.startsWith(stringa, Locale::invariant()) == false);
		testAssert(string8.startsWith(stringb, Locale::invariant()) == false);
	}

	{// 結合文字列の比較
		const wchar_t string0Src[] = {0x0041, 0x0301, L'a', L'b', L'c', 0};
		const wchar_t string1   [] = {0x0041, 0x0301, 0};
		const wchar_t string2   [] = {0x0041, 0x0301, L'a', L'b', 0};
		const wchar_t string3Src[] = {0x00C1, L'a', L'b', L'c', 0};
		const wchar_t string4   [] = {0x00C1, 0};
		const wchar_t string5   [] = {0x00C1, L'a', L'b', 0};
		const wchar_t string6   [] = {0x0301, 0};
		String string0 = string0Src;
		String string3 = string3Src;
		testAssert(string0.startsWith(string1, Locale::invariant()) == true );
		testAssert(string0.startsWith(string2, Locale::invariant()) == true );
		testAssert(string0.startsWith(string3, Locale::invariant()) == true );
		testAssert(string0.startsWith(string4, Locale::invariant()) == true );
		testAssert(string0.startsWith(string5, Locale::invariant()) == true );
		// 結合文字での検索は出来ない
		testAssert(string0.startsWith(string6, Locale::invariant()) == false);
		testAssert(string3.startsWith(string0, Locale::invariant()) == true );
		testAssert(string3.startsWith(string1, Locale::invariant()) == true );
		testAssert(string3.startsWith(string2, Locale::invariant()) == true );
		testAssert(string3.startsWith(string4, Locale::invariant()) == true );
		testAssert(string3.startsWith(string5, Locale::invariant()) == true );
		// 結合文字での検索は出来ない
		testAssert(string3.startsWith(string6, Locale::invariant()) == false);
	}

	{// カルチャの影響（ベトナム語では LogIn と LOGIN がignoreCase でも一致しない）
		String        string0   = L"LogInabc";
		const wchar_t string1[] = L"LOGIN";
		testAssert(string0.startsWith(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true );
		testAssert(string0.startsWith(string1, Locale(L"vi-VN")          , String::CompareOptions::ignoreCase) == false);
	}

	// StringComparison の検証
	{ // ignoreCase
		String        string0   =  L"abcａｂｃABCＡＢＣあいう";
		const wchar_t string1[] =  L"ABCＡＢＣabcａｂｃ";
		testAssert(string0.startsWith(string1, Locale::invariant()) == false);
		testAssert(string0.startsWith(string1, Locale::invariant(), String::CompareOptions::ignoreCase) == true);
	}
	{// ignoreKanaType
		String        string0   = L"アイウabc";
		const wchar_t string1[] = L"あいう";
		testAssert(string0.startsWith(string1, Locale::invariant()) == false);
		testAssert(string0.startsWith(string1, Locale::invariant(), String::CompareOptions::ignoreKanaType) == true);
	}
	{// ignoreWidth
		String        string0   = L"abcｱｲｳ +-@あいう";
		const wchar_t string1[] = L"ａｂｃアイウ　＋－＠";
		testAssert(string0.startsWith(string1, Locale::invariant()) == false);
		testAssert(string0.startsWith(string1, Locale::invariant(), String::CompareOptions::ignoreWidth) == true);
	}
	{// ignoreWidth + ignoreKanaType
		String        string0   = L"ｱｲｳabc";
		const wchar_t string1[] = L"あいう";
		testAssert(string0.startsWith(string1, Locale::invariant()) == false);
		testAssert(string0.startsWith(string1, Locale::invariant(), String::CompareOptions::ignoreWidth | String::CompareOptions::ignoreKanaType) == true);
	}
}


testCase(substring) {
	{// 無効なパラメータ
		String string0 = L"abc";
		testAssertionFailed(string0.substring(-1));
		testNoThrow        (string0.substring( 0));
		testAssertionFailed(string0.substring( 4));
		testNoThrow        (string0.substring( 3));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.substring(0) == L"");
	}

	{// 正常ケース
		String string0 = L"abc";
		testAssert(string0.substring(0) == L"abc");
		testAssert(string0.substring(1) == L"bc");
		testAssert(string0.substring(2) == L"c");
		testAssert(string0.substring(3) == L"");
	}
}


testCase(substringWithStartIndexAndCount) {
	{// 無効なパラメータ
		String string0 = L"abc";
		// startIndex out of range
		testAssertionFailed(string0.substring(-1,  0));
		testNoThrow        (string0.substring( 0,  0));
		testAssertionFailed(string0.substring( 4,  0));
		testNoThrow        (string0.substring( 3,  0));
		// count out of range
		testAssertionFailed(string0.substring( 0, -1));
		testNoThrow        (string0.substring( 0,  0));
		testAssertionFailed(string0.substring( 0,  4));
		testNoThrow        (string0.substring( 0,  3));
		testAssertionFailed(string0.substring( 1,  3));
		testNoThrow        (string0.substring( 1,  2));
	}

	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.substring(0, 0) == L"");
	}

	{// startIndex の検証
		String string0 = L"abc";
		testAssert(string0.substring(0, 3) == L"abc");
		testAssert(string0.substring(1, 2) == L"bc");
		testAssert(string0.substring(2, 1) == L"c");
		testAssert(string0.substring(3, 0) == L"");
	}
	{// count の検証
		String string0 = L"abc";
		testAssert(string0.substring(0, 0) == L"");
		testAssert(string0.substring(0, 1) == L"a");
		testAssert(string0.substring(0, 2) == L"ab");
		testAssert(string0.substring(0, 3) == L"abc");
	}
}


testCase(toLower) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.toLower() == L"");
	}

	{// 正常ケース
		String string0 = L"abczａｂｃｚABCZＡＢＣＺ012あいう";
		String string2 = L"abczａｂｃｚabczａｂｃｚ012あいう";
		testAssert(string0.toLower() == string2);
	}
}


testCase(toLowerWithLocale) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.toLower(Locale::current()) == L"");
	}

	{// 正常ケース（Localeに依存した変換はテストしたかったが.NETと違ってトルコ語の特殊変換は機能していないようなので未テスト）
		Locale locale0(L"ja-JP");
		String string0 = L"abczａｂｃｚABCZＡＢＣＺ012あいう";
		String string2 = L"abczａｂｃｚabczａｂｃｚ012あいう";
		testAssert(string0.toLower(locale0) == string2);
	}
}


testCase(toUpper) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.toUpper() == L"");
	}

	{// 正常ケース
		String string0 = L"abczａｂｃｚABCZＡＢＣＺ012あいう";
		String string2 = L"ABCZＡＢＣＺABCZＡＢＣＺ012あいう";
		testAssert(string0.toUpper() == string2);
	}
}


testCase(toUpperWithLocale) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.toUpper(Locale::current()) == L"");
	}

	{// 正常ケース（localeに依存した変換はテストしたかったが.NETと違ってトルコ語の特殊変換は機能していないようなので未テスト）
		Locale locale0(L"ja-JP");
		String string0 = L"abczａｂｃｚABCZＡＢＣＺ012あいう";
		String string2 = L"ABCZＡＢＣＺABCZＡＢＣＺ012あいう";
		testAssert(string0.toUpper(locale0) == string2);
	}
}


testCase(trim) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.trim() == L"");
	}

	// 正常ケース
	{// 空白無し
		String string0 = L"abc";
		testAssert(string0.trim() == L"abc");
	}
	{// 空白のみ
		String string0 = L" \t\r\n　";
		testAssert(string0.trim() == L"");
	}
	{// 先頭のみ
		String string0 = L" \tabc";
		testAssert(string0.trim() == L"abc");
	}
	{// 末尾のみ
		String string0 = L"abc\r\n";
		testAssert(string0.trim() == L"abc");
	}
	{// 先頭と末尾
		String string0 = L" \tabc\r\n";
		testAssert(string0.trim() == L"abc");
	}
}


testCase(trimWithTrimChars) {
	{// 空文字列の検証
		String string0 = L"";
		testAssert(string0.trim(L"" ) == L"");
		testAssert(string0.trim(L"a") == L"");
	}

	// 正常ケース
	{// トリム文字指定無し
		String string0 = L" \tabc\r\n";
		testAssert(string0.trim(L"") == L"abc");
	}
	{// トリム文字無し
		String string0 = L"abc";
		String trim0 = L"0123";
		testAssert(string0.trim(trim0) == L"abc");
	}
	{// トリム文字のみ
		String string0 = L"0123";
		String trim0 = L"0123";
		testAssert(string0.trim(trim0) == L"");
	}
	{// 先頭のみ
		String string0 = L"01abc";
		String trim0 = L"0123";
		testAssert(string0.trim(trim0) == L"abc");
	}
	{// 末尾のみ
		String string0 = L"abc23";
		String trim0 = L"0123";
		testAssert(string0.trim(trim0) == L"abc");
	}
	{// 先頭と末尾
		String string0 = L"01abc23";
		String trim0 = L"0123";
		testAssert(string0.trim(trim0) == L"abc");
	}
}


testCase(operatorWstring) {
	{// 空文字列の検証
		String string0 = L"";
		wstring wstring0 = string0;
		testAssert(wstring0 == L"");
	}

	{// 正常ケース
		String string0 = L"abc";
		wstring wstring0 = string0;
		testAssert(wstring0 == L"abc");
	}
}


testCase(operatorStringRange) {
	{// 空文字列の検証
		String string0 = L"";
		StringRange range0 = string0;
		testAssert(range0 == L"");
	}

	{// 正常ケース
		String string0 = L"abc";
		StringRange range0 = string0;
		testAssert(range0 == L"abc");
		testAssert(range0.c_str() == string0.c_str());
	}
}


testCase(operatorBracket) {
	{// 無効なパラメータ
		String string0 = L"abc";
		testAssertionFailed(string0[-1]);
		testNoThrow        (string0[ 0]);
		testAssertionFailed(string0[ 4]);
		testNoThrow        (string0[ 3]);
	}

	{// 正常ケース
		String string0 = L"abc";
		testAssert(string0[0] == L'a');
		testAssert(string0[1] == L'b');
		testAssert(string0[2] == L'c');
		testAssert(string0[3] == L'\0');
	}
}


testCase(operatorPlusEqual) {
	{// 空文字列の検証
		String lhs = L"";
		String rhs = L"";
		testAssert((lhs += rhs) == L"");
	}

	{// 左辺が空文字列
		String lhs = L"";
		String rhs = L"abc";
		testAssert((lhs += rhs) == L"abc");
	}
	{// 右辺が空文字列
		String lhs = L"abc";
		String rhs = L"";
		testAssert((lhs += rhs) == L"abc");
	}
	{// 正常ケース
		String         lhs0 = L"abc";
		wstring        rhs0 = L"def";
		const wchar_t* rhs1 = L"def";
		wchar_t*       rhs2 = L"def";
		wchar_t        rhs3 = L'd';
		int            rhs4 = 256;
		double         rhs5 = 1.1;
		testAssert((lhs0 += rhs0) == L"abcdef");
		testAssert((lhs0 += rhs1) == L"abcdefdef");
		testAssert((lhs0 += rhs2) == L"abcdefdefdef");
		testAssert((lhs0 += rhs3) == L"abcdefdefdefd"  );
		testAssert((lhs0 += rhs4) == L"abcdefdefdefd256");
		testAssert((lhs0 += rhs5) == L"abcdefdefdefd2561.1");
	}
}


testCase(operatorPlus) {
	// String + String
	{// 空文字列の検証
		String lhs = L"";
		String rhs = L"";
		testAssert(lhs + rhs == L"");
	}

	{// 左辺が空文字列
		String lhs = L"";
		String rhs = L"abc";
		testAssert(lhs + rhs == L"abc");
	}
	{// 右辺が空文字列
		String lhs = L"abc";
		String rhs = L"";
		testAssert(lhs + rhs == L"abc");
	}
	{// 正常ケース
		String lhs = L"abc";
		String rhs = L"def";
		testAssert(lhs + rhs == L"abcdef");
	}

	{// String + ?
		String         lhs0 = L"abc";
		wstring        rhs0 = L"def";
		const wchar_t* rhs1 = L"def";
		wchar_t*       rhs2 = L"def";
		wchar_t        rhs3 = L'd';
		int            rhs4 = 256;
		double         rhs5 = 1.1;
		testAssert(lhs0 + rhs0 == L"abcdef");
		testAssert(lhs0 + rhs1 == L"abcdef");
		testAssert(lhs0 + rhs2 == L"abcdef");
		testAssert(lhs0 + rhs3 == L"abcd");
		testAssert(lhs0 + rhs4 == L"abc256");
		testAssert(lhs0 + rhs5 == L"abc1.1");
	}

	{// ? + String
		wstring        lhs0 = L"abc";
		const wchar_t* lhs1 = L"abc";
		wchar_t*       lhs2 = L"abc";
		wchar_t        lhs3 = L'a';
		int            lhs4 = 256;
		double         lhs5 = 1.1;
		String         rhs0 = L"def";
		testAssert(lhs0 + rhs0 == L"abcdef");
		testAssert(lhs1 + rhs0 == L"abcdef");
		testAssert(lhs2 + rhs0 == L"abcdef");
		testAssert(lhs3 + rhs0 == L"adef");
		testAssert(lhs4 + rhs0 == L"256def");
		testAssert(lhs5 + rhs0 == L"1.1def");
	}
}


testCase(operatorEqual) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	testAssert(string0 == string1);
	testAssert(!(string0 == string2));
}


testCase(operatorNotEqual) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	testAssert(!(string0 != string1));
	testAssert(string0 != string2);
}


testCase(operatorLess) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	testAssert(!(string0 < string1));
	testAssert(string0 < string2);
}


testCase(operatorLessEqual) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	String string3 = L"aa";
	testAssert(string0 <= string1);
	testAssert(string0 <= string2);
	testAssert(!(string0 <= string3));
}


testCase(operatorMore) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	testAssert(!(string0 > string1));
	testAssert(string2 > string0);
}


testCase(operatorMoreEqual) {
	String string0 = L"abc";
	String string1 = L"abc";
	String string2 = L"abcd";
	String string3 = L"aa";
	testAssert(string1 >= string0);
	testAssert(string2 >= string0);
	testAssert(!(string3 >= string0));
}



	}
}