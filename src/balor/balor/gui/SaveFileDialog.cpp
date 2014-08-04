#include "SaveFileDialog.hpp"

#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>

#include <CommDlg.h>


namespace balor {
	namespace gui {

using std::move;



namespace {
} // namespace



SaveFileDialog::SaveFileDialog() {
	_flags |= OFN_OVERWRITEPROMPT;
}


SaveFileDialog::SaveFileDialog(SaveFileDialog&& value, bool checkSlicing)
	: FileDialog(move(value))
	{
	if (checkSlicing) {
		assert("Object sliced" && typeid(*this) == typeid(value));
	}
}


SaveFileDialog::~SaveFileDialog() {
}


SaveFileDialog& SaveFileDialog::operator=(SaveFileDialog&& value) {
	if (&value != this) {
		this->~SaveFileDialog();
		new (this) SaveFileDialog(move(value));
	}
	return *this;
}


bool SaveFileDialog::overwritePrompt() const {
	return toFlag(_flags)[OFN_OVERWRITEPROMPT];
}


void SaveFileDialog::overwritePrompt(bool value) {
	_flags = toFlag(_flags).set(OFN_OVERWRITEPROMPT, value);
}


bool SaveFileDialog::show(HWND owner) {
	return FileDialog::show(owner, false);
}



	}
}