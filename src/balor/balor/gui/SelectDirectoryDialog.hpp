#pragma once

#include <balor/gui/FileDialog.hpp>


namespace balor {
	namespace gui {



/**
* ディレクトリを選択するダイアログボックス。
*
* SHBrowseForFolder API は機能が貧弱なので FileOpenDialog を改造する。
* 現状 SelectDirectoryDialog::Event::filePath() 関数は正しい結果を返さない。
* 代わりに SelectDirectoryDialog::directoryPath() がリアルタイム更新されるのでこちらを使うこと。
*
* <h3>・サンプルコード</h3>
* <pre><code>
	Frame frame(L"SelectDirectoryDialog Sample");

	Button select(frame, 20, 10, 0, 0, L"ディレクトリを選択する");
	Edit directoryPath(frame, 20, 50, 0, 0, 150);
	directoryPath.readOnly(true);

	SelectDirectoryDialog dialog;
	dialog.title(L"ディレクトリの選択");

	select.onClick() = [&] (Button::Click& ) {
		if (dialog.show(frame)) {
			directoryPath.text(dialog.directoryPath());
		}
	};

	frame.runMessageLoop();
* </code></pre>
*/
class SelectDirectoryDialog : protected FileDialog {
public:
	typedef FileDialog::ControlId ControlId;
	typedef FileDialog::Event Event;
	typedef FileDialog::HelpRequest HelpRequest;
	typedef FileDialog::Resize Resize;
	typedef FileDialog::Accept Accept;
	typedef FileDialog::Init Init;

public:
	SelectDirectoryDialog();
	SelectDirectoryDialog(SelectDirectoryDialog&& value);
	virtual ~SelectDirectoryDialog();
	SelectDirectoryDialog& operator=(SelectDirectoryDialog&& value);

public:
	/// ダイアログ表示前は初期表示されるディレクトリパス。ダイアログ表示後はユーザが選択したディレクトリパス。初期値は空文字列
	const String& directoryPath() const;
	void directoryPath(String value);

	using FileDialog::helpButton;
	using FileDialog::onAccept;
	using FileDialog::onHelpRequest;
	using FileDialog::onInit;
	using FileDialog::onResize;
	using FileDialog::placesBar;

	/// ダイアログボックスを表示する。ファイルを選択したかどうかを返す
	bool show(HWND owner);

	using FileDialog::title;

protected:
	/// フックしたメッセージを処理する
	virtual void processMessage(Message& msg);

protected:
	Procedure _editDefaultProcedure;
};



	}
}