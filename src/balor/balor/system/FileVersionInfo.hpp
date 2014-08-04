#pragma once

#include <balor/system/Version.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

namespace balor {
	namespace io {
		class File;
		class Registry;
	}
}


namespace balor {
	namespace system {



/**
 * ファイルのバージョン情報。
 * 
 * 現在のロケールから言語コードを取得して一致するバージョン情報を探す。
 * みつからない場合はシステムデフォルトロケールかニュートラルに指定されたバージョン情報を探し、
 * それでも見つからなかった場合は最初のバージョン情報を読み取る。
 * バージョン情報が無かった場合は operator SafeBool で false を返す。
 * ファイル自体が見つからなかった場合は ::balor::io::File::NotFoundException を投げる。
 */
class FileVersionInfo : private NonCopyable {
public:
	typedef ::balor::io::File File;
	typedef ::balor::io::Registry Registry;

public:
	/// 指定したファイルのバージョン情報を作成する。
	FileVersionInfo(StringRange filePath);
	FileVersionInfo(FileVersionInfo&& value);
	~FileVersionInfo();

	FileVersionInfo& operator=(FileVersionInfo&& value);

public: // それぞれファイルバージョンリソースの名前に対応している
	/// このバージョン情報を持つアプリケーションのユーザデータのディレクトリ。
	File appDataDirectory(bool useProductVersion = true) const;
	/// このバージョン情報を持つアプリケーションのユーザデータのレジストリ。
	Registry appDataRegistry(bool useProductVersion = true) const;
	String comments() const;
	/// このバージョン情報を持つアプリケーションの共通データのディレクトリ。
	File commonAppDataDirectory(bool useProductVersion = true) const;
	/// このバージョン情報を持つアプリケーションの共通データのレジストリ。
	Registry commonAppDataRegistry(bool useProductVersion = true) const;
	String companyName() const;
	String fileDescription() const;
	Version fileVersion() const;
	String fileVersionText() const;
	String internalName() const;
	bool isDebug() const;
	bool isPatched() const;
	bool isPreRelease() const;
	bool isSpecialBuild() const;
	/// ファイルバージョンの言語コード（そのまま Locale コンストラクタ引数に使える）
	int languageCode() const;
	String legalCopyright() const;
	String legalTrademarks() const;
	/// このバージョン情報を持つアプリケーションのユーザデータ（ローミングあり）のディレクトリ。
	File localAppDataDirectory(bool useProductVersion = true) const;
	String originalFileName() const;
	String privateBuild() const;
	String productName() const;
	Version productVersion() const;
	String productVersionText() const;
	String specialBuild() const;

public:
	/// ファイルバージョン情報が見つかったかどうか。
	operator bool() const;

private:
	unsigned char* _buffer;
	int _languageCode;
	wchar_t _queryHeader[32];
};



	}
}