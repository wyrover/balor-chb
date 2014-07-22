#include "SelectDirectoryDialog.hpp"

#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommDlg.h>
#include <CdErr.h>
#include <Dlgs.h>


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::io;



namespace {
const int editControlId = 2001;
const wchar_t dummyFileName[] = L"0ae9x014z915lkdle7hc";
} // namespace



SelectDirectoryDialog::SelectDirectoryDialog()
	: _editDefaultProcedure(nullptr) {
	_flags |= OFN_HIDEREADONLY;
	// ありえないファイル名フィルタでディレクトリのみ表示する。
	filter(String::literal(L" \0*.<>"));
}


SelectDirectoryDialog::SelectDirectoryDialog(SelectDirectoryDialog&& value)
	: FileDialog(move(value))
	, _editDefaultProcedure(move(value._editDefaultProcedure)) {
}


SelectDirectoryDialog::~SelectDirectoryDialog() {
}


SelectDirectoryDialog& SelectDirectoryDialog::operator=(SelectDirectoryDialog&& value) {
	if (&value != this) {
		this->~SelectDirectoryDialog();
		new (this) SelectDirectoryDialog(move(value));
	}
	return *this;
}


const String& SelectDirectoryDialog::directoryPath() const {
	return initialDirectory();
}


void SelectDirectoryDialog::directoryPath(String value) {
	initialDirectory(value);
}


bool SelectDirectoryDialog::show(HWND owner) {
	bool result = FileDialog::show(owner, true);
	filePath(L"");
	return result;
}


void SelectDirectoryDialog::processMessage(Message& msg) {
	switch (msg.message) {
		case WM_NOTIFY : {
			auto info = (OFNOTIFYW*)msg.lparam;
			auto sender = GetParent(msg.handle);
			switch (info->hdr.code) {
				case CDN_INITDONE : {
					Init event(sender);

					event.hideControl(ControlId::filePathEdit);
					event.hideControl(ControlId::filterComboBox);
					event.hideControl(ControlId::filterLabel);

					auto filePathEdit = Control::Handle(GetDlgItem(sender, ControlId::filePathEdit));
					auto bounds = filePathEdit.bounds();
					auto edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr
						, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | ES_AUTOHSCROLL
						, bounds.x, bounds.y, bounds.width, bounds.height, sender, nullptr,nullptr, nullptr);
					SetWindowLongPtrW(edit, GWLP_ID, editControlId);
					SendMessageW(edit, EM_SETREADONLY, TRUE, 0);

					struct FilePathEdit {
						static LRESULT __stdcall procedure(HWND handle, unsigned int message, WPARAM wparam, LPARAM lparam) {
							auto dialog = (SelectDirectoryDialog*)Control::Handle(handle).control();
							if (message == WM_SIZE) {
								auto parent = GetParent(handle);
								auto edit = Control::Handle(GetDlgItem(parent, editControlId));
								edit.bounds(Control::Handle(handle).bounds());
							}
							return CallWindowProcW(dialog->_editDefaultProcedure, handle, message, wparam, lparam);
						}
					};
					filePathEdit.control((Control*)this);
					_editDefaultProcedure = filePathEdit.procedure(FilePathEdit::procedure);
					FileDialog::processMessage(msg);
				} break;
				case CDN_FOLDERCHANGE : {
					Init event(sender);
					auto edit = Control::Handle(GetDlgItem(sender, editControlId));
					File directory = event.directory();
					if (!directory.empty() && directory.name() == dummyFileName) {
						directory = directory.parent();
					}
					verify(SetWindowTextW(edit, directory.path()));
					directoryPath(directory.path());
					event.setControlText(ControlId::filePathEdit, directory.empty() ? L"" : dummyFileName);
					FileDialog::processMessage(msg);
				} break;
				case CDN_SELCHANGE : {
					Init event(sender);
					auto edit = Control::Handle(GetDlgItem(sender, editControlId));
					File filePath = event.filePath();
					if (!filePath.empty() && filePath.name() == dummyFileName) {
						filePath = filePath.parent();
					}
					verify(SetWindowTextW(edit, filePath.path()));
					directoryPath(filePath.path());
					event.setControlText(ControlId::filePathEdit, filePath.empty() ? L"" : dummyFileName);
					FileDialog::processMessage(msg);
				} break;
				default : {
					FileDialog::processMessage(msg);
				} break;
			}
		} break;
		default : {
			FileDialog::processMessage(msg);
		} break;
	}
}



	}
}