#include <balor/locale/Locale.hpp>

#include <vector>

#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace locale {
		namespace testLocale {



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();
	Locale locale(1041);
	locale.displayName(); // GetLocaleInfoW 関数を LOCALE_SLOCALIZEDDISPLAYNAME で呼び出すと内部でハンドルを保持し続ける
	Locale::locales(); // EnumSystemLocalesW 関数はハンドルを最初の呼び出しで確保し、内部で保持し続ける
}


testCase(constructWithLcid) {
	// 無効なLCID
	testThrow(Locale(-1), Locale::NotFoundException);

	{// ja-JP
		Locale locale(1041);
		testAssert(locale.displayName() == L"日本語 (日本)" || locale.displayName() == L"日本語");
		testAssert(locale.englishCountryName() == L"Japan");
		testAssert(locale.englishLanguageName() == L"Japanese");
		testAssert(locale.id() == 1041);
		testAssert(locale.cLocaleName() == L"Japanese_Japan");
		testAssert(locale.name() == L"ja-JP");
		testAssert(locale.languageName() == L"ja");
		testAssert(locale.countryName() == L"JP");
	}

	{// en-US
		Locale locale(1033);
		testAssert(locale.displayName() == L"英語 (米国)");
		testAssert(locale.englishCountryName() == L"United States");
		testAssert(locale.englishLanguageName() == L"English");
		testAssert(locale.id() == 1033);
		testAssert(locale.cLocaleName() == L"English_United States");
		testAssert(locale.name() == L"en-US");
		testAssert(locale.languageName() == L"en");
		testAssert(locale.countryName() == L"US");
	}

	{// invaliant
		Locale locale(127);
		testAssert(locale.displayName() == L"ロケールに依存しない言語 (ロケールに依存しない国)" || locale.displayName() == L"Invariant Language (Invariant Country)");
		testAssert(locale.englishCountryName() == L"Invariant Country");
		testAssert(locale.englishLanguageName() == L"Invariant Language");
		testAssert(locale.id() == 127);
		testAssert(locale.name() == L"iv-IV");
		testAssert(locale.cLocaleName() == L"");
		testAssert(locale.languageName() == L"iv");
		testAssert(locale.countryName() == L"IV");
	}
}


testCase(constructWithName) {
	// 無効なカルチャ名
	testThrow(Locale(L"invalid culture name"), Locale::NotFoundException);

	{// ja-JP
		Locale locale(L"ja-JP");
		testAssert(locale.id() == 1041);
		testAssert(locale.name() == L"ja-JP");
	}

	{// en-US
		Locale locale(L"en-US");
		testAssert(locale.id() == 1033);
		testAssert(locale.name() == L"en-US");
	}
}


testCase(current) {
	Locale locale = Locale::current();
	testAssert(locale.id() == 1041); // here is JAPAN!
}


testCase(invariant) {
	Locale locale = Locale::invariant();
	testAssert(locale.id() == 127);
}


testCase(locales) {
	auto locales = Locale::locales();
	testAssert(!locales.empty());
	bool ok = true;
	for (auto i = locales.begin(), end = locales.end(); i != end; ++i) {
		try {
			i->name();
		} catch (Exception& ) {
			ok = false;
		}
	}
	testAssert(ok);
}


//testCase(cLocaleName) {} // testCase(lcidConstruct) にてテスト済み
//testCase(countryName) {}
//testCase(displayName) {}
//testCase(englishCountryName) {}
//testCase(englishLanguageName) {}
//testCase(id) {}
//testCase(languageName) {}
//testCase(name) {}



		}
	}
}