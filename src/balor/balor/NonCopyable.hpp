#pragma once


namespace balor {



/**
 * このクラスを継承するクラスのコピーを禁止する。
 *
 * boost::noncopyable と同じ。balor ライブラリではコピーを禁止しているクラスでも右辺値参照を引数に取るコンストラクタ、代入演算子を実装するので
 * コピーを作ることなく関数の戻り値にすることができ、ＳＴＬコンテナに格納することができ、任意のタイミングで一時オブジェクトを作って代入して初期化ができる。
 *
 * こんな車輪の再発明は本当にしたくはないが、特定バージョンの boost のインストールを強制したくない。
 */
class NonCopyable {
protected:
	NonCopyable() {}
	~NonCopyable() {}
private:
	NonCopyable(const NonCopyable& );
	NonCopyable& operator=(const NonCopyable& );
};



}