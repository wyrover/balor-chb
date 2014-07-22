#include "FileVersionInfo.hpp"

#include <utility>

#include <balor/io/File.hpp>
#include <balor/io/Registry.hpp>
#include <balor/locale/Locale.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Convert.hpp>
#include <balor/Flag.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>

#pragma comment(lib, "version.lib")


namespace balor {
	namespace system {


using std::move;
using std::swap;
using namespace balor::io;
using namespace balor::locale;


namespace {
String getVersionString(const unsigned char* buffer, const wchar_t header[32], StringRange name) {
	wchar_t array[64];
	StringBuffer query(array);
	query += header;
	query += String::refer(name);
	UINT valueSize = 0;
	wchar_t* valuePointer = nullptr;
	if (VerQueryValueW(buffer, query.begin(), reinterpret_cast<LPVOID*>(&valuePointer), &valueSize)) {
		return String(valuePointer, valueSize - 1);
	} else {
		return String();
	}
}


const VS_FIXEDFILEINFO* getVersionInfo(const unsigned char* _buffer) {
	VS_FIXEDFILEINFO* info = nullptr;
	UINT size;
	verify(VerQueryValueW(_buffer, L"\\", reinterpret_cast<LPVOID*>(&info), &size)); 
	return info;
}


void makeAppPath(const FileVersionInfo& info, File& file, File::Special special, bool useProductVersion) {
	StringBuffer _buffer(const_cast<wchar_t*>(file.path()), File::maxPath);
	_buffer += String::refer(File::getSpecial(special));
	const String companyName = info.companyName();
	if (0 < companyName.length()) {
		_buffer += L"\\";
		_buffer += companyName;
	}
	const String productName = info.productName();
	if (0 < productName.length()) {
		_buffer += L"\\";
		_buffer += productName;
	}
	if (useProductVersion) {
		const String productVersion = info.productVersionText();
		if (0 < productVersion.length()) {
			_buffer += L"\\";
			_buffer += productVersion;
		}
	}
	file.resetPathLength();
	file.createDirectory();
}


void makeAppRegistry(Registry& key, const FileVersionInfo& info, bool useProductVersion) {
	const String companyName = info.companyName();
	if (0 < companyName.length()) {
		key = key.createKey(companyName, true);
	}
	const String productName = info.productName();
	if (0 < productName.length()) {
		key = key.createKey(productName, true);
	}
	if (useProductVersion) {
		const String productVersion = info.productVersionText();
		if (0 < productVersion.length()) {
			key = key.createKey(productVersion, true);
		}
	}
}
} // namespace



FileVersionInfo::FileVersionInfo(StringRange filePath) : _buffer(nullptr), _languageCode(0) {
	assert("Empty filePath" && !filePath.empty());

	{// バージョン情報全体の取得
		DWORD size = 0;
		size = GetFileVersionInfoSizeW(filePath.c_str(), &size);
		if (size == 0) {
			const DWORD errorCode = GetLastError();
			switch (errorCode) {
				case ERROR_FILE_NOT_FOUND          : throw File::NotFoundException();
				case ERROR_RESOURCE_TYPE_NOT_FOUND :
				case ERROR_RESOURCE_DATA_NOT_FOUND : {
					if (File::exists(filePath)) { // OSによってはERROR_FILE_NOT_FOUNDを返さない
						return; // バージョンリソースが無い
					} else {
						throw File::NotFoundException();
					}
				}
				default                            : assert(false); return; // それほど深刻な事態ではあるまい
			}
		}
		_buffer = new unsigned char[size];
		verify(GetFileVersionInfoW(filePath.c_str(), 0, size, _buffer));
	}

	{// 言語、コードページの決定
		struct LangAndCodePage {
			WORD language;
			WORD codePage;
		} langAndCodePage = {0, 0};
		LangAndCodePage* langAndCodePages = nullptr;
		UINT langAndCodePageSize = 0;
		verify(VerQueryValueW(&_buffer[0], L"\\VarFileInfo\\Translation", reinterpret_cast<LPVOID*>(&langAndCodePages), &langAndCodePageSize));
		assert(0 < langAndCodePageSize);
		Locale currentLocale = Locale::current();
		const int userLanguage = currentLocale.languageCode();
		assert(userLanguage);
		bool find = false;
		for (UINT i = 0; i < langAndCodePageSize / sizeof(LangAndCodePage); ++i) {
			if (langAndCodePages[i].language == userLanguage) { // 現在のロケール言語と一致する言語を検索
				langAndCodePage = langAndCodePages[i];
				find = true;
				break;
			}
		}
		if (!find) {
			for (UINT i = 0; i < langAndCodePageSize / sizeof(LangAndCodePage); ++i) {
				if (langAndCodePages[i].language == LANG_SYSTEM_DEFAULT // みつからないならデフォルトっぽいもので我慢する
				 || langAndCodePages[i].language == LANG_USER_DEFAULT
				 || langAndCodePages[i].language == LOCALE_NEUTRAL
				 || langAndCodePages[i].language == LOCALE_INVARIANT) {
					langAndCodePage = langAndCodePages[i];
					find = true;
					break;
				}
			}
		}
		if (!find) { // それでも見つからなければ最初の言語でいいや
			langAndCodePage = langAndCodePages[0];
		}

		_languageCode = langAndCodePage.language;
		StringBuffer _buffer(_queryHeader);
		_buffer += L"\\StringFileInfo\\";
		Convert::to<StringBuffer>(_buffer, langAndCodePage.language, L"04", 16);
		Convert::to<StringBuffer>(_buffer, langAndCodePage.codePage, L"04", 16);
		_buffer += L"\\";
		assert(_buffer.length() == 25);
	}
}


FileVersionInfo::FileVersionInfo(FileVersionInfo&& value)
	: _buffer(value._buffer), _languageCode(value._languageCode) {
	String::refer(value._queryHeader).copyTo(_queryHeader);
	value._buffer = nullptr;
}


FileVersionInfo::~FileVersionInfo() {
	if (_buffer) {
		delete [] _buffer;
	}
}


FileVersionInfo& FileVersionInfo::operator=(FileVersionInfo&& value) {
	if (&value != this) {
		swap(_buffer, value._buffer);
		swap(_languageCode, value._languageCode);
		String::refer(value._queryHeader).copyTo(_queryHeader);
	}
	return *this;
}


FileVersionInfo::operator bool() const {
	return _buffer != nullptr;
}


File FileVersionInfo::appDataDirectory(bool useProductVersion) const {
	assert("Not found FileVersionInfo" && *this);
	File file;
	makeAppPath(*this, file, File::Special::appData, useProductVersion);
	return file;
}


Registry FileVersionInfo::appDataRegistry(bool useProductVersion) const {
	assert("Not found FileVersionInfo" && *this);
	Registry registry(L"HKEY_CURRENT_USER\\Software");
	makeAppRegistry(registry, *this, useProductVersion);
	return registry;
}


String FileVersionInfo::comments() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"Comments");
}


File FileVersionInfo::commonAppDataDirectory(bool useProductVersion) const {
	assert("Not found FileVersionInfo" && *this);
	File file;
	makeAppPath(*this, file, File::Special::commonAppData, useProductVersion);
	return file;
}


Registry FileVersionInfo::commonAppDataRegistry(bool useProductVersion) const {
	assert("Not found FileVersionInfo" && *this);
	Registry registry(L"HKEY_LOCAL_MACHINE\\Software");
	makeAppRegistry(registry, *this, useProductVersion);
	return registry;
}


String FileVersionInfo::companyName() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"CompanyName");
}


String FileVersionInfo::fileDescription() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"FileDescription");
}


Version FileVersionInfo::fileVersion() const {
	assert("Not found FileVersionInfo" && *this);
	auto info = getVersionInfo(_buffer);
	return Version(HIWORD(info->dwFileVersionMS)
				 , LOWORD(info->dwFileVersionMS)
				 , HIWORD(info->dwFileVersionLS)
				 , LOWORD(info->dwFileVersionLS));
}


String FileVersionInfo::fileVersionText() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"FileVersion");
}


String FileVersionInfo::internalName() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"InternalName");
}


bool FileVersionInfo::isDebug() const {
	assert("Not found FileVersionInfo" && *this);
	return toFlag(getVersionInfo(_buffer)->dwFileFlags)[VS_FF_DEBUG];
}


bool FileVersionInfo::isPatched() const {
	assert("Not found FileVersionInfo" && *this);
	return toFlag(getVersionInfo(_buffer)->dwFileFlags)[VS_FF_PATCHED];
}


bool FileVersionInfo::isPreRelease() const {
	assert("Not found FileVersionInfo" && *this);
	return toFlag(getVersionInfo(_buffer)->dwFileFlags)[VS_FF_PRERELEASE];
}


bool FileVersionInfo::isSpecialBuild() const {
	assert("Not found FileVersionInfo" && *this);
	return toFlag(getVersionInfo(_buffer)->dwFileFlags)[VS_FF_SPECIALBUILD];
}


int FileVersionInfo::languageCode() const {
	assert("Not found FileVersionInfo" && *this);
	return _languageCode;
}


String FileVersionInfo::legalCopyright() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"LegalCopyright");
}


String FileVersionInfo::legalTrademarks() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"LegalTrademarks");
}


File FileVersionInfo::localAppDataDirectory(bool useProductVersion) const {
	assert("Not found FileVersionInfo" && *this);
	File file;
	makeAppPath(*this, file, File::Special::localAppData, useProductVersion);
	return file;
}


String FileVersionInfo::originalFileName() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"OriginalFileName");
}


String FileVersionInfo::privateBuild() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"PrivateBuild");
}


String FileVersionInfo::productName() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"ProductName");
}


Version FileVersionInfo::productVersion() const {
	assert("Not found FileVersionInfo" && *this);
	auto info = getVersionInfo(_buffer);
	return Version(HIWORD(info->dwProductVersionMS)
				 , LOWORD(info->dwProductVersionMS)
				 , HIWORD(info->dwProductVersionLS)
				 , LOWORD(info->dwProductVersionLS));
}


String FileVersionInfo::productVersionText() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"ProductVersion");
}


String FileVersionInfo::specialBuild() const {
	assert("Not found FileVersionInfo" && *this);
	return getVersionString(_buffer, _queryHeader, L"SpecialBuild");
}



	}
}