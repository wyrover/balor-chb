#pragma once

#include <balor/Exception.hpp>
#include <balor/StringRange.hpp>


namespace std {
template<typename T> struct char_traits;
template<typename T> class allocator;
template<typename T, typename Traits, typename Allocator> class basic_string;
}


namespace balor {

class String;
class StringBuffer;



/**
 * 基本データ型と String を相互に変換するテンプレート関数群。
 *
 * 基数や、printf と同様の書式を指定した変換もサポートする。ロケール指定の変換は未実装。全て"C"ロケールで処理される。
 * 基本データ型同士の変換はサポートしない。boost::numeric_cast、SafeInt または static_cast を利用すれば良い。
 * 負数を表す文字列から unsigned 型に変換した場合、signed 型のバイナリ表現になるか OverflowException を発生させるかは型によって不定。
 * 1.#INF や 1.#QNAN は文字列化はできるが数値化しようとすると１になる点に注意。
 * Convert は名前空間として実装してあるので、任意の型について関数を追加したり、Convet::とタイプするのが面倒な場合は using することができる。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	// 数値と文字列の変換
	String s0 = Convert::to<String>(128);
	String s1 = Convert::to<String>(3.1415);
	int i0 = Convert::to<int>(L"128");
	double i1 = Convert::to<double>(L"3.1415");

	// 基数や書式指定をした変換
	String s2 = Convert::to<String>(0xff, 16); // s2 == L"ff"
	String s3 = Convert::to<String>(128, L"05"); // s3 == L"00128"
 * </code></pre>
 */
namespace Convert {
	/// 型変換でオーバーフローが発生した場合に投げられる例外。
	class OverflowException : public Exception {};

	/// 文字列を変換できなかった場合に投げられる例外。
	class StringFormatException : public Exception {};


	/// 任意型から任意型への変換テンプレート関数。サポートする変換のみ特殊化して実装する。基数引数の初期値を 10 に設定する。
	template<typename Dst, typename Src> Dst to(const Src& value, int base = 10) { static_assert(false, "Convert type unsupported"); }

	/// 様々な型の文字列引数を StringRange 引数に変換するテンプレート関数。
	template<typename Dst> Dst to(wchar_t* value, int base = 10) { return to<Dst>(StringRange(value), base); }
	template<typename Dst> Dst to(const wchar_t* value, int base = 10) { return to<Dst>(StringRange(value), base); }
	template<typename Dst> Dst to(const std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >& value, int base = 10) { return to<Dst>(StringRange(value), base); }
	template<typename Dst> Dst to(const String& value, int base = 10) { return to<Dst>(StringRange(value), base); }
	template<typename Dst> Dst to(const StringBuffer& value, int base = 10) { return to<Dst>(StringRange(value), base); }

	/// 文字列から数値への変換。
	/// base に基数として指定できるのは float と double の場合は 10 のみで、それ以外は 2, 8, 10, 16。
	template<> __int64 to<__int64>(const StringRange& value, int base);
	template<> char to<char>(const StringRange& value, int base);
	template<> double to<double>(const StringRange& value, int base);
	template<> float to<float>(const StringRange& value, int base);
	template<> int to<int>(const StringRange& value, int base);
	template<> long to<long>(const StringRange& value, int base);
	template<> short to<short>(const StringRange& value, int base);
	template<> unsigned __int64 to<unsigned __int64>(const StringRange& value, int base);
	template<> unsigned char to<unsigned char>(const StringRange& value, int base);
	template<> unsigned int to<unsigned int>(const StringRange& value, int base);
	template<> unsigned long to<unsigned long>(const StringRange& value, int base);
	template<> unsigned short to<unsigned short>(const StringRange& value, int base);
	/// 数値から String への変換。
	/// base に基数として指定できるのは float と double の場合は 10 のみで、それ以外は 2, 8, 10, 16。
	template<> String to<String>(const __int64& value, int base);
	template<> String to<String>(const char& value, int base);
	template<> String to<String>(const double& value, int base);
	template<> String to<String>(const float& value, int base);
	template<> String to<String>(const int& value, int base);
	template<> String to<String>(const long& value, int base);
	template<> String to<String>(const short& value, int base);
	template<> String to<String>(const unsigned __int64& value, int base);
	template<> String to<String>(const unsigned char& value, int base);
	template<> String to<String>(const unsigned int& value, int base);
	template<> String to<String>(const unsigned long& value, int base);
	template<> String to<String>(const unsigned short& value, int base);
	template<> String to<String>(const bool& value, int base);

	/// 数値を文字列化して StringBuffer へ追加する。
	/// base に基数として指定できるのは float と double の場合は 10 のみで、それ以外は 2, 8, 10, 16。
	template<typename Dst, typename Src> void to(StringBuffer& stringBuffer, const Src& value, int base = 10)  { static_assert(false, "Convert type unsupported"); }
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const __int64& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const char& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const double& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const float& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const int& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const long& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const short& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned __int64& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned char& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned int& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned long& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned short& value, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const bool& value, int base);

	/// 数値から書式指定した String への変換。
	/// base に基数として指定できるのは float と double の場合は 10 のみで、それ以外は 8, 10, 16。
	/// 書式は printf の書式から最初の % と 最後の型フィールド文字を除いたもの。
	/// ただし、Src が float か double の場合は末尾に型フィールド文字（e, E, f, g, G）を指定できる。指定しなかった場合は g となる。
	/// 例えば下記は printf で "%08x" と書式指定するのに等しい。
	/// <pre><code>
	/// String s = Convert::to<String>(65535, L"08", 16);
	/// </code></pre>
	template<typename Dst, typename Src> Dst to(const Src& value, StringRange format, int base = 10)  { static_assert(false, "Convert type unsupported"); }
	template<> String to<String>(const __int64& value, StringRange format, int base);
	template<> String to<String>(const char& value, StringRange format, int base);
	template<> String to<String>(const double& value, StringRange format, int base);
	template<> String to<String>(const float& value, StringRange format, int base);
	template<> String to<String>(const int& value, StringRange format, int base);
	template<> String to<String>(const long& value, StringRange format, int base);
	template<> String to<String>(const short& value, StringRange format, int base);
	template<> String to<String>(const unsigned __int64& value, StringRange format, int base);
	template<> String to<String>(const unsigned char& value, StringRange format, int base);
	template<> String to<String>(const unsigned int& value, StringRange format, int base);
	template<> String to<String>(const unsigned long& value, StringRange format, int base);
	template<> String to<String>(const unsigned short& value, StringRange format, int base);

	/// 数値から書式指定した文字列へ変換して StringBuffer に追加する。
	/// base に基数として指定できるのは float と double の場合は 10 のみで、それ以外は 8, 10, 16。
	/// 書式は printf の書式から最初の % と 最後の型フィールド文字を除いたもの。
	/// ただし、Src が float か double の場合は末尾に型フィールド文字（e, E, f, g, G）を指定できる。指定しなかった場合は g となる。
	template<typename Dst, typename Src> void to(StringBuffer& stringBuffer, const Src& value, StringRange format, int base = 10)  { static_assert(false, "Convert type unsupported"); }
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const __int64& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const char& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const double& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const float& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const int& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const long& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const short& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned __int64& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned char& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned int& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned long& value, StringRange format, int base);
	template<> void to<StringBuffer>(StringBuffer& stringBuffer, const unsigned short& value, StringRange format, int base);
}



}