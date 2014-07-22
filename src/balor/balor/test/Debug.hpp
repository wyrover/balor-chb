#pragma once

#include <balor/Listener.hpp>
#include <balor/StringRange.hpp>

struct _EXCEPTION_POINTERS;

namespace balor {
class String;
}


namespace balor {
	namespace test {



/**
 * デバッグ出力、メモリーリーク検出、クラッシュダンプ出力、スタックトレースといったデバッグに使う関数をサポートする。
 */
class Debug {
public:
	typedef ::_EXCEPTION_POINTERS EXCEPTION_POINTERS;

public:
	/// ミニダンプファイルを作成する。
	/// ファイル名やメッセージを指定しなかった場合は enableCrashDumpHandler 関数で設定された値が使用される。
	static bool createDumpFile(EXCEPTION_POINTERS* exceptions = nullptr, StringRange filePath = L"", bool showMessageBox = false, StringRange message = L"", StringRange failedMessage = L"");

	/// デバッガ上で実行している場合は実行を中断してデバッガに制御を移す。デバッガ上ではない場合は構造化例外を発生させる。
	static void debugBreak();

	/// catch されない C++ 例外あるいは構造化例外が発生した時に、強制終了する前にミニダンプファイルを吐き出すように設定する。
	/// この設定はスレッド、DLLを超えて有効。ただし既に起動されている他スレッドには効果が無い。
	/// デバッガで実行している場合はこの機能は有効にならないが、例外発生時にはそのままデバッガで見るべきである。
	/// ダンプファイル名やメッセージを指定しなかった場合は実行ファイルと同じディレクトリ上に作成するように適当に生成される。
	/// この時生成されるファイル名には実行ファイルのファイルバージョンが含まれる。<br><br>
	/// 
	/// C++ 例外が発生した場合はダンプを吐いた後 ::balor::test::UnhandledException を投げる。
	/// この例外を catch する事でクラッシュダンプを吐きつつデストラクタを呼ぶ事ができる。catch しない場合デストラクタは呼ばれずにその場で終了する。
	/// 構造化例外が発生した場合、ダンプを吐いた後アプリケーションはその場で終了する。
	/// 構造化例外が発生した場合でもダンプを吐きつつデストラクタを呼ぶには、まずコンパイラのオプションを下記のように変更する。<br>
	/// C/C++＞コード生成＞C++の例外を有効にする = はい - SEH の例外あり(/EHa)
	/// <br>次に _set_se_translator 関数を使って構造化例外を C++ 例外に変換する。
	/// この関数の中で、引数に渡ってきた EXCEPTION_POINTERS を使って Debug::createDumpFile 関数でダンプを出力する。
	/// 変換して投げた例外を catch すればデストラクタが呼ばれる。以下はそのコード例。
	/// <pre><code>
	///#include <windows.h>
	///#include <eh.h>
	///#include <balor/test/all.hpp>
	///
	///void se_translator(unsigned int /*code*/, struct _EXCEPTION_POINTERS* ep)
	///{
	///	balor::test::Debug::createDumpFile(ep);
	///	throw balor::test::UnhandledException(); // 標準C++の例外を発生させる
	///}
	///
	///int APIENTRY wWinMain(HINSTANCE ,HINSTANCE ,LPWSTR, int ) {
	///	balor::test::Debug::enableCrashDumpHandler();
	///	_set_se_translator(&se_translator);
	///
	///	try {
	///		runApplication(); // ユーザの処理
	///	} catch (balor::test::UnhandledException& ) { // これでデストラクタを呼ぶ
	///	}
	///	return 0;
	///}
	////</code></pre>
	static void enableCrashDumpHandler(StringRange crashDumpFilePath = L"", bool showMessageBox = true, StringRange message = L"", StringRange failedMessage = L"");

	/// デバッグビルド時のみ、メモリリークのチェックを有効にする。
	/// 実行終了時にデバッグ出力にリークしたメモリを報告する。この設定はスレッド、DLLを超えて有効。<br><br>
	/// ・メモリリークしたソースコード位置の特定<br>
	/// まずデバッグウインドウのメモリリークレポートを確認し、例えば下記のように報告されたとすると<br>
	/// {1873} normal block at 0x00B64868, 4 bytes long.
	/// <br>{1873} というのがメモリ割り当てに割り振られた番号になるで、デバッガを起動してプログラムの最初で止めてウォッチウインドウに下記の名前を追加する。<br>
	/// {,,msvcr100d.dll}_crtBreakAlloc
	/// <br>※ msvcr100d.dll という DLL 名は Visual Studio 2010 用のものなので、使用している Visual Studio にあった DLL 名に変更が必要。<br>
	/// ただし、プロジェクトの設定（構成プロパティ＞C/C++＞コード生成＞ランタイムライブラリ）を「マルチスレッドデバッグ(/MTd)」に変更しているなら<br>
	/// _crtBreakAlloc
	/// <br>という名前にする。そして値に 1873 と入力して実行すると、レポートされたメモリを割り当てた時点でブレイク（実行中断）する。
	static void enableMemoryLeakCheck();

	/// 現在のプロセスがデバッガ上で動いているかどうか。
	static bool isDebuggerPresent();

	/// デバッグ出力イベント。設定しない場合は単にデバッグ出力を行う。この関数はスレッドセーフではないので注意。
	static Listener<ByteStringRange>& onWrite();

	/// スタックトレースを返す。
	/// 開発環境以外で使用する場合は exe ファイルと共に pdf ファイルも配布する必要がある。
	/// ソースコード情報を取り除いた pdf ファイルの配布が一般的。（リンカー＞デバッグ＞プライベートシンボルの削除）
	/// この関数を使う場合、Dbghelp.dll は VS2010EE に付属のものでは他 OS 環境でうまく動作しないので注意。最新版をダウンロードしてアプリケーションといっしょに配布する必要がある。
	/// 準備が面倒なので開発環境以外ではクラッシュダンプを使うほうが良い。
	static String stackTrace();

	/// デバッグ出力をする。
	static void write(ByteStringRange message);
	static void write(StringRange message);
	/// 改行を追加してデバッグ出力をする。
	static void writeLine(ByteStringRange message);
	static void writeLine(StringRange message);
};



	}
}