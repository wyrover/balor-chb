#pragma once

#include <balor/Exception.hpp>


namespace balor {
	namespace system {



/**
 * COM の初期化と終了の自動化を提供する。
 * 
 * メインスレッドで initialize または oleInitialize 関数を呼んだ場は、プロセス終了時に自動的に uninitialize または oleUninitialize 関数を呼ぶ。
 * メインスレッドではないスレッドから呼ぶ場合は自動的な終了処理は行われないことに注意。
 * プロセス終了時に初期化処理と終了処理の呼び出し回数が等しくなければ assert で警告する。
 */
class Com {
public:
	/// COM が未初期化だった場合。
	class UninitializedException : public Exception {};

	/// COM インターフェースがレジストリに未登録だったり未実装だった場合。
	class InterfaceNotFoundException : public Exception {};

public:
	/// COM を初期化する。isSTA が true ならば STA、false ならば MTA で初期化する。
	static void initialize(bool isSTA = true);
	/// メインスレッドにおいて COM が初期済みか否かを返す。
	static bool initialized();
	/// メインスレッドかどうか。
	static bool isMainThread();
	/// OLE を初期化する。COM が未初期化であった場合は同時に COM を STA で初期化する。
	static void oleInitialize();
	/// メインスレッドにおいて OLE が初期済みかどうか。
	static bool oleInitialized();
	/// OLE の終了処理を行う。
	static void oleUninitialize();
	/// COM の終了処理を行う。
	static void uninitialize();
};



	}
}