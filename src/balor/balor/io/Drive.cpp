#include "Drive.hpp"

#include <utility>

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace io {

using std::move;


namespace {
static_assert(Drive::Type::unknown         == DRIVE_UNKNOWN    , "Invalid enum value");
static_assert(Drive::Type::noRootDirectory == DRIVE_NO_ROOT_DIR, "Invalid enum value");
static_assert(Drive::Type::removable       == DRIVE_REMOVABLE  , "Invalid enum value");
static_assert(Drive::Type::fixed           == DRIVE_FIXED      , "Invalid enum value");
static_assert(Drive::Type::remote          == DRIVE_REMOTE     , "Invalid enum value");
static_assert(Drive::Type::cdRom           == DRIVE_CDROM      , "Invalid enum value");
static_assert(Drive::Type::ramDisk         == DRIVE_RAMDISK    , "Invalid enum value");


// ディスクが準備できていない場合の警告ダイアログを出さないようにする。
struct DiskErrorSkip {
	DiskErrorSkip() : before(SetErrorMode(SEM_FAILCRITICALERRORS)) {
	}
	~DiskErrorSkip() {
		SetErrorMode(before);
	}
	UINT before;
};

void checkResult(BOOL result) {
	if (!result) {
		switch (GetLastError()) {
			case ERROR_ACCESS_DENIED  : throw Drive::AccessDeniedException();
			case ERROR_NOT_READY      : throw Drive::NotReadyException();
			case ERROR_PATH_NOT_FOUND : throw Drive::NotFoundException();
			default                   : assert("Failed to drive function" && false); break;
		}
	}
}
} // namespace



Drive::DrivesIterator::DrivesIterator(bool begin) {
	mask = GetLogicalDrives();
	assert("Failed to GetLogicalDrives" && mask);
	if (begin) {
		for (current = 0; !(mask & (1 << current)) && current < 26; ++current) {
		}
	} else {
		current = 26;
	}
}


Drive Drive::DrivesIterator::operator*() {
	assert("DrivesIterator out of range" && current < 26);
	return Drive(static_cast<wchar_t>(L'A' + current));
}


Drive::DrivesIterator& Drive::DrivesIterator::operator++() {
	assert("DrivesIterator out of range" && current < 26);
	for (; current < 26 && !(mask & (1 << ++current));) {
	}
	return *this;
}


Drive::DrivesIterator& Drive::DrivesIterator::operator--() {
	assert("DrivesIterator out of range" && 0 < current);
	for (; 0 < current && !(mask & (1 << --current)); ) {
	}
	assert("DrivesIterator out of range" && mask & (1 << current));
	return *this;
}


bool Drive::DrivesIterator::operator<(const Drive::DrivesIterator& rhs) {
	return current < rhs.current; // mask は違うかもしれない。
}


Drive::Drive(wchar_t letter) {
	assert("Invalid drive name" && ((L'a' <= letter && letter <= L'z') || (L'A' <= letter && letter <= L'Z')));
	_name[0] = letter;
	_name[1] = L':';
	_name[2] = L'\\';
	_name[3] = L'\0';
}


Drive::Drive(StringRange name) {
	assert("Invalid drive name" && ((L'a' <= name.c_str()[0] && name.c_str()[0] <= L'z') || (L'A' <= name.c_str()[0] && name.c_str()[0] <= L'Z')));
	assert("Invalid drive name" && (name.length() < 2 || name.c_str()[1] == L':'));
	assert("Invalid drive name" && (name.length() < 3 || name.c_str()[2] == L'\\'));
	_name[0] = name.c_str()[0];
	_name[1] = L':';
	_name[2] = L'\\';
	_name[3] = L'\0';
}


__int64 Drive::availableFreeBytes() const {
	ULARGE_INTEGER availableFreeBytes;
	ULARGE_INTEGER totalBytes;
	DiskErrorSkip skip;
	availableFreeBytes.QuadPart = 0;
	checkResult(GetDiskFreeSpaceExW(_name, &availableFreeBytes, &totalBytes, nullptr));
	return availableFreeBytes.QuadPart;
}


Drive::DrivesIterator Drive::drivesBegin() {
	return DrivesIterator(true);
}


Drive::DrivesIterator Drive::drivesEnd() {
	return DrivesIterator(false);
}


String Drive::fileSystem() const {
	wchar_t array[MAX_PATH];
	StringBuffer buffer(array);
	fileSystemToBuffer(buffer);
	return buffer;
}


void Drive::fileSystemToBuffer(StringBuffer& buffer) const {
	DWORD maxFileName = 0;
	DWORD fileSystemFlags = 0;
	buffer.reserveAdditionally(MAX_PATH - 1);
	DiskErrorSkip skip;
	checkResult(GetVolumeInformationW(_name, nullptr, 0, nullptr, &maxFileName, &fileSystemFlags, buffer.end(), MAX_PATH));
	buffer.expandLength();
}


wchar_t Drive::letter() const {
	return _name[0];
}


const wchar_t* Drive::name() const {
	return _name;
}


bool Drive::ready() const {
	ULARGE_INTEGER availableFreeBytes;
	ULARGE_INTEGER totalBytes;
	DiskErrorSkip skip;
	return GetDiskFreeSpaceExW(_name, &availableFreeBytes, &totalBytes, nullptr) != 0;
}


__int64 Drive::totalFreeBytes() const {
	ULARGE_INTEGER availableFreeBytes;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytes;
	DiskErrorSkip skip;
	checkResult(GetDiskFreeSpaceExW(_name, &availableFreeBytes, &totalBytes, &totalFreeBytes));
	return totalFreeBytes.QuadPart;
}


__int64 Drive::totalBytes() const {
	ULARGE_INTEGER availableFreeBytes;
	ULARGE_INTEGER totalBytes;
	DiskErrorSkip skip;
	checkResult(GetDiskFreeSpaceExW(_name, &availableFreeBytes, &totalBytes, nullptr));
	return totalBytes.QuadPart;
}


Drive::Type Drive::type() const {
	return static_cast<Type>(GetDriveTypeW(_name));
}


String Drive::volumeLabel() const {
	wchar_t array[MAX_PATH];
	StringBuffer buffer(array);
	volumeLabelToBuffer(buffer);
	return buffer;
}


void Drive::volumeLabel(StringRange value) {
	DiskErrorSkip skip;
	checkResult(SetVolumeLabelW(_name, value.c_str()));
}


void Drive::volumeLabelToBuffer(StringBuffer& buffer) const {
	DWORD maxFileName = 0;
	DWORD fileSystemFlags = 0;
	buffer.reserveAdditionally(MAX_PATH - 1);
	DiskErrorSkip skip;
	checkResult(GetVolumeInformationW(_name, buffer.end(), MAX_PATH, nullptr, &maxFileName, &fileSystemFlags, nullptr, 0));
	buffer.expandLength();
}



	}
}