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
 * 文字列を?す。
 *
 * Java や .NET の String と同様、Unicode のみを扱い文字列?イン?の内容の変更を許可しない。
 * std::wstring と相互に変換可?。ただし basic_string を引数に取るテンプレ?ト関数には渡せないものが多いのでその場合は begin、end 関数を使う。
 * 関数引数に balor::locale::Locale を渡さない場合は常に序数処理を行う。（地域差を無視して文字コ?ドを数値として比較する）
 * 特に operator < 等の演算子の処理では Locale を指定できないので全て序数処理になることに注意。
 * 序数処理では全角アルフ?ベットに対する ignoreCase はサ??トしない。
 * Convert::to<String> 関数で変換可?な?と String は + 演算子で連結することができる。
 *
 * Unicode 正規化の為の isNormalized, normalize 関数は Vista 以?か、IE7 以?のインスト?ルされた XP か、または normaliz.dll をインスト?ルした XP でのみサ??トする。
 *
 * Vista 以?であれば ::FindNLSString 関数が使えるのだが balor は XP 以?のサ??トなので
 * indexOf, lastIndexOf, startsWith 及び endsWith 関数は自力実装となっている。
 * API が対応していないのでサロゲ?トペアでかつ結合文字である文字コ?ドを結合文字と判断できない。
 *
 * 内容を変更しないという仕様を生かして refer, literal 関数を使って文字列?イン?を参照だけすることができるが、?イン?参照の危険性を?分に考慮する必要がある。
 *
 * 文字列クラスの自作は車輪の再発明であり、できればやりたくはないが
 * std::wstring 及び Boost.string algo ではロケ?ルや Unicode 関連の処理を完全に無視しているので対応した文字列クラスを作成した。
 *
 * ※サロゲ?トペア、正規化、結合文字は Unicode の仕様
 *
 * <h3>・サンプルコ?ド</h3>
 * <pre><code>
	String s(L"abc");

	// std::wstring との相互変換
	std::wstring ws = s;
	s = ws;

	// ロケ?ルを考慮した比較
	bool result = String::compare(s, L"あいう", Locale::current());

	// 文字列同士、または数値との連結
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

	/// 文字列比較のオプション。組み合わせで指定する。
	/// 文字列を検索する関数（indexOf, lastIndexOf, startsWith, endsWith等）では ignoreNonSpace, ignoreSymbols は指定できない。
	struct CompareOptions {
		enum _enum {
			none           = 0         , 
			ignoreCase     = 0x00000001, /// 大文字と小文字を区別しない。
			ignoreNonSpace = 0x00000002, /// 場所を取らない結合文字を無視する。無視した結果が等しいか合成した結果が等しい場合を等しいとする。
			ignoreSymbols  = 0x00000004, /// 記号を無視する。
			ignoreKanaType = 0x00010000, /// ひらがなとカ?カナを区別しない。
			ignoreWidth    = 0x00020000, /// 全角と半角を区別しない。
			stringSort     = 0x00001000, /// 「-」や「'」を特別扱いしない。指定しなかった場合はこれらが無いものとして比較される。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(CompareOptions);
	};

	/// Unicode の正規化?式。
	struct NormalizationForm {
		enum _enum {
			c  = 0x1, /// 正規化?式 C
			d  = 0x2, /// 正規化?式 D
			kc = 0x5, /// 正規化?式 KC
			kd = 0x6, /// 正規化?式 KD
		};
		BALOR_NAMED_ENUM_MEMBERS(NormalizationForm);
	};

	/// normaliz.dll が見つからない場合に投げられる。
	class NormalizeDllNotFoundException : public Exception {};

	/// 欠けたサロゲ?トペアなどの正規化不?な文字を正規化しようとしたときに投げられる。
	class NormalizeFailedException : public Exception {};

public:
	/// 空文字列（L""）で初期化。
	String();
	String(const String& value);
	String(String&& value);
	/// StringBuffer から変換。
	String(const StringBuffer& value);
	String(StringBuffer&& value);
	/// std::wstring で初期化。
	String(const wstring& value);
	/// 文字列?イン?で初期化。
	String(const wchar_t* value);
	/// キャラク??と繰り返し数で初期化。
	String(wchar_t c, int count);
	/// 文字列?イン?と文字列の長さで初期化。
	String(const wchar_t* value, int length);
	/// バイト文字列をキャラク??セットでデコ?ドして初期化。
	String(ByteStringRange value, const ::balor::locale::Charset& charset);
	~String();

	String& operator=(const String& value);
	String& operator=(String&& value);
	/// StringBuffer をｴ�ﾀﾔ。
	String& operator=(const StringBuffer& value);
	String& operator=(StringBuffer&& value);
	/// std::wstring をｴ�ﾀﾔ。
	String& operator=(const wstring& value);
	/// 文字列?イン?をｴ�ﾀﾔ。
	String& operator=(const wchar_t* value);

public:
	/// 文字列への?イン?。
	const wchar_t* begin() const { return _c_str; }
	/// 文字列への?イン?。
	const wchar_t* c_str() const { return _c_str; }
	/// 文字列の比較。戻り値は C 言語の strcmp 系の関数と同じ。
	static int compare(StringRange lhs, StringRange rhs, bool ignoreCase = false);
	static int compare(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	static int compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, bool ignoreCase = false);
	static int compare(StringRange lhs, int lindex, StringRange rhs, int rindex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	/// 文字列の比較。戻り値は C 言語の strcmp 系の関数と同じ。
	int compareTo(StringRange rhs) const;
	/// 文字列内で value が見つかるかどうか。
	bool contains(StringRange value) const;
	/// 文字配列に null終?を含む文字列全体をコピ?する。
	template<int Size> void copyTo(wchar_t (&array)[Size]) const { copyTo(array, Size); }
	void copyTo(wchar_t* array, int arraySize) const;
	/// 文字配列に部分文字列をコピ?する。
	template<int Size> void copyTo(int beginIndex, wchar_t (&array)[Size], int arrayIndex, int length) const { copyTo(beginIndex, array, Size, arrayIndex, length); }
	void copyTo(int beginIndex, wchar_t* array, int arraySize, int arrayIndex, int length) const;
	/// 空文字列かどうか。長さが０ではなくても空文字列になりえることに注意。
	bool empty() const;
	/// 文字列終?への?イン?。
	const wchar_t* end() const { return _c_str + length(); }
	/// 指定した文字列で終わるかどうか。
	bool endsWith(StringRange value, bool ignoreCase = false) const;
	bool endsWith(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 文字列が等しいかどうか。
	static bool equals(ByteStringRange lhs, ByteStringRange rhs, bool ignoreCase = false);
	static bool equals(StringRange lhs, StringRange rhs, bool ignoreCase = false);
	static bool equals(StringRange lhs, StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none);
	/// 文字列が等しいかどうか。
	bool equalTo(StringRange rhs, bool ignoreCase = false) const;
	bool equalTo(StringRange rhs, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 指定した部分を削除した文字列を返す。
	String erase(int beginIndex) const;
	String erase(int beginIndex, int length) const;
	/// バイト文字列?イン?のさす文字列の長さを返す。
	static int getLength(const char* value);
	/// 文字列?イン?のさす文字列の長さを返す。
	static int getLength(const wchar_t* value);
	/// 結合文字やサロゲ?トペアを考慮して beginIndex の次の文字のインデックスを返す。
	int getNextLetterIndex(int beginIndex = 0) const;
	/// 文字列のハッシュ値。
	SizeType hashCode() const;
	/// 指定した文字や文字列が最初に見つかった位置を返す。
	int indexOf(StringRange value, bool ignoreCase = false) const;
	int indexOf(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int indexOf(wchar_t value, int beginIndex = 0) const;
	int indexOf(StringRange value, int beginIndex, bool ignoreCase = false) const;
	int indexOf(StringRange value, int beginIndex, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int indexOf(wchar_t value, int beginIndex, int length) const;
	int indexOf(StringRange value, int beginIndex, int length, bool ignoreCase = false) const;
	int indexOf(StringRange value, int beginIndex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 指定した文字列の中の文字のどれかが最初に見つかった位置を返す。
	int indexOfAny(StringRange anyOf, int beginIndex = 0) const;
	int indexOfAny(StringRange anyOf, int beginIndex, int length) const;
	/// 文字列を?入して返す。
	String insert(int beginIndex, StringRange value) const;
	/// 指定した正規化?式で正規化されているかどうか。
	bool isNormalized(String::NormalizationForm frame = NormalizationForm::c) const;
	/// 空白文字しか含まれないかどうか。
	bool isWhiteSpace() const;
	/// 複数の文字列を separator で結合して返す。
	static String join(StringRange separator, StringRangeArray values);
	/// 指定した文字や文字列が最後に見つかった位置を返す。
	int lastIndexOf(StringRange value, bool ignoreCase = false) const;
	int lastIndexOf(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int lastIndexOf(wchar_t value, int beginIndex = 0) const;
	int lastIndexOf(StringRange value, int beginIndex, bool ignoreCase = false) const;
	int lastIndexOf(StringRange  value, int beginIndex, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	int lastIndexOf(wchar_t value, int beginIndex, int length) const;
	int lastIndexOf(StringRange value, int beginIndex, int length, bool ignoreCase = false) const;
	int lastIndexOf(StringRange  value, int beginIndex, int length, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 指定した文字列の中の文字のどれかが最後に見つかった位置を返す。
	int lastIndexOfAny(StringRange anyOf, int beginIndex = 0) const;
	int lastIndexOfAny(StringRange anyOf, int beginIndex, int length) const;
	/// 文字列の長さ。
	int length() const;
	/// 結合文字やサロゲ?トペアを考慮した?示される文字列の長さ。
	int lengthInLetter() const;
	/// 文字列リテラルを参照する String を作成する。
	template<int Size> static String literal(const wchar_t (&string)[Size]) { return String(1 - Size, string); }
	/// 指定した正規化?式で正規化して返す。
	String normalize(String::NormalizationForm frame = NormalizationForm::c) const;
	/// 指定した長さになるまで先頭に文字を埋め込んで返す。
	String padLeft(int length, wchar_t paddingChar = L' ') const;
	/// 指定した長さになるまで末尾に文字を埋め込んで返す。
	String padRight(int length, wchar_t paddingChar = L' ') const;
	/// 文字列?イン?を参照する String を作成する。String は内容の変更を許さないが、参照元が先に破壊されると危険。
	static String refer(StringRange value);
	static String refer(const wchar_t* value, int length);
	/// 文字列?イン?を参照しているかどうか。
	bool referred() const { return _length <= 0; }
	/// 指定した文字や文字列を置換して返す。
	String replace(wchar_t oldValue, wchar_t newValue) const;
	String replace(StringRange oldValue, StringRange newValue) const;
	/// 指定した複数の区切り文字で分割した文字列配列を返す。separators が空の場合は区切り文字を whiteSpace とみなす。
	std::vector<String, std::allocator<String> > split(StringRange separators = L"") const;
	/// 指定した複数の区切り文字列で分割した文字列配列を返す。
	std::vector<String, std::allocator<String> > split(StringRangeArray separators) const;
	/// 指定した文字列で始まるかどうか。
	bool startsWith(StringRange value, bool ignoreCase = false) const;
	bool startsWith(StringRange value, const Locale& locale, String::CompareOptions options = CompareOptions::none) const;
	/// 部分文字列を返す。
	String substring(int beginIndex) const;
	String substring(int beginIndex, int length) const;
	/// 大文字を小文字にして返す。
	String toLower() const;
	String toLower(const Locale& locale) const;
	/// 小文字を大文字にして返す。
	String toUpper() const;
	String toUpper(const Locale& locale) const;
	/// 先頭と末尾の trimChars に含まれる文字を全て取り除いて返す。trimChars が空文字列の場合は whiteSpace とみなす。
	String trim(StringRange trimChars = String()) const;

public:
	/// wstring への変換。
	operator wstring() const;
	/// StringRange への変換。
	operator StringRange() const { return StringRange(_c_str, length()); }
	/// 文字要素を返す。
	wchar_t operator[] (int index) const;
	/// 文字列の追加。
	String& operator+=(const String& rhs);
	String& operator+=(const wstring& rhs);
	String& operator+=(const wchar_t* rhs);
	String& operator+=(wchar_t* rhs);
	String& operator+=(wchar_t rhs);
	/// Convert::to<String> が可?な?の += 演算。
	template<typename T> String& operator+=(const T& rhs) { return *this += Convert::to<String>(rhs); }
	/// 文字列の加算。
	friend String operator+(const String& lhs, const String& rhs);
	friend String operator+(const String& lhs, const wstring& rhs);
	friend String operator+(const String& lhs, const wchar_t* rhs);
	friend String operator+(const String& lhs, wchar_t* rhs);
	friend String operator+(const String& lhs, wchar_t rhs);
	friend String operator+(const wstring& lhs, const String& rhs);
	friend String operator+(const wchar_t* lhs, const String& rhs);
	friend String operator+(wchar_t* lhs, const String& rhs);
	friend String operator+(wchar_t lhs, const String& rhs);
	/// Convert::to<String> が可?な?との + 演算。
	template<typename T> friend String operator+(const String& lhs, const T& rhs) { return lhs + Convert::to<String>(rhs); }
	template<typename T> friend String operator+(const T& lhs, const String& rhs) { return Convert::to<String>(lhs) + rhs; }
	/// 比較演算。
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