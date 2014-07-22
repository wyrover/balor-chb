#pragma once

#include <balor/StringRange.hpp>


namespace balor {
	namespace system {



/**
 * コンソールウインドウの表示や文字の出力。
 * 
 * コンソールはプロセスにつき一つ持つ事ができる。WIN32 アプリケーションの為のもので、コンソールアプリケーションでは使えない。
 */
class Console {
public:
	/// 開いていたコンソールを閉じる。
	static void close();
	/// コンソールを開く。
	static void open();
	/// 文字列を書き込む。
	static void write(StringRange text);
	static void write(ByteStringRange text);
	/// 文字列を書き込んで改行する。
	static void writeLine(StringRange text);
	static void writeLine(ByteStringRange text);
};



	}
}