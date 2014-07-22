#include <balor/io/File.hpp>

#include <utility>
#include <vector>

#include <balor/io/Drive.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace io {
		namespace testFile {

using std::move;
using std::vector;


namespace {
const wchar_t testDirectoryName[] = L"testBalor_io_File_e8oj8ljl9fcjasldji7980894sa";
File getTestDirectory() {
	File dir(File::getSpecial(File::Special::temporary), testDirectoryName);
	if (dir.exists()) {
		dir.remove(true);
	}
	dir.createDirectory();
	return dir;
}


void removeTestDirectory() {
	File(File::getSpecial(File::Special::temporary), testDirectoryName).remove(true);
}


File::Special folders[] = {
	 File::Special::adminTools
	,File::Special::appData
	,File::Special::CDBurnArea
	,File::Special::commonAdminTools
	,File::Special::commonAppData
	,File::Special::commonDesktopDirectory
	,File::Special::commonDocuments
	,File::Special::commonMusic
	,File::Special::commonOemLinks
	,File::Special::commonPictures
	,File::Special::commonPrograms
	,File::Special::commonStartMenu
	,File::Special::commonStartup
	,File::Special::commonTemplates
	,File::Special::commonVideos
	,File::Special::cookies
	,File::Special::desktop
	,File::Special::desktopDirectory
	,File::Special::favorites
	,File::Special::fonts
	,File::Special::history
	,File::Special::internetCache
	,File::Special::localAppData
	,File::Special::localizedResources
	,File::Special::myComputer
	,File::Special::myDocuments
	,File::Special::myMusic
	,File::Special::myPictures
	,File::Special::myVideos
	,File::Special::netHood
	,File::Special::personal
	,File::Special::printHood
	,File::Special::programFiles
	,File::Special::programFilesX86
	,File::Special::programFilesCommon
	,File::Special::programFilesCommonX86
	,File::Special::programs
	,File::Special::recent
	,File::Special::resources
	,File::Special::sendTo
	,File::Special::startMenu
	,File::Special::startup
	,File::Special::system
	,File::Special::systemX86
	,File::Special::templates
	,File::Special::userProfile
	,File::Special::windows
};
} // namespace



testCase(startup) { // 긪깛긤깑귩띍룊궻뚁귂뢯궢궳둴뺎궢갂볙븫궳뺎렃궢뫏궚귡걁덇뙥깏?긏궸뙥궑귡걂듫릶귩궇귞궔궣귕뚁귪궳궓궘
	balor::test::UnitTest::ignoreHandleLeakCheck();
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	{
		auto stream = file1.create();
		stream.write("0123", 0, 4);
	}
	File file2(dir, L"file2.txt");

	file0.replace(file1, file2); // XP궸궓궋궲궞궻룉뿚귩덇뱗믅궢궲궓궔궶궋궴긪깛긤깑깏?긏궕뙚뢯궠귢귡

	for (int i = 0, end = sizeof(folders) / sizeof(folders[0]); i < end; ++i) {
		testNoThrow(File::getSpecial(folders[i], File::SpecialOption::none);); // SHGetFolderPathW 듫릶궼긪깛긤깑귩몵돿궠궧귡귝궎궬
	}

}


testCase(constructAndAssignment) {
	{// 뗴궻긬긚
		File file(L"");
		testAssert(!file.exists());
	}
	{// 뮮궥궗귡긬긚
		testThrow(File file(String(L'a', MAX_PATH)), File::PathTooLongException);
	}
	{// 긬긚궻 move
		File file(L"c:\\abc");
		testAssert(file == L"c:\\abc");
		File file2 = move(file);
		testAssert(file2 == L"c:\\abc");
		File file3(L"c:\\def");
		File& result = file3 = move(file2);
		testAssert(&result == &file3);
		testAssert(file3 == L"c:\\abc");
		file3 = move(file3);
		testAssert(file3 == L"c:\\abc");
	}
}


testCase(attributes) {
	scopeExit(&removeTestDirectory);
	
	File dir = getTestDirectory();
	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	File file2(dir, L"hoge\\file2.txt");
	File file3(L"c::\\abc");

	// 뫔띪궢궶궋긲?귽깑
	testThrow(file1.attributes(), File::NotFoundException);
	testThrow(file1.attributes(File::Attributes::normal), File::NotFoundException);

	// 뫔띪궢궶궋긢귻깒긏긣깏둏몏
	testThrow(file2.attributes(), File::NotFoundException);
	testThrow(file2.attributes(File::Attributes::normal), File::NotFoundException);

	// 뼰뚼궶긬긚
	testThrow(file3.attributes(), File::InvalidPathException);
	testThrow(file3.attributes(File::Attributes::none), File::InvalidPathException);

	{// 긲?귽깑궔귞롦벦궴빾뛛
		auto attributes = file0.attributes();
		testAssert((attributes & File::Attributes::archive) != 0);
		file0.attributes(attributes | File::Attributes::readOnly);
		scopeExit([&] () {
			file0.attributes(attributes & ~File::Attributes::readOnly);
		});
		testAssert(file0.attributes() == (attributes | File::Attributes::readOnly));
		testThrow(file0.remove(), File::AccessDeniedException);
	}

	{// 긢귻깒긏긣깏궔귞롦벦궴빾뛛
		File sub0(dir, L"sub0");
		sub0.createDirectory();
		auto attributes = sub0.attributes();
		testAssert((attributes & File::Attributes::directory) != 0);
		sub0.attributes(attributes | File::Attributes::readOnly);
		scopeExit([&] () {
			sub0.attributes(attributes & ~File::Attributes::readOnly);
		});
		testAssert(sub0.attributes() == (attributes | File::Attributes::readOnly));
		testThrow(sub0.remove(), File::AccessDeniedException);
	}


	// Caution: 뫌맜 취득갂빾뛛뙛뙽궻궶궋긢귻깒긏긣깏궸귺긏긜긚궢궲 File::AccessDeniedException 궕뵯맯궥귡궞궴귩뽞럨궳둴봃
}


testCase(copyTo) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	{
		auto stream = file1.create();
		stream.write("0123", 0, 4);
	}
	File file2(dir, L"file2.txt");
	File file3(dir, L"file3.txt");

	// 뼰뚼궶긬깋긽??
	testAssertionFailed(file0.copyTo(L""));

	// 뫔띪궢궶궋긲?귽깑뼹
	testThrow(file2.copyTo(file3), File::NotFoundException);

	// 긓긯?먩궕딓궸뫔띪궥귡걁뤵룕궖럚믦궶궢걂
	testThrow(file0.copyTo(file1), File::AlreadyExistsException);

	{// 귺긏긜긚뙛뙽궕뼰궋
		file1.attributes(file1.attributes() | File::Attributes::readOnly);
		testThrow(file0.copyTo(file1, true), File::AccessDeniedException);
		file1.attributes(file1.attributes() & ~File::Attributes::readOnly);
	}

	{// 뫔띪궥귡긢귻깒긏긣깏궸긓긯?
		auto sub0(dir, L"sub0");
		auto sub2(dir, L"sub2");
		sub0.createDirectory();
		sub2.createDirectory();
		testThrow(sub0.copyTo(sub2), File::AlreadyExistsException);
	}

	{// 긢귻깒긏긣깏궻긓긯?
		auto sub0(dir, L"sub0");
		auto subsub0(sub0, L"subsub0");
		auto file4(sub0, L"file4");
		auto sub1(dir, L"sub1");
		auto file41(sub1, L"file4");
		auto subsub01(sub1, L"subsub0");
		sub0.createDirectory();
		subsub0.createDirectory();
		file4.create();
		sub0.copyTo(sub1);
		testAssert(sub0.exists());
		testAssert(subsub0.exists());
		testAssert(file4.exists());
		testAssert(sub1.exists());
		testAssert(file41.exists());
		testAssert(subsub01.exists());
	}

	{// 긲?귽깑귩긢귻깒긏긣깏궸긓긯?궢귝궎궴궢궫
		testThrow(file0.copyTo(File(dir, L"sub0")), File::AccessDeniedException);
	}

	// 긬긚궕찾을 수 없는
	testThrow(File(dir, L"hoge\\file4.txt").copyTo(file0), File::NotFoundException);
	testThrow(file0.copyTo(File(dir, L"hoge\\file4.txt")), File::NotFoundException);

	{// 떎뾎귺긏긜긚궳궖궶궋
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.copyTo(file2), File::SharingViolationException);
	}

	// 븉맫궶긲?귽깑긬긚
	testThrow(file0.copyTo(L"c::\\hoge.txt"), File::InvalidPathException);

	{// 맫륂긑?긚걁뤵룕궖궶궢걂
		file0.copyTo(file2);
		testAssert(file2.openRead().length() == 3);
		char buffer[4] = {0};
		file2.openRead().read(buffer, 0, 3);
		testAssert(String::equals("abc", buffer));
		testAssert(file0.exists());
	}

	{// 맫륂긑?긚걁뤵룕궖궇귟걂
		file0.copyTo(file1, true);
		auto stream = file1.openRead();
		testAssert(stream.length() == 3);
		char buffer[4] = {0};
		stream.read(buffer, 0, 3);
		testAssert(String::equals("abc", buffer));
		testAssert(file0.exists());
	}
}


//testCase(create) { // testCase(open) 궳긡긚긣
//}


testCase(createDirectory) {
	scopeExit(&removeTestDirectory);
	auto dir = getTestDirectory();

	{// 긲?귽깑궕딓궸뫔띪궢궲궋귡
		auto file(dir, L"file0");
		file.create();
		testThrow(file.createDirectory(), File::AlreadyExistsException);
	}
	{// 딓궸긢귻깒긏긣깏궕뫔띪궥귡궶귞돺귖궢궶궋
		testAssert(dir.exists());
		testNoThrow(dir.createDirectory());
		testAssert(dir.exists());
	}
	{// 뫔띪궢궶궋깑?긣긢귻깒긏긣깏궳띿맟
		auto last = *(--Drive::drivesEnd());
		File dir(String(last.letter() + 1, 1) + L":\\sub0\\subsub0");
		testAssert(!dir.exists());
		testThrow(dir.createDirectory(), File::NotFoundException);
	}
	{// 뼰뚼궶긬긚
		testThrow(File(dir, L"a?b").createDirectory(), File::InvalidPathException);
		testThrow(File(L"c::\\test").createDirectory(), File::InvalidPathException);
	}
	{// 븕믅궸띿맟
		auto sub0(dir, L"sub0");
		testAssert(!sub0.exists());
		sub0.createDirectory();
		testAssert(sub0.exists());
	}
	{// 둏몏귩덇딠궸띿맟
		auto sub1(dir, L"sub1\\subsub1\\subsubsub1");
		testAssert(!sub1.exists());
		sub1.createDirectory();
		testAssert(sub1.exists());
	}

	// Caution: 뙸띪궻깇?긗궻긢귻깒긏긣깏띿맟뙛뙽귩땻붬궢궫긢귻깒긏긣깏귩띿맟궢갂궩궻긢귻깒긏긣깏뤵궳 createDirectory 귩렳뛱궢궲
	//          UnauthorizedAccessException궕뵯맯궥귡궞궴귩뽞럨궳둴봃
}


testCase(createTempFile) {
	auto file = File::createTempFile();
	scopeExit([&] () {
		file.remove();
	});
	testAssert(file.exists());
	testAssert(file.openRead().length() == 0);
}


testCase(current) {
	scopeExit(&removeTestDirectory);

	File dir = getTestDirectory();
	File sub(dir, L"sub");
	sub.createDirectory();
	File file(dir, L"file");
	file.create();

	auto current = File::current();
	testAssert(current.exists());

	testAssertionFailed(File::current(L""));
	testThrow(File::current(L"ljflajlaijfaodajfldjlaifjdoi\\fldjaldjafli"), File::NotFoundException);
	testThrow(File::current(file), File::AlreadyExistsException);

	File::current(dir);
	scopeExit([&] () {
		File::current(current);
	});
	testAssert(File(sub.name()).exists());
}


testCase(exists) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	File sub0(dir, L"sub0");
	File sub1(dir, L"sub1");
	sub1.createDirectory();

	testAssert(file0.exists());
	testAssert(!file1.exists());
	testAssert(!sub0.exists());
	testAssert(sub1.exists());
}


testCase(extension) {
	testAssert(File(L"c:\\dir\\file.txt").extension() == L".txt");
	testAssert(File(L"file.txt").extension() == L".txt");
	testAssert(File(L"c:\\dir").extension() == L"");
	testAssert(File(L"c:\\dir.txt\\file").extension() == L"");
}


testCase(fullPathFile) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	auto current = File::current();
	File::current(dir);
	scopeExit([&] () {
		File::current(current);
	});

	File file0(L".");
	testAssert(file0.fullPathFile() == dir.fullPathFile());
	testAssert(File(L"c:\\dir\\..").fullPathFile() == L"c:\\");
	testAssert(File(L"c:\\..").fullPathFile() == L"c:\\");
}


testCase(getFile) {
	File dir(L"c:\\sub0");
	File file0(dir, L"file0.txt");
	testAssert(file0 == L"c:\\sub0\\file0.txt");
}


testCase(getFiles) {
	scopeExit(&removeTestDirectory);

	File dir = getTestDirectory();
	// 긡긚긣뾭긢귻깒긏긣깏둏몏궻띿맟
	File file0(dir, L"file0.txt");
	File file1(dir, L"file1.doc");
	File sub0(dir, L"sub0");
	File subsub0(dir, L"sub0\\subsub0");
	File subsub1(dir, L"sub0\\subsub1");
	File subsub2(dir, L"sub0\\subsub2");
	File file2(subsub2, L"file2.txt");
	File file3(subsub2, L"file3.doc");
	File sub1(dir, L"sub1");
	File sub2(dir, L"sub2");
	File file4(sub2, L"file4.txt");
	
	subsub0.createDirectory();
	subsub1.createDirectory();
	subsub2.createDirectory();
	sub1.createDirectory();
	sub2.createDirectory();
	file0.create();
	file1.create();
	file2.create();
	file3.create();
	file4.create();

	// 뼰뚼궶긬깋긽??
	testAssert(!dir.getFilesIterator(L"c:\\"));
	testThrow(dir.getFiles(String(L'a', MAX_PATH)), File::PathTooLongException);
	testThrow(File(L"c::\\hoge").getFiles(), File::NotFoundException);
	testThrow(dir.getFiles(L"sub*\\subsub*"), File::InvalidPathException);

	// 뙚랊륆뙊궶궢궶귞궽뙚랊뙅됈궶궢
	testAssert(dir.getFiles(L"").empty());
	testAssert(!dir.getFilesIterator(L""));

	{// 띋딞궶궢멣뙚랊
		auto files = dir.getFiles();
		testAssert(files.size() == 5);
		testAssert(files[0] == file0.path());
		testAssert(files[1] == file1.path());
		testAssert(files[2] == sub0.path());
		testAssert(files[3] == sub1.path());
		testAssert(files[4] == sub2.path());
	}
	{// 띋딞궇귟멣뙚랊
		auto files = dir.getFiles(L"?*", true);
		testAssert(files.size() == 11);
		testAssert(files[ 0] == file2.path());
		testAssert(files[ 1] == file3.path());
		testAssert(files[ 2] == subsub0.path());
		testAssert(files[ 3] == subsub1.path());
		testAssert(files[ 4] == subsub2.path());
		testAssert(files[ 5] == file4.path());
		testAssert(files[ 6] == file0.path());
		testAssert(files[ 7] == file1.path());
		testAssert(files[ 8] == sub0.path());
		testAssert(files[ 9] == sub1.path());
		testAssert(files[10] == sub2.path());
	}
	{// 뙚랊긬??깛럚믦뙚랊
		auto files = dir.getFiles(L"*.doc", true);
		testAssert(files.size() == 2);
		testAssert(files[0] == file3.path());
		testAssert(files[1] == file1.path());
	}
	{// 뙚랊긬??깛럚믦뗴뙚랊
		auto files = dir.getFiles(L"hoge?", true);
		testAssert(files.empty());
	}
	{// 뗴긢귻깒긏긣깏뙚랊
		auto files = sub1.getFiles(L"?*", true);
		testAssert(files.empty());
	}
	{// 븸릶둏몏뙚랊긬??깛럚믦뙚랊
		auto files = dir.getFiles(L"sub0\\subsub2\\*.txt");
		testAssert(files.size() == 1);
		testAssert(files[0] == file2.path());
	}
	{// 븸릶둏몏뙚랊긬??깛럚믦뙚랊 띋딞궇귟
		auto files = dir.getFiles(L"subsub2\\?*.txt", true);
		testAssert(files.size() == 1);
		testAssert(files[0] == file2.path());
	}
}


testCase(getSpecial) {
	// 뼰뚼궶긬깋긽??
	testAssertionFailed(File::getSpecial(File::Special::_enum(-1)));
	testAssertionFailed(File::getSpecial(File::Special::adminTools, File::SpecialOption::_enum(-1)));

	for (int i = 0, end = sizeof(folders) / sizeof(folders[0]); i < end; ++i) {
		testNoThrow(File::getSpecial(folders[i], File::SpecialOption::none));
	}

	//auto testFolder = folders[0];
	//testAssert(testFolder != -1);
	//if (testFolder != -1) {
	//	testAssert(File::getSpecial(testFolder, File::SpecialOption::create).fullName() == L"");
	//	auto path = File::getSpecial(testFolder, File::SpecialOption::none).fullName();
	//	testAssert(File::getSpecial(testFolder, File::SpecialOption::doNotVerify).fullName() == path);
	//	File dir(path);
	//	testAssert(dir.exists());
	//	dir.remove();
	//}

	// Caution: 긘긚긡?궻긲긅깑?귩랁룣궢궲띿맟궠귢귡궻귩긡긚긣궥귡궻궼봂귂궶궋갃
	//       File::SpecialOption::doNotVerify 궓귝귂 File::SpecialOption::create 궻긑?긚귩됀?궳궇귢궽뽞럨궳둴봃궥귡궞궴
}


testCase(isDirectory) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	File sub0(dir, L"sub0");
	File sub1(dir, L"sub1");
	File file0(dir, L"file0");

	sub1.createDirectory();
	file0.create();

	testAssert(!sub0.isDirectory());
	testAssert(sub1.isDirectory());
	testAssert(!file0.isDirectory());
}


testCase(moveTo) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File sub0(dir, L"sub0");
	sub0.createDirectory();
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	{
		file1.create();
	}
	File file2(dir, L"file2.txt");
	File file3(dir, L"file3.txt");

	// 뼰뚼궶긬깋긽??
	testAssertionFailed(file0.moveTo(L""));

	// 뫔띪궢궶궋긲?귽깑뼹
	testThrow(file2.moveTo(file3), File::NotFoundException);

	// 긓긯?먩궕딓궸뫔띪궥귡
	testThrow(file0.moveTo(file1), File::AlreadyExistsException);

	// 뫔띪궥귡긢귻깒긏긣깏궸댷벍궢귝궎궴궢궫
	testThrow(file0.moveTo(sub0), File::AlreadyExistsException);

	{// 귺긏긜긚뙛뙽궕뼰궋
	}

	// 긬긚궕찾을 수 없는
	testThrow(File(dir, L"hoge\\file4.txt").moveTo(file0), File::NotFoundException);
	testThrow(file0.moveTo(File(dir, L"hoge\\file4.txt")), File::NotFoundException);

	{// 떎뾎귺긏긜긚궳궖궶궋
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.moveTo(file2), File::SharingViolationException);
	}

	// 븉맫궶긲?귽깑긬긚
	testThrow(file0.moveTo(L"c::\\hoge.txt"), File::InvalidPathException);

	{// 맫륂긑?긚걁뤵룕궖궶궢걂
		file0.moveTo(file2);
		testAssert(!file0.exists());
		testAssert(file2.exists());
		testAssert(file2.openRead().length() == 3);
		char buffer[4] = {0};
		file2.openRead().read(buffer, 0, 3);
		testAssert(String::equals("abc", buffer));
	}
}


testCase(moveToDirectory) {
	scopeExit(&removeTestDirectory);
	
	File dir = getTestDirectory();
	// 긡긚긣뾭긢귻깒긏긣깏둏몏궻띿맟
	File sub0(dir, L"sub0");
	sub0.createDirectory();
	File file0(sub0, L"file0.txt");
	file0.create();
	File(sub0, L"subsub0\\subsubsub0").createDirectory();
	File subsub1(sub0, L"subsub1");
	subsub1.createDirectory();
	File sub1(dir, L"sub1");
	File sub2(dir, L"sub2");
	sub2.createDirectory();

	// 뼰뚼궶긬깋긽??
	testAssertionFailed(dir.moveTo(L""));

	// 뫔띪궢궶궋긢귻깒긏긣깏
	testThrow(sub1.moveTo(sub0), File::NotFoundException);

	// 뫔띪궢궶궋긢귻깒긏긣깏둏몏귉궻댷벍
	testThrow(sub0.moveTo(File(dir, L"subUnknown\\subsub0")), File::NotFoundException);

	// 댷벍먩궻긫긞긡귻깛긐
	testThrow(sub0.moveTo(sub2), File::AlreadyExistsException);
	testThrow(sub2.moveTo(file0), File::AlreadyExistsException);

	// 렔빁궻긖긳긢귻깒긏긣깏귉궻댷벍
	testThrow(sub0.moveTo(subsub1), File::SharingViolationException);

	// 뼰뚼궶긬긚
	testThrow(sub0.moveTo(L"c:\\a?c"), File::InvalidPathException);
	testThrow(sub0.moveTo(L"c::\\abc"), File::InvalidPathException);

	{// 긲?귽깑긆?긵깛궠귢궫긢귻깒긏긣깏궻댷벍
		auto stream = file0.openRead();
		testThrow(sub0.moveTo(sub1), File::AccessDeniedException);
	}
	{// 맫륂긑?긚
		testNoThrow(sub0.moveTo(sub1));
		testAssert(!sub0.exists());
		testAssert(sub1.exists());
		testAssert(sub1.getFiles(L"?*", true).size() == 4);
	}

	// Caution: 긢귻깒긏긣깏궻댷벍뙛뙽궻뼰궋깇?긗궳 moveTo 귩렳뛱궢갂File::AccessDeniedException 궕뵯맯궥귡궞궴귩뽞럨궳둴봃
	// Caution: 댶궶귡긢귻긚긏?깏깄??귉궻 moveTo 귩렳뛱궢갂File::AccessDeniedException 궕뵯맯궥귡궞궴귩뽞럨궳둴봃
}



testCase(name) {
	testAssert(File(L"name.txt").name() == L"name.txt");
	testAssert(File(L"name.txt\\").name() == L"");
	testAssert(File(L"\\name.txt").name() == L"name.txt");
	testAssert(File(L"\\\\name.txt").name() == L"name.txt");
	testAssert(File(L"c:\\sub0\\name.txt").name() == L"name.txt");
	testAssert(File(L"c:\\").name() == L"");
	testAssert(File(L".").name() == File::current().name());
}


testCase(nameWithoutExtension) {
	testAssert(File(L"c:\\dir\\").nameWithoutExtension() == L"");
	testAssert(File(L"c:").nameWithoutExtension() == File::current().name());
	testAssert(File(L"c:\\dir\\file.txt").nameWithoutExtension() == L"file");
	testAssert(File(L"c:\\dir").nameWithoutExtension() == L"dir");
	testAssert(File(L".").nameWithoutExtension() == File::current().nameWithoutExtension());
}


#pragma warning(push)
#pragma warning(disable : 4189) // 'buffer' : 깓?긇깑빾릶궕룊딖돸궠귢귏궢궫궕갂랷뤖궠귢궲궋귏궧귪
testCase(open) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");

	{// create
		testAssert(!file0.exists());
		file0.create();
		testAssert(file0.exists());
		testAssert(file0.openRead().length() == 0);
		file0.create().write("def", 0, 3);
		testAssert(file0.openRead().length() == 3);
		file0.create();
		testAssert(file0.openRead().length() == 0);
		file0.create().write("def", 0, 3);
	}
	{// open read
		{
			auto stream = file0.openRead();
			char buffer[4] = {0};
			stream.read(buffer, 0, 3);
			testAssert(String::equals(buffer, "def"));
			testAssertionFailed(stream.write("abc", 0, 3));
		}
	}
	{// open read
		{
			auto stream = file0.openRead();
			char buffer[4] = {0};
			stream.read(buffer, 0, 3);
			testAssert(String::equals(buffer, "def"));
			testAssertionFailed(stream.write("abc", 0, 3));
		}
		file0.remove();
		testThrow(file0.openRead(), FileStream::NotFoundException);
	}
	{// open write
		testThrow(file0.openWrite(), FileStream::NotFoundException);
		file0.create();
		{
			auto stream = file0.openWrite();
			char buffer[4] = {0};
			testAssertionFailed(stream.read(buffer, 0, 3));
			stream.write("abc", 0, 3);
		}
		testAssert(file0.openRead().length() == 3);
	}
}
#pragma warning(pop)


testCase(path) {
	testAssert(File(L"abc") == L"abc");
}


testCase(parent) {
	testAssert(File(L"c:\\").parent() == L"");
	testAssert(File(L"c:\\sub0").parent() == L"c:\\");
	testAssert(File(L"c:\\sub0\\").parent() == L"c:\\");
	testAssert(File(L"c:\\sub0\\subsub0").parent() == L"c:\\sub0");
	testAssert(File(L".").parent() == File::current().parent());
}


testCase(remove) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
	}
	File file2(dir, L"file2.txt");

	// 뫔띪궢궶궋긲?귽깑뼹
	testNoThrow(file2.remove());

	{// 귺긏긜긚뙛뙽궕뼰궋
		file0.attributes(file0.attributes() | File::Attributes::readOnly);
		testThrow(file0.remove(), File::AccessDeniedException);
		file0.attributes(file0.attributes() & ~File::Attributes::readOnly);
	}

	// 긬긚궕찾을 수 없는
	testThrow(File(dir, L"hoge\\file4.txt").remove(), File::NotFoundException);

	{// 떎뾎귺긏긜긚궳궖궶궋
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.remove(), File::SharingViolationException);
	}

	// 븉맫궶긲?귽깑긬긚
	testThrow(File(L"c::\\hoge.txt").remove(), File::InvalidPathException);

	{// 맫륂긑?긚
		testAssert(file0.exists());
		file0.remove();
		testAssert(!file0.exists());
	}


	{// 뤑궥뙛뙽궻뼰궋긢귻깒긏긣깏궴뭁뫬긢귻깒긏긣깏랁룣
		File sub1(dir, L"sub1");
		sub1.createDirectory();
		sub1.attributes(sub1.attributes() | File::Attributes::readOnly);
		testThrow(sub1.remove(), File::AccessDeniedException);
		sub1.attributes(sub1.attributes() & ~File::Attributes::readOnly);
		testNoThrow(sub1.remove());
		testAssert(!sub1.exists());
	}
	{// 뗴궳궼궶궋긢귻깒긏긣깏궴띋딞랁룣
		File sub0(dir, L"sub0");
		sub0.createDirectory();
		{
			File file0(sub0, L"file0.txt");
			file0.create();
		}
		File(sub0, L"subsub0\\subsubsub0").createDirectory();
		File(sub0, L"subsub1").createDirectory();

		testThrow(sub0.remove(), File::NotEmptyException);
		testAssert(sub0.exists());
		testNoThrow(sub0.remove(true));
		testAssert(!sub0.exists());
	}

}


testCase(replace) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 긡긚긣긢??띿맟
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	{
		auto stream = file1.create();
		stream.write("0123", 0, 4);
	}
	File file2(dir, L"file2.txt");
	File file3(dir, L"file3.txt");
	{
		file3.create();
	}
	File file4(dir, L"hoge\\file4.txt");
	File file5(L"c::\\hoge.txt");
	File sub0(dir, L"sub0");
	sub0.createDirectory();

	// 뼰뚼궶긬깋긽??
	testAssertionFailed(file0.replace(L"", L" "));

	// 뫔띪궢궶궋긲?귽깑뼹
	testThrow(file2.replace(file0, L""), File::NotFoundException);
	testThrow(file0.replace(file2, L""), File::NotFoundException);

	// 긢귻깒긏긣깏귩뭫궖듂궑궢귝궎궴궢궫
	testThrow(sub0.replace(file0, L""), File::AccessDeniedException);
	testThrow(file0.replace(sub0, L""), File::AccessDeniedException);
	testThrow(file0.replace(file1, sub0), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED

	// 긬긚궕찾을 수 없는
	testThrow(file4.replace(file0, L""), File::NotFoundException);
	testThrow(file0.replace(file4, L""), File::NotFoundException);
	testThrow(file0.replace(file1, file4), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED

	{// 떎뾎귺긏긜긚궳궖궶궋
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.replace(file1, L""), File::SharingViolationException);
		testThrow(file1.replace(file0, L""), File::SharingViolationException);
		testThrow(file1.replace(file3, file0), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED
	}

	// 븉맫궶긲?귽깑긬긚
	testThrow(file5.replace(file0, L""), File::InvalidPathException);
	testThrow(file0.replace(file5, L""), File::InvalidPathException);
	testThrow(file0.replace(file0, file5), File::SharingViolationException);

	{// 맫륂긑?긚걁긫긞긏귺긞긵궇귟걂
		file0.replace(file1, file2);
		testAssert(!file0.exists());
		testAssert(file1.openRead().length() == 3);
		{
			auto stream = file1.openRead();
			char buffer[4] = {0};
			stream.read(buffer, 0, 3);
			testAssert(String::equals(buffer, "abc"));
		}
		testAssert(file2.openRead().length() == 4);
		{
			auto stream = file2.openRead();
			char buffer[5] = {0};
			stream.read(buffer, 0, 4);
			testAssert(String::equals(buffer, "0123"));
		}
	}
	{// 맫륂긑?긚걁긫긞긏귺긞긵뼰궢걂
		file1.replace(file2, L"");
		testAssert(!file1.exists());
		testAssert(file2.openRead().length() == 3);
		{
			auto stream = file2.openRead();
			char buffer[4] = {0};
			stream.read(buffer, 0, 3);
			testAssert(String::equals(buffer, "abc"));
		}
	}
}


testCase(root) {
	testAssert(File(L"c:\\").root() == L"c:\\");
	testAssert(File(L"c:\\sub0").root() == L"c:\\");
	testAssert(File(L"c:\\sub0\\subsub0").root() == L"c:\\");
	testAssert(File(L".").root() == File::current().root().path());
}



		}
	}
}