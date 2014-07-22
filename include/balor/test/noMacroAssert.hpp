#pragma once


namespace balor {
	namespace test {



/**
 * マクロを使わない assert。主にヘッダのみで実装されるテンプレートクラスで使用する。
 *
 * balor では独自の assert マクロは使っていないがプロジェクトによっては改造 assert マクロを使うかもしれないのでその対策。
 * noMacroAssert 関数内では単にassertを行うが、表示されるソースコード位置は固定されてしまうのでユニットテストかデバッガでの使用が前提。
 * リリースコンパイル時にも残ってしまうが空の関数なので最適化で削除されることを期待する。
 */
void noMacroAssert(bool expression);



	}
}