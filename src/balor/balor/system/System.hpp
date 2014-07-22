#pragma once

#include <balor/Enum.hpp>
#include <balor/StringRange.hpp>


namespace balor {
	namespace system {

class Version;



/**
 * システム全体についての関数を持つ。（未分類とも言える）
 */
class System {
public:
	/// システムの起動モード
	struct BootMode {
		enum _enum {
			normal              = 0, /// 通常モード。
			failSafe            = 1, /// セーフモード。
			failSafeWithNetwork = 2, /// ネットワーク接続するセーフモード。
		};
		BALOR_NAMED_ENUM_MEMBERS(BootMode);
	};

	/// ＯＳの種類。
	struct OsKind {
		enum _enum {
			unknown,
			windows95,
			windows98,
			windowsME,
			windowsNT4,
			windows2000,
			windowsXP,
			windowsHomeServer,
			windowsServer2003,
			windowsServer2003R2,
			windowsVista,
			windowsServer2008,
			windowsServer2008R2,
			windows7,
		};
		BALOR_NAMED_ENUM_MEMBERS(OsKind);
	};


public:
	/// システムの起動モード。
	static System::BootMode bootMode();
	/// コマンドライン引数の数。
	static int commandLineArgCount();
	/// コンピュータ名。（NetBIOS名）
	static String computerName();
	/// ＯＳがマルチバイト文字をサポートするかどうか。
	static bool dbcsEnabled();
	/// デバッグ版ＯＳが有効かどうか。
	static bool debugOS();
	/// ウインドウ内容を表示したままドラッグできるかどうか。
	static bool dragFullWindows();
	/// コマンドライン引数。
	static String getCommandLineArg(int index);
	/// 64ビットＯＳかどうか。
	static bool is64BitOs();
	/// 現在のプロセスが64ビットプロセスかどうか。
	static bool is64BitProcess();
	/// ネットワーク接続が有効かどうか。
	static bool network();
	/// ＯＳの種類。
	static System::OsKind osKind();
	/// サービスパック情報。
	static String osServicePack();
	/// ＯＳのバージョン。
	static Version osVersion();
	/// プロセッサの数。
	static int processorCount();
	/// システムを中断または休止状態にする。引数と戻り値は ::SetSuspendState 関数と同じ。
	static bool suspend(bool hibernate, bool force, bool disableWakeEvent);
	/// ユーザ名。Vistaにおいてインストーラ内で使用すると権限昇格の関係で不具合があるらしい。
	static String userName();
	/// ビジュアルスタイルを使用できるかどうか。＝ Comctl32.dll のバージョンが６以上であるかどうか。
	/// balor/gui/EnableVisualStyle.hpp を参照のこと。
	static bool visualStyleEnabled();
};



	}
}