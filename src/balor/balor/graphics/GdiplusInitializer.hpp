#pragma once


namespace balor {
	namespace graphics {



/**
 * GDI+ ライブラリの初期化と終了を自動化する。
 * 
 * balor のクラスが内部で使用しており、主に画像の読み込みや保存時に初期化される。
 * ユーザプログラム側でこれ以上は GDI+ は使わない場合に uninitialize() を呼ぶことが出来る。
 * uninitialize() を呼ばなかった場合でもプロセス終了時に自動的に呼ばれる。
 */
class GdiplusInitializer {
public:
	/// GDI+の初期化を行う.
	/// 既に initialize 関数が呼ばれていたら何もしない。
	static void initialize();
	/// 初期済みか否かを返す
	static bool initialized();
	/// GDI+の終了処理を行う.
	/// initialize 関数が呼ばれていないか既に uninitialize 関数が呼ばれていたら何もしない。
	static void uninitialize();
};



	}
}