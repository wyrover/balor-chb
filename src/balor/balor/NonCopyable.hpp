#pragma once


namespace balor {



/**
 * このクラスを継承するクラスのコピ?を禁?する。
 *
 * boost::noncopyable と同じ。balor ライブラリではコピ?を禁?しているクラスでも右辺値参照を引数に取るコンストラク?、ｴ・ﾔ演算子を実装するので
 * コピ?を作ることなく関数の戻り値にすることができ、ＳＴＬコンテナに格?することができ、任意の?イ?ングで一時オブジェクトを作ってｴ・ﾔして初期化ができる。
 *
 * こんな車輪の再発明は?当にしたくはないが、特定バ?ジョンの boost のインスト?ルを強制したくない。
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