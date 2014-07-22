#include "FileDialog.hpp"

#include <balor/gui/Message.hpp>
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
static_assert(FileDialog::ControlId::acceptButton      == IDOK, "Invalid enum value");
static_assert(FileDialog::ControlId::cancelButton      == IDCANCEL, "Invalid enum value");
static_assert(FileDialog::ControlId::directoryComboBox == cmb2, "Invalid enum value");
static_assert(FileDialog::ControlId::directoryLabel    == stc4, "Invalid enum value");
static_assert(FileDialog::ControlId::directoryListView == lst1, "Invalid enum value");
static_assert(FileDialog::ControlId::filterComboBox    == cmb1, "Invalid enum value");
static_assert(FileDialog::ControlId::filterLabel       == stc2, "Invalid enum value");
static_assert(FileDialog::ControlId::filePathEdit      == cmb13, "Invalid enum value");
static_assert(FileDialog::ControlId::filePathLabel     == stc3, "Invalid enum value");
static_assert(FileDialog::ControlId::helpButton        == IDHELP, "Invalid enum value");
static_assert(FileDialog::ControlId::readOnlyCheckBox  == chx1, "Invalid enum value");
} // namespace



bool FileDialog::ControlId::_validate(ControlId value) {
	switch (value) {
		case acceptButton      :
		case cancelButton      :
		case directoryComboBox :
		case directoryLabel    :
		case directoryListView :
		case filterComboBox    :
		case filterLabel       :
		case filePathEdit      :
		case filePathLabel     :
		case helpButton        :
		case readOnlyCheckBox  : return true;
		default : return false;
	}
}



FileDialog::Event::Event(HWND sender)
	: _sender(sender) {
}


String FileDialog::Event::filePath() const {
	wchar_t buffer[32768] = {0};
	if (0 <= SendMessageW(sender(), CDM_GETFILEPATH, sizeof(buffer) / sizeof(buffer[0]), (LPARAM)buffer)) {
		return buffer;
	}
	return String(); // 긨긞긣깗?긏귘긓깛긯깄??뱳궻븿뿚밒궸뫔띪궢궶궋긲긅깑?귩둎궘궴렪봲궥귡갃
}


String FileDialog::Event::directory() const {
	wchar_t buffer[32768] = {0};
	if (0 <= SendMessageW(sender(), CDM_GETFOLDERPATH, sizeof(buffer) / sizeof(buffer[0]), (LPARAM)buffer)) {
		return buffer;
	}
	return String(); // 긨긞긣깗?긏귘긓깛긯깄??뱳궻븿뿚밒궸뫔띪궢궶궋긲긅깑?귩둎궘궴렪봲궥귡갃
}


HWND FileDialog::Event::sender() const {
	return _sender;
}


Size FileDialog::Event::clientSize() const {
	RECT rect = {0};
	verify(GetClientRect(sender(), &rect));
	return Size(rect.right - rect.left, rect.bottom - rect.top);
}



FileDialog::Accept::Accept(HWND sender)
	: FileDialog::Event(sender), _cancel(false) {
}


bool FileDialog::Accept::cancel() const { return _cancel; }
void FileDialog::Accept::cancel(bool value) { _cancel = value; }



FileDialog::Init::Init(HWND sender)
	: FileDialog::Event(sender) {
}


Rectangle FileDialog::Init::getControlBounds(FileDialog::ControlId id) const {
	assert("Invalid FileDialog::ControlId" && ControlId::_validate(id));
	return Control::Handle(GetDlgItem(sender(), id)).bounds();
}


void FileDialog::Init::hideControl(FileDialog::ControlId id) {
	assert("Invalid FileDialog::ControlId" && ControlId::_validate(id));
	SendMessageW(sender(), CDM_HIDECONTROL, id, 0);
}


Point FileDialog::Init::position() const {
	RECT rect = {0};
	verify(GetWindowRect(sender(), &rect));
	return Point(rect.left, rect.top);
}


void FileDialog::Init::position(const Point& value) {
	verify(SetWindowPos(sender(), nullptr, value.x, value.y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE));
}


void FileDialog::Init::position(int x, int y) {
	position(Point(x, y));
}


void FileDialog::Init::setControlText(FileDialog::ControlId id, StringRange text) {
	assert("Invalid FileDialog::ControlId" && ControlId::_validate(id));
	SendMessageW(sender(), CDM_SETCONTROLTEXT, id, (LPARAM)text.c_str());
}


void FileDialog::Init::clientSize(const Size& value) {
	RECT rect = {0, 0, value.width, value.height};
	verify(AdjustWindowRectEx(&rect,  GetWindowLongPtrW(sender(), GWL_STYLE), FALSE, GetWindowLongPtrW(sender(), GWL_EXSTYLE)));
	verify(SetWindowPos(sender(), nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE));
}


void FileDialog::Init::clientSize(int width, int height) {
	clientSize(Size(width, height));
}



FileDialog::FileDialog()
	: _filterIndex(0)
	, _flags(0)
	, _flagsEx(0)
	, _defaultProcedure(nullptr) {
}


FileDialog::FileDialog(FileDialog&& value)
	: _defaultExtention(move(value._defaultExtention))
	, _filePath(move(value._filePath))
	, _filter(move(value._filter))
	, _filterIndex(move(value._filterIndex))
	, _flags(move(value._flags))
	, _flagsEx(move(value._flagsEx))
	, _initialDirectory(move(value._initialDirectory))
	, _title(move(value._title))
	, _onAccept(move(value._onAccept))
	, _onDirectoryChange(move(value._onDirectoryChange))
	, _onHelpRequest(move(value._onHelpRequest))
	, _onInit(move(value._onInit))
	, _onResize(move(value._onResize))
	, _onSelect(move(value._onSelect))
	, _defaultProcedure(move(value._defaultProcedure))
	{
}


FileDialog::~FileDialog() {
}


const String& FileDialog::defaultExtention() const {
	return _defaultExtention;
}


void FileDialog::defaultExtention(String value) {
	_defaultExtention = move(value);
}


const String& FileDialog::filePath() const {
	return _filePath;
}


void FileDialog::filePath(String value) {
	_filePath = move(value);
}


std::vector<String> FileDialog::filePaths() const {
	vector<String> paths;
	if (!multiselect()) {
		return paths;
	}
	int length = String::getLength(_filePath.c_str());
	String path = String::refer(_filePath.c_str(), length);
	while (length < _filePath.length() && _filePath[++length]) {
		paths.push_back(File(path, _filePath.c_str() + length).path());
		length += String::getLength(_filePath.c_str() + length);
	}
	if (paths.empty()) {
		paths.push_back(path);
	}
	return paths;
}


const String& FileDialog::filter() const {
	return _filter;
}


void FileDialog::filter(String value) {
	_filter = move(value);
}


int FileDialog::filterIndex() const {
	return _filterIndex;
}


void FileDialog::filterIndex(int value) {
	assert("Negative fileterIndex" && 0 <= value);
	_filterIndex = value;
}


bool FileDialog::helpButton() const {
	return toFlag(_flags)[OFN_SHOWHELP];
}


void FileDialog::helpButton(bool value) {
	_flags = toFlag(_flags).set(OFN_SHOWHELP, value);
}


const String& FileDialog::initialDirectory() const {
	return _initialDirectory;
}


void FileDialog::initialDirectory(String value) {
	_initialDirectory = move(value);
}


bool FileDialog::multiselect() const {
	return toFlag(_flags)[OFN_ALLOWMULTISELECT];
}


void FileDialog::multiselect(bool value) {
	_flags = toFlag(_flags).set(OFN_ALLOWMULTISELECT, value);
}


bool FileDialog::noDereferenceLinks() const {
	return toFlag(_flags)[OFN_NODEREFERENCELINKS];
}


void FileDialog::noDereferenceLinks(bool value) {
	_flags = toFlag(_flags).set(OFN_NODEREFERENCELINKS, value);
}


Listener<FileDialog::Accept&>& FileDialog::onAccept() { return _onAccept; }
Listener<FileDialog::DirectoryChange&>& FileDialog::onDirectoryChange() { return _onDirectoryChange; }
Listener<FileDialog::HelpRequest&>& FileDialog::onHelpRequest() { return _onHelpRequest; }
Listener<FileDialog::Init&>& FileDialog::onInit() { return _onInit; }
Listener<FileDialog::Resize&>& FileDialog::onResize() { return _onResize; }
Listener<FileDialog::Select&>& FileDialog::onSelect() { return _onSelect; }


bool FileDialog::placesBar() const {
	return !toFlag(_flagsEx)[OFN_EX_NOPLACESBAR];
}


void FileDialog::placesBar(bool value) {
	_flagsEx = toFlag(_flagsEx).set(OFN_EX_NOPLACESBAR, !value);
}


const String& FileDialog::title() const {
	return _title;
}


void FileDialog::title(String value) {
	_title = move(value);
}


void FileDialog::processMessage(Message& msg) {
	switch (msg.message) {
			case WM_INITDIALOG : {
				struct Dialog {
					static LRESULT __stdcall procedure(HWND handle, unsigned int message, WPARAM wparam, LPARAM lparam) {
						auto dialog = (FileDialog*)Control::Handle(handle).control();
						if (message == WM_SIZE) {
							Resize event(handle);
							dialog->onResize()(event);
						}
						return CallWindowProcW(dialog->_defaultProcedure, handle, message, wparam, lparam);
					}
				};
				auto sender = GetParent(msg.handle);
				Control::Handle(sender).control((Control*)this);
				_defaultProcedure = Control::Handle(sender).procedure(Dialog::procedure);
		} break;
		case WM_NOTIFY : {
			auto info = (OFNOTIFYW*)msg.lparam;
			auto sender = GetParent(msg.handle);
			switch (info->hdr.code) {
				case CDN_INITDONE : {
					Init event(sender);
					onInit()(event);
					msg.result = TRUE;
				} break;
				case CDN_FILEOK : {
					Accept event(sender);
					onAccept()(event);
					SetWindowLongPtrW(msg.handle, GWLP_WNDPROC, event.cancel() ? TRUE : 0); // GWLP_WNDPROC의 원 코드는 DWL_MSGRESULT
					msg.result = event.cancel() ? TRUE : 0;
				} break;
				case CDN_HELP : {
					HelpRequest event(sender);
					onHelpRequest()(event);
				} break;
				case CDN_FOLDERCHANGE : {
					DirectoryChange event(sender);
					onDirectoryChange()(event);
				} break;
				case CDN_SELCHANGE : {
					Select event(sender);
					onSelect()(event);
				} break;
			}
		} break;
	}
}


bool FileDialog::show(HWND owner, bool open) {
	OPENFILENAMEW info;
	ZeroMemory(&info, sizeof(info));
	info.lStructSize = sizeof(info);
	//info.lStructSize = 0x4C;

	info.hwndOwner = owner;
	info.lCustData = (LPARAM)this;
	String convertedFilter = filter().replace('\n', '\0');
	info.lpstrFilter = convertedFilter.empty() ? nullptr : convertedFilter.c_str();
	info.nFilterIndex = filterIndex() + 1;
	info.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING | _flags;
	info.FlagsEx = _flagsEx;

	struct Fook {
		static UINT_PTR CALLBACK procedure(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
			if (message == WM_INITDIALOG) {
				auto info = (OPENFILENAMEW*)lparam;
				Control::Handle(handle).control((Control*)info->lCustData);
			}
			Message msg(handle, message, wparam, lparam);
			FileDialog* dialog = (FileDialog*)Control::Handle(handle).control();
			if (dialog) {
				dialog->processMessage(msg);
			}
			return msg.result;
		}
	};
	info.lpfnHook = Fook::procedure;

	wchar_t buffer[32768] = {0};
	filePath().copyTo(buffer);
	info.lpstrFile = buffer;
	info.nMaxFile = sizeof(buffer) / sizeof(buffer[0]);

	info.lpstrDefExt = defaultExtention().empty() ? nullptr : defaultExtention().c_str();
	info.lpstrInitialDir = initialDirectory().empty() ? nullptr : initialDirectory().c_str();
	info.lpstrTitle = title().empty() ? nullptr : title().c_str();

	BOOL result;
	if (open) {
		result = GetOpenFileNameW(&info);
		assert("Failed to GetOpenFileNameW" && (result || !CommDlgExtendedError()));
	} else {
		result = GetSaveFileNameW(&info);
		assert("Failed to GetSaveFileNameW" && (result || !CommDlgExtendedError()));
	}

	if (result) {
		if (multiselect()) {
			int length = 0;
			do {
				length += String::getLength(buffer + length);
			} while (length < sizeof(buffer) / sizeof(buffer[0]) && buffer[++length]);
			_filePath = String(buffer, length);
		} else {
		_filePath = buffer;
		}
		_filterIndex = info.nFilterIndex - 1;
		_flags = info.Flags;
	}

	return result != 0;
}



	}
}