#pragma once

#include <balor/gui/FileDialog.hpp>


namespace balor {
	namespace gui {



/**
 * ファイルを開くダイアログボックス。
 *
 * <h3>・サンプルコード</h3>
 * <pre><code>
	Frame frame(L"OpenFileDialog Sample");

	Button open(frame, 20, 10, 0, 0, L"ファイルを開く");
	Edit filePath(frame, 20, 50, 0, 0, 150);
	filePath.readOnly(true);
	const wchar_t* encodes[] = {
		L"acsii",
		L"ShiftJis",
		L"euc-jp"
	};
	ComboBox encode(frame, 20, 90, 0, 0, encodes);
	encode.selectedIndex(0);

	OpenFileDialog dialog;
	dialog.filter(L"全てのファイル\n*.*\nテキストファイル\n*.txt;*.log\n\n");
	dialog.readOnlyCheckBox(false);
	// ファイルを開くダイアログの UI を拡張する。
	Panel panel;
	Label label;
	ComboBox combo;
	dialog.onInit() = [&] (OpenFileDialog::Init& e) {
		auto clientSize = e.clientSize();
		panel = Panel::fromParentHandle(e.sender(), 0, clientSize.height, 0, 0);
		label = Label(panel, e.getControlBounds(OpenFileDialog::ControlId::filePathLabel).x, 0, 0, 0, L"エンコード");
		combo = ComboBox(panel, e.getControlBounds(OpenFileDialog::ControlId::filePathEdit).x, 0, 0, 0, encodes);
		combo.selectedIndex(encode.selectedIndex());
		combo.onSelect() = [&] (ComboBox::Select& e) {
			encode.selectedIndex(combo.selectedIndex());
		};
		panel.resize();
		e.clientSize(clientSize.width, clientSize.height + panel.size().height * 2);
	};

	open.onClick() = [&] (Button::Click& ) {
		if (dialog.show(frame)) {
			filePath.text(dialog.filePath());
		}
	};

	frame.runMessageLoop();
 * </code></pre>
 */
class OpenFileDialog : public FileDialog {
public:
	OpenFileDialog();
	OpenFileDialog(OpenFileDialog&& value, bool checkSlicing = true);
	virtual ~OpenFileDialog();
	OpenFileDialog& operator=(OpenFileDialog&& value);

public:
	/// 存在しないファイル名またはファイルパスを入力して開くボタンを押した場合に警告するかどうか。初期値は true。
	bool fileMustExist() const;
	void fileMustExist(bool value);
	/// 存在しないファイルパスを入力して開くボタンを押した場合に警告するかどうか。ファイル名は存在しなくても許可する。初期値は true。
	bool pathMustExist() const;
	void pathMustExist(bool value);
	/// 読み取り専用ファイルとして開くあるいは開いたかどうか。初期値は false。
	bool readOnly() const;
	void readOnly(bool value);
	/// 読み取り専用ファイルとして開くチェックボックスを表示するかどうか。初期値は true。
	bool readOnlyCheckBox() const;
	void readOnlyCheckBox(bool value);
	/// ダイアログボックスを表示する。ファイルを選択したかどうかを返す。
	bool show(HWND owner);

protected:
};



	}
}