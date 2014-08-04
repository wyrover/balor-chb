#include "File.hpp"

#include <utility>
#include <vector>
#include <Shlwapi.h>
#pragma comment (lib,"Shlwapi.lib")

#include <balor/graphics/Icon.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>

#include <ShlObj.h>


namespace balor {
	namespace io {

using std::move;
using std::swap;
using std::vector;
using namespace balor::graphics;


namespace {
static_assert(File::Attributes::readOnly          == FILE_ATTRIBUTE_READONLY           , "Invalid enum value");
static_assert(File::Attributes::hidden            == FILE_ATTRIBUTE_HIDDEN             , "Invalid enum value");
static_assert(File::Attributes::system            == FILE_ATTRIBUTE_SYSTEM             , "Invalid enum value");
static_assert(File::Attributes::directory         == FILE_ATTRIBUTE_DIRECTORY          , "Invalid enum value");
static_assert(File::Attributes::archive           == FILE_ATTRIBUTE_ARCHIVE            , "Invalid enum value");
static_assert(File::Attributes::normal            == FILE_ATTRIBUTE_NORMAL             , "Invalid enum value");
static_assert(File::Attributes::temporary         == FILE_ATTRIBUTE_TEMPORARY          , "Invalid enum value");
static_assert(File::Attributes::sparseFile        == FILE_ATTRIBUTE_SPARSE_FILE        , "Invalid enum value");
static_assert(File::Attributes::reparsePoint      == FILE_ATTRIBUTE_REPARSE_POINT      , "Invalid enum value");
static_assert(File::Attributes::compressed        == FILE_ATTRIBUTE_COMPRESSED         , "Invalid enum value");
static_assert(File::Attributes::offline           == FILE_ATTRIBUTE_OFFLINE            , "Invalid enum value");
static_assert(File::Attributes::notContentIndexed == FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, "Invalid enum value");
static_assert(File::Attributes::encrypted         == FILE_ATTRIBUTE_ENCRYPTED          , "Invalid enum value");
static_assert(File::Attributes::virtualFile       == FILE_ATTRIBUTE_VIRTUAL            , "Invalid enum value");

static_assert(File::Special::adminTools             == CSIDL_ADMINTOOLS, "Invalid enum value");
static_assert(File::Special::appData                == CSIDL_APPDATA, "Invalid enum value");
static_assert(File::Special::CDBurnArea             == CSIDL_CDBURN_AREA, "Invalid enum value");
static_assert(File::Special::commonAdminTools       == CSIDL_COMMON_ADMINTOOLS, "Invalid enum value");
static_assert(File::Special::commonAppData          == CSIDL_COMMON_APPDATA, "Invalid enum value");
static_assert(File::Special::commonDesktopDirectory == CSIDL_COMMON_DESKTOPDIRECTORY, "Invalid enum value");
static_assert(File::Special::commonDocuments        == CSIDL_COMMON_DOCUMENTS, "Invalid enum value");
static_assert(File::Special::commonMusic            == CSIDL_COMMON_MUSIC, "Invalid enum value");
static_assert(File::Special::commonOemLinks         == CSIDL_COMMON_OEM_LINKS, "Invalid enum value");
static_assert(File::Special::commonPictures         == CSIDL_COMMON_PICTURES, "Invalid enum value");
static_assert(File::Special::commonPrograms         == CSIDL_COMMON_PROGRAMS, "Invalid enum value");
static_assert(File::Special::commonStartMenu        == CSIDL_COMMON_STARTMENU, "Invalid enum value");
static_assert(File::Special::commonStartup          == CSIDL_COMMON_STARTUP, "Invalid enum value");
static_assert(File::Special::commonTemplates        == CSIDL_COMMON_TEMPLATES, "Invalid enum value");
static_assert(File::Special::commonVideos           == CSIDL_COMMON_VIDEO, "Invalid enum value");
static_assert(File::Special::cookies                == CSIDL_COOKIES, "Invalid enum value");
static_assert(File::Special::desktop                == CSIDL_DESKTOP, "Invalid enum value");
static_assert(File::Special::desktopDirectory       == CSIDL_DESKTOPDIRECTORY, "Invalid enum value");
static_assert(File::Special::favorites              == CSIDL_FAVORITES, "Invalid enum value");
static_assert(File::Special::fonts                  == CSIDL_FONTS, "Invalid enum value");
static_assert(File::Special::history                == CSIDL_HISTORY, "Invalid enum value");
static_assert(File::Special::internetCache          == CSIDL_INTERNET_CACHE, "Invalid enum value");
static_assert(File::Special::localAppData           == CSIDL_LOCAL_APPDATA, "Invalid enum value");
static_assert(File::Special::localizedResources     == CSIDL_RESOURCES_LOCALIZED, "Invalid enum value");
static_assert(File::Special::myComputer             == CSIDL_DRIVES, "Invalid enum value");
static_assert(File::Special::myDocuments            == CSIDL_MYDOCUMENTS, "Invalid enum value");
static_assert(File::Special::myMusic                == CSIDL_MYMUSIC, "Invalid enum value");
static_assert(File::Special::myPictures             == CSIDL_MYPICTURES, "Invalid enum value");
static_assert(File::Special::myVideos               == CSIDL_MYVIDEO, "Invalid enum value");
static_assert(File::Special::netHood                == CSIDL_NETHOOD, "Invalid enum value");
static_assert(File::Special::personal               == CSIDL_PERSONAL, "Invalid enum value");
static_assert(File::Special::printHood              == CSIDL_PRINTHOOD, "Invalid enum value");
static_assert(File::Special::programFiles           == CSIDL_PROGRAM_FILES, "Invalid enum value");
static_assert(File::Special::programFilesX86        == CSIDL_PROGRAM_FILESX86, "Invalid enum value");
static_assert(File::Special::programFilesCommon     == CSIDL_PROGRAM_FILES_COMMON, "Invalid enum value");
static_assert(File::Special::programFilesCommonX86  == CSIDL_PROGRAM_FILES_COMMONX86, "Invalid enum value");
static_assert(File::Special::programs               == CSIDL_PROGRAMS, "Invalid enum value");
static_assert(File::Special::recent                 == CSIDL_RECENT, "Invalid enum value");
static_assert(File::Special::resources              == CSIDL_RESOURCES, "Invalid enum value");
static_assert(File::Special::sendTo                 == CSIDL_SENDTO, "Invalid enum value");
static_assert(File::Special::startMenu              == CSIDL_STARTMENU, "Invalid enum value");
static_assert(File::Special::startup                == CSIDL_STARTUP, "Invalid enum value");
static_assert(File::Special::system                 == CSIDL_SYSTEM, "Invalid enum value");
static_assert(File::Special::systemX86              == CSIDL_SYSTEMX86, "Invalid enum value");
static_assert(File::Special::templates              == CSIDL_TEMPLATES, "Invalid enum value");
static_assert(File::Special::userProfile            == CSIDL_PROFILE, "Invalid enum value");
static_assert(File::Special::windows                == CSIDL_WINDOWS, "Invalid enum value");

static_assert(File::SpecialOption::create      == CSIDL_FLAG_CREATE, "Invalid enum value");
static_assert(File::SpecialOption::doNotVerify == CSIDL_FLAG_DONT_VERIFY, "Invalid enum value");

static_assert(File::maxPath == MAX_PATH, "Invalid maxPath");


void checkError(DWORD errorCode) {
	switch (errorCode) {
		case ERROR_ACCESS_DENIED                : throw File::AccessDeniedException(); // 異なるボリュームに移動しようとしたか、アクセス権がない
		case ERROR_ALREADY_EXISTS               :
		case ERROR_FILE_EXISTS                  : throw File::AlreadyExistsException();
		case ERROR_DIR_NOT_EMPTY                : throw File::NotEmptyException(); // ディレクトリが空ではない
		case ERROR_FILE_NOT_FOUND               :
		case ERROR_PATH_NOT_FOUND               : throw File::NotFoundException();
		case ERROR_INVALID_NAME                 : throw File::InvalidPathException();
		case ERROR_SHARING_VIOLATION            : throw File::SharingViolationException(); // 移動先が自分のサブディレクトリ
		case ERROR_UNABLE_TO_REMOVE_REPLACED    : // バックアップファイルを削除できなかった
		case ERROR_UNABLE_TO_MOVE_REPLACEMENT   : // リネームができなかった
		case ERROR_UNABLE_TO_MOVE_REPLACEMENT_2 : throw File::AccessDeniedException(); // リネームができなかった
		default                      : assert("Failed to file api" && false);
	}
}


void getFilesToVector(vector<File>& files, const File& file, StringRange searchPettern, bool recursive) {
	if (recursive) {
		auto i = file.getFilesIterator(); // サブディレクトリを再帰的に検索していく
		while (i) {
			if (i->isDirectory()) {
				getFilesToVector(files, *i, searchPettern, recursive);
			}
			++i;
		}
	}

	auto i = file.getFilesIterator(searchPettern);
	while (i) {
		files.push_back(*i);
		++i;
	}
}
} // namespace


bool File::Special::_validate(Special value) {
	switch (value) {
		case adminTools             :
		case appData                :
		case CDBurnArea             :
		case commonAdminTools       :
		case commonAppData          :
		case commonDesktopDirectory :
		case commonDocuments        :
		case commonMusic            :
		case commonOemLinks         :
		case commonPictures         :
		case commonPrograms         :
		case commonStartMenu        :
		case commonStartup          :
		case commonTemplates        :
		case commonVideos           :
		case cookies                :
		case desktop                :
		case desktopDirectory       :
		case favorites              :
		case fonts                  :
		case history                :
		case internetCache          :
		case localAppData           :
		case localizedResources     :
		case myComputer             :
		case myDocuments            :
		case myMusic                :
		case myPictures             :
		case myVideos               :
		case netHood                :
//		case personal               :
		case printHood              :
		case programFiles           :
		case programFilesX86        :
		case programFilesCommon     :
		case programFilesCommonX86  :
		case programs               :
		case recent                 :
		case resources              :
		case sendTo                 :
		case startMenu              :
		case startup                :
		case system                 :
		case systemX86              :
		case templates              :
		case userProfile            :
		case windows                :
		case temporary              : return true;
		default                     : return false;
	}
}


bool File::SpecialOption::_validate(SpecialOption value) {
	switch (value) {
		case none        :
		case create      :
		case doNotVerify : return true;
		default          : return false;
	}
}



File::FilesIterator::FilesIterator(FilesIterator&& value)
	: current(value.current), nameIndex(value.nameIndex), handle(value.handle) {
	value.handle = nullptr;
}


File::FilesIterator::FilesIterator(const File& file, StringRange searchPettern) {
	if (searchPettern.empty()) {
		handle = nullptr;
		return;
	}

	current.path(file.fullPathFile(), searchPettern);
	{// 検索するディレクトリパスと検索パターンを分離する。
		const wchar_t* last = nullptr;
		const wchar_t* first = current._path;
		const wchar_t* i = first;
		while ((i = PathFindNextComponentW(i)) != nullptr) {
			if (*i == L'\0') {
				break;
			}
			last = i;
		}
		if (!last) {
			nameIndex = 0;
		} else if (last - first == 3 && first[1] == L':' && first[2] == L'\\') { // ルートディレクトリを示す場合は\\を末尾に入れたままにしないとパスと見なされなくなる
			nameIndex = last - first;
		} else {
			nameIndex = last - first - 1;
		}
	}

	WIN32_FIND_DATAW data;
	handle = FindFirstFileW(current._path, &data);
	current._path[nameIndex] = L'\0';
	if (handle == INVALID_HANDLE_VALUE) {
		handle = nullptr;
		const DWORD errorCode = GetLastError();
		if (errorCode != ERROR_FILE_NOT_FOUND
		 && errorCode != ERROR_PATH_NOT_FOUND) {
			checkError(errorCode);
		}
	} else if (String::equals(L".", data.cFileName) || String::equals(L"..", data.cFileName)) {
		++*this;
	} else {
		current.path(current._path, data.cFileName);
	}
}


File::FilesIterator::~FilesIterator() {
	if (handle) {
		verify(FindClose(handle));
	}
}


File::FilesIterator& File::FilesIterator::operator=(FilesIterator&& value) {
	if (this != &value) {
		current = value.current;
		nameIndex = value.nameIndex;
		swap(handle, value.handle);
	}
	return *this;
}


File& File::FilesIterator::operator*() {
	assert("Iterator out of range" && *this);
	return current;
}


File* File::FilesIterator::operator->() {
	assert("Iterator out of range" && *this);
	return &current;
}


File::FilesIterator& File::FilesIterator::operator++() {
	assert("Iterator out of range" && *this);
	WIN32_FIND_DATAW data;
	do {
		if (!FindNextFileW(handle, &data)) {
			const DWORD errorCode = GetLastError();
			if (errorCode != ERROR_NO_MORE_FILES) {
				checkError(errorCode);
			}
			verify(FindClose(handle));
			handle = nullptr;
			return *this;
		}
	} while (String::equals(L".", data.cFileName) || String::equals(L"..", data.cFileName));
	current._path[nameIndex] = L'\0';
	current.path(current._path, data.cFileName);
	return *this;
}


File::FilesIterator::operator bool() const {
	return handle != nullptr;
}



File::File() {
	_path[0] = L'\0';
	_pathLength = 0;
}


File::File(const File& value) : _pathLength(value._pathLength) {
	String::refer(value._path, value._pathLength).copyTo(_path);
}


File::File(StringRange path) {
	this->path(path);
}


File::File(StringRange directoryName, StringRange fileName) {
	path(directoryName, fileName);
}


File& File::operator=(const File& value) {
	if (this != &value) {
		String::refer(value._path, value._pathLength).copyTo(_path);
		_pathLength = value._pathLength;
	}
	return *this;
}


File::Attributes File::attributes() const {
	DWORD result = GetFileAttributesW(_path);
	if (result == INVALID_FILE_ATTRIBUTES) {
		checkError(GetLastError());
	}
	return static_cast<Attributes>(result);
}


void File::attributes(File::Attributes value) {
	if (!SetFileAttributesW(_path, value)) {
		checkError(GetLastError());
	}
}


void File::copyTo(StringRange destPath, bool overwrite) const {
	assert("Empty destPath" && !destPath.empty());
	if (isDirectory()) {
		File dest(destPath);
		if (!overwrite && dest.exists()) {
			throw AlreadyExistsException();
		}
		dest.createDirectory();
		for (auto i = getFilesIterator(); i; ++i) {
			i->copyTo(File(dest, i->name()), overwrite);
		}
	} else {
		if (!CopyFileW(_path, destPath.c_str(), overwrite ? FALSE : TRUE)) {
			checkError(GetLastError());
		}
	}
}


FileStream File::create() {
	return FileStream(_path, FileStream::Mode::createAlways, FileStream::Access::readWrite);
}


void File::createDirectory() {
	if (exists()) {
		if (!isDirectory()) {
			throw AlreadyExistsException(); // ディレクトリではなくファイルが既に存在する。
		}
		return;
	}
	if (!CreateDirectoryW(_path, nullptr)) {
		const DWORD errorCode = GetLastError();
		if (errorCode == ERROR_PATH_NOT_FOUND) {
			auto parentDir = parent();
			if (parentDir._pathLength) {
				parentDir.createDirectory(); // 親ディレクトリから再帰的に作成を試みる
				verify(CreateDirectoryW(_path, nullptr)); // 親が作成されたから再び作成を試みる
				return;
			}
			throw NotFoundException(); // 親ディレクトリ作成の再帰がルートディレクトリに到達した
		} else if (errorCode != ERROR_ALREADY_EXISTS) {
			checkError(errorCode);
		}
	}
}


File File::createTempFile() {
	wchar_t tempPathBuffer[MAX_PATH];
	tempPathBuffer[0] = 0;
	verify(GetTempPathW(MAX_PATH, tempPathBuffer));
	File file;
	verify(GetTempFileNameW(tempPathBuffer, L"tmp", 0, file._path));
	file._pathLength = String::getLength(file._path);
	return file;
}


File File::current() {
	File file;
	file._pathLength = GetCurrentDirectoryW(MAX_PATH, file._path);
	assert(file._pathLength);
	assert(file._pathLength < MAX_PATH);
	return file;
}


void File::current(StringRange path) {
	assert("Empty path" && !path.empty());

	if (!SetCurrentDirectoryW(path.c_str())) {
		auto errorCode = GetLastError();
		if (errorCode == ERROR_DIRECTORY) {
			throw AlreadyExistsException();
		}
		checkError(errorCode);
	}
}


bool File::empty() const {
	return _path[0] == L'\0';
}


bool File::exists() const {
	return PathFileExistsW(_path) != FALSE;
}


bool File::exists(StringRange path) {
	return PathFileExistsW(path.c_str()) != FALSE;
}


String File::extension() const {
	const wchar_t* result = PathFindExtensionW(_path);
	if (result) {
		return result;
	} else {
		return String();
	}
}


void File::extensionToBuffer(StringBuffer& buffer) const {
	const wchar_t* result = PathFindExtensionW(_path);
	if (result) {
		buffer += result;
	}
}


File File::fullPathFile() const {
	File file;
	wchar_t* fileName;
	file._pathLength = GetFullPathNameW(_path, maxPath, file._path, &fileName);
	if (MAX_PATH < file._pathLength) {
		throw PathTooLongException();
	}
	assert(L"Failed to GetFullPathNameW" && file._pathLength);
	return file;
}


vector<File> File::getFiles(StringRange searchPettern, bool recursive) const {
	if (!isDirectory()) {
		throw File::NotFoundException();
	}

	vector<File> files;
	getFilesToVector(files, *this, searchPettern, recursive);
	return files;
}


File::FilesIterator File::getFilesIterator(StringRange searchPettern) const {
	return FilesIterator(*this, searchPettern);
}


File File::getSpecial(File::Special special, File::SpecialOption option) {
	assert("Invalid Directory::Special" && File::Special::_validate(special));
	assert("Invalid Directory::SpecialOption" && File::SpecialOption::_validate(option));
	File file;
	if (special == Special::temporary) {
		file._pathLength = GetTempPathW(MAX_PATH, file._path);
		assert(file._pathLength);
	} else {
		struct ResultChecker {
			ResultChecker(HRESULT result) : result(result) {
				assert(SUCCEEDED(result) || result == E_FAIL || result == 0x80070002 || result == 0x80004005 || result == 0x80070057);
			}
			HRESULT result;
		};

		ResultChecker(SHGetFolderPathW(nullptr, special | option, nullptr, SHGFP_TYPE_CURRENT, file._path));
		file.resetPathLength();
	}
	return file;
}


Icon File::icon() const {
	if (!exists()) {
		return Icon();
	}
	wchar_t path[MAX_PATH];
	String::refer(*this).copyTo(path);
	WORD index = 0;
	Icon icon(ExtractAssociatedIconW(nullptr, path, &index), true);
	return icon;
}


bool File::isDirectory() const {
	return PathIsDirectoryW(_path) != 0;
}


void File::moveTo(StringRange destPath) {
	assert("Empty destPath" && !destPath.empty());

	if (!exists()) { // ”ファイル”として存在するかチェック
		throw NotFoundException();
	}
	if (!MoveFileW(_path, destPath.c_str())) {
		checkError(GetLastError());
	}
}


String File::name() const {
	StringBuffer buffer;
	nameToBuffer(buffer);
	return move(buffer);
}


void File::nameToBuffer(StringBuffer& buffer) const {
	File file = fullPathFile();
	if (!file.empty()) {
		wchar_t last = file._path[file._pathLength - 1];
		if (last == L'\\' || last == L':') {
			return;
		}
	}
	const wchar_t* result = PathFindFileNameW(file._path);
	if (result) {
		buffer += result;
	}
}


String File::nameWithoutExtension() const {
	StringBuffer buffer;
	nameWithoutExtensionToBuffer(buffer);
	return move(buffer);
}


void File::nameWithoutExtensionToBuffer(StringBuffer& buffer) const {
	wchar_t array[maxPath];
	StringBuffer name(array);
	nameToBuffer(name);
	verify(PathRenameExtensionW(name.begin(), L""));
	buffer += name.begin();
}


FileStream File::openAppend() {
	return FileStream(_path, FileStream::Mode::append, FileStream::Access::write);
}


FileStream File::openRead() const {
	return FileStream(_path, FileStream::Mode::open, FileStream::Access::read);
}


FileStream File::openWrite() {
	return FileStream(_path, FileStream::Mode::open, FileStream::Access::write);
}


void File::path(StringRange value) {
	if (maxPath <= value.length()) {
		throw PathTooLongException();
	}
	String::refer(value).copyTo(_path);
	_pathLength = value.length();
}


void File::path(StringRange directoryName, StringRange fileName) {
	if (*fileName.c_str() == L'\\') { // PathCombine 関数は Vista と XP で挙動が異なる。XP ではこの処理がなくともこのような結果を返すが、Vistaでは関数が失敗する。
		path(fileName);
	} else {
		if (!PathCombineW(_path, directoryName.c_str(), fileName.c_str())) {
			throw PathTooLongException(); // おそらくはMAX_PATHオーバー
		}
		resetPathLength();
	}
}


int File::pathLength() const {
	return _pathLength;
}


File File::parent() const {
	File file = fullPathFile();
	const wchar_t* last = nullptr;
	const wchar_t* first = file._path;
	const wchar_t* i = first;
	while ((i = PathFindNextComponentW(i)) != nullptr) {
		if (*i == L'\0') {
			break;
		}
		last = i;
	}
	if (!last) {
		file.path(L"");
	} else if (last - first == 3 && first[1] == L':' && first[2] == L'\\') { // ルートディレクトリを示す場合は\\を末尾に入れたままにしないとパスと見なされなくなる
		file._pathLength = last - first;
		file._path[file._pathLength] = L'\0';
	} else {
		file._pathLength = last - first - 1;
		file._path[file._pathLength] = L'\0';
	}
	return file;
}


void File::remove(bool recursive) {
	if (isDirectory()) {
		if (recursive) {
			for (auto i = getFilesIterator(); i; ++i) {
				i->remove(true);
			}
		}
		if (!RemoveDirectoryW(_path)) {
			checkError(GetLastError());
		}
	} else {
		if (!DeleteFileW(_path)) {
			const DWORD errorCode = GetLastError();
			if (errorCode != ERROR_FILE_NOT_FOUND) {
				checkError(errorCode);
			}
		}
	}
}


void File::replace(StringRange destFilePath, StringRange backupFilePath) {
	assert("Empty destFilePath" && !destFilePath.empty());

	if (!ReplaceFileW(destFilePath.c_str(), _path, backupFilePath.empty() ? nullptr : backupFilePath.c_str()
					, REPLACEFILE_WRITE_THROUGH, nullptr, nullptr)) {
		checkError(GetLastError());
	}
}


void File::resetPathLength() {
	_pathLength = String::getLength(_path);
}


File File::root() const {
	File file = fullPathFile();
	PathStripToRootW(file._path);
	file.resetPathLength();
	return file;
}



	}
}