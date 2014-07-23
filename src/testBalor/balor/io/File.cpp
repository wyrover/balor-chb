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



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다
	balor::test::UnitTest::ignoreHandleLeakCheck();
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 테스트 데이터 작성
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

	file0.replace(file1, file2); // XP에서는 이 처리를 한번 통하지 않으면 핸들 릭이 검출된다 

	for (int i = 0, end = sizeof(folders) / sizeof(folders[0]); i < end; ++i) {
		testNoThrow(File::getSpecial(folders[i], File::SpecialOption::none);); // SHGetFolderPathW 함수는 핸들을 증가시키지 않도록 
	}

}


testCase(constructAndAssignment) {
	{// 빈 패스
		File file(L"");
		testAssert(!file.exists());
	}
	{// 너무 긴 패스
		testThrow(File file(String(L'a', MAX_PATH)), File::PathTooLongException);
	}
	{// 패스의 move
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
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	File file2(dir, L"hoge\\file2.txt");
	File file3(L"c::\\abc");

	// 존재하지 않는 파일
	testThrow(file1.attributes(), File::NotFoundException);
	testThrow(file1.attributes(File::Attributes::normal), File::NotFoundException);

	// 존재하지 않는 디렉토리 계층
	testThrow(file2.attributes(), File::NotFoundException);
	testThrow(file2.attributes(File::Attributes::normal), File::NotFoundException);

	// 무효한 패스
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

	{// 파일에서 취득과 갱신
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


	// Caution: 속성의 취득, 변경 권한이 없는 디렉토리에 접근해서 File::AccessDeniedException 가 발생하는 것을 눈으로 확인 
}


testCase(copyTo) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 테스트 데이터 작성
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

	// 무효한 파라미터
	testAssertionFailed(file0.copyTo(L""));

	// 존재하지 않는 파일 이름
	testThrow(file2.copyTo(file3), File::NotFoundException);

	// 복사처가 이미 존재한다(덮어 쓰기 지정 없음)
	testThrow(file0.copyTo(file1), File::AlreadyExistsException);

	{// 접근 권한 없음
		file1.attributes(file1.attributes() | File::Attributes::readOnly);
		testThrow(file0.copyTo(file1, true), File::AccessDeniedException);
		file1.attributes(file1.attributes() & ~File::Attributes::readOnly);
	}

	{// 존재하는 디렉토리에 복사
		auto sub0(dir, L"sub0");
		auto sub2(dir, L"sub2");
		sub0.createDirectory();
		sub2.createDirectory();
		testThrow(sub0.copyTo(sub2), File::AlreadyExistsException);
	}

	{// 디렉토리에 복사 
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

	{// 파일을 디렉토리에 복사하려고 하였다
		testThrow(file0.copyTo(File(dir, L"sub0")), File::AccessDeniedException);
	}

	// 패스를 찾을 수 없는
	testThrow(File(dir, L"hoge\\file4.txt").copyTo(file0), File::NotFoundException);
	testThrow(file0.copyTo(File(dir, L"hoge\\file4.txt")), File::NotFoundException);

	{// 공유 접근 할 수 없다
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.copyTo(file2), File::SharingViolationException);
	}

	// 부정한 파일 패스
	testThrow(file0.copyTo(L"c::\\hoge.txt"), File::InvalidPathException);

	{// 정상 케이스(덮어 쓰기 없음)
		file0.copyTo(file2);
		testAssert(file2.openRead().length() == 3);
		char buffer[4] = {0};
		file2.openRead().read(buffer, 0, 3);
		testAssert(String::equals("abc", buffer));
		testAssert(file0.exists());
	}

	{// 정상 케이스(덮어 쓰기 있음)
		file0.copyTo(file1, true);
		auto stream = file1.openRead();
		testAssert(stream.length() == 3);
		char buffer[4] = {0};
		stream.read(buffer, 0, 3);
		testAssert(String::equals("abc", buffer));
		testAssert(file0.exists());
	}
}


//testCase(create) { // testCase(open) 에서 테스트
//}


testCase(createDirectory) {
	scopeExit(&removeTestDirectory);
	auto dir = getTestDirectory();

	{// 파일이 이미 존재하고 있다
		auto file(dir, L"file0");
		file.create();
		testThrow(file.createDirectory(), File::AlreadyExistsException);
	}
	{// 이미 디렉토리가 존재하지만 아무것도 없다
		testAssert(dir.exists());
		testNoThrow(dir.createDirectory());
		testAssert(dir.exists());
	}
	{// 존재 하지 않는 루트 디렉토리에서 작성
		auto last = *(--Drive::drivesEnd());
		File dir(String(last.letter() + 1, 1) + L":\\sub0\\subsub0");
		testAssert(!dir.exists());
		testThrow(dir.createDirectory(), File::NotFoundException);
	}
	{// 무효한 패스
		testThrow(File(dir, L"a?b").createDirectory(), File::InvalidPathException);
		testThrow(File(L"c::\\test").createDirectory(), File::InvalidPathException);
	}
	{// 보통으로 작성
		auto sub0(dir, L"sub0");
		testAssert(!sub0.exists());
		sub0.createDirectory();
		testAssert(sub0.exists());
	}
	{// 계층을 한번에 작성 
		auto sub1(dir, L"sub1\\subsub1\\subsubsub1");
		testAssert(!sub1.exists());
		sub1.createDirectory();
		testAssert(sub1.exists());
	}

	// Caution: 현재 유저 디렉토리 작성 권한을 거부한 디렉토리를 작성, 그 디렉토리에 위에서 createDirectory 를 실행하여
	//          UnauthorizedAccessException 가 발생하는 것을 눈으로 확인 
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

	// 테스트 데이터 작성
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
	// 테스트 용 디렉토리 계층 작성 
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

	// 무효한 파라미터
	testAssert(!dir.getFilesIterator(L"c:\\"));
	testThrow(dir.getFiles(String(L'a', MAX_PATH)), File::PathTooLongException);
	testThrow(File(L"c::\\hoge").getFiles(), File::NotFoundException);
	testThrow(dir.getFiles(L"sub*\\subsub*"), File::InvalidPathException);

	// 검색 조건 없음 이라면 검색 결과 없음
	testAssert(dir.getFiles(L"").empty());
	testAssert(!dir.getFilesIterator(L""));

	{// 재귀 없이 모든 검색
		auto files = dir.getFiles();
		testAssert(files.size() == 5);
		testAssert(files[0] == file0.path());
		testAssert(files[1] == file1.path());
		testAssert(files[2] == sub0.path());
		testAssert(files[3] == sub1.path());
		testAssert(files[4] == sub2.path());
	}
	{// 재귀 있는 모든 검색
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
	{// 검색 패턴 지정 검색 
		auto files = dir.getFiles(L"*.doc", true);
		testAssert(files.size() == 2);
		testAssert(files[0] == file3.path());
		testAssert(files[1] == file1.path());
	}
	{// 검색 패턴 지정 빈 검색
		auto files = dir.getFiles(L"hoge?", true);
		testAssert(files.empty());
	}
	{// 빈 디렉토리 검색
		auto files = sub1.getFiles(L"?*", true);
		testAssert(files.empty());
	}
	{// 복수 계층 검색 패턴 지정 검색
		auto files = dir.getFiles(L"sub0\\subsub2\\*.txt");
		testAssert(files.size() == 1);
		testAssert(files[0] == file2.path());
	}
	{// 복수 계층 검색 패턴 지정 검색 재귀 있음
		auto files = dir.getFiles(L"subsub2\\?*.txt", true);
		testAssert(files.size() == 1);
		testAssert(files[0] == file2.path());
	}
}


testCase(getSpecial) {
	// 무효한 파라미터
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

	// Caution: 시스템의 폴더를 삭제하고 작성되는 것을 테스트는 할수 없음
	//       File::SpecialOption::doNotVerify 및 File::SpecialOption::create 케이스가 가능한 것을 눈으로 확인
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

	// 테스트 데이터 작성
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

	// 무효한 파라미터
	testAssertionFailed(file0.moveTo(L""));

	// 존재하지 않는 파일 이름
	testThrow(file2.moveTo(file3), File::NotFoundException);

	// 복사할 곳이 이미 존재
	testThrow(file0.moveTo(file1), File::AlreadyExistsException);

	// 존재하는 디렉토리에 이동 하려면
	testThrow(file0.moveTo(sub0), File::AlreadyExistsException);

	{// 접근 권한 없음
	}

	// 패스를 찾을 수 없는
	testThrow(File(dir, L"hoge\\file4.txt").moveTo(file0), File::NotFoundException);
	testThrow(file0.moveTo(File(dir, L"hoge\\file4.txt")), File::NotFoundException);

	{// 공유 접근 할 수 없음
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.moveTo(file2), File::SharingViolationException);
	}

	// 부정한 파일 패스
	testThrow(file0.moveTo(L"c::\\hoge.txt"), File::InvalidPathException);

	{// 정상 케이스(덮어 쓰기 없음)
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
	// 테스트용 디렉토리 계층 작성
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

	// 무효한 파라미터
	testAssertionFailed(dir.moveTo(L""));

	// 존재하지 않는 디렉토리
	testThrow(sub1.moveTo(sub0), File::NotFoundException);

	// 존재하지 않는 디렉토리 계층으로 이동
	testThrow(sub0.moveTo(File(dir, L"subUnknown\\subsub0")), File::NotFoundException);

	// 이동 처 벤딩 
	testThrow(sub0.moveTo(sub2), File::AlreadyExistsException);
	testThrow(sub2.moveTo(file0), File::AlreadyExistsException);

	// 자신의 서브 디렉토리로 이동
	testThrow(sub0.moveTo(subsub1), File::SharingViolationException);

	// 무효한 패스
	testThrow(sub0.moveTo(L"c:\\a?c"), File::InvalidPathException);
	testThrow(sub0.moveTo(L"c::\\abc"), File::InvalidPathException);

	{// 파일 오픈된 디렉토리 이동
		auto stream = file0.openRead();
		testThrow(sub0.moveTo(sub1), File::AccessDeniedException);
	}
	{// 정상 케이스
		testNoThrow(sub0.moveTo(sub1));
		testAssert(!sub0.exists());
		testAssert(sub1.exists());
		testAssert(sub1.getFiles(L"?*", true).size() == 4);
	}

	// Caution: 디렉토리 이동 권한 없은 유저로 moveTo를 실행하면 File::AccessDeniedException가 발생하는 것을 눈으로 확인
	// Caution: 다른 디스크 불륨으로 moveTo를 실행하면 File::AccessDeniedException가 발생하는 것을 눈으로 확인
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
#pragma warning(disable : 4189) // 'buffer' : 로컬 변수가 초기화 되었지만 참조 되지 않았다
testCase(open) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 테스트 데이터 작성
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

	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
	}
	File file2(dir, L"file2.txt");

	// 존재하지 않는 파일 이름
	testNoThrow(file2.remove());

	{// 접근 권한 없음
		file0.attributes(file0.attributes() | File::Attributes::readOnly);
		testThrow(file0.remove(), File::AccessDeniedException);
		file0.attributes(file0.attributes() & ~File::Attributes::readOnly);
	}

	// 패스를 찾을 수 없다
	testThrow(File(dir, L"hoge\\file4.txt").remove(), File::NotFoundException);

	{// 공유 접근 할 수 없다
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.remove(), File::SharingViolationException);
	}

	// 부정한 파일 패스
	testThrow(File(L"c::\\hoge.txt").remove(), File::InvalidPathException);

	{// 정상 케이스
		testAssert(file0.exists());
		file0.remove();
		testAssert(!file0.exists());
	}


	{// 삭제 권한 없는 디렉토리와 단체 디렉토리 삭제
		File sub1(dir, L"sub1");
		sub1.createDirectory();
		sub1.attributes(sub1.attributes() | File::Attributes::readOnly);
		testThrow(sub1.remove(), File::AccessDeniedException);
		sub1.attributes(sub1.attributes() & ~File::Attributes::readOnly);
		testNoThrow(sub1.remove());
		testAssert(!sub1.exists());
	}
	{// 비지 않은 디렉토리와 재귀 삭제 
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

	// 테스트 데이터 작성
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

	// 무효한 파라미터
	testAssertionFailed(file0.replace(L"", L" "));

	// 존재하지 않는 파일 이름
	testThrow(file2.replace(file0, L""), File::NotFoundException);
	testThrow(file0.replace(file2, L""), File::NotFoundException);

	// 디렉토리를 바꾼다
	testThrow(sub0.replace(file0, L""), File::AccessDeniedException);
	testThrow(file0.replace(sub0, L""), File::AccessDeniedException);
	testThrow(file0.replace(file1, sub0), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED

	// 패스를 찾을 수 없다
	testThrow(file4.replace(file0, L""), File::NotFoundException);
	testThrow(file0.replace(file4, L""), File::NotFoundException);
	testThrow(file0.replace(file1, file4), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED

	{// 공유 접근 할 수 없다
		FileStream stream(file0, FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(file0.replace(file1, L""), File::SharingViolationException);
		testThrow(file1.replace(file0, L""), File::SharingViolationException);
		testThrow(file1.replace(file3, file0), File::AccessDeniedException); // ERROR_UNABLE_TO_REMOVE_REPLACED
	}

	// 부정한 파일 패스 
	testThrow(file5.replace(file0, L""), File::InvalidPathException);
	testThrow(file0.replace(file5, L""), File::InvalidPathException);
	testThrow(file0.replace(file0, file5), File::SharingViolationException);

	{// 정상 케이스(버퍼 있음)
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
	{// 정상 케이스(버퍼 없음)
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