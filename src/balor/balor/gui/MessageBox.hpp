#pragma once

#include <balor/Enum.hpp>
#include <balor/StringRange.hpp>

struct HWND__;

namespace balor {
	namespace graphics {
		class Font;
	}
}


namespace balor {
	namespace gui {



/**
* メッセージボックスの表示を行う。
*
* windows.h に MessageBox というマクロが定義されているのでクラス名を MsgBox としているが、
* typedef で MessageBox という名前も使えるようにしてあるので windows.h と一緒に使わない場合はMessageBox のほうの名前を使用できる。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"MessageBox Sample");

Button warning(frame, 20, 10, 0, 0, L"警告", [&] (Button::Click& ) {
MsgBox::show(frame, L"警告！");
});
Button close(frame, 20, 50, 0, 0, L"閉じる", [&] (Button::Click& ) {
if (MsgBox::show(frame, L"ウインドウを閉じますか？", L"終了", MsgBox::Buttons::yesNo) == MsgBox::Result::yes) {
frame.close();
}
});

frame.runMessageLoop();
* </code></pre>
*/
class MsgBox {
public:
	typedef ::HWND__* HWND;
	typedef ::balor::graphics::Font Font;


	/// 表示するボタンの組み合わせ。
	struct Buttons {
		enum _enum {
			ok = 0x00000000L, /// OK。
			okCancel = 0x00000001L, /// OK、キャンセル。
			abortRetryIgnore = 0x00000002L, /// 中止、再試行、無視。
			yesNoCancel = 0x00000003L, /// はい、いいえ、キャンセル。
			yesNo = 0x00000004L, /// はい、いいえ。
			retryCancel = 0x00000005L, /// 再試行、キャンセル。
			cancelTryContinue = 0x00000006L, /// キャンセル、再実行、続行。
		};
		BALOR_NAMED_ENUM_MEMBERS(Buttons);
	};


	/// 表示するアイコン。
	struct Icon {
		enum _enum {
			none = 0, /// アイコン非表示。
			hand = 0x00000010L, /// 停止アイコン。
			question = 0x00000020L, /// 疑問符アイコン。
			exclamation = 0x00000030L, /// 感嘆符アイコン。
			asterisk = 0x00000040L, ///「i]マークアイコン。
			stop = 0x00000010L, /// 停止アイコン。
			error = 0x00000010L, /// 停止アイコン。
			warning = 0x00000030L, /// 感嘆符アイコン。
			information = 0x00000040L, ///「i]マークアイコン。
		};
		BALOR_NAMED_ENUM_MEMBERS(Icon);
	};


	/// 既定のボタン。
	struct Default {
		enum _enum {
			button1 = 0x00000000L, /// 一番目のボタン。
			button2 = 0x00000100L, /// 二番目のボタン。
			button3 = 0x00000200L, /// 三番目のボタン。
		};
		BALOR_NAMED_ENUM_MEMBERS(Default);
	};


	/// 表示オプション。組み合わせで指定する。
	struct Options {
		enum _enum {
			none = 0,
			topMost = 0x00040000L, /// 最前面ウインドウにする。
			serviceNotification = 0x00200000L, /// サービスから表示する。
			defaultDesktopOnly = 0x00020000L, /// サービスから表示する場合に既定のデスクトップでのみ表示する。
			rightAlign = 0x00080000L, /// テキストを右寄せする。
		};
		BALOR_NAMED_LOGICAL_ENUM_MEMBERS(Options);
	};


	/// ボタンの選択結果。
	struct Result {
		enum _enum {
			ok = 1, /// OK。
			cancel = 2, /// キャンセル。
			abort = 3, /// 中止。
			retry = 4, /// 再試行。
			ignore = 5, /// 無視。
			yes = 6, /// はい。
			no = 7, /// いいえ。
			tryAgain = 10, /// 再実行。
			continues = 11, /// 続行。
		};
		BALOR_NAMED_ENUM_MEMBERS(Result);
	};

public:
	/// メッセージボックスが使用するフォント。
	static Font font();
	/// 同じスレッドの全てのウインドウを使用不可にしてメッセージボックスを表示する。
	static MsgBox::Result show(StringRange text, StringRange caption = L"", MsgBox::Buttons buttons = Buttons::ok, MsgBox::Icon icon = Icon::none, MsgBox::Default defaultButton = Default::button1, MsgBox::Options options = Options::none);
	/// 指定したウインドウのみ使用不可にしてメッセージボックスを表示する。
	static MsgBox::Result show(HWND owner, StringRange text, StringRange caption = L"", MsgBox::Buttons buttons = Buttons::ok, MsgBox::Icon icon = Icon::none, MsgBox::Default defaultButton = Default::button1, MsgBox::Options options = Options::none);
};


typedef MsgBox MessageBox; // MessageBoxマクロ対策



	}
}