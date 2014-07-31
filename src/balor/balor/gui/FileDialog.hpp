#pragma once

#include <vector>

#include <balor/gui/Control.hpp>


namespace balor {
	namespace gui {



/**
* ファイルを開く・保存するダイアログボックスの基底クラス。
*
*/
class FileDialog {
public:
	typedef Control::HWND HWND;
	typedef Control::Procedure Procedure;


	/// ダイアログ上のコントロールID。
	struct ControlId {
		enum _enum {
			acceptButton = 0x0001, /// 決定ボタン。
			cancelButton = 0x0002, /// キャンセルボタン。
			directoryComboBox = 0x0471, /// 現在のディレクトリを表示するコンボボックス。
			directoryLabel = 0x0443, /// 現在のディレクトリを表示するコンボボックスのラベル。
			directoryListView = 0x0460, /// 現在のディレクトリを表示するリストビュー。
			filterComboBox = 0x0470, /// フィルターを表示するコンボボックス。
			filterLabel = 0x0441, /// フィルターを表示するコンボボックスのラベル。
			filePathEdit = 0x047c, /// ファイルパスを入力するエディットコントロール。
			filePathLabel = 0x0442, /// ファイルパスを入力するエディットコントロールのラベル。
			helpButton = 0x0009, /// ヘルプボタン。
			readOnlyCheckBox = 0x0410, /// 読み取り専用で開くチェックボックス。
		};
		BALOR_NAMED_ENUM_MEMBERS(ControlId);
	};


	/// FileDialog のイベントの親クラス。
	struct Event : public balor::Event {
		Event(HWND sender);

		/// 現在選択されているファイルパスまたはディレクトリパス。
		String filePath() const;
		/// 現在表示しているディレクトリ。
		String directory() const;
		/// ダイアログのウインドウハンドル。
		HWND sender() const;
		/// ダイアログのクライアント領域の大きさ。
		Size clientSize() const;

	private:
		HWND _sender;
	};

	typedef Event DirectoryChange;
	typedef Event HelpRequest;
	typedef Event Resize;
	typedef Event Select;


	/// 決定ボタンを押したイベント。
	struct Accept : public Event {
		Accept(HWND sender);

		/// 決定をキャンセルするかどうか。初期値は false。
		bool cancel() const;
		void cancel(bool value);

	private:
		bool _cancel;
	};


	/// ダイアログを初期化するイベント。
	struct Init : public Event {
		Init(HWND sender);

		/// ダイアログ上のコントロールの領域。
		Rectangle getControlBounds(FileDialog::ControlId id) const;
		/// ダイアログ上のコントロールを非表示にする。
		void hideControl(FileDialog::ControlId id);
		/// ダイアログのスクリーン座標位置。
		/// 現状ではアプリケーションのプロセスが起動してから最初の一回しか変更できない模様。次回からは前回の位置が復元されている。
		Point position() const;
		void position(const Point& value);
		void position(int x, int y);
		/// ダイアログ上のコントロールの文字列を変更する。
		void setControlText(FileDialog::ControlId id, StringRange text);
		/// ダイアログのクライアント領域の大きさ。
		using Event::clientSize;
		void clientSize(int width, int height);
		void clientSize(const Size& value);
	};


public:
	FileDialog();
	FileDialog(FileDialog&& value);
	virtual ~FileDialog() = 0;

	/// ユーザがファイル名に拡張子を入力しなかった場合に自動的に追加される拡張子。カンマは含まない。初期値は空文字列。
	const String& defaultExtention() const;
	void defaultExtention(String value);
	/// ダイアログ表示前は初期表示されるファイルパスまたはファイル名。ダイアログ表示後はユーザが選択したファイルパス。multiselect() が true の場合は filePaths() のほうを使用する。初期値は空文字列。
	const String& filePath() const;
	void filePath(String value);
	/// ダイアログでユーザが選択したファイルパスリスト。multiselect() が true の場合のみ有効。
	std::vector<String> filePaths() const;
	/// ダイアログで選択できるファイルの拡張子名と拡張子の組のリスト。拡張子名と拡張子は'\n'で区切り、拡張子はセミコロンで区切って複数指定できる。さらに'\n'区切りで複数の組を指定する。
	/// "\n\n"で組の終端になる。例えば下記のように指定する。<br>
	/// "すべてのファイル\n*.*\nテキストファイル\n*.txt\nJPEGファイル\n*.jpg;*.jpeg\n\n"
	/// <br>区切り文字は'\0'でも良いが文字列リテラルに'\0'が含まれると String のコンストラクタはそこで文字列が終了していると判断してしまうので String::literal() 関数を使用するかコンストラクタで文字列の長さを指定する事。
	/// 初期値は空文字列。
	const String& filter() const;
	void filter(String value);
	/// ダイアログ表示前は filter() で指定した拡張子リストの初期インデックス。ダイアログ表示後はユーザが最後に選択したインデックス。初期値は 0。
	int filterIndex() const;
	void filterIndex(int value);
	/// ヘルプボタンを表示するかどうか。初期値は false。
	bool helpButton() const;
	void helpButton(bool value);
	/// ダイアログを表示した時に最初に表示されるディレクトリ。ただし filePath() にパスが含まれる場合はそちらを優先する。
	/// filePath() も initialDirectory() も空文字列の場合はシステムのカレントディレクトリになる。初期値は空文字列。
	const String& initialDirectory() const;
	void initialDirectory(String value);
	/// CTRL キーを押しながらファイルを複数選択できるかどうか。初期値は false。
	bool multiselect() const;
	void multiselect(bool value);
	/// リンクファイルを指定した時にリンク先とみなさずリンクファイルとして開くかどうか。初期値は false。
	bool noDereferenceLinks() const;
	void noDereferenceLinks(bool value);
	/// 決定ボタンを押したイベント。
	Listener<FileDialog::Accept&>& onAccept();
	/// ディレクトリを変更したイベント。
	Listener<FileDialog::DirectoryChange&>& onDirectoryChange();
	/// ヘルプボタンが押されたイベント。helpButton() が true の場合のみ発生する。
	Listener<FileDialog::HelpRequest&>& onHelpRequest();
	/// ダイアログを初期化するイベント。
	Listener<FileDialog::Init&>& onInit();
	/// ダイアログの大きさを変更したイベント。
	Listener<FileDialog::Resize&>& onResize();
	/// ファイルまたはディレクトリを選択したイベント。
	Listener<FileDialog::Select&>& onSelect();
	/// ダイアログの左側にファイルの場所リストを表示するかどうか。初期値は true。
	bool placesBar() const;
	void placesBar(bool value);
	/// ダイアログボックスのタイトル。空文字列を指定した場合はシステムデフォルトになる。初期値は空文字列。
	const String& title() const;
	void title(String value);

protected:
	/// フックしたメッセージを処理する。
	virtual void processMessage(Message& msg);
	/// ダイアログボックスを表示する。ファイルを選択したかどうかを返す。
	bool show(HWND owner, bool open);

protected:
	String _defaultExtention;
	String _filePath;
	String _filter;
	int _filterIndex;
	int _flags;
	int _flagsEx;
	String _initialDirectory;
	String _title;
	Listener<FileDialog::Accept&> _onAccept;
	Listener<FileDialog::DirectoryChange&> _onDirectoryChange;
	Listener<FileDialog::HelpRequest&> _onHelpRequest;
	Listener<FileDialog::Init&> _onInit;
	Listener<FileDialog::Resize&> _onResize;
	Listener<FileDialog::Select&> _onSelect;
	Procedure _defaultProcedure;
};



	}
}