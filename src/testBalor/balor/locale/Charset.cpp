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



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	balor::test::UnitTest::ignoreHandleLeakCheck();

	Com::initialize(); // CoInitializeEx 함수는 핸들을 처음 호출에서 확보하고, 내부에서 계속 유지한다
	{
		ComPtr<IMultiLanguage2> ptr(CLSID_CMultiLanguage); // CoCreateInstance 함수는 핸들을 처음 호출에서 확보하고, 내부에서 계속 유지한다
	}
	Com::uninitialize();

	testAssert(SUCCEEDED(CoInitialize(nullptr))); // CoInitialize 함수는 핸들을 처음 호출에서 확보하고, 내부에서 계속 유지한다
	CoUninitialize();

	IsValidCodePage(0); // IsValidCodePage 함수는 핸들을 처음 호출에서 확보하고, 내부에서 계속 유지한다
}


testCase(ComInitialize) { // COM 초기화 코드와 공존. 실제로 까다롭지만 어떻게 할 수 없다
	testAssert(!Com::initialized()); // 미 초기화 되었을 것

	{// Charset가 COM을 사용하지 않는 패턴
		{
			Charset charset = Charset::default();
			testAssert(charset.encode(L"abc") == "abc");
			testAssert(charset.decode("abc") == L"abc");
		}
		testAssert(!Com::initialized());
	}
	{// Charset가 COM을 초기화 한다  
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		testAssert(Com::initialized());
		Com::uninitialize();
	}
	{// COM을 MTA로 초기화 해 두고 Charset을 사용한다.
		Com::initialize(false);
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		Com::uninitialize();
	}
	{// COM을 STA로 초기화 해 두고 Charset을 사용한다
		Com::initialize();
		{
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		}
		Com::uninitialize();
	}
	{// COM을 MTA로 초기화 해 두고 다른 스레드에서 Charset을 사용한다.
		Com::initialize(false);
		thread workor([] () {
			Charset charset(1200);
			testAssert(charset.name() == L"unicode");
		});
		workor.join();
		Com::uninitialize();
	}
	{// COM을 STA로 초기화 해 두고 다른 스레드에서 STA의 Charset을 사용한다.
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
	{// Charset 이 다른 스레드에서 MTA 의 COM을 초기화 한다 
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


testCase(secondStartup) { // 핸들 릭 체크에 걸리지 않도록 처음부터 COM 초기화를 끝내둔다
	balor::test::UnitTest::ignoreHandleLeakCheck();
	Charset charset(932);
	Charset::charsets(); // IEnumCodePage->Next 함수는 핸들을 처음 호출에서 확보하고, 내부에서 계속 유지한다
}


testCase(codePageAndIsWindowsCodePageConstruct) {
	// 무효한 파라미터
	testAssertionFailed(Charset(-1));
	testAssertionFailed(Charset(65536));

	{// isWindowsCodePage = true 이면서 windows 코드 페이지는 없다
		testAssertionFailed(Charset(51932, true));
		testNoThrow(Charset(932, true));
	}

	{// mlang 의 지원 범위 외의 코드 페이지
		testThrow(Charset(7777), Charset::NotFoundException);
		testThrow(Charset(1), Charset::NotFoundException);
		testNoThrow(Charset(51932));
	}

	{// 정상 케이스 1
		Charset shift_jis(932, true);
		testAssert(shift_jis.codePage() == 932);
		testAssert(shift_jis.name() == L"shift_jis");
		testAssert(shift_jis.fallbackChar() == L'?');
		testAssert(!shift_jis.throwable());
	}
	{// 정상 케이스 2
		Charset big5(950);
		testAssert(big5.codePage() == 950);
		testAssert(big5.name() == L"big5");
	}
}


testCase(nameConstruct) {
	{// 무효한 인코딩 이름
		testThrow(Charset(L"unknowncode"), Charset::NotFoundException);
	}

	{// 정상 케이스
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
	testAssert(charset.displayName() == L"Unicode (긮긞긐 긄깛긢귻귺깛)" // .NET은 Unicode (Big-Endian)
			|| charset.displayName() == L"Unicode (Big-Endian)"); // 여기는 XP 표기
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"unicodeFEFF" // .NET는 "utf-16BE"
			|| charset.name() == L"unicodeFFFE"); // 여기는 XP 표기 

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




testCase(decode) {
	{// 무효한 파라미터
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
	{// 버퍼 길이가 부족하지 않다(WindowsCode 판)
		Charset charset(932, true);
		wchar_t buffer[3];
		testThrow  (charset.decode("012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.decode("012", 2, buffer));
	}
	{// 버퍼 길이가 부족하지 않다
		Charset charset(932);
		wchar_t buffer[3];
		testThrow  (charset.decode("012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.decode("012", 2, buffer));
	}
	{// 정상 케이스 1(WindowsCode 판)
		Charset charset(932, true);
		string srcString = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcString) == testString);
	}
	{// 정상 케이스 1
		Charset charset(L"shift_jis");
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes) == testString);
	}
	{// 정상 케이스 2 빈 문자(WindowsCode 판)
		Charset charset(932, true);
		testAssert(charset.decode("") == L"");
		testAssert(charset.decode("", 0) == L"");
		wchar_t buffer[1];
		testAssert(charset.decode("", 0, buffer) == 0);
		testAssert(buffer == String());
	}
	{// 정상 케이스 2 빈문자
		Charset charset(L"euc-jp");
		testAssert(charset.decode("") == L"");
		testAssert(charset.decode("", 0) == L"");
		wchar_t buffer[1];
		testAssert(charset.decode("", 0, buffer) == 0);
		testAssert(buffer == String());
	}
	{// 정상 케이스 3(WindowsCode 판)
		Charset charset(932, true);
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes, 15) == String(testString, 15));
	}
	{// 정상 케이스 3
		Charset charset(L"us-ascii");
		string srcBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.decode(srcBytes, 15) == String(testString, 15));
	}
	{// 정상 케이스 4(WindowsCode 판)
		Charset charset(65001, true);
		string srcBytes = getResourceString(L"utf-8.txt");
		wchar_t buffer[41];
		testAssert(charset.decode(srcBytes, srcBytes.length(), buffer) == 40);
		testAssert(buffer == String(testString));
	}
	{// 정상 케이스 4
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
	testAssert(charset.displayName() == L"볷?뚭 (긘긲긣 JIS)");
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



testCase(encodeFromByteString) {
	{// 무효한 파라미터
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
	{// 버퍼 길이가 부족하지 않다
		Charset src(932);
		Charset dst(51932);
		char buffer[3];
		testThrow  (dst.encode("012", 3, buffer, src), Charset::InsufficientBufferExceptoin);
		testNoThrow(dst.encode("012", 2, buffer, src));
	}
	{// 부정한 문자 구별(구별하는 문자는 무시)
		Charset src(L"shift_jis");
		Charset dst(L"euc-jp");
		char shift_jisBytes[] = "あい";
		src.throwable(true);
		string temp = dst.encode(shift_jisBytes, 3, src);
		testAssert(temp.length() == 2);
		testAssert(src.encode(temp, dst) == "あ");
	}
	{// 정상 케이스 1
		Charset src(L"shift_jis");
		Charset dst(L"utf-8");
		string srcBytes = getResourceString(L"shift_jis.txt");
		string dstBytes = getResourceString(L"utf-8.txt");
		testAssert(srcBytes != dstBytes);
		testAssert(dst.encode(srcBytes, src) == dstBytes);
	}
	{// 정상 케이스 2 빈 문자
		Charset src(L"shift_jis");
		Charset dst(L"euc-jp");
		testAssert(dst.encode("", src) == "");
		testAssert(dst.encode("", 0, src) == "");
		char buffer[1];
		testAssert(dst.encode("", 0, buffer, src) == 0);
		testAssert(buffer == string());
	}
	{// 정상 케이스 3(직접 변환은 지원 되지 않지만 unicode를 통해서 변환)
		Charset src(L"iso-2022-jp");
		Charset dst(L"euc-jp");
		string srcBytes = getResourceString(L"jis.txt");
		string dstBytes = getResourceString(L"euc-jp.txt");
		testAssert(srcBytes != dstBytes);
		testAssert(dst.encode(srcBytes, 10, src) == string(dstBytes.begin(), dstBytes.begin() + 10));
	}
	{// 정상 케이스 4(unicode를 통해서 fallback 거동)
		Charset src(L"shift_jis");
		Charset dst(L"us-ascii");
		char buffer[4];
		testAssert(dst.encode("a궋궎", 5, buffer, src) == 3);
		testAssert(buffer == string("a??"));
	}
	{// 정상 케이스 5 Unicode가 썩인 변환
		Charset src(L"shift_jis");
		Charset dst = Charset::unicode();
		string temp = dst.encode("abcあいう", src);
		String result(reinterpret_cast<const wchar_t*>(temp.c_str()), temp.length() / sizeof(wchar_t));
		testAssert(result == L"abcあいう");
	}
	{// 정상 케이스 6 Unicode가 썩인 변환
		Charset src = Charset::unicode();
		Charset dst(L"shift_jis");
		String srcChars = L"abcあいう";
		testAssert(dst.encode(ByteStringRange(reinterpret_cast<const char*>(srcChars.c_str()), srcChars.length() * sizeof(wchar_t)), src) == "abc궇궋궎");
	}
	{// 정상 케이스 7 Unicode가 썩인 변환
		Charset src = Charset::unicode();
		Charset dst = Charset::unicode();
		String srcChars = L"abcあいう";
		string temp = dst.encode(ByteStringRange(reinterpret_cast<const char*>(srcChars.c_str()), srcChars.length() * sizeof(wchar_t)), src);
		String result(reinterpret_cast<const wchar_t*>(temp.c_str()), temp.length() / sizeof(wchar_t));
		testAssert(result == srcChars);
	}
}


testCase(encode) {
	{// 무효한 파라미터
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
	{// 버퍼 길이가 부족하지 않다(WindowsCode 판)
		Charset charset(932, true);
		char buffer[3];
		testThrow  (charset.encode(L"012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.encode(L"012", 2, buffer));
	}
	{// 버퍼 길이가 부족하지 않다
		Charset charset(932);
		char buffer[3];
		testThrow  (charset.encode(L"012", 3, buffer), Charset::InsufficientBufferExceptoin);
		testNoThrow(charset.encode(L"012", 2, buffer));
	}
	{// 정상 케이스 1(WindowsCode 판)
		Charset charset(932, true);
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString) == resultBytes);
	}
	{// 정상 케이스 1
		Charset charset(L"shift_jis");
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString) == resultBytes);
	}
	{// 정상 케이스 2 빈 문자(WindowsCode 판)
		Charset charset(932, true);
		testAssert(charset.encode(L"") == "");
		testAssert(charset.encode(L"", 0) == "");
		char buffer[1];
		testAssert(charset.encode(L"", 0, buffer) == 0);
		testAssert(buffer == string());
	}
	{// 정상 케이스 2 빈 문자
		Charset charset(L"euc-jp");
		testAssert(charset.encode(L"") == "");
		testAssert(charset.encode(L"", 0) == "");
		char buffer[1];
		testAssert(charset.encode(L"", 0, buffer) == 0);
		testAssert(buffer == string());
	}
	{// 정상 케이스 3(WindowsCode 판)
		Charset charset(932, true);
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString, 15) == string(resultBytes.begin(), resultBytes.begin() + 15));
	}
	{// 정상 케이스 3
		Charset charset(L"us-ascii");
		string resultBytes = getResourceString(L"shift_jis.txt");
		testAssert(charset.encode(testString, 15) == string(resultBytes.begin(), resultBytes.begin() + 15));
	}
	{// 정상 케이스 4(WindowsCode 판)
		Charset charset(65001, true);
		string resultBytes = getResourceString(L"utf-8.txt");
		char buffer[91];
		testAssert(charset.encode(testString, String::getLength(testString), buffer) == 90);
		testAssert(buffer == resultBytes);
	}
	{// 정상 케이스 4
		Charset charset(L"utf-8");
		string resultBytes = getResourceString(L"utf-8.txt");
		char buffer[91];
		testAssert(charset.encode(testString, String::getLength(testString), buffer) == 90);
		testAssert(buffer == resultBytes);
	}
	{// 정상 케이스 5 fallback(WindowsCode 판)
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
	{// 정상 케이스 5 fallback
		Charset charset(L"us-ascii");
		String testString = L"abc012ＡＢＣ０１２あいう漢字";
		testAssert(charset.encode(testString) == "abc012ABC012?????"); // mlang 쪽이 고 성능 
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
	testAssert(charset.displayName() == L"볷?뚭 (EUC)");
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"euc-jp");

	string bytes = getResourceString(L"euc-jp.txt");
	testAssert(charset.encode(testString) == bytes);
	testAssert(charset.decode(bytes) == testString);
	testAssertionFailed(Charset(charset.codePage(), true));
}




testCase(unicode) {
	Charset charset = Charset::unicode();
	testAssert(charset.codePage() == 1200);
	testAssert(charset.displayName() == L"Unicode"); // .NET은 Unicode
	testAssert(charset.fallbackChar() == L'?');
	testAssert(charset.throwable() == false);
	testAssert(charset.name() == L"unicode"); // .NET은 "utf-16"

	string temp = charset.encode(L"abc");
	testAssert(charset.decode(temp) == L"abc");
	testAssertionFailed(Charset(charset.codePage(), true)); // 1200은 windowsCodePage로서는 부정한 사양 
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



testCase(cleanup) { // COM 개시
	Com::uninitialize();
}



		}
	}
}