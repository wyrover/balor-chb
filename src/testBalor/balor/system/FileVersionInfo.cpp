#include <balor/system/FileVersionInfo.hpp>

#include <vector>

#include "../../Revision.h"
#include <balor/locale/Locale.hpp>
#include <balor/io/File.hpp>
#include <balor/system/Module.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {
		namespace testFileVersionInfo {


using std::move;
using namespace balor::locale;
using namespace balor::io;
using namespace balor::system;



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	Locale::locales(); // EnumSystemLocalesW 関数は最初の呼び出しでだけハンドルを複数開いて保持する
}


testCase(constructAndProperties) {
	// 無効なパラメータ
	testAssertionFailed(FileVersionInfo(L""));

	// 見つからないファイル名
	testThrow(FileVersionInfo(File(Module::current().directory(), L"eliajlfre098faljeoijalfjalheiothjali")), File::NotFoundException);

	{// バージョン情報の存在しないファイル
		FileVersionInfo info(File(Module::current().directory(), L"balor_singleton.dll"));
		testAssertionFailed(info.comments());
		testAssertionFailed(info.companyName());
		testAssertionFailed(info.fileDescription());
		testAssertionFailed(info.fileVersion());
		testAssertionFailed(info.fileVersionText());
		testAssertionFailed(info.internalName());
		testAssertionFailed(info.isDebug());
		testAssertionFailed(info.isPatched());
		testAssertionFailed(info.isPreRelease());
		testAssertionFailed(info.isSpecialBuild());
		testAssertionFailed(info.languageCode());
		testAssertionFailed(info.legalCopyright());
		testAssertionFailed(info.legalTrademarks());
		testAssertionFailed(info.originalFileName());
		testAssertionFailed(info.privateBuild());
		testAssertionFailed(info.productName());
		testAssertionFailed(info.productVersion());
		testAssertionFailed(info.productVersionText());
		testAssertionFailed(info.specialBuild());
	}

	{// 日本語
		Locale backup = Locale::current();
		Locale current(L"ja-JP");
		Locale::current(current);
		scopeExit([&] () {
			Locale::current(backup);
		});
		FileVersionInfo info(Module::current().file());
		testAssert(info.comments() == L"コメント");
		testAssert(info.companyName() == L"カンパニーネーム");
		testAssert(info.fileDescription() == L"ファイルデスクリプション");
		testAssert(info.fileVersion() == Version(1, 2, 3, REVISION));
		testAssert(info.fileVersionText() == L"ファイルバージョン");
		testAssert(info.internalName() == L"インターナルネーム");
#ifdef _DEBUG
		testAssert(info.isDebug() == true);
		testAssert(info.isPatched() == true);
		testAssert(info.isPreRelease() == false);
		testAssert(info.isSpecialBuild() == false);
#else
		testAssert(info.isDebug() == false);
		testAssert(info.isPatched() == false);
		testAssert(info.isPreRelease() == true);
		testAssert(info.isSpecialBuild() == true);
#endif
		testAssert(info.languageCode() == 0x411);
		testAssert(info.legalCopyright() == L"リーガルコピーライト");
		testAssert(info.legalTrademarks() == L"リーガルトレードマーク");
		testAssert(info.originalFileName() == L"オリジナルファイルネーム");
		testAssert(info.privateBuild() == L"プライベートビルド");
		testAssert(info.productName() == L"プロダクトネーム");
		testAssert(info.productVersion() == Version(5, 6, 7, 8));
		testAssert(info.productVersionText() == L"プロダクトバージョン");
		testAssert(info.specialBuild() == L"スペシャルビルド");
	}

	{// 英語
		Locale backup = Locale::current();
		Locale::current(Locale(L"en-US"));
		scopeExit([&] () {
			Locale::current(backup);
		});
		FileVersionInfo info(Module::current().file());
		testAssert(info.comments() == L"comments");
		testAssert(info.companyName() == L"companyName");
		testAssert(info.fileDescription() == L"fileDescription");
		testAssert(info.fileVersion() == Version(1, 2, 3, REVISION));
		testAssert(info.fileVersionText() == L"fileVersion");
		testAssert(info.internalName() == L"internalName");
#ifdef _DEBUG
		testAssert(info.isDebug() == true);
		testAssert(info.isPatched() == true);
		testAssert(info.isPreRelease() == false);
		testAssert(info.isSpecialBuild() == false);
#else
		testAssert(info.isDebug() == false);
		testAssert(info.isPatched() == false);
		testAssert(info.isPreRelease() == true);
		testAssert(info.isSpecialBuild() == true);
#endif
		testAssert(info.languageCode() == 0x409);
		testAssert(info.legalCopyright() == L"legalCopyright");
		testAssert(info.legalTrademarks() == L"legalTrademarks");
		testAssert(info.originalFileName() == L"originalFilename");
		testAssert(info.privateBuild() == L"privateBuild");
		testAssert(info.productName() == L"productName");
		testAssert(info.productVersion() == Version(5, 6, 7, 8));
		testAssert(info.productVersionText() == L"productVersion");
		testAssert(info.specialBuild() == L"specialBuild");
	}

	{// 用意されていないカルチャ（デフォルトカルチャになる）
		Locale backup = Locale::current();
		Locale::current(Locale(L"it-CH"));
		scopeExit([&] () {
			Locale::current(backup);
		});
		FileVersionInfo info(Module::current().file());
		testAssert(info.comments() == L"default comments");
		testAssert(info.languageCode() == 0x800);
	}
}


testCase(rvalueConstruct) {
	FileVersionInfo source(Module::current().file());
	testAssert(source);
	FileVersionInfo info = move(source);
	testAssert(!source);
	testAssert(info);
	testAssertionFailed(source.comments());
	testAssert(info.comments() == L"コメント");
}


testCase(rvalueAssignment) {
	FileVersionInfo source(Module::current().file());
	testAssert(source);
	FileVersionInfo info(File(Module::current().directory(), L"balor_singleton.dll"));
	testAssert(!info);
	info = move(source);
	testAssert(!source);
	testAssert(info);
	testAssertionFailed(source.comments());
	testAssert(info.comments() == L"コメント");

	info = move(info);
	testAssert(info);
	testAssert(info.comments() == L"コメント");
}



		}
	}
}