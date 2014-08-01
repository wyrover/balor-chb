#include "OpenFileDialog.hpp"

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommDlg.h>


namespace balor {
	namespace gui {

using std::move;



namespace {
} // namespace



OpenFileDialog::OpenFileDialog() {
	_flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
}


OpenFileDialog::OpenFileDialog(OpenFileDialog&& value, bool checkSlicing)
	: FileDialog(move(value))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


OpenFileDialog::~OpenFileDialog() {
}


OpenFileDialog& OpenFileDialog::operator=(OpenFileDialog&& value) {
	if (&value != this) {
		this->~OpenFileDialog();
		new (this) OpenFileDialog(move(value));
	}
	return *this;
}


bool OpenFileDialog::fileMustExist() const {
	return toFlag(_flags)[OFN_FILEMUSTEXIST];
}


void OpenFileDialog::fileMustExist(bool value) {
	_flags = toFlag(_flags).set(OFN_FILEMUSTEXIST, value);
}


bool OpenFileDialog::pathMustExist() const {
	return toFlag(_flags)[OFN_PATHMUSTEXIST];
}


void OpenFileDialog::pathMustExist(bool value) {
	_flags = toFlag(_flags).set(OFN_PATHMUSTEXIST, value);
}


bool OpenFileDialog::readOnly() const {
	return toFlag(_flags)[OFN_READONLY];
}


void OpenFileDialog::readOnly(bool value) {
	_flags = toFlag(_flags).set(OFN_READONLY, value);
}


bool OpenFileDialog::readOnlyCheckBox() const {
	return !toFlag(_flags)[OFN_HIDEREADONLY];
}


void OpenFileDialog::readOnlyCheckBox(bool value) {
	_flags = toFlag(_flags).set(OFN_HIDEREADONLY, !value);
}


bool OpenFileDialog::show(HWND owner) {
	return FileDialog::show(owner, true);
}



	}
}