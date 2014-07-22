#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/Exception.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>

struct HKEY__;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}


namespace balor {
	namespace io {



/**
 * レジストリにアクセスするクラス。
 */
class Registry : private NonCopyable {
public:
	typedef ::HKEY__* HKEY;

	/// レジストリの値の種類
	struct ValueKind {
		enum _enum {
			string       = 1 , /// 文字列値。
			expandString = 2 , /// %PATH% のような環境変数文字列。
			binary       = 3 , /// バイナリデータ。
			dword        = 4 , /// ３２ビット数値。
			multiString  = 7 , /// ヌル文字で区切られた複数の文字列値。二連続のヌル文字が終端を表す。
			qword        = 11, /// ６４ビット数値。
			unknown      = -1, /// サポートしない値の種類。
			notFound     = -2, /// 値が見つからなかった。
		};
		BALOR_NAMED_ENUM_MEMBERS(ValueKind);
	};

	/// キー名や値名を列挙するイテレータ。
	struct NamesIterator {
		NamesIterator(HKEY handle, bool isKeyName);

		/// 現在位置の名前を列挙しなおす。列挙しながらキーや値を削除する場合は ++ ではなくこっちを使う。
		void refresh();
		/// 列挙した名前。
		const wchar_t* operator*() const;
		/// 次の名前に移動。
		Registry::NamesIterator& operator++();
		/// 列挙し終わったかどうか。
		operator bool() const;

	private:
		HKEY handle;
		bool isKeyName;
		unsigned long count;
		wchar_t name[256];
	};

	/// キーや値にアクセス権が無かった。あるいはサブキーをもつレジストリを削除しようとした。
	class AccessDeniedException : public Exception {};

	/// HKEY_LOCAL_MACHINE 直下にキーを作成しようとした場合等。
	class InvalidParameterException : public Exception {};

	/// 操作しようとしたキーが既に削除されていた。
	class KeyDeletedException : public Exception {};

	/// 値が見つからなかった。
	class ValueNotFoundException : public Exception {};

	/// 値の種類と変数の型があってない。
	class ValueKindMismatchException : public Exception {};

public:
	/// 未初期化状態。
	Registry();
	Registry(Registry&& value);
	/// HKEY_～で始まるレジストリキー名から作成。
	Registry(StringRange path, bool writable = false);
	~Registry();
	Registry& operator=(Registry&& value);

public:
	/// HKEY_CLASSES_ROOT で作成。
	static Registry classesRoot();
	/// サブキーを作成して返す。
	Registry createKey(StringRange keyName, bool writable = false);
	/// HKEY_CURRENT_CONFIG で作成。
	static Registry currentConfig();
	/// HKEY_CURRENT_USER で作成。
	static Registry currentUser();
	/// キャッシュされている現在のキーの変更をディスクに反映させる。
	void flush();
	/// あらゆる種類の値をバイナリ形式で返す。
	std::vector<unsigned char, std::allocator<unsigned char> > getBinary(StringRange valueName) const;
	/// DWORD 型の値を返す。
	unsigned long getDword(StringRange valueName) const;
	/// QWORD 型の値を返す。
	unsigned __int64 getQword(StringRange valueName) const;
	/// String 型で受け取れる値を返す。
	String getString(StringRange valueName) const;
	void getStringToBuffer(StringBuffer& buffer, StringRange valueName) const;
	/// 値の種類を返す。値が見つからなければ Registry::ValueKind::notFound を返す。
	Registry::ValueKind getValueKind(StringRange valueName) const;
	/// HKEY_LOCAL_MACHINE で作成。
	static Registry localMachine();
	/// サブキーの数。
	int keyCount() const;
	/// サブキー名を列挙するイテレータ。
	Registry::NamesIterator keyNamesIterator() const;
	/// サブキーを開く。存在しなかった場合は空の Registry を返す。
	Registry openKey(StringRange subKeyName, bool writable = false) const;
	/// サブキーを削除する。
	void removeKey(StringRange keyName, bool recursive = false);
	/// 値を削除する。
	void removeValue(StringRange valueName);
	/// 任意の型の値を設定する。
	void setBinary(StringRange valueName, ArrayRange<const unsigned char> value, Registry::ValueKind kind = ValueKind::binary);
	/// Registry::ValueKind::dword の値を設定する。
	void setDword(StringRange valueName, unsigned long value);
	/// Registry::ValueKind::qword の値を設定する。
	void setQword(StringRange valueName, unsigned __int64 value);
	/// String 型 の値を設定する。
	void setString(StringRange valueName, StringRange value, Registry::ValueKind kind = ValueKind::string);
	/// HKEY_USERS で作成。
	static Registry users();
	/// 値の数。
	int valueCount() const;
	/// 値名を列挙するイテレータ。
	Registry::NamesIterator valueNamesIterator() const;

public:
	/// HKEY への自動変換 ＆ null チェック用。
	operator HKEY() const { return _handle; }

private:
	HKEY _handle;
};



	}
}