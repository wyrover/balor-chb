#pragma once

#include <balor/OutOfMemoryException.hpp>
#include <balor/StringRange.hpp>
#include <balor/StringRangeArray.hpp>

struct HBITMAP__;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
	namespace graphics {
		class Bitmap;
	}
	namespace io {
		class MemoryStream;
		class Stream;
	}
	class String;
}


namespace balor {
	namespace gui {



/**
* クリップボード。
*
* ウインドウのメッセージループを実行していないと動作しない。
* DIB ビットマップの貼り付けはまともにサポートしているソフトは少ない上、DIB から DDB の自動変換はうまくいかない場合もあるようなので
* 別のソフトに転送を考えている場合は DDB ビットマップのみ貼り付けるほうが無難なようだ。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"Clipboard Sample");

Edit edit(frame, 20, 100, 0, 0, 50, 10, Edit::Options::multiline);
edit.hScrollBar(true);
Button button0(frame, 20, 10, 0, 0, L"クリップボードの文字を貼り付け", [&] (Button::Click& ) {
edit.text(Clipboard::getText());
});
Button button1(frame, 20, 50, 0, 0, L"クリップボードにエディットの文字を貼り付け", [&] (Button::Click& ) {
Clipboard::setText(edit.text());
});

frame.runMessageLoop();
* </code></pre>
*/
class Clipboard {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::balor::graphics::Bitmap Bitmap;
	typedef ::balor::io::MemoryStream MemoryStream;
	typedef ::balor::io::Stream Stream;

	///// クリップボードにアクセスできなかった。
	//class AccessDeniedException : public Exception {};

	/// メモリが足りなかった。
	struct OutOfMemoryException : public ::balor::OutOfMemoryException {};


public:
	/// 貼り付けられた全てを削除する。
	static void clear();
	/// ビットマップが貼り付けられているかどうか。
	static bool containsBitmap();
	/// DIB ビットマップが貼り付けられているかどうか。ただし DDB も DIB に変換可能なので区別して判断はできない。
	static bool containsDIB();
	/// ファイルドロップリストが貼り付けられているかどうか。
	static bool containsFileDropList();
	/// ユーザ定義のメモリデータが貼り付けられているかどうか。
	static bool containsMemory(int memoryFormat);
	/// 文字列が貼り付けられているかどうか。
	static bool containsText();
	/// DDB ビットマップを取得する。無い場合はヌルハンドルのビットマップを返す。
	static Bitmap getBitmap();
	/// DIB ビットマップを取得する。無い場合はヌルハンドルのビットマップを返す。
	static Bitmap getDIB();
	/// ファイルドロップリストを取得する。無い場合は空の配列を返す。
	static std::vector<String, std::allocator<String> > getFileDropList();
	/// ユーザ定義のメモリデータを取得する。無い場合は空のメモリストリームを返す。
	static MemoryStream getMemory(int memoryFormat);
	/// 文字列を取得する。無い場合は空文字列を返す。
	static String getText();
	/// ユーザ定義のメモリフォーマット名を登録し、メモリフォーマットを返す。メモリフォーマット名が他のプロセスで既に登録されていたら同じメモリフォーマットを返す。大文字と小文字は区別しない。
	static int registerMemoryFormat(StringRange memoryFormatName);
	/// DDB ビットマップとして貼り付ける。
	static void setBitmap(HBITMAP value);
	/// DIB ビットマップとして貼り付ける。
	static void setDIB(HBITMAP value);
	/// ファイルドロップリストを貼り付ける。
	static void setFileDropList(StringRangeArray value);
	/// ユーザ定義のメモリデータを設定する。
	static void setMemory(int memoryFormat, Stream& stream);
	/// 文字列を貼り付ける。
	static void setText(StringRange value);


public: // 内部使用関数
	static void* _dibToHgrobal(HBITMAP bitmap);
	static void* _fileDropListToHglobal(StringRangeArray fileDropList);
	static std::vector<String, std::allocator<String> > _hdropToFileDropList(void* drop);
	static Bitmap _hgrobalToDIB(void* global);
};



	}
}