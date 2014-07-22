#pragma once


namespace balor {
	namespace test {



/**
 * ユニットテストの準備や実行をする実装を提供する。
 *
 * testCase マクロを使ってテストケースの関数を登録し、関数の中では testAssert, testThrow 等のマクロを使ってテストコードを書く。
 * UnitTest::run() 関数でテストを実行し、結果は Debug::write() 関数で出力される。
 *
 * <h3>・注意！</h3>
 * run メンバ関数では assert マクロの失敗をテストする為にＣランタイムの abort 関数が呼ばれると balor::test::UnitTest::AssertionFailedException 例外を投げるようにしている。
 * abort 関数を使用するプログラムのテストではこれを考慮する必要がある。
 */
class UnitTest {
public:
	/// registerTestCaseを呼ぶためだけのクラス。testCaseマクロ内で使われる。
	class FunctionRegister {
	public:
		FunctionRegister(void (*function) (), const char* functionName, const char* fileName, int line);
	};

	/// ユニットテスト中に assert に失敗するとスローされる例外.
	struct AssertionFailedException {
	};

public:
	/// 現在のテストケースではハンドルリークのチェックが必要ないことを通知する。
	static void ignoreHandleLeakCheck();
	/// テストが実行中であるか否かを返す。
	static bool isRunning();
	/// テストケースの関数を登録する。testCaseマクロ内で使われる。
	static void registerTestCase(void (*function) (), const char* functionName, const char* fileName, int line);
	/// テスト結果をレポートする。testAssert、testThrow、tstAssertマクロ内で使われる。
	static void report(bool result, const char* expression, int line);
	/// 登録されたテストケースを全て実行する。
	static void run();
	/// 登録されたテストケースのうち、ファイル名がpetternで示される正規表現に一致するものだけを実行する。
	static void run(const char* pettern);
};


// ユニットテストコードで使用するマクロ関数群。


/// テストケース関数を登録しつつ、定義する。
#define testCase(functionName) \
void functionName();\
::balor::test::UnitTest::FunctionRegister functionName##Register(functionName, #functionName, __FILE__, __LINE__);\
void functionName()


/// expression が true かどうかをテストする。
#define testAssert(expression) ::balor::test::UnitTest::report((expression) ? true : false, #expression, __LINE__);


/// expression が assert に失敗するかどうかをテストする。NDEBUGマクロが定義されている場合は実行すらしないことに注意する。
#if defined(NDEBUG)
#define testAssertionFailed(expression) ::balor::test::UnitTest::report(true, #expression, __LINE__);
#else
#define testAssertionFailed(expression) try { expression; ::balor::test::UnitTest::report(false, #expression, __LINE__); } catch (::balor::test::UnitTest::AssertionFailedException& ) { ::balor::test::UnitTest::report(true, #expression, __LINE__); }
#endif


/// expression が例外を throw しないかどうかをテストする。
#define testNoThrow(expression) try { expression; ::balor::test::UnitTest::report(true, #expression, __LINE__); } catch (...) { ::balor::test::UnitTest::report(false, #expression, __LINE__); }


/// expression が exception を throw するかどうかをテストする。
#define testThrow(expression, exception) try { expression; ::balor::test::UnitTest::report(false, #expression, __LINE__); } catch (exception& ) { ::balor::test::UnitTest::report(true, #expression, __LINE__); }



	}
}