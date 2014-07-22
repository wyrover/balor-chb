#include <balor/locale/Charset.hpp>

#include <boost/thread.hpp>
#include <MLang.h>

#include <balor/io/MemoryStream.hpp>
#include <balor/io/Resource.hpp>
#include <balor/system/Com.hpp>
#include <balor/system/ComPtr.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace locale {
		namespace testCharset {


using std::move;
using std::string;
using std::vector;
using boost::thread;
using namespace balor::io;
using namespace balor::system;


namespace {
const wchar_t testString[] = L"+-\\\\@~012abcABCｱｲｳ＋－￥～＠０１２ａｂｃＡＢＣアイウあいう漢字";


string getResourceString(const wchar_t* name) {
	auto stream = Resource().getRawData(String::literal(L"balor::locale::charset::") + name);
	string result(static_cast<int>(stream.length()), '\0');
	stream.read(&result[0], 0, result.length());
	return move(result);
}
} // namespace



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	Com::initialize(); // CoInitializeEx 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
	{
		ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage); // CoCreateInstance 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
	}
	Com::uninitialize();

	testAssert(SUCCEEDED(CoInitialize(nullptr))); // CoInitialize 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
	CoUninitialize();

	IsValidCodePage(0); // IsValidCodePage 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
}


testCase(ComInitialize) { // COM初期化コードとの共存。実にややこしい、どうにかならないものか
	testAssert(!Com::initialized()); // 未初期化のはず

	{// Charset がＣＯＭを使用しないパターン
		{
			Charset charset = Charset::default();
			testAssert(charset.encode(L"abc") == "abc");
			testAssert(charset.decode("abc") == L"abc");
		}
		testAssert(!Com::initialized());
	}
	{// Charset がＣＯＭを初期化する
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		testAssert(Com::initialized());
		Com::uninitialize();
	}
	{// ＣＯＭをＭＴＡで初期化しておいてCharsetを使用する
		Com::initialize(false);
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		Com::uninitialize();
	}
	{// ＣＯＭをＳＴＡで初期化しておいてCharsetを使用する
		Com::initialize();
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		Com::uninitialize();
	}
	{// ＣＯＭをＭＴＡで初期化しておいて別スレッドでCharsetを使用する
		Com::initialize(false);
		thread workor([] () {
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		});
		workor.join();
		Com::uninitialize();
	}
	{// ＣＯＭをＳＴＡで初期化しておいて別スレッドで STA の Charset を使用する。
		Com::initialize();
		thread workor([] () {
			{
				testThrow(Charset charset(1200), Com::UninitializedException);
				Com::initialize();
				Charset charset(1200);
				testAssert(charset.name() == L"unicode");
			}
			Com::uninitialize();
		});
		workor.join();
		Com::uninitialize();
	}
	{// Charset が別スレッドでMTA の ＣＯＭを初期化する。
		thread work([] () {
			Com::initialize(false);
			{
				Charset charset(1200);
				testAssert(charset.name() == L"unicode");
			}
			Com::uninitialize();
		});
		work.join();
		testNoThrow(Charset charset(1200));
		testAssert(Com::initialized());
		Com::uninitialize();
	}
}


testCase(secondStartup) { // ハンドルリークチェックに引っかからないようにあらかじめＣＯＭの初期化をすませておく
	balor::test::UnitTest::ignoreHandleLeakCheck();
	Charset charset(932);
	Charset::charsets(); // IEnumCodePage->Next 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
}


testCase(codePageAndIsWindowsCodePageConstruct) {
	// 無効なパラメータ
	testAssertionFailed(Charset(-1));
	testAssertionFailed(Charset(65536));

	{// isWindowsCodePage = true でありながら windows のコードページではない
		testAssertionFailed(Charset(51932, true));
		testNoThrow(Charset(932, true));
	}

	{// mlang のサポート範囲外のコードページ
		testThrow(Charset(7777), Charset::NotFoundException);
		testThrow(Charset(1), Charset::NotFoundException);
		testNoThrow(Charset(51932));
	}

	{// 正常ケース 1
		Charset shift_jis(932, true);
		testAssert(shift_jis.codePage() == 932);
		testAssert(shift_jis.name() == L"shift_jis");
		testAssert(shift_jis.fallbackChar() == L'?');
		testAssert(!shift_jis.throwable());
	}
	{// 正常ケース 2
		Charset big5(950);
		testAssert(big5.codePage() == 950);
		testAssert(big5.name() == L"big5");
	}
}


testCase(nameConstruct) {
	{// 無効なエンコーディング名
		testThrow(Charset(L"unknowncode"), Charset::NotFoundException);
	}

	{// 正常ケース
		Charset euc_jp(L"euc-jp");
		testAssert(euc_jp.codePage() == 51932);
		testAssert(euc_jp.name() == L"euc-jp");
		testAssert(euc_jp.fallbackChar() == L'?');
		testAssert(!euc_jp.throwable());
		Charset utf8(L"utf-8");
		testAssert(utf8.codePage() == 65001);
		testAssert(utf8.name() == L"utf-8");
	}
}


testCase(rvalueConstruct) {
	Charset source(20127);
	source.fallbackChar(L'*');
	source.throwable(true);
	Charset charset = move(source);
	testAssert(charset.codePage() == 20127);
	testAssert(charset.fallbackChar() == L'*');
	testAssert(charset.throwable());
	testAssert(charset.name() == L"us-ascii");
}


testCase(rvalueAssignment) {
	Charset source(1200);
	source.fallbackChar(L'*');
	source.throwable(true);
	Charset charset = Charset::default();
	charset = move(source);
	testAssert(charset.codePage() == 1200);
	testAssert(charset.fallbackChar() == L'*');
	testAssert(charset.throwable());
	testAssert(charset.name() == L"unicode");
}


testCase(ascii) {
	Charset charset = Charset::ascii();
	testAssert(charset.codePage() == 20127);
	testAssert(charset.displayName() == L"US-ASCII");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"us-ascii");

	String testString = L"abc012ＡＢＣ０１２あいう漢字";
	string bytes;
	testAssert((bytes = charset.encode(testString)) == "abc012ABC012?????");
	testAssert(charset.decode(bytes) == L"abc012ABC012?????");
	charset = Charset(charset.codePage(), true);
	testAssert((bytes = charset.encode(testString)) == "abc012???????????");
	testAssert(charset.decode(bytes) == L"abc012???????????");
}


testCase(bigEndianUnicode) {
	Charset charset = Charset::bigEndianUnicode();
	testAssert(charset.codePage() == 1201);
	testAssert(charset.displayName() == L"Unicode (ビッグ エンディアン)" // .NETは Unicode (Big-Endian)
			|| charset.displayName() == L"Unicode (Big-Endian)"); // これはXPの表記
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"unicodeFEFF" // .NETは "utf-16BE"
			|| charset.name() == L"unicodeFFFE"); // これはXPの表記

	string temp = charset.encode(ByteStringRange(reinterpret_cast<const char*>(testString), String::getLength(testString) * sizeof(wchar_t)), Charset::unicode());
	temp        = Charset::unicode().encode(temp, charset);
	String result(reinterpret_cast<const wchar_t*>(temp.c_str()), temp.length() / sizeof(wchar_t));
	testAssert(result == testString);
}


testCase(charsets) {
	vector<Charset> charsets;
	testNoThrow(charsets = Charset::charsets());
	testAssert(!charsets.empty());
	for (auto i = charsets.begin(), end = charsets.end(); i != end; ++i) {
		auto name = i->name();
		//i->encode(L"abc");
		i->decode("abc");
	}
}


//testCase(codePage) {} // 各エンコーディング取得のstatic関数の testCase でテスト済み


testCase(decode) {
	{// 無効なパラメータ
		Charset charset(932);
		testAssertionFailed(charset.decode("012", -1));
		testAssertionFailed(charset.decode("012",  4));
		testNoThrow        (charset.decode("012",  3));
		wchar_t buffer[4];
		testAssertionFailed(charset.decode("012", -1, buffer));
		testAssertionFailed(charset.decode("012",  4, buffer));
		testAssertionFailed(charset.decode("012",  3, ArrayRange<wchar_t>(buffer, 0)));
		//testAssertionFailed(Charset::bigEndianUnicode().decode("012",  3, buffer));
		//testAssertionFailed(Charset::unicode().decode("012",  3, buffer));
		testNoThrow        (charset.decode("012",  3, buffer));
	}
	{// バッファの長さが足りない（WindowsCode版）
		Charset charset(932, true);
		wchar_t buffer[3];
		testThrow  (charset.decode("012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.decode("012", 2, buffer));
	}
	{// バッファの長さが足りない
		Charset charset(932);
		wchar_t buffer[3];
		testThrow  (charset.decode("012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.decode("012", 2, buffer));
	}
	{// 正常ケース 1（WindowsCode版）
		Charset charset(932, true);
		string srcString = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcString) == testString);
	}
	{// 正常ケース 1
		Charset charset(L"shift_jis");
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes) == testString);
	}
	{// 正常ケース 2　空文字（WindowsCode版)
		Charset charset(932, true);
		testAssert(charset.decode("") == L"");
		testAssert(charset.decode("", 0) == L"");
		wchar_t buffer[1];
		testAssert(charset.decode("", 0, buffer) == 0);
		testAssert(buffer == String());
	}
	{// 正常ケース 2　空文字
		Charset charset(L"euc-jp");
		testAssert(charset.decode("") == L"");
		testAssert(charset.decode("", 0) == L"");
		wchar_t buffer[1];
		testAssert(charset.decode("", 0, buffer) == 0);
		testAssert(buffer == String());
	}
	{// 正常ケース 3（WindowsCode版）
		Charset charset(932, true);
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes, 15) == String(testString, 15));
	}
	{// 正常ケース 3
		Charset charset(L"us-ascii");
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes, 15) == String(testString, 15));
	}
	{// 正常ケース 4（WindowsCode版）
		Charset charset(65001, true);
		string srcBytes = getResourceString(L"utf-8.txt");
		wchar_t buffer[41];
		testAssert(charset.decode(srcBytes, srcBytes.length(), buffer) == 40);
		testAssert(buffer == String(testString));
	}
	{// 正常ケース 4
		Charset charset(L"utf-8");
		string srcBytes = getResourceString(L"utf-8.txt");
		wchar_t buffer[41];
		testAssert(charset.decode(srcBytes, srcBytes.length(), buffer) == 40);
		testAssert(buffer == String(testString));
	}
}


testCase(default) {
	Charset charset = Charset::default(); // Here is Japan!!
	testAssert(charset.codePage() == 932);
	testAssert(charset.displayName() == L"日本語 (シフト JIS)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"shift_jis");

	string bytes = getResourceString(L"shift_jis.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	charset = Charset(charset.codePage(), true);
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
}


//testCase(displayName) {} // 各エンコーディング取得のstatic関数の testCase でテスト済み


testCase(encodeFromByteString) {
	{// 無効なパラメータ
		Charset src(932);
		Charset dst(51932);
		testAssertionFailed(dst.encode("012", -1, src));
		testAssertionFailed(dst.encode("012",  4, src));
		testNoThrow        (dst.encode("012",  3, src));
		char buffer[4];
		testAssertionFailed(dst.encode("012", -1, buffer, src));
		testAssertionFailed(dst.encode("012",  4, buffer, src));
		testAssertionFailed(dst.encode("012",  3, ArrayRange<char>(buffer, 0), src));
		testNoThrow        (dst.encode("012",  3, buffer, src));
	}
	{// バッファの長さが足りない
		Charset src(932);
		Charset dst(51932);
		char buffer[3];
		testThrow  (dst.encode("012", 3, buffer, src), Charset::InsufficientBufferExceptoin);
		testNoThrow(dst.encode("012", 2, buffer, src));
	}
	{// 不正な文字の区切り（区切れた文字は無視）
		Charset src(L"shift_jis");
		Charset dst(L"euc-jp");
		char shift_jisBytes[] = "あい";
		src.throwable(true);
		string temp = dst.encode(shift_jisBytes, 3, src);
		testAssert(temp.length() == 2);
		testAssert(src.encode(temp, dst) == "あ");
	}
	{// 正常ケース 1
		Charset src(L"shift_jis");
		Charset dst(L"utf-8");
		string srcBytes = getResourceString(L"shift_jis.txt");
		string dstBytes = getResourceString(L"utf-8.txt");
		testAssert(srcBytes != dstBytes);
		testAssert(dst.encode(srcBytes, src) == dstBytes);
	}
	{// 正常ケース 2　空文字
		Charset src(L"shift_jis");
		Charset dst(L"euc-jp");
		testAssert(dst.encode("", src) == "");
		testAssert(dst.encode("", 0, src) == "");
		char buffer[1];
		testAssert(dst.encode("", 0, buffer, src) == 0);
		testAssert(buffer == string());
	}
	{// 正常ケース 3（直接変換はサポートされないがunicodeを介して変換）
		Charset src(L"iso-2022-jp");
		Charset dst(L"euc-jp");
		string srcBytes = getResourceString(L"jis.txt");
		string dstBytes = getResourceString(L"euc-jp.txt");
		testAssert(srcBytes != dstBytes);
		testAssert(dst.encode(srcBytes, 10, src) == string(dstBytes.begin(), dstBytes.begin() + 10));
	}
	{// 正常ケース 4（unicodeを介してfallback挙動）
		Charset src(L"shift_jis");
		Charset dst(L"us-ascii");
		char buffer[4];
		testAssert(dst.encode("aいう", 5, buffer, src) == 3);
		testAssert(buffer == string("a??"));
	}
	{// 正常ケース 5 Unicodeの混ざった変換
		Charset src(L"shift_jis");
		Charset dst = Charset::unicode();
		string temp = dst.encode("abcあいう", src);
		String result(reinterpret_cast<const wchar_t*>(temp.c_str()), temp.length() / sizeof(wchar_t));
		testAssert(result == L"abcあいう");
	}
	{// 正常ケース 6 Unicodeの混ざった変換
		Charset src = Charset::unicode();
		Charset dst(L"shift_jis");
		String srcChars = L"abcあいう";
		testAssert(dst.encode(ByteStringRange(reinterpret_cast<const char*>(srcChars.c_str()), srcChars.length() * sizeof(wchar_t)), src) == "abcあいう");
	}
	{// 正常ケース 7 Unicodeの混ざった変換
		Charset src = Charset::unicode();
		Charset dst = Charset::unicode();
		String srcChars = L"abcあいう";
		string temp = dst.encode(ByteStringRange(reinterpret_cast<const char*>(srcChars.c_str()), srcChars.length() * sizeof(wchar_t)), src);
		String result(reinterpret_cast<const wchar_t*>(temp.c_str()), temp.length() / sizeof(wchar_t));
		testAssert(result == srcChars);
	}
}


testCase(encode) {
	{// 無効なパラメータ
		Charset charset(932);
		testAssertionFailed(charset.encode(L"012", -1));
		testAssertionFailed(charset.encode(L"012",  4));
		testNoThrow        (charset.encode(L"012",  3));
		char buffer[4];
		testAssertionFailed(charset.encode(L"012", -1, buffer));
		testAssertionFailed(charset.encode(L"012",  4, buffer));
		testAssertionFailed(charset.encode(L"012",  3, ArrayRange<char>(buffer, 0)));
		//testAssertionFailed(Charset::bigEndianUnicode().encode(L"012",  3, buffer));
		//testAssertionFailed(Charset::unicode().encode(L"012",  3, buffer));
		testNoThrow        (charset.encode(L"012",  3, buffer));
	}
	{// バッファの長さが足りない（WindowsCode版）
		Charset charset(932, true);
		char buffer[3];
		testThrow  (charset.encode(L"012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.encode(L"012", 2, buffer));
	}
	{// バッファの長さが足りない
		Charset charset(932);
		char buffer[3];
		testThrow  (charset.encode(L"012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.encode(L"012", 2, buffer));
	}
	{// 正常ケース 1（WindowsCode版）
		Charset charset(932, true);
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString) == resultBytes);
	}
	{// 正常ケース 1
		Charset charset(L"shift_jis");
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString) == resultBytes);
	}
	{// 正常ケース 2　空文字（WindowsCode版)
		Charset charset(932, true);
		testAssert(charset.encode(L"") == "");
		testAssert(charset.encode(L"", 0) == "");
		char buffer[1];
		testAssert(charset.encode(L"", 0, buffer) == 0);
		testAssert(buffer == string());
	}
	{// 正常ケース 2　空文字
		Charset charset(L"euc-jp");
		testAssert(charset.encode(L"") == "");
		testAssert(charset.encode(L"", 0) == "");
		char buffer[1];
		testAssert(charset.encode(L"", 0, buffer) == 0);
		testAssert(buffer == string());
	}
	{// 正常ケース 3（WindowsCode版）
		Charset charset(932, true);
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString, 15) == string(resultBytes.begin(), resultBytes.begin() + 15));
	}
	{// 正常ケース 3
		Charset charset(L"us-ascii");
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString, 15) == string(resultBytes.begin(), resultBytes.begin() + 15));
	}
	{// 正常ケース 4（WindowsCode版）
		Charset charset(65001, true);
		string resultBytes = getResourceString(L"utf-8.txt");
		char buffer[91];
		testAssert(charset.encode(testString, String::getLength(testString), buffer) == 90);
		testAssert(buffer == resultBytes);
	}
	{// 正常ケース 4
		Charset charset(L"utf-8");
		string resultBytes = getResourceString(L"utf-8.txt");
		char buffer[91];
		testAssert(charset.encode(testString, String::getLength(testString), buffer) == 90);
		testAssert(buffer == resultBytes);
	}
	{// 正常ケース 5 fallback（WindowsCode版）
		Charset charset(20127, true);
		String testString = L"abc012ＡＢＣ０１２あいう漢字";
		testAssert(charset.encode(testString) == "abc012???????????");
		charset.fallbackChar(L'+');
		testAssert(charset.fallbackChar() == L'+');
		testAssert(charset.encode(testString) == "abc012+++++++++++");
		charset.throwable(true);
		testAssert(charset.throwable() == true);
		testThrow(charset.encode(testString), Charset::ConversionFailedException);
		testNoThrow(charset.encode(testString, 6));
	}
	{// 正常ケース 5 fallback
		Charset charset(L"us-ascii");
		String testString = L"abc012ＡＢＣ０１２あいう漢字";
		testAssert(charset.encode(testString) == "abc012ABC012?????"); // mlangのほうが高性能！
		charset.fallbackChar(L'+');
		testAssert(charset.fallbackChar() == L'+');
		testAssert(charset.encode(testString) == "abc012ABC012+++++");
		charset.throwable(true);
		testAssert(charset.throwable() == true);
		testThrow(charset.encode(testString), Charset::ConversionFailedException);
		testNoThrow(charset.encode(testString, 12));
	}
}


testCase(eucJp) {
	Charset charset(L"euc-jp");
	testAssert(charset.codePage() == 51932);
	testAssert(charset.displayName() == L"日本語 (EUC)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"euc-jp");

	string bytes = getResourceString(L"euc-jp.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	testAssertionFailed(Charset(charset.codePage(), true));
}


//testCase(fallbackChar) {} // testCase(encode) でテスト済み


//testCase(name) {} // 各エンコーディング取得のstatic関数の testCase でテスト済み

//testCase(shiftJis) {} // testCase(default) でテスト済み

//testCase(throwable) {} // testCase(encode) でテスト済み


testCase(unicode) {
	Charset charset = Charset::unicode();
	testAssert(charset.codePage() == 1200);
	testAssert(charset.displayName() == L"Unicode"); // .NETは Unicode
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"unicode"); // .NETは "utf-16"

	string temp = charset.encode(L"abc");
	testAssert(charset.decode(temp) == L"abc");
	testAssertionFailed(Charset(charset.codePage(), true)); // 1200はwindowsCodePageとしては不正な模様
}


testCase(utf7) {
	Charset charset = Charset::utf7();
	testAssert(charset.codePage() == 65000);
	testAssert(charset.displayName() == L"Unicode (UTF-7)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"utf-7");

	string bytes = getResourceString(L"utf-7.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	charset = Charset(charset.codePage(), true);
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
}


testCase(utf8) {
	Charset charset = Charset::utf8();
	testAssert(charset.codePage() == 65001);
	testAssert(charset.displayName() == L"Unicode (UTF-8)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"utf-8");

	string bytes = getResourceString(L"utf-8.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	charset = Charset(charset.codePage(), true);
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
}


testCase(jis) { // オマケのテストケース
	Charset charset(L"csISO2022JP");
	//Charset charset(L"iso-2022-jp"); // こっちだと50220のほうになる
	testAssert(charset.codePage() == 50221);
	testAssert(charset.displayName() == L"日本語 (JIS 1 バイト カタカナ可)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"csISO2022JP");

	string bytes = getResourceString(L"jis.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	charset = Charset(charset.codePage(), true); // windowsCodePage としては正しいようだが
	testAssertionFailed(charset.encode(testString)); // 変換には失敗する。なぜだ
	testAssert(charset.decode(bytes) != String(testString)); // 全角「－」を半角「-」に変換している。どういうこっちゃ
}


testCase(cleanup) { // ＣＯＭの開放
	Com::uninitialize();
}



		}
	}
}