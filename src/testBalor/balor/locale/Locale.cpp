#include <balor/locale/Locale.hpp>

#include <vector>

#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace locale {
		namespace testLocale {



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	balor::test::UnitTest::ignoreHandleLeakCheck();
	Locale locale(1041);
	locale.displayName(); // GetLocaleInfoW 함수를 LOCALE_SLOCALIZEDDISPLAYNAME 로 호출하면 내부에서 핸들을 계속 유지한다
	Locale::locales(); // EnumSystemLocalesW 함수는 처음 호출에서 핸들을 복수 열고 유지한다
}


testCase(constructWithLcid) {
	// 무효한 LCID
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
		testAssert(locale.displayName() == L"로케일이 의존하지 않는 언어(로케일에 의존하지 않는 나라)" || locale.displayName() == L"Invariant Language (Invariant Country)");
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
	// 무효한 컬쳐 이름 
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





		}
	}
}