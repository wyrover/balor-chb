#pragma once

#include <balor/Enum.hpp>
#include <balor/Exception.hpp>
#include <balor/StringRange.hpp>


namespace balor {
	namespace io {



/**
 * システムのドライブを表す。
 */
class Drive {
public:
	/// ドライブの種類。
	struct Type {
		enum _enum {
			unknown         = 0, /// 判別不能。
			noRootDirectory = 1, /// ルートディレクトリが存在しなかった。
			removable       = 2, /// リムーバブルドライブ。
			fixed           = 3, /// 取り出せない HDD 等のドライブ。
			remote          = 4, /// リモートドライブ。
			cdRom           = 5, /// CD-ROM ドライブ。
			ramDisk         = 6, /// RAMディスク。
		};
		BALOR_NAMED_ENUM_MEMBERS(Type);
	};

	/// 論理ドライブを列挙するイテレータ。
	struct DrivesIterator {
		DrivesIterator(bool begin);

		Drive operator*();
		Drive::DrivesIterator& operator++();
		Drive::DrivesIterator& operator--();
		bool operator<(const Drive::DrivesIterator& rhs);
	private:
		unsigned long mask;
		int current;
	};

	/// アクセス権限がなかった。
	class AccessDeniedException : public Exception {};

	/// ドライブが見つからなかった。
	class NotFoundException : public Exception {};

	/// ドライブが準備できていなかった。
	class NotReadyException : public Exception {};

public:
	/// ドライブ名から作成。
	explicit Drive(wchar_t letter);
	explicit Drive(StringRange name);

public:
	/// 現在のユーザが使用できる空きバイト数。
	__int64 availableFreeBytes() const;
	/// 論理ドライブリストのイテレータ。
	static Drive::DrivesIterator drivesBegin();
	static Drive::DrivesIterator drivesEnd();
	/// ファイルシステム名
	String fileSystem() const;
	void fileSystemToBuffer(StringBuffer& buffer) const;
	/// ドライブ文字。
	wchar_t letter() const;
	/// ドライブ名。
	const wchar_t* name() const;
	/// 準備ができているかどうか。
	bool ready() const;
	/// 空きバイト数。
	__int64 totalFreeBytes() const;
	/// ドライブの総バイト数。
	__int64 totalBytes() const;
	/// ドライブのタイプ。
	Drive::Type type() const;
	/// ボリュームラベル。
	String volumeLabel() const;
	void volumeLabel(StringRange value);
	void volumeLabelToBuffer(StringBuffer& buffer) const;

private:
	wchar_t _name[4];
};



	}
}