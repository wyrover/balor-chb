#pragma once

#include <balor/system/ComPtr.hpp>
#include <balor/ArrayRange.hpp>
#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct IMultiLanguage2;

namespace std {
template<typename T> struct char_traits;
template<typename T> class allocator;
template<typename T, typename Traits, typename Allocator> class basic_string;
}

namespace balor {
class String;
}


namespace balor {
	namespace locale {



/**
 * 文字コードを表し、コード間の文字列変換を提供する。
 *
 * 変換には mlang.dll の COM オブジェクトを使用しているが、コンストラクタ引数の isWindowCodePage を true にした場合に限り
 * WIN32 API の WideCharToMultiByte, MultiByteToWideChar 関数を使用する。
 * この場合 COM オブジェクトを必要とする関数を呼ばない限り COM オブジェクトは作成されないので、パフォーマンスが向上する可能性がある。
 * ただしこの設定は WIN32 API でサポートされるコードページでしか使えない。また、変換の結果が COM のものと異なる。
 * 例えば、全角のアルファベットを ASCII で encode した場合に COM は半角のアルファベットに変換するが WIN32 API では変換失敗になる。
 * COM の初期化は、メインスレッドで COM オブジェクトの初期化に失敗した場合のみ ::balor::system::Com で STA の COM 初期化を行っている。
 * マルチスレッドで Charset を使用する場合はメインスレッドで MTA の COM 初期化を行うか、新しいスレッド内で明示的に COM の初期化処理、終了処理を行う必要がある。
 * なお mlang.dll のCOMオブジェクトの threadingModel は both であるようだ。
 */
class Charset : private NonCopyable {
public:
	typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> > string;

	/// キャラクターセットが見つからなかった。
	class NotFoundException : public Exception {};

	/// encode, decode, convert が失敗し、throwable 引数が true の場合に投げられる。
	class ConversionFailedException : public Exception {};

	/// 変換結果を書き込む配列の長さが足りなかった。
	class InsufficientBufferExceptoin : public Exception {};

public:
	/// コードページから作成。isWindowsCodePage についてはクラスのドキュメントを参照。
	explicit Charset(int codePage, bool isWindowsCodePage = false);
	/// キャラクターセット名から作成。
	explicit Charset(StringRange name);
	Charset(Charset&& value);
	~Charset();

	Charset& operator=(Charset&& value);

public:
	/// ASCII 文字コード。
	static Charset ascii();
	/// ビッグエンディアンユニコード。
	static Charset bigEndianUnicode();
	/// 使用可能なキャラクターセット一覧。
	static std::vector<Charset, std::allocator<Charset> > charsets();
	/// コードページ。
	int codePage() const;
	/// Unicode への変換。
	String decode(ByteStringRange src) const;
	String decode(ByteStringRange src, int count) const;
	/// Unicode への変換。dst に変換結果を書き込み、変換した文字数を返す。
	int decode(ByteStringRange src, int count, ArrayRange<wchar_t> dst) const;
	/// システムのデフォルトキャラクターセット。isWindowsCodePage が true で作成される事に注意。
	static Charset default();
	/// キャラクターセットの表示名。
	String displayName() const;
	/// srcCharset からの変換。
	string encode(ByteStringRange src, const Charset& srcCharset) const;
	string encode(ByteStringRange src, int count, const Charset& srcCharset) const;
	/// srcCharset からの変換。dst に変換結果を書き込み、変換した文字数を返す。
	int encode(ByteStringRange src, int count, ArrayRange<char> dst, const Charset& srcCharset) const;
	/// Unicode からの変換。
	string encode(StringRange src) const;
	string encode(StringRange src, int count) const;
	/// Unicode からの変換。dst に変換結果を書き込み、変換した文字数を返す。
	int encode(StringRange src, int count, ArrayRange<char> dst) const;
	/// EUC-JP コード。
	static Charset eucJp();
	/// Unicode からの encode に失敗した文字を置換する文字。初期値は L'?'。Unicode 以外からの encode, decode ではサポートされない。
	wchar_t fallbackChar() const;
	void fallbackChar(wchar_t value);
	/// キャラクターセット名。
	String name() const;
	/// ShiftJIS コード。
	static Charset shiftJis();
	/// 変換に失敗した場合に Charset::ConversionFailedException を投げるかどうか。初期値は false。
	bool throwable() const;
	void throwable(bool value);
	/// ユニコード。
	static Charset unicode();
	/// UTF7。
	static Charset utf7();
	/// UTF8。
	static Charset utf8();

private:
	const ::balor::system::ComPtr<::IMultiLanguage2>& _getMultiLanguage() const;

	int _codePage;
	bool _isWindowsCodePage;
	bool _throwable;
	wchar_t _fallbackChar;
	char _fallbackBytes[6];
	::balor::system::ComPtr<::IMultiLanguage2> _multiLanguage;
};



	}
}