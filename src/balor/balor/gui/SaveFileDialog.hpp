#pragma once

#include <balor/gui/FileDialog.hpp>


namespace balor {
	namespace gui {



/**
* ファイル保存ダイアログボックス。
*
* OFN_CREATEPROMPT は動作せず。なぜか GetOpenFileName では動作する。.NET では自力でメッセージボックスを表示している。
* OFN_PATHMUSTEXIST は設定してもしなくても存在しないパスで警告が出る。その上ファイル作成警告もでる。
*
* <h3>・サンプルコード</h3>
* <pre><code>
	Frame frame(L"OpenFileDialog Sample");

	Button open(frame, 20, 10, 0, 0, L"ファイルを保存する");
	Edit filePath(frame, 20, 50, 0, 0, 150);
	filePath.readOnly(true);

	SaveFileDialog dialog;
	dialog.filter(L"全てのファイル\n*.*\nテキストファイル\n*.txt;*.log\n\n");
	dialog.overwritePrompt(true);
	dialog.defaultExtention(L"txt");

	open.onClick() = [&] (Button::Click& ) {
		if (dialog.show(frame)) {
			filePath.text(dialog.filePath());
		}
	};

	frame.runMessageLoop();
* </code></pre>
*/
class SaveFileDialog : public FileDialog {
public:
	SaveFileDialog();
	SaveFileDialog(SaveFileDialog&& value, bool checkSlicing = true);
	virtual ~SaveFileDialog();
	SaveFileDialog& operator=(SaveFileDialog&& value);

public:
	/// 存在するファイル名またはファイルパスを入力した場合に上書きを警告するかどうか。初期値は true
	bool overwritePrompt() const;
	void overwritePrompt(bool value);
	/// ダイアログボックスを表示する。ファイルを選択したかどうかを返す
	bool show(HWND owner);

protected:
};



	}
}