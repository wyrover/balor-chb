#include "FileStream.hpp"

#include <algorithm>
#include <type_traits>
#include <utility>

#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace io {

using std::max;


namespace {
static_assert(FileStream::Mode::create       == CREATE_NEW, "Invalid enum value");
static_assert(FileStream::Mode::createAlways == CREATE_ALWAYS, "Invalid enum value");
static_assert(FileStream::Mode::open         == OPEN_EXISTING, "Invalid enum value");
static_assert(FileStream::Mode::openAlways   == OPEN_ALWAYS, "Invalid enum value");
static_assert(FileStream::Mode::truncate     == TRUNCATE_EXISTING, "Invalid enum value");
static_assert(FileStream::Mode::append       == 6, "Invalid enum value");

static_assert(FileStream::Access::read      == GENERIC_READ, "Invalid enum value");
static_assert(FileStream::Access::write     == GENERIC_WRITE, "Invalid enum value");
static_assert(FileStream::Access::readWrite == (GENERIC_READ | GENERIC_WRITE), "Invalid enum value");

static_assert(FileStream::Share::read      == FILE_SHARE_READ, "Invalid enum value");
static_assert(FileStream::Share::write     == FILE_SHARE_WRITE, "Invalid enum value");
static_assert(FileStream::Share::remove    == FILE_SHARE_DELETE, "Invalid enum value");

static_assert(FileStream::Options::writeThrough   == FILE_FLAG_WRITE_THROUGH, "Invalid enum value");
static_assert(FileStream::Options::randomAccess   == FILE_FLAG_RANDOM_ACCESS, "Invalid enum value");
static_assert(FileStream::Options::removeOnClose  == FILE_FLAG_DELETE_ON_CLOSE, "Invalid enum value");
static_assert(FileStream::Options::sequentialScan == FILE_FLAG_SEQUENTIAL_SCAN, "Invalid enum value");
static_assert(FileStream::Options::encrypted      == FILE_ATTRIBUTE_ENCRYPTED, "Invalid enum value");

static_assert(std::is_same<HANDLE, FileStream::HANDLE>::value, "Invalid typedef");


void checkError(DWORD errorCode) {
	switch (errorCode) {
		case ERROR_FILE_NOT_FOUND    : 
		case ERROR_PATH_NOT_FOUND    : throw FileStream::NotFoundException();
		case ERROR_FILE_EXISTS       : throw FileStream::AlreadyExistsException();
		case ERROR_ACCESS_DENIED     : throw FileStream::AccessDeniedException();
		case ERROR_SHARING_VIOLATION : throw FileStream::SharingViolationException();
		case ERROR_LOCK_VIOLATION    : throw FileStream::LockViolationException();
		//case ERROR_IO_PENDING        : 
		default                      : assert("Failed to file stream function" && false); break;
	}
}
} // namespace


bool FileStream::Mode::_validate(Mode value) {
	return create <= value && value <= append;
}


bool FileStream::Access::_validate(Access value) {
	switch (value) {
		case read      :
		case write     :
		case readWrite : return true;
		default        : return false;
	}
}


bool FileStream::Share::_validate(Share value) {
	return (value & ~(
		  read     
		| write    
		| remove   
		)) == 0;
}


bool FileStream::Options::_validate(Options value) {
	return (value & ~(
		  FileStream::Options::writeThrough  
		| FileStream::Options::randomAccess  
		| FileStream::Options::removeOnClose 
		| FileStream::Options::sequentialScan
		| FileStream::Options::encrypted     
		)) == 0;
}


FileStream::Lock::Lock(HANDLE handle, __int64 position, __int64 length) : handle(handle), position(position), length(length) {
}


FileStream::Lock::Lock(Lock&& lock) : handle(lock.handle), position(lock.position), length(lock.length) {
	lock.handle = nullptr;
}


FileStream::Lock::~Lock() {
	if (handle) {
		OVERLAPPED overlapped;
		ZeroMemory(&overlapped, sizeof(overlapped));
		overlapped.Offset     = position & 0xffffffff;
		overlapped.OffsetHigh = position >> 32;
		if (!UnlockFileEx(handle, 0, length & 0xffffffff, length >> 32, &overlapped)) {
			checkError(GetLastError());
		}
		//handle = nullptr;
	}
}



FileStream::FileStream() : _handle(nullptr), _access(Access::readWrite) {
}


FileStream::FileStream(StringRange path, FileStream::Mode mode, FileStream::Access access, FileStream::Share share, FileStream::Options options) : _handle(nullptr) {
	assert("Empty path" && !path.empty());
	assert("Invalid mode" && Mode::_validate(mode));
	assert("Invalid access" && Access::_validate(access));
	assert("Invalid share" && Share::_validate(share));
	assert("Invalid options" && Options::_validate(options));

	_handle = CreateFileW(path.c_str(), access, share, nullptr
		, mode == FileStream::Mode::append ? FileStream::Mode::openAlways : mode
		, (options & FILE_ATTRIBUTE_ENCRYPTED) == FILE_ATTRIBUTE_ENCRYPTED ? options : (options | FILE_ATTRIBUTE_NORMAL)
		, nullptr);
	if (_handle == INVALID_HANDLE_VALUE) {
		_handle = nullptr;
		checkError(GetLastError());
	}

	if (mode == FileStream::Mode::append) {
		try {
			skipToEnd();
		} catch (Exception& exception) {
			verify(CloseHandle(_handle));
			_handle = nullptr;
			throw exception;
		}
	}
	this->_access = access;
}


FileStream::FileStream(FileStream&& value) : _handle(value._handle), _access(value._access) {
	value._handle = nullptr;
}


FileStream::~FileStream() {
	if (_handle) {
		verify(CloseHandle(_handle));
		//_handle = nullptr;
	}
}


FileStream& FileStream::operator=(FileStream&& value) {
	std::swap(_handle, value._handle);
	std::swap(_access, value._access);
	return *this;
}


void FileStream::flush() {
	assert("Null _handle" && _handle);

	if (writable()) {
		verify(FlushFileBuffers(_handle));
	}
}


__int64 FileStream::length() const {
	assert("Null _handle" && _handle);

	LARGE_INTEGER size;
	size.QuadPart = 0;
	if (!GetFileSizeEx(_handle, &size)) {
		checkError(GetLastError());
	}
	return size.QuadPart;
}


FileStream::Lock FileStream::lock(__int64 position, __int64 length) {
	assert("Null _handle" && _handle);
	assert("Negative position" && 0 <= position);
	assert("Negative length" && 0 <= length);

	OVERLAPPED overlapped;
	ZeroMemory(&overlapped, sizeof(overlapped));
	overlapped.Offset     = position & 0xffffffff;
	overlapped.OffsetHigh = position >> 32;
	if (!LockFileEx(_handle, LOCKFILE_FAIL_IMMEDIATELY | LOCKFILE_EXCLUSIVE_LOCK, 0, length & 0xffffffff, length >> 32, &overlapped)) {
		checkError(GetLastError());
	}
	return Lock(_handle, position, length);
}


__int64 FileStream::position() const {
	assert("Null _handle" && _handle);

	LARGE_INTEGER offsetStruct;
	offsetStruct.QuadPart = 0;
	LARGE_INTEGER newPosition;
	newPosition.QuadPart = 0;
	if (!SetFilePointerEx(_handle, offsetStruct, &newPosition, FILE_CURRENT)) {
		checkError(GetLastError());
	}
	return newPosition.QuadPart;
}


void FileStream::position(__int64 value) {
	assert("Null _handle" && _handle);
	assert("Negative position" && 0 <= value);

	LARGE_INTEGER offsetStruct;
	offsetStruct.QuadPart = max(static_cast<__int64>(0), value);
	LARGE_INTEGER newPosition;
	newPosition.QuadPart = 0;
	if (!SetFilePointerEx(_handle, offsetStruct, &newPosition, FILE_BEGIN)) {
		checkError(GetLastError());
	}
}


int FileStream::read(void* buffer, int offset, int count) {
	assert("Null _handle" && _handle);
	assert("Null buffer" && buffer);
	assert("Negative offset" && 0 <= offset);
	assert("Negative count" && 0 <= count);
	assert("buffer is bad write pointer" && !IsBadWritePtr(buffer, offset + count));
	assert("read unsupported" && readable());

	DWORD readCount = 0;
	if (!ReadFile(_handle, static_cast<char*>(buffer) + offset, count, &readCount, nullptr)) {
		checkError(GetLastError());
	}
	return readCount;
}


bool FileStream::readable() const {
	return (_access & FileStream::Access::read) != 0;
}


__int64 FileStream::skip(__int64 offset) {
	assert("Null _handle" && _handle);

	LARGE_INTEGER offsetStruct;
	offsetStruct.QuadPart = offset;
	LARGE_INTEGER newPosition;
	newPosition.QuadPart = 0;
	if (!SetFilePointerEx(_handle, offsetStruct, &newPosition, FILE_CURRENT)) {
		const DWORD errorCode = GetLastError();
		if (errorCode == ERROR_NEGATIVE_SEEK) {
			position(0);
			return position();
		} else {
			checkError(errorCode);
		}
	}
	return newPosition.QuadPart;
}


__int64 FileStream::skipToEnd() {
	assert("Null _handle" && _handle);

	LARGE_INTEGER offsetStruct;
	offsetStruct.QuadPart = 0;
	LARGE_INTEGER newPosition;
	newPosition.QuadPart = 0;
	if (!SetFilePointerEx(_handle, offsetStruct, &newPosition, FILE_END)) {
		checkError(GetLastError());
	}
	return newPosition.QuadPart;
}


void FileStream::write(const void* buffer, int offset, int count) {
	assert("Null _handle" && _handle);
	assert("Null buffer" && buffer);
	assert("Negative offset" && 0 <= offset);
	assert("Negative count" && 0 <= count);
	assert("buffer is bad read pointer" && !IsBadReadPtr(buffer, offset + count));
	assert("write unsupported" && writable());

	DWORD writeCount = 0;
	if (!WriteFile(_handle, static_cast<const char*>(buffer) + offset, count, &writeCount, nullptr)) {
		checkError(GetLastError());
	}
}


bool FileStream::writable() const {
	return (_access & FileStream::Access::write) != 0;
}



	}
}