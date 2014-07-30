#include "AutoComplete.hpp"

#include <utility>
#include <vector>
#include <Shlwapi.h>
#include <ShlDisp.h>
#pragma comment (lib,"Shlwapi.lib")
#include <ShlGuid.h>

#include <balor/gui/Edit.hpp>
#include <balor/system/Com.hpp>
#include <balor/system/ComBase.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::system;



namespace {
static_assert(AutoComplete::Mode::append           == SHACF_AUTOAPPEND_FORCE_ON, "Invalid enum value");
static_assert(AutoComplete::Mode::suggest          == SHACF_AUTOSUGGEST_FORCE_ON, "Invalid enum value");
static_assert(AutoComplete::Mode::appendAndSuggest == (SHACF_AUTOAPPEND_FORCE_ON | SHACF_AUTOSUGGEST_FORCE_ON), "Invalid enum value");

static_assert(AutoComplete::SystemItems::fileSystem          == SHACF_FILESYSTEM, "Invalid enum value");
static_assert(AutoComplete::SystemItems::fileSystemDirectory == SHACF_FILESYS_DIRS, "Invalid enum value");
static_assert(AutoComplete::SystemItems::urlHistory          == SHACF_URLHISTORY, "Invalid enum value");
static_assert(AutoComplete::SystemItems::recentlyUsedUrl     == SHACF_URLMRU, "Invalid enum value");
} // namespace



// ただ文字列を列挙するだけの COM インターフェース
class AutoComplete::EnumString : public ComBase<IEnumString> {
	EnumString() : _current(0) {
	}

	virtual ~EnumString() {
	}

public:
	static ComPtr<EnumString> create() {
		ComPtr<EnumString> ptr;
		*(&ptr) = new EnumString();
		return ptr;
	}

private:
	virtual HRESULT STDMETHODCALLTYPE Clone(IEnumString** enumString) {
		*enumString = nullptr;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Next(ULONG delta, LPOLESTR* buffers, ULONG* fetchedCount) {
		assert(0 <= delta);
		ULONG i = 0;
		for (unsigned int end = items.size(); i < delta && _current < end; ++i, ++_current) {
			auto& item = items[_current];
			auto buffer = static_cast<wchar_t*>(CoTaskMemAlloc((item.length() + 1) * sizeof(wchar_t)));
			item.copyTo(buffer, item.length() + 1);
			buffers[i] = buffer;
		}
		if (fetchedCount) {
			*fetchedCount = i;
		}
		return i == delta ? S_OK : S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE Reset() {
		_current = 0;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Skip(ULONG delta) {
		_current += delta;
		return _current < items.size() ? S_OK : S_FALSE;
	}

private:
	unsigned int _current;

public:
	vector<String> items;
};


bool AutoComplete::Mode::_validate(Mode value) {
	switch (value) {
		case append           :
		case suggest          :
		case appendAndSuggest : return true;
		default               : return false;
	}
}


bool AutoComplete::SystemItems::_validate(SystemItems value) {
	return !(value & ~(
		  fileSystem         
		| fileSystemDirectory
		| urlHistory         
		| recentlyUsedUrl));
}


AutoComplete::AutoComplete()
	: _enabled(false), _mode(Mode::appendAndSuggest), _systemItems(SystemItems::none) {
}


AutoComplete::AutoComplete(AutoComplete&& value)
	: _target(move(value._target))
	, _enabled(move(value._enabled))
	, _mode(move(value._mode))
	, _systemItems(move(value._systemItems))
	, _autoComplete(move(value._autoComplete))
	, _enumString(move(value._enumString))
	{
}


AutoComplete::AutoComplete(Edit& target, StringRangeArray items, AutoComplete::Mode mode)
	: _target(&target), _enabled(true), _mode(mode), _systemItems(SystemItems::none) {
	assert("target not created" && target);
	assert("Invalid AutoCompelte::Mode" && Mode::_validate(mode));

	ComPtr<IAutoComplete> source(CLSID_AutoComplete);
	source.queryInterface(_autoComplete);
	assert(_autoComplete);

	_enumString = EnumString::create();
	_enumString->items.reserve(items.length());
	for (auto i = 0, end = items.length(); i < end; ++i) {
		_enumString->items.push_back(items[i].c_str());
	}
	DWORD options = 0;
	if (mode == Mode::append || mode == Mode::appendAndSuggest) {
		options |= ACO_AUTOAPPEND;
	}
	if (mode == Mode::suggest || mode == Mode::appendAndSuggest) {
		options |= ACO_AUTOSUGGEST;
	}
	verify(SUCCEEDED(_autoComplete->SetOptions(options)));
	verify(SUCCEEDED(_autoComplete->Init(target.handle(), &(*_enumString), nullptr, nullptr)));
}


AutoComplete::AutoComplete(Edit& target, AutoComplete::SystemItems systemItems, AutoComplete::Mode mode)
	: _target(&target), _enabled(true), _mode(mode), _systemItems(systemItems) {
	assert("target not created" && target);
	assert("Invalid AutoCompelte::Mode" && Mode::_validate(mode));
	assert("Invalid AutoCompelte::SystemItems" && SystemItems::_validate(systemItems));
	int flags = mode | systemItems;
	if (!(flags & Mode::append)) {
		flags |= SHACF_AUTOAPPEND_FORCE_OFF;
	}
	if (!(flags & Mode::suggest)) {
		flags |= SHACF_AUTOSUGGEST_FORCE_OFF;
	}
	if (FAILED(SHAutoComplete(*_target, flags))) {
		if (Com::isMainThread()) {
			Com::initialize();
		}
		verify(SUCCEEDED(SHAutoComplete(*_target, flags)));
	}
}


AutoComplete::~AutoComplete() {
	if (_target && IsWindow(*_target)) {
		enabled(false);
	}
}


AutoComplete& AutoComplete::operator=(AutoComplete&& value) {
	if (this != &value) {
		this->~AutoComplete();
		new (this) AutoComplete(move(value));
	}
	return *this;
}


bool AutoComplete::enabled() const {
	return _enabled;
}


void AutoComplete::enabled(bool value) {
	assert("Null target" && _target);
	assert("target not created" && *_target);

	if (value != enabled()) {
		if (_autoComplete) {
			verify(SUCCEEDED(_autoComplete->Enable(value ? TRUE : FALSE)));
		} else {
			if (value) {
				*this = AutoComplete(*_target, _systemItems, _mode);
			} else {
				verify(SUCCEEDED(SHAutoComplete(*_target, SHACF_AUTOAPPEND_FORCE_OFF | SHACF_AUTOSUGGEST_FORCE_OFF)));
			}
		}
		_enabled = value;
	}
}


::std::vector<String, ::std::allocator<String> >& AutoComplete::items() const {
	assert("items not available" && _enumString);
	return _enumString->items;
}


AutoComplete::Mode AutoComplete::mode() const {
	return _mode;
}


AutoComplete::SystemItems AutoComplete::systemItems() const {
	return _systemItems;
}



	}
}