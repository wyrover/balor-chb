#pragma once

#include <balor/io/Stream.hpp>
#include <balor/Enum.hpp>
#include <balor/Exception.hpp>
#include <balor/StringRange.hpp>


namespace balor {
	namespace io {



/**
 * Win32 API のファイルアクセス機能をサポートするストリーム。
 */
class FileStream : public Stream {
public:
	typedef void* HANDLE;

	/// ファイルのオープンモード。
	struct Mode {
		enum _enum {
			create       = 1, /// ファイルの新規作成。既に存在していた場合は AlreadyExistsException を投げる。
			createAlways = 2, /// ファイルの新規作成。既に存在していた場合は消してから作成。
			open         = 3, /// ファイルを開く。存在しない場合は NotFoundException を投げる。
			openAlways   = 4, /// ファイルを開く。存在しない場合は新規作成する。
			truncate     = 5, /// ファイルを開き、サイズを０にする。存在しない場合は NotFoundException を投げる。
			append       = 6, /// openAlways で開いてファイル位置を終端に移動する。
		};
		BALOR_NAMED_ENUM_MEMBERS(Mode);
	};

	/// ファイルへのアクセス権。
	struct Access {
		enum _enum {
			read      = 0x80000000L, /// 読み取りアクセス。
			write     = 0x40000000L, /// 書き込みアクセス。
			readWrite = 0xc0000000L, /// 読み書きアクセス。
		};
		BALOR_NAMED_ENUM_MEMBERS(Access);
	};

	/// ファイルへの共有方式。組み合わせで指定する。
	struct Share {
		enum _enum {
			none      = 0x00000000, /// 他スレッド、プロセスの一切のアクセスを禁じる。
			read      = 0x00000001, /// 読み取りの共有。
			write     = 0x00000002, /// 書き込みの共有。
			remove    = 0x00000004, /// 削除の共有。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Share);
	};

	/// ファイルオープンのオプション。組み合わせで指定する。
	struct Options {
		enum _enum {
			none           = 0         , 
			writeThrough   = 0x80000000, /// キャッシュせずに直接ディスクに書き込む。
			randomAccess   = 0x10000000, /// ランダムアクセスすることをシステムに指示する。
			removeOnClose  = 0x04000000, /// ファイルを閉じた時に削除する。
			sequentialScan = 0x08000000, /// シーケンシャルにアクセスすることをシステムに指示する。
			encrypted      = 0x00004000, /// ファイルを暗号化する。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};

	/// アクセス権がなかった。
	class AccessDeniedException : public Exception {};

	/// ファイルが既に存在していた。
	class AlreadyExistsException : public Exception {};

	/// ロックされていて読み書きできなかった。
	class LockViolationException : public Exception {};

	/// ファイルが見つからなかった。
	class NotFoundException : public Exception {};

	/// 共有方式に反する競合があった。
	class SharingViolationException : public Exception {};

	// オーバーロード関数のオーバーライド用
	using Stream::read;
	using Stream::write;

public:
	/// ファイルの自動ロック解除用オブジェクト。
	/// MSDNによればファイルのロック解除は position と length をロック処理と厳密一致させて行う必要があり、
	/// ロック解除を行わずにファイルハンドルを閉じたりプロセスを終了したりすると未定義の動作とあるのでデストラクタで確実に処理したい。
	class Lock {
		friend FileStream;

		Lock(HANDLE handle, __int64 position, __int64 length);

	public:
		Lock(Lock&& lock);
		~Lock();

	private:
		Lock(const Lock& );
		Lock& operator=(const Lock& );

		HANDLE handle;
		__int64 position;
		__int64 length;
	};

public:
	/// ヌルハンドルで作成する。
	FileStream();
	/// 指定したパラメータでファイルをオープンする。
	FileStream(StringRange path, FileStream::Mode mode, FileStream::Access access = Access::readWrite, FileStream::Share share = Share::read, FileStream::Options options = Options::none);
	FileStream(FileStream&& value);
	/// ファイルをクローズする。
	virtual ~FileStream();

	FileStream& operator=(FileStream&& value);

public:
	virtual void flush();
	virtual __int64 length() const;
	/// 指定した領域の他のハンドルの読み書きを禁止する。戻り値の Lock オブジェクトが存在している間だけ有効。
	Lock lock(__int64 position, __int64 length);
	virtual __int64 position() const;
	virtual void position(__int64 value);
	virtual int read(void* buffer, int offset, int count);
	virtual bool readable() const;
	virtual __int64 skip(__int64 offset);
	/// ファイルの終端に移動する。
	virtual __int64 skipToEnd();
	virtual void write(const void* buffer, int offset, int count);
	virtual bool writable() const;

public:
	/// HANDLE への自動変換 ＆ null チェック用
	operator HANDLE() const { return _handle; }

private:
	HANDLE _handle;
	Access _access;
};



	}
}