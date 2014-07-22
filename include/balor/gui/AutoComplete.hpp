#pragma once

#include <balor/system/ComPtr.hpp>
#include <balor/Enum.hpp>
#include <balor/Reference.hpp>
#include <balor/StringRangeArray.hpp>

struct IAutoComplete2;

namespace balor {
class String;
}


namespace balor {
	namespace gui {

class Edit;



/**
 * エディットコントロールに特定の文字を入力すると文字列の続きの候補を列挙したり補間できるようにする。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"AutoComplete Sample");

	Edit edit(frame, 20, 10, 0, 0, 10);
	const wchar_t* items[] = {
		L"012345",
		L"012abc",
		L"012あいう",
		L"abc",
		L"abcdef",
		L"あいう",
	};
	AutoComplete complete(edit, items);

	frame.runMessageLoop();
 * </code></pre>
 */
class AutoComplete : private NonCopyable {
public:
	/// オートコンプリートの動作モード。
	struct Mode {
		enum _enum {
			append           = 0x40000000, /// 完成された文字列がハイライトされて追加表示される。
			suggest          = 0x10000000, /// 完成された文字列がドロップダウンリストで表示される。
			appendAndSuggest = 0x50000000, /// append と suggest 両方。
		};
		BALOR_NAMED_ENUM_MEMBERS(Mode);
	};

	/// システムが用意したオートコンプリートする文字列リスト。組み合わせで指定する。
	struct SystemItems {
		enum _enum {
			none                = 0         ,
			fileSystem          = 0x00000001, /// ファイルシステムのパス。
			fileSystemDirectory = 0x00000020, /// ファイルシステムのディレクトリパス。
			urlHistory          = 0x00000002, /// ＵＲＬ履歴。
			recentlyUsedUrl     = 0x00000004, /// 最近使ったＵＲＬリスト。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(SystemItems);
	};

public:
	AutoComplete();
	AutoComplete(AutoComplete&& value);
	/// エディットコントロールとオートコンプリートする文字列の配列から作成。
	AutoComplete(Edit& target, StringRangeArray items, AutoComplete::Mode mode = Mode::appendAndSuggest);
	/// エディットコントロールとオートコンプリートする文字列の種類から作成。
	AutoComplete(Edit& target, AutoComplete::SystemItems systemItems, AutoComplete::Mode mode = Mode::appendAndSuggest);
	~AutoComplete();
	AutoComplete& operator=(AutoComplete&& value);

public:
	/// 有効かどうか。
	bool enabled() const;
	void enabled(bool value);
	/// オートコンプリートする文字列の配列。
	std::vector<String, ::std::allocator<String> >& items() const;
	/// オートコンプリートのモード。
	AutoComplete::Mode mode() const;
	/// システムが用意するオートコンプリートする文字列の配列。
	AutoComplete::SystemItems systemItems() const;

private:
	class EnumString;

	Reference<Edit> _target;
	bool _enabled;
	Mode _mode;
	SystemItems _systemItems;
	::balor::system::ComPtr<::IAutoComplete2> _autoComplete;
	::balor::system::ComPtr<EnumString> _enumString;
};



	}
}