#pragma once

#include <balor/io/FileStream.hpp>
#include <balor/Enum.hpp>
#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
	namespace graphics {
		class Icon;
	}
}


namespace balor {
	namespace io {



/**
 * ファイルまたはディレクトリを表す。パスを保持し、パスに対する操作もサポートする。
 */
class File {
public:
	typedef ::balor::graphics::Icon Icon;

	/// ファイル属性。組み合わせで指定する。
	struct Attributes {
		enum _enum {
			none              = 0         , 
			readOnly          = 0x00000001, /// 読み取り専用。
			hidden            = 0x00000002, /// 隠しファイルまたはディレクトリ。
			system            = 0x00000004, /// システムファイルまたはディレクトリ。
			directory         = 0x00000010, /// ディレクトリである。
			archive           = 0x00000020, /// アーカイブファイルまたはディレクトリ。
			normal            = 0x00000080, /// 特に属性が無い。
			temporary         = 0x00000100, /// 一時ファイル。
			sparseFile        = 0x00000200, /// スパースファイル。
			reparsePoint      = 0x00000400, /// 再解析ポイントが関連付けられている。
			compressed        = 0x00000800, /// 圧縮されている。
			offline           = 0x00001000, /// ファイルの存在するリモート記憶域がオフライン。
			notContentIndexed = 0x00002000, /// インデックスサービスの対象になっていない。
			encrypted         = 0x00004000, /// 暗号化されている。
			virtualFile       = 0x00010000, /// UAC によってファイルが仮想化されている。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Attributes);
	};

	/// システムの特別なディレクトリの種類。
	struct Special {
		enum _enum {
			adminTools             = 0x0030, /// 管理ツール。
			appData                = 0x001a, /// ローミングありのアプリケーションデータ。
			CDBurnArea             = 0x003b, /// CD への書き込みを待機しているファイルの領域。
			commonAdminTools       = 0x002f, /// 全ユーザの管理ツール。
			commonAppData          = 0x0023, /// 全ユーザのアプリケーションデータ。
			commonDesktopDirectory = 0x0019, /// 全ユーザのデスクトップ。
			commonDocuments        = 0x002e, /// 全ユーザのドキュメント。
			commonMusic            = 0x0035, /// 全ユーザのミュージック。
			commonOemLinks         = 0x003a, /// common OEM link。
			commonPictures         = 0x0036, /// 全ユーザのピクチャ。
			commonPrograms         = 0X0017, /// 全ユーザのスタートメニューの全てのプログラム。
			commonStartMenu        = 0x0016, /// 全ユーザのスタートメニュー。
			commonStartup          = 0x0018, /// 全ユーザのスタートアップ。
			commonTemplates        = 0x002d, /// 全ユーザのドキュメントテンプレート。
			commonVideos           = 0x0037, /// 全ユーザのビデオ。
			cookies                = 0x0021, /// Cookie。
			desktop                = 0x0000, /// デスクトップ。
			desktopDirectory       = 0x0010, /// デスクトップ。
			favorites              = 0x0006, /// お気に入り。
			fonts                  = 0x0014, /// フォント。
			history                = 0x0022, /// 履歴。
			internetCache          = 0x0020, /// IEキャッシュ。
			localAppData           = 0x001c, /// アプリケーションデータ。
			localizedResources     = 0x0039, /// ローカライズされたリソースデータ。
			myComputer             = 0x0011, /// マイコンピュータ。
			myDocuments            = 0x0005, /// マイドキュメント。
			myMusic                = 0x000d, /// マイミュージック。
			myPictures             = 0x0027, /// マイピクチャ。
			myVideos               = 0x000e, /// マイビデオ。
			netHood                = 0x0013, /// NetHood。
			personal               = 0x0005, /// マイドキュメント。
			printHood              = 0x001b, /// PrintHood。
			programFiles           = 0x0026, /// Program Files。
			programFilesX86        = 0x002a, /// Program Files。（６４ビット）
			programFilesCommon     = 0x002b, /// Program Files/common。
			programFilesCommonX86  = 0x002c, /// Program Files/common。（６４ビット）
			programs               = 0x0002, /// スタートメニューの全てのプログラム。
			recent                 = 0x0008, /// 最近使ったファイル。
			resources              = 0x0038, /// リソースデータ。
			sendTo                 = 0x0009, /// 送る。
			startMenu              = 0x000b, /// スタートメニュー。
			startup                = 0x0007, /// スタートアップ。
			system                 = 0x0025, /// ＯＳのシステムディレクトリ。
			systemX86              = 0x0029, /// ＯＳのシステムディレクトリ。
			templates              = 0x0015, /// ドキュメントテンプレート。
			userProfile            = 0x0028, /// プロファイル。
			windows                = 0x0024, /// ＯＳのディレクトリ。
			temporary              = 0xffff, /// テンポラリファイルの置き場。(オリジナル）
		};
		BALOR_NAMED_ENUM_MEMBERS(Special);
	};

	/// システムの特殊なディレクトリの取得オプション。
	struct SpecialOption {
		enum _enum {
			none        = 0     , /// 存在しない場合は空文字列を返す。
			create      = 0x8000, /// 存在しない場合は作成する。
			doNotVerify = 0x4000, /// 存在するかどうかを確認せずに返す。
		};
		BALOR_NAMED_ENUM_MEMBERS(SpecialOption);
	};


	/// ファイルを列挙するイテレータ。再帰検索はサポートしない。再帰検索をするにはスタックなどの処理が必要になるのでそれならば getFiles で配列に入れたほうが早いだろう。
	struct FilesIterator;


	/// アクセス権限がなかった。あるいは異なるボリュームに移動しようとした。
	class AccessDeniedException : public Exception {};

	/// 移動先等にファイルやディレクトリが既に存在していた。
	class AlreadyExistsException : public Exception {};

	/// ディレクトリのパスが不正だった。
	class InvalidPathException : public Exception {};

	/// ディレクトリが空ではなくて削除できなかった。
	class NotEmptyException : public Exception {};

	/// ディレクトリが見つからなかった。
	class NotFoundException : public Exception {};

	/// パスが長すぎた。
	class PathTooLongException : public Exception {};

	/// 他スレッドとの Share アクセス競合があった。あるいは移動先が自分のサブディレクトリだった。
	class SharingViolationException : public Exception {};

	/// パス文字列の最大長。
	static const int maxPath = 260;

public:
	/// 空文字列のパスから作成。
	File();
	File(const File& value);
	/// ファイルパスから作成。
	File(StringRange path);
	/// ディレクトリ名とファイル名から作成。
	File(StringRange direcotryName, StringRange fileName);
	File& operator=(const File& value);

public:
	/// ファイル属性。
	File::Attributes attributes() const;
	void attributes(File::Attributes value);
	/// ファイルをコピーする。
	void copyTo(StringRange destPath, bool overwrite = false) const;
	/// ファイルを作成し、ファイルストリームを返す。
	FileStream create();
	/// ディレクトリを作成する。
	void createDirectory();
	/// テンポラリファイルを作成する。
	static File createTempFile();
	/// カレントディレクトリ
	static File current();
	static void current(StringRange path);
	/// ファイルパスが空文字列であるかどうか。
	bool empty() const;
	/// 存在するかどうか。
	bool exists() const;
	static bool exists(StringRange path);
	/// 拡張子。無い場合は空文字列が返る。
	String extension() const;
	void extensionToBuffer(StringBuffer& buffer) const;
	/// フルファイルパスのファイル。
	File fullPathFile() const;
	/// 現在のディレクトリの下のファイル一覧を返す。ワイルドカード指定可能。
	/// ワイルドカードはファイル名または最後のディレクトリ名にのみ使用できる。
	std::vector<File, std::allocator<File> > getFiles(StringRange searchPettern = L"?*", bool recursive = false) const;
	File::FilesIterator getFilesIterator(StringRange searchPettern = L"?*") const;
	/// システムの特殊なディレクトリを返す。引数によっては空のパスや存在しないパスが返る事がある。
	static File getSpecial(File::Special special, File::SpecialOption option = SpecialOption::create);
	/// エクスプローラ上でこのファイルが表示される時のアイコン。
	Icon icon() const;
	/// ディレクトリかどうか。存在しない場合は false を返す。
	bool isDirectory() const;
	/// ファイルまたはディレクトリを移動する。ディレクトリのボリュームをまたいだ移動はできない。
	void moveTo(StringRange destPath);
	/// ファイルまたはディレクトリ名。
	String name() const;
	void nameToBuffer(StringBuffer& buffer) const;
	/// 拡張子無しのファイル名。
	String nameWithoutExtension() const;
	void nameWithoutExtensionToBuffer(StringBuffer& buffer) const;
	/// ファイルを追加書き込みモードでオープンする。
	FileStream openAppend();
	/// ファイルを読み取りモードでオープンする。
	FileStream openRead() const;
	/// ファイルを書き込みモードでオープンする。
	FileStream openWrite();
	/// ファイルパス。
	const wchar_t* path() const { return _path; }
	void path(StringRange value);
	void path(StringRange directoryName, StringRange fileName);
	/// ファイルパスの長さ。
	int pathLength() const;
	/// 親ディレクトリ。無い場合は空のパスを持った File が返る。
	File parent() const;
	/// ファイルを削除する。
	void remove(bool recursive = false);
	/// バックアップを作成しない場合は destinationBackupFileName に長さ０の文字列を渡す
	void replace(StringRange destFilePath, StringRange backupFilePath);
	/// ファイルパスの長さを設定し直す。
	void resetPathLength();
	/// ルートディレクトリ。
	File root() const;

public:
	/// パスを表す StringRange への変換。
	operator StringRange() const { return StringRange(_path, _pathLength); }

private:
	wchar_t _path[maxPath];
	int _pathLength;
};



/// ファイルを列挙するイテレータ。再帰検索はサポートしない。再帰検索をするにはスタックなどの処理が必要になるのでそれならば getFiles で配列に入れたほうが早いだろう。
struct File::FilesIterator : private NonCopyable {
	FilesIterator(FilesIterator&& value);
	FilesIterator(const File& file, StringRange searchPettern = L"?*");
	~FilesIterator();
	File::FilesIterator& operator=(FilesIterator&& value);

public:
	/// 列挙したファイルへのアクセス。
	File& operator*();
	File* operator->();
	/// 次のファイルへ移動。
	File::FilesIterator& operator++();
	/// 列挙中かどうか。
	operator bool() const;

private:
	File current;
	int nameIndex;
	void* handle;
};



	}
}