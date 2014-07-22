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



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다
	balor::test::UnitTest::ignoreHandleLeakCheck();

	Locale::locales(); // EnumSystemLocalesW 함수는 처음 호출에서 핸들을 복수 열고 유지한다
}


testCase(constructAndProperties) {
	// 무효한 파라미터
	testAssertionFailed(FileVersionInfo(L""));

	// 찾지 못한 파일 이름 
	testThrow(FileVersionInfo(File(Module::current().directory(), L"eliajlfre098faljeoijalfjalheiothjali")), File::NotFoundException);

	{// 버전 정보의 존재하지 않은 파일 
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

	{// 한국어
		Locale backup = Locale::current();
		Locale current(L"kor");
		Locale::current(current);
		scopeExit([&] () {
			Locale::current(backup);
		});
		FileVersionInfo info(Module::current().file());
		testAssert(info.comments() == L"설명");
		testAssert(info.companyName() == L"회사 이름");
		testAssert(info.fileDescription() == L"파일 디스크립션");
		testAssert(info.fileVersion() == Version(1, 2, 3, REVISION));
		testAssert(info.fileVersionText() == L"파일 버전");
		testAssert(info.internalName() == L"인터널 네임");
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
		testAssert(info.legalCopyright() == L"깏?긊깑긓긯?깋귽긣");
		testAssert(info.legalTrademarks() == L"깏?긊깑긣깒?긤??긏");
		testAssert(info.originalFileName() == L"긆깏긙긥깑긲?귽깑긨??");
		testAssert(info.privateBuild() == L"긵깋귽긹?긣긮깑긤");
		testAssert(info.productName() == L"긵깓?긏긣긨??");
		testAssert(info.productVersion() == Version(5, 6, 7, 8));
		testAssert(info.productVersionText() == L"긵깓?긏긣긫?긙깈깛");
		testAssert(info.specialBuild() == L"긚긻긘긿깑긮깑긤");
	}

	{// 영어
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

	{// 준비 되지 앟ㄴ은 컬쳐(default 컬쳐가 된다)
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
	testAssert(info.comments() == L"커맨트");
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
	testAssert(info.comments() == L"커맨트");

	info = move(info);
	testAssert(info);
	testAssert(info.comments() == L"커맨트");
}



		}
	}
}