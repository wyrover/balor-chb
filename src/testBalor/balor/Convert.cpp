#include <balor/Convert.hpp>

#include <cmath>
#include <string>
#include <limits>

#include <balor/locale/Locale.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>
#include "../tools/floatEquals.hpp"


namespace balor {
	namespace testConvert {


using std::move;
using std::wstring;
using namespace balor::locale;
using namespace balor::Convert;
using tools::floatEquals;



testCase(startup) {
	balor::test::UnitTest::ignoreHandleLeakCheck();
	_wsetlocale(LC_ALL, Locale(L"de-DE").cLocaleName().c_str()); // 로컬 영향을 받지 않는 것을 증명으로서 독일어로 해둔다 
}


testCase(toAnyFromStringLikeType) {
	// StringRange 에 변환 가능한 형은 모두 StringRange 로 변환 되고, StringRange 판이 호출되는 것을 확인 
	{
		wchar_t* string0 = L"0x100";
		testAssert(to<int>(string0, 16) == 0x100);
	}
	{
		wchar_t string0[] = L"010";
		testAssert(to<int>(string0, 8) == 8);
	}
	{
		const wchar_t* string0 = L"3.14";
		testAssert(to<double>(string0) == 3.14);
	}
	{
		const wchar_t string0[] = L"1111";
		testAssert(to<short>(string0, 2) == 15);
	}
	{
		wstring string0 = L"127";
		testAssert(to<char>(string0) == 127);
	}
	{
		String string0 = L"9223372036854775807";
		testAssert(to<__int64>(string0) == 9223372036854775807);
	}
}


testCase(to__int64FromStringRange) {
	{// 에러 케이스 
		testAssertionFailed(to<__int64>(L"0", 1));
		testThrow(to<__int64>(L""), Convert::StringFormatException);
		testThrow(to<__int64>(L"a10"), Convert::StringFormatException);
		testAssert(to<__int64>(L"10a") == 10);
		testAssert(to<__int64>(L"01") == 1);
		testThrow(to<__int64>(L"f"), Convert::StringFormatException);
		testAssert(to<__int64>(L"f", 16) == 15);
		testThrow(to<__int64>(L"-"), Convert::StringFormatException);
		testAssert(to<__int64>(L"-1") == -1);
		testThrow(to<__int64>(L"-9223372036854775809"), Convert::OverflowException);
		testAssert(to<__int64>(L"-9223372036854775808") == -9223372036854775808i64);
		testThrow(to<__int64>(L"9223372036854775808"), Convert::OverflowException);
		testAssert(to<__int64>(L"9223372036854775807") == 9223372036854775807i64);
	}
	{// base 검증 
		testAssert(to<__int64>(L"1000000000000000000000000000000000000000000000000000000000000000", 2) == -9223372036854775808i64);
		testAssert(to<__int64>(L"111111111111111111111111111111111111111111111111111111111111111", 2) == 9223372036854775807i64);
		testAssert(to<__int64>(L"1777777777777777777777", 8) == -1i64);
		testAssert(to<__int64>(L"20", 8) == 16i64);
		testAssert(to<__int64>(L"0xffffffffffffffff", 16) == -1i64);
		testAssert(to<__int64>(L"f", 16) == 15i64);
	}
}


testCase(toCharFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<char>(L"0", 1));
		testThrow(to<char>(L""), Convert::StringFormatException);
		testThrow(to<char>(L"a10"), Convert::StringFormatException);
		testAssert(to<char>(L"10a") == 10);
		testAssert(to<char>(L"01") == 1);
		testThrow(to<char>(L"f"), Convert::StringFormatException);
		testAssert(to<char>(L"f", 16) == 15);
		testThrow(to<char>(L"-"), Convert::StringFormatException);
		testAssert(to<char>(L"-1") == -1);
		testThrow(to<char>(L"-129"), Convert::OverflowException);
		testAssert(to<char>(L"-128") == -128);
		testThrow(to<char>(L"128"), Convert::OverflowException);
		testAssert(to<char>(L"127") == 127);
	}
	{// base 검증
		testAssert(to<char>(L"10000000", 2) == -128);
		testAssert(to<char>(L"1111111", 2) == 127);
		testAssert(to<char>(L"377", 8) == -1);
		testAssert(to<char>(L"20", 8) == 16);
		testAssert(to<char>(L"0xFF", 16) == -1);
		testAssert(to<char>(L"F", 16) == 15);
	}
}


testCase(toDoubleFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<double>(L"0", 16));
		testThrow(to<double>(L""), Convert::StringFormatException);
		testThrow(to<double>(L"a10"), Convert::StringFormatException);
		testAssert(floatEquals(to<double>(L"10a"), 10.));
		testAssert(floatEquals(to<double>(L"01"), 1.));
		testThrow(to<double>(L"-"), Convert::StringFormatException);
		testAssert(floatEquals(to<double>(L"-1"), -1.));
		testThrow(to<double>(L"-1.7976931348623159e+308"), Convert::OverflowException);
		testAssert(floatEquals(to<double>(L"-1.7976931348623158e+308"), -1.7976931348623158e+308));
		testThrow(to<double>(L"1.7976931348623159e+308"), Convert::OverflowException);
		testAssert(floatEquals(to<double>(L"1.7976931348623158e+308"), 1.7976931348623158e+308));
	}
	{// 부동소수 형식 검증
		testAssert(floatEquals(to<double>(L".0"), 0.));
		testAssert(floatEquals(to<double>(L"3.14"), 3.14));
		testAssert(floatEquals(to<double>(L"2.2204460492503131e-016"), 2.2204460492503131e-016));
		// ラウンドトリップが成り立たないが仕様なので仕方あるまい
		testAssert(floatEquals(to<double>(L"1.#INF"), 1.0));
		testAssert(floatEquals(to<double>(L"1.#QNAN"), 1.0));
	}
}


testCase(toFloatFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<float>(L"0", 16));
		testThrow(to<float>(L""), Convert::StringFormatException);
		testThrow(to<float>(L"a10"), Convert::StringFormatException);
		testAssert(floatEquals(to<float>(L"10a"), 10.f));
		testAssert(floatEquals(to<float>(L"01"), 1.f));
		testThrow(to<float>(L"-"), Convert::StringFormatException);
		testAssert(floatEquals(to<float>(L"-1"), -1.f));
		testThrow(to<float>(L"-3.402823467e+38F"), Convert::OverflowException);
		testAssert(floatEquals(to<float>(L"-3.402823466e+38F"), -3.402823466e+38F));
		testThrow(to<float>(L"3.402823467e+38F"), Convert::OverflowException);
		testAssert(floatEquals(to<float>(L"3.402823466e+38F"), 3.402823466e+38F));
	}
	{// 부동소수 형식 검증
		testAssert(floatEquals(to<float>(L".0"), 0.f));
		testAssert(floatEquals(to<float>(L"3.14"), 3.14f));
		testAssert(floatEquals(to<float>(L"1.192092896e-07F"), 1.192092896e-07F));
		// ラウンドトリップが成り立たないが仕様なので仕方あるまい
		testAssert(floatEquals(to<float>(L"1.#INF"), 1.f));
		testAssert(floatEquals(to<float>(L"1.#QNAN"), 1.f));
	}
}


testCase(toIntFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<int>(L"0", 1));
		testThrow(to<int>(L""), Convert::StringFormatException);
		testThrow(to<int>(L"a10"), Convert::StringFormatException);
		testAssert(to<int>(L"10a") == 10);
		testAssert(to<int>(L"01") == 1);
		testThrow(to<int>(L"f"), Convert::StringFormatException);
		testAssert(to<int>(L"f", 16) == 15);
		testThrow(to<int>(L"-"), Convert::StringFormatException);
		testAssert(to<int>(L"-1") == -1);
		testThrow(to<int>(L"-2147483649"), Convert::OverflowException);
		testAssert(to<int>(L"-2147483648") == -2147483647L - 1);
		testThrow(to<int>(L"2147483648"), Convert::OverflowException);
		testAssert(to<int>(L"2147483647") == 2147483647L);
	}
	{// base 검증
		testAssert(to<int>(L"10000000000000000000000000000000", 2) == -2147483647- 1);
		testAssert(to<int>(L"1111111111111111111111111111111", 2) == 2147483647);
		testAssert(to<int>(L"37777777777", 8) == -1);
		testAssert(to<int>(L"20", 8) == 16);
		testAssert(to<int>(L"0xffffffff", 16) == -1);
		testAssert(to<int>(L"f", 16) == 15);
	}
}


testCase(toLongFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<long>(L"0", 1));
		testThrow(to<long>(L""), Convert::StringFormatException);
		testThrow(to<long>(L"a10"), Convert::StringFormatException);
		testAssert(to<long>(L"10a") == 10);
		testAssert(to<long>(L"01") == 1);
		testThrow(to<long>(L"f"), Convert::StringFormatException);
		testAssert(to<long>(L"f", 16) == 15);
		testThrow(to<long>(L"-"), Convert::StringFormatException);
		testAssert(to<long>(L"-1") == -1);
		testThrow(to<long>(L"-2147483649"), Convert::OverflowException);
		testAssert(to<long>(L"-2147483648") == -2147483647L - 1);
		testThrow(to<long>(L"2147483648"), Convert::OverflowException);
		testAssert(to<long>(L"2147483647") == 2147483647L);
	}
	{// base 검증
		testAssert(to<long>(L"10000000000000000000000000000000", 2) == -2147483647- 1);
		testAssert(to<long>(L"1111111111111111111111111111111", 2) == 2147483647);
		testAssert(to<long>(L"37777777777", 8) == -1);
		testAssert(to<long>(L"20", 8) == 16);
		testAssert(to<long>(L"0xffffffff", 16) == -1);
		testAssert(to<long>(L"f", 16) == 15);
	}
}


testCase(toShortFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<short>(L"0", 1));
		testThrow(to<short>(L""), Convert::StringFormatException);
		testThrow(to<short>(L"a10"), Convert::StringFormatException);
		testAssert(to<short>(L"10a") == 10);
		testAssert(to<short>(L"01") == 1);
		testThrow(to<short>(L"f"), Convert::StringFormatException);
		testAssert(to<short>(L"f", 16) == 15);
		testThrow(to<short>(L"-"), Convert::StringFormatException);
		testAssert(to<short>(L"-1") == -1);
		testThrow(to<short>(L"-32769"), Convert::OverflowException);
		testAssert(to<short>(L"-32768") == -32768);
		testThrow(to<short>(L"32768"), Convert::OverflowException);
		testAssert(to<short>(L"32767") == 32767);
	}
	{// base 검증
		testAssert(to<short>(L"1000000000000000", 2) == -32768);
		testAssert(to<short>(L"111111111111111", 2) == 32767);
		testAssert(to<short>(L"177777", 8) == -1);
		testAssert(to<short>(L"20", 8) == 16);
		testAssert(to<short>(L"0xffff", 16) == -1);
		testAssert(to<short>(L"f", 16) == 15);
	}
}


testCase(toUnsigned__int64FromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<unsigned __int64>(L"0", 1));
		testThrow(to<unsigned __int64>(L""), Convert::StringFormatException);
		testThrow(to<unsigned __int64>(L"a10"), Convert::StringFormatException);
		testAssert(to<unsigned __int64>(L"10a") == 10);
		testAssert(to<unsigned __int64>(L"01") == 1);
		testThrow(to<unsigned __int64>(L"f"), Convert::StringFormatException);
		testAssert(to<unsigned __int64>(L"f", 16) == 15);
		testThrow(to<unsigned __int64>(L"-"), Convert::StringFormatException);
		testAssert(to<unsigned __int64>(L"-1") == 18446744073709551615ui64);
		testThrow(to<unsigned __int64>(L"18446744073709551616"), Convert::OverflowException);
		testAssert(to<unsigned __int64>(L"18446744073709551615") == 18446744073709551615ui64);
	}
	{// base 검증
		testAssert(to<unsigned __int64>(L"1111", 2) == 15);
		testAssert(to<unsigned __int64>(L"100", 8) == 64);
		testAssert(to<unsigned __int64>(L"100", 10) == 100);
		testAssert(to<unsigned __int64>(L"0x7F", 16) == 127);
	}
}


testCase(toUnsignedCharFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<unsigned char>(L"0", 1));
		testThrow(to<unsigned char>(L""), Convert::StringFormatException);
		testThrow(to<unsigned char>(L"a10"), Convert::StringFormatException);
		testAssert(to<unsigned char>(L"10a") == 10);
		testAssert(to<unsigned char>(L"01") == 1);
		testThrow(to<unsigned char>(L"f"), Convert::StringFormatException);
		testAssert(to<unsigned char>(L"f", 16) == 15);
		testThrow(to<unsigned char>(L"-"), Convert::StringFormatException);
		testThrow(to<unsigned char>(L"-1"), Convert::OverflowException);
		testThrow(to<unsigned char>(L"256"), Convert::OverflowException);
		testAssert(to<unsigned char>(L"255") == 255);
	}
	{// base 검증
		testAssert(to<unsigned char>(L"1111", 2) == 15);
		testAssert(to<unsigned char>(L"100", 8) == 64);
		testAssert(to<unsigned char>(L"100", 10) == 100);
		testAssert(to<unsigned char>(L"0x7F", 16) == 127);
	}
}


testCase(toUnsignedIntFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<unsigned int>(L"0", 1));
		testThrow(to<unsigned int>(L""), Convert::StringFormatException);
		testThrow(to<unsigned int>(L"a10"), Convert::StringFormatException);
		testAssert(to<unsigned int>(L"10a") == 10);
		testAssert(to<unsigned int>(L"01") == 1);
		testThrow(to<unsigned int>(L"f"), Convert::StringFormatException);
		testAssert(to<unsigned int>(L"f", 16) == 15);
		testThrow(to<unsigned int>(L"-"), Convert::StringFormatException);
		testAssert(to<unsigned int>(L"-1") == 4294967295);
		testThrow(to<unsigned int>(L"4294967296"), Convert::OverflowException);
		testAssert(to<unsigned int>(L"4294967295") == 4294967295);
	}
	{// base 검증
		testAssert(to<unsigned int>(L"1111", 2) == 15);
		testAssert(to<unsigned int>(L"100", 8) == 64);
		testAssert(to<unsigned int>(L"100", 10) == 100);
		testAssert(to<unsigned int>(L"0x7F", 16) == 127);
	}
}


testCase(toUnsignedLongFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<unsigned long>(L"0", 1));
		testThrow(to<unsigned long>(L""), Convert::StringFormatException);
		testThrow(to<unsigned long>(L"a10"), Convert::StringFormatException);
		testAssert(to<unsigned long>(L"10a") == 10);
		testAssert(to<unsigned long>(L"01") == 1);
		testThrow(to<unsigned long>(L"f"), Convert::StringFormatException);
		testAssert(to<unsigned long>(L"f", 16) == 15);
		testThrow(to<unsigned long>(L"-"), Convert::StringFormatException);
		testAssert(to<unsigned long>(L"-1") == 4294967295);
		testThrow(to<unsigned long>(L"4294967296"), Convert::OverflowException);
		testAssert(to<unsigned long>(L"4294967295") == 4294967295);
	}
	{// base 검증
		testAssert(to<unsigned long>(L"1111", 2) == 15);
		testAssert(to<unsigned long>(L"100", 8) == 64);
		testAssert(to<unsigned long>(L"100", 10) == 100);
		testAssert(to<unsigned long>(L"0x7F", 16) == 127);
	}
}


testCase(toUnsignedShortFromStringRange) {
	{// 에러 케이스
		testAssertionFailed(to<unsigned short>(L"0", 1));
		testThrow(to<unsigned short>(L""), Convert::StringFormatException);
		testThrow(to<unsigned short>(L"a10"), Convert::StringFormatException);
		testAssert(to<unsigned short>(L"10a") == 10);
		testAssert(to<unsigned short>(L"01") == 1);
		testThrow(to<unsigned short>(L"f"), Convert::StringFormatException);
		testAssert(to<unsigned short>(L"f", 16) == 15);
		testThrow(to<unsigned short>(L"-"), Convert::StringFormatException);
		testThrow(to<unsigned short>(L"-1"), Convert::OverflowException);
		testThrow(to<unsigned short>(L"65536"), Convert::OverflowException);
		testAssert(to<unsigned short>(L"65535") == 65535);
	}
	{// base 검증
		testAssert(to<unsigned short>(L"1111", 2) == 15);
		testAssert(to<unsigned short>(L"100", 8) == 64);
		testAssert(to<unsigned short>(L"100", 10) == 100);
		testAssert(to<unsigned short>(L"0x7F", 16) == 127);
	}
}


testCase(toStringFrom__int64) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1i64, 1));
	}
	{// base 검증
		testAssert(to<String>(-9223372036854775808i64, 2) == L"1000000000000000000000000000000000000000000000000000000000000000");
		testAssert(to<String>( 9223372036854775807i64, 2) ==  L"111111111111111111111111111111111111111111111111111111111111111");
		testAssert(to<String>(-1i64, 8) == L"1777777777777777777777");
		testAssert(to<String>(16i64, 8) == L"20");
		testAssert(to<String>(-1i64, 16) == L"ffffffffffffffff");
		testAssert(to<String>(15i64, 16) == L"f");
		testAssert(to<String>(-9223372036854775808i64) == L"-9223372036854775808");
		testAssert(to<String>( 9223372036854775807i64) ==  L"9223372036854775807");
	}
}


testCase(toStringFromChar) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<char>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<char>(-128), 2) == L"10000000");
		testAssert(to<String>(static_cast<char>( 127), 2) ==  L"1111111");
		testAssert(to<String>(static_cast<char>(-1), 8) == L"377");
		testAssert(to<String>(static_cast<char>(16), 8) == L"20");
		testAssert(to<String>(static_cast<char>(-1), 16) == L"ff");
		testAssert(to<String>(static_cast<char>(15), 16) == L"f");
		testAssert(to<String>(static_cast<char>(-128)) == L"-128");
		testAssert(to<String>(static_cast<char>( 127)) ==  L"127");
	}
}


testCase(toStringFromDouble) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1.0, 16));
	}
	{// 부동소수 형식 검증
		testAssert(to<String>(.0) ==  L"0");
		testAssert(to<String>(-3.14) == L"-3.14");
		testAssert(to<String>(2.2250738585072014e-308) == L"2.225073858507201e-308");
		testAssert(to<String>(1.7976931348623158e+308) == L"1.797693134862316e+308");
		testAssert(to<String>(std::numeric_limits<double>::infinity()) == L"1.#INF");
		testAssert(to<String>(std::numeric_limits<double>::quiet_NaN()) == L"1.#QNAN");
	}
	{// 라운드트립
		testAssert(floatEquals(to<double>(to<String>(3.14)), 3.14));
	}
}


testCase(toStringFromFloat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1.f, 16));
	}
	{// 부동소수 형식 검증
		testAssert(to<String>(.0f) ==  L"0");
		testAssert(to<String>(-3.14f) == L"-3.1400001");
		testAssert(to<String>(1.175494351e-38F) == L"1.1754944e-038");
		testAssert(to<String>(3.402823466e+38F) == L"3.4028235e+038");
		testAssert(to<String>(std::numeric_limits<float>::infinity()) == L"1.#INF");
		testAssert(to<String>(std::numeric_limits<float>::quiet_NaN()) == L"1.#QNAN");
	}
	{// 라운드트립
		testAssert(floatEquals(to<float>(to<String>(3.14f)), 3.14f));
	}
}


testCase(toStringFromInt) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<int>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<int>(-2147483647- 1), 2) == L"10000000000000000000000000000000");
		testAssert(to<String>(static_cast<int>( 2147483647   ), 2) ==  L"1111111111111111111111111111111");
		testAssert(to<String>(static_cast<int>(-1), 8) == L"37777777777");
		testAssert(to<String>(static_cast<int>(16), 8) == L"20");
		testAssert(to<String>(static_cast<int>(-1), 16) == L"ffffffff");
		testAssert(to<String>(static_cast<int>(15), 16) == L"f");
		testAssert(to<String>(static_cast<int>(-2147483647 - 1)) == L"-2147483648");
		testAssert(to<String>(static_cast<int>( 2147483647    )) ==  L"2147483647");
	}
}


testCase(toStringFromLong) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<long>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<long>(-2147483647- 1), 2) == L"10000000000000000000000000000000");
		testAssert(to<String>(static_cast<long>( 2147483647   ), 2) ==  L"1111111111111111111111111111111");
		testAssert(to<String>(static_cast<long>(-1), 8) == L"37777777777");
		testAssert(to<String>(static_cast<long>(16), 8) == L"20");
		testAssert(to<String>(static_cast<long>(-1), 16) == L"ffffffff");
		testAssert(to<String>(static_cast<long>(15), 16) == L"f");
		testAssert(to<String>(static_cast<long>(-2147483647 - 1)) == L"-2147483648");
		testAssert(to<String>(static_cast<long>( 2147483647    )) ==  L"2147483647");
	}
}


testCase(toStringFromShort) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<short>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<short>(-32768), 2) == L"1000000000000000");
		testAssert(to<String>(static_cast<short>( 32767), 2) ==  L"111111111111111");
		testAssert(to<String>(static_cast<short>(-1), 8) == L"177777");
		testAssert(to<String>(static_cast<short>(16), 8) == L"20");
		testAssert(to<String>(static_cast<short>(-1), 16) == L"ffff");
		testAssert(to<String>(static_cast<short>(15), 16) == L"f");
		testAssert(to<String>(static_cast<short>(-32768)) == L"-32768");
		testAssert(to<String>(static_cast<short>( 32767)) ==  L"32767");
	}
}


testCase(toStringFromUnsigned__int64) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1ui64, 1));
	}
	{// base 검증
		testAssert(to<String>(18446744073709551615ui64, 2) ==  L"1111111111111111111111111111111111111111111111111111111111111111");
		testAssert(to<String>(18446744073709551615ui64, 8) == L"1777777777777777777777");
		testAssert(to<String>(18446744073709551615ui64, 16) == L"ffffffffffffffff");
		testAssert(to<String>(18446744073709551615ui64) == L"18446744073709551615");
	}
}


testCase(toStringFromUnsignedChar) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned char>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned char>(255), 2) ==  L"11111111");
		testAssert(to<String>(static_cast<unsigned char>(255), 8) == L"377");
		testAssert(to<String>(static_cast<unsigned char>(255), 16) == L"ff");
		testAssert(to<String>(static_cast<unsigned char>(255)) == L"255");
	}
}


testCase(toStringFromUnsignedInt) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned int>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned int>(4294967295), 2) ==  L"11111111111111111111111111111111");
		testAssert(to<String>(static_cast<unsigned int>(4294967295), 8) == L"37777777777");
		testAssert(to<String>(static_cast<unsigned int>(4294967295), 16) == L"ffffffff");
		testAssert(to<String>(static_cast<unsigned int>(4294967295)) == L"4294967295");
	}
}


testCase(toStringFromUnsignedLong) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned long>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned long>(4294967295), 2) ==  L"11111111111111111111111111111111");
		testAssert(to<String>(static_cast<unsigned long>(4294967295), 8) == L"37777777777");
		testAssert(to<String>(static_cast<unsigned long>(4294967295), 16) == L"ffffffff");
		testAssert(to<String>(static_cast<unsigned long>(4294967295)) == L"4294967295");
	}
}


testCase(toStringFromUnsignedShort) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned short>(1), 1));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned short>(65535), 2) ==  L"1111111111111111");
		testAssert(to<String>(static_cast<unsigned short>(65535), 8) == L"177777");
		testAssert(to<String>(static_cast<unsigned short>(65535), 16) == L"ffff");
		testAssert(to<String>(static_cast<unsigned short>(65535)) == L"65535");
	}
}


testCase(toStringBufferFrom__int64) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1i64, 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, -9223372036854775808i64, 2); testAssert(buffer == L"1000000000000000000000000000000000000000000000000000000000000000");
		buffer.length(0); to<StringBuffer>(buffer,  9223372036854775807i64, 2); testAssert(buffer ==  L"111111111111111111111111111111111111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, -1i64, 8); testAssert(buffer == L"1777777777777777777777");
		buffer.length(0); to<StringBuffer>(buffer, 16i64, 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, -1i64, 16); testAssert(buffer == L"ffffffffffffffff");
		buffer.length(0); to<StringBuffer>(buffer, 15i64, 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, -9223372036854775808i64); testAssert(buffer == L"-9223372036854775808");
		buffer.length(0); to<StringBuffer>(buffer,  9223372036854775807i64); testAssert(buffer ==  L"9223372036854775807");
	}
}


testCase(toStringBufferFromChar) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<char>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-128), 2); testAssert(buffer == L"10000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 127), 2); testAssert(buffer ==  L"1111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-1), 8); testAssert(buffer == L"377");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(16), 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-1), 16); testAssert(buffer == L"ff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(15), 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-128)); testAssert(buffer == L"-128");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 127)); testAssert(buffer ==  L"127");
	}
}


testCase(toStringBufferFromDouble) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1.0, 16));
	}
	{// 부동소수 형식 검증
		buffer.length(0); to<StringBuffer>(buffer, .0); testAssert(buffer ==  L"0");
		buffer.length(0); to<StringBuffer>(buffer, -3.14); testAssert(buffer == L"-3.14");
		buffer.length(0); to<StringBuffer>(buffer, 2.2250738585072014e-308); testAssert(buffer == L"2.225073858507201e-308");
		buffer.length(0); to<StringBuffer>(buffer, 1.7976931348623158e+308); testAssert(buffer == L"1.797693134862316e+308");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<double>::infinity()); testAssert(buffer == L"1.#INF");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<double>::quiet_NaN()); testAssert(buffer == L"1.#QNAN");
	}
	{// 라운드트립
		buffer.length(0); to<StringBuffer>(buffer, 3.14); testAssert(floatEquals(to<double>(buffer), 3.14));
	}
}


testCase(toStringBufferFromFloat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1.f, 16));
	}
	{// 부동소수 형식 검증
		buffer.length(0); to<StringBuffer>(buffer, .0f); testAssert(buffer ==  L"0");
		buffer.length(0); to<StringBuffer>(buffer, -3.14f); testAssert(buffer == L"-3.1400001");
		buffer.length(0); to<StringBuffer>(buffer, 1.175494351e-38F); testAssert(buffer == L"1.1754944e-038");
		buffer.length(0); to<StringBuffer>(buffer, 3.402823466e+38F); testAssert(buffer == L"3.4028235e+038");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<float>::infinity()); testAssert(buffer == L"1.#INF");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<float>::quiet_NaN()); testAssert(buffer == L"1.#QNAN");
	}
	{// 라운드트립
		buffer.length(0); to<StringBuffer>(buffer, 3.14f); testAssert(floatEquals(to<float>(buffer), 3.14f));
	}
}


testCase(toStringBufferFromInt) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<int>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-2147483647- 1), 2); testAssert(buffer == L"10000000000000000000000000000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 2147483647   ), 2); testAssert(buffer ==  L"1111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-1), 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(16), 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-1), 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(15), 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-2147483647 - 1)); testAssert(buffer == L"-2147483648");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 2147483647    )); testAssert(buffer ==  L"2147483647");
	}
}


testCase(toStringBufferFromLong) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<long>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-2147483647- 1), 2); testAssert(buffer == L"10000000000000000000000000000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 2147483647   ), 2); testAssert(buffer ==  L"1111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-1), 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(16), 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-1), 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(15), 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-2147483647 - 1)); testAssert(buffer == L"-2147483648");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 2147483647    )); testAssert(buffer ==  L"2147483647");
	}
}


testCase(toStringBufferFromShort) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<short>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-32768), 2); testAssert(buffer == L"1000000000000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 32767), 2); testAssert(buffer ==  L"111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-1), 8); testAssert(buffer == L"177777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(16), 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-1), 16); testAssert(buffer == L"ffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(15), 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-32768)); testAssert(buffer == L"-32768");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 32767)); testAssert(buffer ==  L"32767");
	}
}


testCase(toStringBufferFromUnsigned__int64) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1ui64, 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, 2); testAssert(buffer ==  L"1111111111111111111111111111111111111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, 8); testAssert(buffer == L"1777777777777777777777");
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, 16); testAssert(buffer == L"ffffffffffffffff");
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64); testAssert(buffer == L"18446744073709551615");
	}
}


testCase(toStringBufferFromUnsignedChar) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned char>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), 2); testAssert(buffer ==  L"11111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), 8); testAssert(buffer == L"377");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), 16); testAssert(buffer == L"ff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255)); testAssert(buffer == L"255");
	}
}


testCase(toStringBufferFromUnsignedInt) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned int>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), 2); testAssert(buffer ==  L"11111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295)); testAssert(buffer == L"4294967295");
	}
}


testCase(toStringBufferFromUnsignedLong) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned long>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), 2); testAssert(buffer ==  L"11111111111111111111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295)); testAssert(buffer == L"4294967295");
	}
}


testCase(toStringBufferFromUnsignedShort) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned short>(1), 1));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), 2); testAssert(buffer ==  L"1111111111111111");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), 8); testAssert(buffer == L"177777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), 16); testAssert(buffer == L"ffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535)); testAssert(buffer == L"65535");
	}
}


testCase(toStringFrom__int64WithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1i64, L"a"));
		testAssertionFailed(to<String>(1i64, L"", 2));
	}
	{// base 검증
		testAssert(to<String>(-1i64, L"", 8) == L"1777777777777777777777");
		testAssert(to<String>(16i64, L"", 8) == L"20");
		testAssert(to<String>(-1i64, L"", 16) == L"ffffffffffffffff");
		testAssert(to<String>(15i64, L"", 16) == L"f");
		testAssert(to<String>(-9223372036854775808i64, L"") == L"-9223372036854775808");
		testAssert(to<String>( 9223372036854775807i64, L"") ==  L"9223372036854775807");
	}
	{// format 검증
		testAssert(to<String>(static_cast<__int64>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<__int64>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<__int64>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<__int64>( 100), L" ") == L" 100");
		testAssert(to<String>(static_cast<__int64>(-100), L" ") == L"-100");
		testAssert(to<String>(static_cast<__int64>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<__int64>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<__int64>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromCharWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<char>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<char>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<char>(-1), L"", 8) == L"377");
		testAssert(to<String>(static_cast<char>(16), L"", 8) == L"20");
		testAssert(to<String>(static_cast<char>(-1), L"", 16) == L"ff");
		testAssert(to<String>(static_cast<char>(15), L"", 16) == L"f");
		testAssert(to<String>(static_cast<char>(-128), L"") == L"-128");
		testAssert(to<String>(static_cast<char>( 127), L"") ==  L"127");
	}
	{// format 검증
		testAssert(to<String>(static_cast<char>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<char>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<char>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<char>( 100), L" ") == L" 100");
		testAssert(to<String>(static_cast<char>(-100), L" ") == L"-100");
		testAssert(to<String>(static_cast<char>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<char>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<char>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromDoubleWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1.0, L"a"));
		testAssertionFailed(to<String>(1.0, L"", 16));
	}
	{// 부동소수 형식 검증
		testAssert(to<String>(.0, L"") ==  L"0");
		testAssert(to<String>(-3.14, L"") == L"-3.14");
		testAssert(to<String>(2.2250738585072014e-308, L"") == L"2.22507e-308");
		testAssert(to<String>(1.7976931348623158e+308, L"") == L"1.79769e+308");
		testAssert(to<String>(std::numeric_limits<double>::infinity(), L"") == L"1.#INF");
		testAssert(to<String>(std::numeric_limits<double>::quiet_NaN(), L"") == L"1.#QNAN");
	}
	{// 라운드트립
		testAssert(floatEquals(to<double>(to<String>(3.14, L"")), 3.14));
	}
	{// format 검증
		testAssert(to<String>(static_cast<double>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<double>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<double>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<double>( 100), L" " ) == L" 100");
		testAssert(to<String>(static_cast<double>(-100), L" " ) == L"-100");
		testAssert(to<String>(static_cast<double>( 100), L"#" ) == L"100.000");
		testAssert(to<String>(static_cast<double>( 100), L"5" ) == L"  100");
		testAssert(to<String>(static_cast<double>(3.14), L".2") == L"3.1");
		testAssert(to<String>(static_cast<double>( 100), L"e" ) == L"1.000000e+002");
		testAssert(to<String>(static_cast<double>( 100), L"E" ) == L"1.000000E+002");
		testAssert(to<String>(static_cast<double>( 100), L"f" ) == L"100.000000");
		testAssert(to<String>(static_cast<double>( 1e-308), L"f" ) == L"0.000000");
		testAssert(to<String>(static_cast<double>( 100), L"g" ) == L"100");
		testAssert(to<String>(static_cast<double>( 1e-308), L"g" ) == L"1e-308");
		testAssert(to<String>(static_cast<double>( 100), L"G" ) == L"100");
		testAssert(to<String>(static_cast<double>( 1e-308), L"G" ) == L"1E-308");
	}
}


testCase(toStringFromFloatWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1.f, L"a"));
		testAssertionFailed(to<String>(1.f, L"", 16));
	}
	{// 부동소수 형식 검증
		testAssert(to<String>(.0f, L"") ==  L"0");
		testAssert(to<String>(-3.14f, L"") == L"-3.14");
		testAssert(to<String>(1.175494351e-38F, L"") == L"1.17549e-038");
		testAssert(to<String>(3.402823466e+38F, L"") == L"3.40282e+038");
		testAssert(to<String>(std::numeric_limits<float>::infinity(), L"") == L"1.#INF");
		testAssert(to<String>(std::numeric_limits<float>::quiet_NaN(), L"") == L"1.#QNAN");
	}
	{// 라운드트립
		testAssert(floatEquals(to<float>(to<String>(3.14f, L"")), 3.14f));
	}
	{// format 검증
		testAssert(to<String>(static_cast<float>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<float>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<float>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<float>( 100), L" " ) == L" 100");
		testAssert(to<String>(static_cast<float>(-100), L" " ) == L"-100");
		testAssert(to<String>(static_cast<float>( 100), L"#" ) == L"100.000");
		testAssert(to<String>(static_cast<float>( 100), L"5" ) == L"  100");
		testAssert(to<String>(static_cast<float>(3.14), L".2") == L"3.1");
		testAssert(to<String>(static_cast<float>( 100), L"e" ) == L"1.000000e+002");
		testAssert(to<String>(static_cast<float>( 100), L"E" ) == L"1.000000E+002");
		testAssert(to<String>(static_cast<float>( 100), L"f" ) == L"100.000000");
		testAssert(to<String>(static_cast<float>( 1e-38), L"f" ) == L"0.000000");
		testAssert(to<String>(static_cast<float>( 100), L"g" ) == L"100");
		testAssert(to<String>(static_cast<float>( 1e-38), L"g" ) == L"1e-038");
		testAssert(to<String>(static_cast<float>( 100), L"G" ) == L"100");
		testAssert(to<String>(static_cast<float>( 1e-38), L"G" ) == L"1E-038");
	}
}


testCase(toStringFromIntWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<int>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<int>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<int>(-1), L"", 8) == L"37777777777");
		testAssert(to<String>(static_cast<int>(16), L"", 8) == L"20");
		testAssert(to<String>(static_cast<int>(-1), L"", 16) == L"ffffffff");
		testAssert(to<String>(static_cast<int>(15), L"", 16) == L"f");
		testAssert(to<String>(static_cast<int>(-2147483647 - 1), L"") == L"-2147483648");
		testAssert(to<String>(static_cast<int>( 2147483647    ), L"") ==  L"2147483647");
	}
	{// format 검증
		testAssert(to<String>(static_cast<int>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<int>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<int>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<int>( 100), L" ") == L" 100");
		testAssert(to<String>(static_cast<int>(-100), L" ") == L"-100");
		testAssert(to<String>(static_cast<int>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<int>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<int>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromLongWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<long>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<long>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<long>(-1), L"", 8) == L"37777777777");
		testAssert(to<String>(static_cast<long>(16), L"", 8) == L"20");
		testAssert(to<String>(static_cast<long>(-1), L"", 16) == L"ffffffff");
		testAssert(to<String>(static_cast<long>(15), L"", 16) == L"f");
		testAssert(to<String>(static_cast<long>(-2147483647 - 1), L"") == L"-2147483648");
		testAssert(to<String>(static_cast<long>( 2147483647    ), L"") ==  L"2147483647");
	}
	{// format 검증
		testAssert(to<String>(static_cast<long>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<long>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<long>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<long>( 100), L" ") == L" 100");
		testAssert(to<String>(static_cast<long>(-100), L" ") == L"-100");
		testAssert(to<String>(static_cast<long>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<long>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<long>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromShortWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<short>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<short>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<short>(-1), L"", 8) == L"177777");
		testAssert(to<String>(static_cast<short>(16), L"", 8) == L"20");
		testAssert(to<String>(static_cast<short>(-1), L"", 16) == L"ffff");
		testAssert(to<String>(static_cast<short>(15), L"", 16) == L"f");
		testAssert(to<String>(static_cast<short>(-32768), L"") == L"-32768");
		testAssert(to<String>(static_cast<short>( 32767), L"") ==  L"32767");
	}
	{// format 검증
		testAssert(to<String>(static_cast<short>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<short>( 100), L"+5") == L" +100");
		testAssert(to<String>(static_cast<short>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<short>( 100), L" ") == L" 100");
		testAssert(to<String>(static_cast<short>(-100), L" ") == L"-100");
		testAssert(to<String>(static_cast<short>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<short>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<short>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromUnsigned__int64WithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(1ui64, L"a"));
		testAssertionFailed(to<String>(1ui64, L"", 2));
	}
	{// base 검증
		testAssert(to<String>(18446744073709551615ui64, L"", 8) == L"1777777777777777777777");
		testAssert(to<String>(18446744073709551615ui64, L"", 16) == L"ffffffffffffffff");
		testAssert(to<String>(18446744073709551615ui64, L"") == L"18446744073709551615");
	}
	{// format 검증
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L"+") == L"100");
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L" ") == L"100");
		testAssert(to<String>(static_cast<unsigned __int64>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<unsigned __int64>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromUnsignedCharWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned char>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<unsigned char>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned char>(255), L"", 8) == L"377");
		testAssert(to<String>(static_cast<unsigned char>(255), L"", 16) == L"ff");
		testAssert(to<String>(static_cast<unsigned char>(255), L"") == L"255");
	}
	{// format 검증
		testAssert(to<String>(static_cast<unsigned char>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<unsigned char>( 100), L"+") == L"100");
		testAssert(to<String>(static_cast<unsigned char>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<unsigned char>( 100), L" ") == L"100");
		testAssert(to<String>(static_cast<unsigned char>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<unsigned char>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<unsigned char>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromUnsignedIntWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned int>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<unsigned int>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned int>(4294967295), L"", 8) == L"37777777777");
		testAssert(to<String>(static_cast<unsigned int>(4294967295), L"", 16) == L"ffffffff");
		testAssert(to<String>(static_cast<unsigned int>(4294967295), L"") == L"4294967295");
	}
	{// format 검증
		testAssert(to<String>(static_cast<unsigned int>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<unsigned int>( 100), L"+") == L"100");
		testAssert(to<String>(static_cast<unsigned int>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<unsigned int>( 100), L" ") == L"100");
		testAssert(to<String>(static_cast<unsigned int>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<unsigned int>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<unsigned int>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromUnsignedLongWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned long>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<unsigned long>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned long>(4294967295), L"", 8) == L"37777777777");
		testAssert(to<String>(static_cast<unsigned long>(4294967295), L"", 16) == L"ffffffff");
		testAssert(to<String>(static_cast<unsigned long>(4294967295), L"") == L"4294967295");
	}
	{// format 검증
		testAssert(to<String>(static_cast<unsigned long>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<unsigned long>( 100), L"+") == L"100");
		testAssert(to<String>(static_cast<unsigned long>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<unsigned long>( 100), L" ") == L"100");
		testAssert(to<String>(static_cast<unsigned long>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<unsigned long>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<unsigned long>( 100), L".5") == L"00100");
	}
}


testCase(toStringFromUnsignedShortWithFormat) {
	{// 에러 케이스
		testAssertionFailed(to<String>(static_cast<unsigned short>(1), L"a"));
		testAssertionFailed(to<String>(static_cast<unsigned short>(1), L"", 2));
	}
	{// base 검증
		testAssert(to<String>(static_cast<unsigned short>(65535), L"", 8) == L"177777");
		testAssert(to<String>(static_cast<unsigned short>(65535), L"", 16) == L"ffff");
		testAssert(to<String>(static_cast<unsigned short>(65535), L"") == L"65535");
	}
	{// format 검증
		testAssert(to<String>(static_cast<unsigned short>( 100), L"-5") == L"100  ");
		testAssert(to<String>(static_cast<unsigned short>( 100), L"+") == L"100");
		testAssert(to<String>(static_cast<unsigned short>( 100), L"05") == L"00100");
		testAssert(to<String>(static_cast<unsigned short>( 100), L" ") == L"100");
		testAssert(to<String>(static_cast<unsigned short>(  15), L"#", 16) == L"0xf");
		testAssert(to<String>(static_cast<unsigned short>( 100), L"5") == L"  100");
		testAssert(to<String>(static_cast<unsigned short>( 100), L".5") == L"00100");
	}
}


testCase(toStringBufferFrom__int64WithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1i64, L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, 1i64, L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, -1i64, L"", 8); testAssert(buffer == L"1777777777777777777777");
		buffer.length(0); to<StringBuffer>(buffer, 16i64, L"", 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, -1i64, L"", 16); testAssert(buffer == L"ffffffffffffffff");
		buffer.length(0); to<StringBuffer>(buffer, 15i64, L"", 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, -9223372036854775808i64, L""); testAssert(buffer == L"-9223372036854775808");
		buffer.length(0); to<StringBuffer>(buffer,  9223372036854775807i64, L""); testAssert(buffer ==  L"9223372036854775807");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L" "); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>(-100), L" "); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<__int64>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromCharWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<char>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<char>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-1), L"", 8); testAssert(buffer == L"377");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(16), L"", 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-1), L"", 16); testAssert(buffer == L"ff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(15), L"", 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-128), L""); testAssert(buffer == L"-128");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 127), L""); testAssert(buffer ==  L"127");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L" "); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(-100), L" "); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<char>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromDoubleWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1.0, L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, 1.0, L"", 16));
	}
	{// 부동소수 형식 검증
		buffer.length(0); to<StringBuffer>(buffer, .0, L""); testAssert(buffer ==  L"0");
		buffer.length(0); to<StringBuffer>(buffer, -3.14, L""); testAssert(buffer == L"-3.14");
		buffer.length(0); to<StringBuffer>(buffer, 2.2250738585072014e-308, L""); testAssert(buffer == L"2.22507e-308");
		buffer.length(0); to<StringBuffer>(buffer, 1.7976931348623158e+308, L""); testAssert(buffer == L"1.79769e+308");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<double>::infinity(), L""); testAssert(buffer == L"1.#INF");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<double>::quiet_NaN(), L""); testAssert(buffer == L"1.#QNAN");
	}
	{// 라운드트립
		buffer.length(0); to<StringBuffer>(buffer, 3.14, L""); testAssert(floatEquals(to<double>(buffer), 3.14));
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L" " ); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>(-100), L" " ); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"#" ); testAssert(buffer == L"100.000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"5" ); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>(3.14), L".2"); testAssert(buffer == L"3.1");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"e" ); testAssert(buffer == L"1.000000e+002");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"E" ); testAssert(buffer == L"1.000000E+002");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"f" ); testAssert(buffer == L"100.000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 1e-308), L"f" ); testAssert(buffer == L"0.000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"g" ); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 1e-308), L"g" ); testAssert(buffer == L"1e-308");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 100), L"G" ); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<double>( 1e-308), L"G" ); testAssert(buffer == L"1E-308");
	}
}


testCase(toStringBufferFromFloatWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1.f, L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, 1.f, L"", 16));
	}
	{// 부동소수 형식 검증
		buffer.length(0); to<StringBuffer>(buffer, .0f, L""); testAssert(buffer ==  L"0");
		buffer.length(0); to<StringBuffer>(buffer, -3.14f, L""); testAssert(buffer == L"-3.14");
		buffer.length(0); to<StringBuffer>(buffer, 1.175494351e-38F, L""); testAssert(buffer == L"1.17549e-038");
		buffer.length(0); to<StringBuffer>(buffer, 3.402823466e+38F, L""); testAssert(buffer == L"3.40282e+038");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<float>::infinity(), L""); testAssert(buffer == L"1.#INF");
		buffer.length(0); to<StringBuffer>(buffer, std::numeric_limits<float>::quiet_NaN(), L""); testAssert(buffer == L"1.#QNAN");
	}
	{// 라운드트립
		buffer.length(0); to<StringBuffer>(buffer, 3.14f, L""); testAssert(floatEquals(to<float>(buffer), 3.14f));
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L" " ); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>(-100), L" " ); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"#" ); testAssert(buffer == L"100.000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"5" ); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>(3.14), L".2"); testAssert(buffer == L"3.1");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"e" ); testAssert(buffer == L"1.000000e+002");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"E" ); testAssert(buffer == L"1.000000E+002");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"f" ); testAssert(buffer == L"100.000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 1e-38), L"f" ); testAssert(buffer == L"0.000000");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"g" ); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 1e-38), L"g" ); testAssert(buffer == L"1e-038");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 100), L"G" ); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<float>( 1e-38), L"G" ); testAssert(buffer == L"1E-038");
	}
}


testCase(toStringBufferFromIntWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<int>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<int>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-1), L"", 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(16), L"", 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-1), L"", 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(15), L"", 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-2147483647 - 1), L""); testAssert(buffer == L"-2147483648");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 2147483647    ), L""); testAssert(buffer ==  L"2147483647");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L" "); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(-100), L" "); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<int>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromLongWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<long>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<long>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-1), L"", 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(16), L"", 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-1), L"", 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(15), L"", 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-2147483647 - 1), L""); testAssert(buffer == L"-2147483648");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 2147483647    ), L""); testAssert(buffer ==  L"2147483647");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L" "); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(-100), L" "); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<long>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromShortWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<short>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<short>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-1), L"", 8); testAssert(buffer == L"177777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(16), L"", 8); testAssert(buffer == L"20");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-1), L"", 16); testAssert(buffer == L"ffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(15), L"", 16); testAssert(buffer == L"f");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-32768), L""); testAssert(buffer == L"-32768");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 32767), L""); testAssert(buffer ==  L"32767");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L"+5"); testAssert(buffer == L" +100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L" "); testAssert(buffer == L" 100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(-100), L" "); testAssert(buffer == L"-100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<short>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromUnsigned__int64WithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, 1ui64, L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, 1ui64, L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, L"", 8); testAssert(buffer == L"1777777777777777777777");
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, L"", 16); testAssert(buffer == L"ffffffffffffffff");
		buffer.length(0); to<StringBuffer>(buffer, 18446744073709551615ui64, L""); testAssert(buffer == L"18446744073709551615");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L"+"); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L" "); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned __int64>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromUnsignedCharWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned char>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned char>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), L"", 8); testAssert(buffer == L"377");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), L"", 16); testAssert(buffer == L"ff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(255), L""); testAssert(buffer == L"255");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L"+"); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L" "); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned char>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromUnsignedIntWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned int>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned int>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), L"", 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), L"", 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(4294967295), L""); testAssert(buffer == L"4294967295");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L"+"); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L" "); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned int>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromUnsignedLongWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned long>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned long>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), L"", 8); testAssert(buffer == L"37777777777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), L"", 16); testAssert(buffer == L"ffffffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(4294967295), L""); testAssert(buffer == L"4294967295");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L"+"); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L" "); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned long>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(toStringBufferFromUnsignedShortWithFormat) {
	StringBuffer buffer;
	{// 에러 케이스
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned short>(1), L"a"));
		testAssertionFailed(to<StringBuffer>(buffer, static_cast<unsigned short>(1), L"", 2));
	}
	{// base 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), L"", 8); testAssert(buffer == L"177777");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), L"", 16); testAssert(buffer == L"ffff");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(65535), L""); testAssert(buffer == L"65535");
	}
	{// format 검증
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L"-5"); testAssert(buffer == L"100  ");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L"+"); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L"05"); testAssert(buffer == L"00100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L" "); testAssert(buffer == L"100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>(  15), L"#", 16); testAssert(buffer == L"0xf");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L"5"); testAssert(buffer == L"  100");
		buffer.length(0); to<StringBuffer>(buffer, static_cast<unsigned short>( 100), L".5"); testAssert(buffer == L"00100");
	}
}


testCase(cleanup) {
	_wsetlocale(LC_ALL, L"C");
}



	}
}