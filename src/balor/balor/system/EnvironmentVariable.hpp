#pragma once

#include <balor/StringRange.hpp>

namespace balor {
	namespace io {
		class Registry;
	}
}


namespace balor {
	namespace system {



/**
 * 環境変数に関する操作。
 */
class EnvironmentVariable {
public:
	typedef ::balor::io::Registry Registry;

	/// 現在のプロセスの全ての環境変数を列挙するイテレータ。列挙中に環境変数を削除したりすると予期せぬ動作を起こす可能性がある。
	/// 名前や値へのアクセスはスレッドセーフではない。
	struct VariablesIterator {
		VariablesIterator();

		/// 環境変数名。
		String name() const;
		void nameToBuffer(StringBuffer& buffer) const;
		/// 環境変数の値。
		String value() const;
		void valueToBuffer(StringBuffer& buffer) const;
		/// 次の環境変数へ。
		VariablesIterator& operator++();
		/// 列挙中かどうか。
		operator bool() const;

	private:
		int _current;
	};

public:
	/// 文字列に含まれる % で囲まれた環境変数名を展開して返す。
	static String expandStrings(StringRange string);
	static void expandStringsToBuffer(StringBuffer& buffer, StringRange string);
	/// 現在のプロセスの環境変数が存在するかどうか。
	static bool find(StringRange valueName);
	/// 現在のプロセスの環境変数を返す。見つからなかった場合は空文字列を返す。
	static String get(StringRange valueName);
	static void getToBuffer(StringBuffer& buffer, StringRange valueName);
	/// 現在のプロセスの環境変数を削除する。
	static void remove(StringRange valueName);
	/// 現在のプロセスの環境変数を変更する。
	static void set(StringRange valueName, StringRange value);
	/// システムの環境変数が設定されているレジストリを返す。
	static Registry systemRegistry();
	/// 環境変数のレジストリの変更を全てのプロセスに反映させる。WM_SETTINGCHANGE メッセージを処理しないプロセスには効果がない。
	/// TODO:コマンドプロンプトに反映されないようだ。自作のアプリでも反映されない。どうなっているのか・・
	static void updateAllProcess();
	/// ログイン中のユーザの環境変数が設定されているレジストリを返す。
	static Registry userRegistry();
	/// 現在のプロセスの全ての環境変数を列挙するイテレータ。
	static EnvironmentVariable::VariablesIterator variablesBegin();
};



	}
}