#pragma once

#include <hash_map>

#include <balor/locale/Locale.hpp>
#include <balor/OutOfMemoryException.hpp>
#include <balor/String.hpp>

struct HINSTANCE__;

namespace balor {
	namespace graphics {
		class Bitmap;
		class Cursor;
		class Icon;
	}
	class Size;
}


namespace balor {
	namespace io {

class MemoryStream;



/**
 * リソースを表す。
 *
 * ロケールを指定した場合、リソース名の末尾にピリオド＋ロケールの ISO 名（Locale::name() 関数の結果）をつけてリソースを検索する。
 * 見つからなかった場合は末尾になにもつけずに再度検索する。ただしリソース ID を使用する場合はロケールについて何もしない。
 * 文字列リソースについては特別で、リソース ID を使用する場合は従来どおり STRINGTABLE リソースを使うがリソース名を使用する場合は
 * namedStringTable という名前の RCDATA リソースを探す。この名前にもロケール名の修飾が付く。
 * RCDATA リソースの内容として文字列名と文字列の二つのカラムを持つユニコードの CSV テキストファイルを用意する必要がある。
 */
class Resource {
public:
	typedef ::HINSTANCE__* HMODULE;
	typedef ::balor::graphics::Bitmap Bitmap;
	typedef ::balor::graphics::Cursor Cursor;
	typedef ::balor::graphics::Icon Icon;
	typedef ::balor::locale::Locale Locale;

	/// リソースが見つからなかった
	class NotFoundException : public Exception {};

	/// メモリが足りなかった
	struct OutOfMemoryException : public ::balor::OutOfMemoryException {};

public:
	/// モジュールから作成。
	explicit Resource(HMODULE module = nullptr);
	/// ロケールとモジュールから作成。
	explicit Resource(const Locale& locale, HMODULE module = nullptr);

public:
	/// BITMAP リソースまたは RCDATA リソースから Bitmap を作成して返す。Bitmap クラスの対応している画像形式を読み込める。
	Bitmap getBitmap(int id);
	Bitmap getBitmap(StringRange name);
	/// CURSOR リソースから Cursor を作成して返す。
	Cursor getCursor(int id);
	Cursor getCursor(StringRange name);
	/// ICON リソースから Icon を作成して返す。size に Size(0, 0) を指定すると最初に見つかったアイコンの原寸で作成される。
	Icon getIcon(int id);
	Icon getIcon(int id, const Size& size);
	Icon getIcon(int id, int width, int height);
	Icon getIcon(StringRange name);
	Icon getIcon(StringRange name, const Size& size);
	Icon getIcon(StringRange name, int width, int height);
	/// RCDATA リソースをメモリストリームとして取得する。メモリはリソースの入ったモジュールがアンロードされるまで有効。
	MemoryStream getRawData(int id);
	MemoryStream getRawData(StringRange name);
	/// 文字列リソースを取得する。名前を指定する文字列リソースの作り方についてはクラスドキュメントを参照。
	String getString(int id);
	String getString(StringRange name);
	void getStringToBuffer(StringBuffer& buffer, int id);
	void getStringToBuffer(StringBuffer& buffer, StringRange name);
	/// リソースのロケール。
	Locale locale() const;

private:
	HMODULE _module;
	Locale _locale;
	wchar_t _localeName[32];
	std::hash_map<String, String> _namedStringTable;
};



	}
}