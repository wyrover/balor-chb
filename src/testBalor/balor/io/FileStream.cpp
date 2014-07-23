#include <balor/io/FileStream.hpp>

#include <utility>

#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace io {
		namespace testFileStream {


using std::move;
using std::vector;
using namespace balor::io;


namespace {
const wchar_t testDirectoryName[] = L"testBalor_io_FileStream_e8oj67gjkf0adfj67fhal0894sa";
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
} // namespace



testCase(construct) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("abc", 0, 3);
	}
	File file1(dir, L"file1.txt");
	File file2(dir, L"sub0\\file2.txt");

	// 무효한 파라미터
	testAssertionFailed(FileStream(L"", FileStream::Mode::createAlways));
	testAssertionFailed(FileStream(file0.path(), FileStream::Mode::_enum(-1)));
	testAssertionFailed(FileStream(file0.path(), FileStream::Mode::createAlways, FileStream::Access::_enum(-1)));
	testAssertionFailed(FileStream(file0.path(), FileStream::Mode::createAlways, FileStream::Access::write, FileStream::Share::_enum(-1)));
	testAssertionFailed(FileStream(file0.path(), FileStream::Mode::createAlways, FileStream::Access::write, FileStream::Share::write, FileStream::Options::_enum(-1)));

	// 존재하지 않는 파일
	testThrow(FileStream(file1.path(), FileStream::Mode::open), FileStream::NotFoundException);
	testThrow(FileStream(file1.path(), FileStream::Mode::truncate), FileStream::NotFoundException);

	// 존재하지 않는 디렉토리
	testThrow(FileStream(file2.path(), FileStream::Mode::openAlways), FileStream::NotFoundException);

	{// 이미 파일이 존재한다
		testThrow(FileStream(file0.path(), FileStream::Mode::create), FileStream::AlreadyExistsException);
	}
	{// 접근 권한이 없다
		file0.attributes(file0.attributes() | File::Attributes::readOnly);
		testThrow(FileStream(file0.path(), FileStream::Mode::append, FileStream::Access::write), FileStream::AccessDeniedException);
		file0.attributes(file0.attributes() & ~File::Attributes::readOnly);
		file0.attributes(file0.attributes() | File::Attributes::hidden);
		testThrow(FileStream(file0.path(), FileStream::Mode::createAlways), FileStream::AccessDeniedException);
		file0.attributes(file0.attributes() & ~ File::Attributes::hidden);
	}
	{// 접근 공유를 할 수 없다
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::none);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read), FileStream::SharingViolationException);
	}
	{
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::write), FileStream::SharingViolationException);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::read), FileStream::SharingViolationException);
	}
	{
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::write);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::write), FileStream::SharingViolationException);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::read), FileStream::SharingViolationException);
	}
	{// FileAccess와 FileShare가 일치하지 않으면 잘 되지 않는듯
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::readWrite, FileStream::Share::read);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read), FileStream::SharingViolationException);
		testThrow(FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::read), FileStream::SharingViolationException);
	}

	{// FileStream::Mode::create
		{
			testAssert(!file1.exists());
			FileStream stream(file1.path(), FileStream::Mode::create);
			testAssert(file1.exists());
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 0);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 3);
		}
		file1.remove();
	}
	{// FileStream::Mode::createAlways
		{// 존재하는 파일 덮어 쓰기
			testAssert(file0.openRead().length() == 3);
			FileStream stream(file0.path(), FileStream::Mode::createAlways);
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 0);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 3);
		}
		{// 파일 신규 작성 
			testAssert(!file1.exists());
			FileStream stream(file1.path(), FileStream::Mode::createAlways);
			testAssert(file1.exists());
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 0);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 3);
		}
		file1.remove();
	}
	{// FileStream::Mode::open
		testAssert(file0.openRead().length() == 3);
		FileStream stream(file0.path(), FileStream::Mode::open);
		testAssert(stream.position() == 0);
		testAssert(stream.length() == 3);
		char buffer[4] = {0};
		stream.read(buffer, 0, 3);
		testAssert(String::equals(buffer, "abc"));
	}
	{// FileStream::Mode::openAlways
		{// open
			testAssert(file0.openRead().length() == 3);
			FileStream stream(file0.path(), FileStream::Mode::openAlways);
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 3);
			char buffer[4] = {0};
			stream.read(buffer, 0, 3);
			testAssert(String::equals(buffer, "abc"));
		}
		{// create
			testAssert(!file1.exists());
			FileStream stream(file1.path(), FileStream::Mode::openAlways);
			testAssert(file1.exists());
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 0);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 3);
		}
		file1.remove();
	}
	{// FileStream::Mode::truncate
		testAssert(file0.openRead().length() == 3);
		FileStream stream(file0.path(), FileStream::Mode::truncate);
		testAssert(stream.position() == 0);
		testAssert(stream.length() == 0);
		stream.write("abc", 0, 3);
		testAssert(stream.length() == 3);
	}
	{// FileStream::Mode::append
		{// 추가
			testAssert(file0.openRead().length() == 3);
			FileStream stream(file0.path(), FileStream::Mode::append);
			testAssert(stream.position() == 3);
			testAssert(stream.length() == 3);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 6);
		}
		{// 신규
			testAssert(!file1.exists());
			FileStream stream(file1.path(), FileStream::Mode::append);
			testAssert(file1.exists());
			testAssert(stream.position() == 0);
			testAssert(stream.length() == 0);
			stream.write("abc", 0, 3);
			testAssert(stream.length() == 3);
		}
		file1.remove();
	}

	{// FileAccess
		FileStream stream(file0.path(), FileStream::Mode::open);
		testAssert(stream.readable());
		testAssert(stream.writable());
	}
	{
		FileStream stream = FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::read);
		testAssert(stream.readable());
		testAssert(!stream.writable());
	}
	{
		FileStream stream = FileStream(file0.path(), FileStream::Mode::open, FileStream::Access::write);
		testAssert(!stream.readable());
		testAssert(stream.writable());
	}

	{// FileShare
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read);
		FileStream stream1(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read);
		testAssert(stream0.length() == 6);
		testAssert(stream1.length() == 6);
		char buffer0[4] = {0};
		stream0.read(buffer0, 0, 3);
		testAssert(String::equals(buffer0, "abc"));
		char buffer1[4] = {0};
		stream1.read(buffer1, 0, 3);
		testAssert(String::equals(buffer1, "abc"));
	}
	{
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::write);
		FileStream stream1(file0.path(), FileStream::Mode::open, FileStream::Access::write, FileStream::Share::write);
		testAssert(stream0.length() == 6);
		testAssert(stream1.length() == 6);
		stream0.position(6);
		stream0.write("012", 0, 3);
		testAssert(stream1.length() == 9);
	}
	{
		FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::readWrite, FileStream::Share::read | FileStream::Share::write);
		FileStream stream1(file0.path(), FileStream::Mode::open, FileStream::Access::read, FileStream::Share::read | FileStream::Share::write);
		testAssert(stream0.length() == 9);
		testAssert(stream1.length() == 9);
		stream0.write("012", 0, 3);
		testAssert(stream1.length() == 9);
		char buffer1[4] = {0};
		stream1.read(buffer1, 0, 3);
		testAssert(String::equals(buffer1, "012"));
	}

	{// FileOptions
		{
			FileStream stream(file1.path(), FileStream::Mode::create, FileStream::Access::write, FileStream::Share::none, FileStream::Options::removeOnClose);
			testAssert(file1.exists());
		}
		testAssert(!file1.exists());
	}
}


testCase(moveConstructAndAssignment) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	file0.create();
	File file1(dir, L"file1.txt");
	{
		auto stream = file1.create();
		stream.write("abc", 0, 3);
	}

	// move construct
	FileStream stream0(file0.path(), FileStream::Mode::open, FileStream::Access::read);
	FileStream stream1 = move(stream0);
	testAssert(stream1.length() == 0);
	testAssert(stream1.readable());
	testAssert(!stream1.writable());

	// move assignment
	FileStream stream2(file1.path(), FileStream::Mode::open, FileStream::Access::write);
	FileStream& result = stream1 = move(stream2);
	testAssert(&result == &stream1);
	testAssert(stream1.length() == 3);
	testAssert(!stream1.readable());
	testAssert(stream1.writable());
}


testCase(destruct) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");

	balor::test::HandleLeakChecker checker;
	{
		FileStream stream(file0.path(), FileStream::Mode::createAlways, FileStream::Access::write);
		testAssert(checker.getSystemHandleChange() == 1);
	}
	testAssert(checker.getSystemHandleChange() == 0);
}


testCase(flush) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	file0.create();

	FileStream stream0(file0.path(), FileStream::Mode::open);
	FileStream stream1 = move(stream0);
	testAssertionFailed(stream0.flush());
	stream1.write("abc", 0, 3);
	testNoThrow(stream1.flush());
}


testCase(lengthAndPosition) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("0123", 0, 4);
	}
	File file1(dir, L"file1.txt");
	{
		auto stream = file1.create();
		stream.write("01", 0, 2);
	}

	FileStream stream0(file0, FileStream::Mode::open, FileStream::Access::read);
	auto stream1 = move(stream0);
	auto stream2 = file1.openRead();

	// 무효한 파라미터
	testAssertionFailed(stream0.length());
	testAssertionFailed(stream0.position());
	testAssertionFailed(stream0.position(0));
	testAssertionFailed(stream1.position(-1));

	// length
	testAssert(stream1.length() == 4);
	testAssert(stream2.length() == 2);

	// position
	testAssert(stream1.position() == 0);
	stream1.position(1);
	testAssert(stream1.position() == 1);
	stream1.position(5);
	testAssert(stream1.position() == 5);
}


testCase(lock) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("0123", 0, 4);
	}

	{
		auto stream0 = file0.openRead();
		auto stream1 = move(stream0);
		
		// 무효한 파라미터
		testAssertionFailed(stream0.lock(0, 2));
		testAssertionFailed(stream1.lock(-1, 2));
		testAssertionFailed(stream1.lock(0, -1));
		testNoThrow        (stream1.lock(0, 0));

		auto stream2 = file0.openRead();
		{// 락
			auto lock = stream1.lock(0, 2);
			char buffer[3] = {0};
			stream1.read(buffer, 0, 2);
			testAssert(String::equals(buffer, "01"));

			// 락 범위 중복
			testThrow(stream1.lock(1, 2), FileStream::LockViolationException);

			// 락 범위 읽기
			char buffer2[3] = {0};
			testThrow(stream2.read(buffer2, 0, 2), FileStream::LockViolationException);
			stream2.position(2);
			stream2.read(buffer2, 0, 2);
			testAssert(String::equals(buffer2, "23"));
		}
		{// 락 해제
			stream2.position(0);
			char buffer2[3] = {0};
			stream2.read(buffer2, 0, 2);
			testAssert(String::equals(buffer2, "01"));
		}
	}
}


testCase(read) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("0123", 0, 4);
	}
	File file1(dir, L"file1.txt");
	file1.create();

	auto stream0 = file0.openRead();
	auto stream1 = move(stream0);
	auto stream2 = file1.openWrite();

	// 무효한 파라미터
	char buffer[5] = {0};
	testAssertionFailed(stream0.read(buffer, 0, 4));
	testAssertionFailed(stream1.read(nullptr, 0, 4));
	testAssertionFailed(stream1.read(buffer, -1, 4));
	testAssertionFailed(stream1.read(buffer, 0, -1));
	testAssertionFailed(stream1.read("abcd", 0, 4));
	testNoThrow        (stream1.read(buffer, 0, 0));
	testAssertionFailed(stream2.read(buffer, 0, 4));

	{// 락 범위 읽기
		auto stream3 = file0.openRead();
		auto lock = stream3.lock(0, 4);
		testThrow(stream1.read(buffer, 0, 4), FileStream::LockViolationException);
	}

	// 읽기
	testAssert(stream1.read(buffer, 0, 4) == 4);
	testAssert(stream1.position() == 4);
	testAssert(String::equals(buffer, "0123"));
	testAssert(stream1.read(buffer, 0, 0) == 0);
	testAssert(stream1.position() == 4);
	testAssert(String::equals(buffer, "0123"));
	stream1.position(0);
	testAssert(stream1.read(buffer, 1, 2) == 2);
	testAssert(stream1.position() == 2);
	testAssert(String::equals(buffer, "0013"));
	testAssert(stream1.read(buffer, 1, 0) == 0);
	testAssert(stream1.position() == 2);
	testAssert(String::equals(buffer, "0013"));

	// 락 범위 외 읽기 
	testAssert(stream1.read(buffer, 0, 4) == 2);
	testAssert(String::equals(buffer, "2313"));

	testAssert(stream1.position() == 4);
	testAssert(stream1.read(buffer, 0, 1) == 0);
	testAssert(stream1.position() == 4);
	testAssert(String::equals(buffer, "2313"));
}


//testCase(readable) { // testCase(construct) 에서 테스트 완료
//}


testCase(skip) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("0123", 0, 4);
	}

	auto stream0 = file0.openRead();
	auto stream1 = move(stream0);

	// 무효한 파라미터
	testAssertionFailed(stream0.skip(1));

	// 틀린 위치 seek
	stream1.position(0);
	testAssert(stream1.skip(-1) == 0);

	// seek 테스트
	char buffer[2] = {0};
	stream1.position(0);
	testAssert(stream1.skip(1) == 1);
	stream1.read(buffer, 0, 1);
	testAssert(buffer[0] == L'1');
	testAssert(stream1.skip(-2) == 0);
	stream1.read(buffer, 0, 1);
	testAssert(buffer[0] == L'0');
	testAssert(stream1.skip(0) == 1);
	stream1.read(buffer, 0, 1);
	stream1.position(0);

	// 범위 외 seek 
	testAssert(stream1.skip(5) == 5);
	testAssert(stream1.length() == 4);
	stream1.position(4);
	testAssert(stream1.read(buffer, 0, 1) == 0);
}


testCase(write) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();
	// 테스트 데이터 작성
	File file0(dir, L"file0.txt");
	{
		auto stream = file0.create();
		stream.write("0123", 0, 4);
	}
	File file1(dir, L"file1.txt");
	file1.create();

	FileStream stream0(file0, FileStream::Mode::open, FileStream::Access::readWrite, FileStream::Share::read | FileStream::Share::write);
	auto stream1 = move(stream0);
	auto stream2 = file1.openRead();

	// 무효한 파라미터
	testAssertionFailed(stream0.write("0123", 0, 4));
	testAssertionFailed(stream1.write(nullptr, 0, 4));
	testAssertionFailed(stream1.write("0123", -1, 4));
	testAssertionFailed(stream1.write("0123", 0, -1));
	testNoThrow        (stream1.write("0123", 0, 0));
	testAssertionFailed(stream2.write("0123", 0, 4));

	{// 락 범위 쓰기
		FileStream stream3(file0, FileStream::Mode::open, FileStream::Access::readWrite, FileStream::Share::read | FileStream::Share::write);
		auto lock = stream3.lock(0, 4);
		testThrow(stream1.write("0123", 0, 4), FileStream::LockViolationException);
	}

	// 쓰기
	char buffer[10] = {0};
	stream1.write("abcd", 0, 4);
	testAssert(stream1.length() == 4);
	stream1.position(0);
	stream1.read(buffer, 0, 4);
	testAssert(String::equals(buffer, "abcd"));
	stream1.write("abcd", 0, 0);
	testAssert(stream1.length() == 4);
	stream1.position(0);
	stream1.read(buffer, 0, 4);
	testAssert(String::equals(buffer, "abcd"));
	stream1.write("0123", 1, 2);
	testAssert(stream1.length() == 6);
	stream1.position(0);
	stream1.read(buffer, 0, 6);
	testAssert(String::equals(buffer, "abcd12"));
	stream1.write("0123", 1, 0);
	testAssert(stream1.length() == 6);
	stream1.position(0);
	stream1.read(buffer, 0, 6);
	testAssert(String::equals(buffer, "abcd12"));

	// 락 범위 외 쓰기
	stream1.position(7);
	stream1.write("+", 0, 1);
	testAssert(stream1.length() == 8);
	stream1.position(0);
	stream1.read(buffer, 0, 8);
	testAssert(std::memcmp(buffer, "abcd12\0+", 8) == 0);
}


//testCase(writable) { // testCase(construct) 에서 테스트 완료
//}



		}
	}
}