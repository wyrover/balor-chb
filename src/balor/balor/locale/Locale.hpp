#pragma once

#include <balor/Exception.hpp>
#include <balor/StringRange.hpp>


namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
class String;
}


namespace balor {
	namespace locale {



/**
 * 言語、文字列処理に関わるロケールを表す。
 * 
 * ロケールの ISO 名とは日本なら "ja-JP" で、米国ならば "en-US" といった形式。
 * .NET のサポートするロケールと完全に一致するわけではないが大部分は同じ。 http://msdn.microsoft.com/ja-jp/library/system.globalization.cultureinfo(v=vs.80).aspx
 */
class Locale {
public:
	/// ロケールが見つからなかった。
	class NotFoundException : public Exception {};

public:
	/// 地域に依存しないロケールで作成。
	Locale();
	/// ロケールＩＤから作成。
	explicit Locale(int lcid);
	/// ロケールの ISO 名から作成。
	explicit Locale(StringRange name);

public:
	/// C/C++ の標準ライブラリで使用できるロケール名。
	String cLocaleName() const;
	/// ISO 国名。
	String countryName() const;
	/// 現在のスレッドのロケール。
	static Locale current();
	static void current(const Locale& value);
	/// 現在のロケールでの表示に適したロケール名。
	String displayName() const;
	/// 英語の国名。
	String englishCountryName() const;
	/// 英語の言語名。
	String englishLanguageName() const;
	/// ロケールＩＤ。
	int id() const;
	/// 地域に依存しないロケール。
	static Locale invariant();
	/// 言語コード。
	int languageCode() const;
	/// ISO 言語名。
	String languageName() const;
	/// インストールされている全てのロケール。
	static std::vector<Locale, std::allocator<Locale> > locales();
	/// ロケールの ISO 名。
	String name() const;
	void nameToBuffer(StringBuffer& buffer) const;

public:
	bool operator==(const Locale& rhs) const;
	bool operator!=(const Locale& rhs) const;

private:
	unsigned int _id;
};



	}
}