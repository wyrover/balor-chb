#pragma once


namespace balor {
	namespace test {



/// Debug::enableCrashDumpHandler 関数で設定された例外ハンドラがＣ＋＋例外で起動された場合に投げられる例外。
/// プログラムの最後にこの例外をキャッチすることでデストラクタの起動を保証する
class UnhandledException {
};



	}
}