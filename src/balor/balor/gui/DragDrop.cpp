#include "DragDrop.hpp"

#include <string>
#include <vector>
#include <ObjBase.h>
#include <ShlObj.h>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/gui/Clipboard.hpp>
#include <balor/gui/Frame.hpp>
#include <balor/gui/Mouse.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/locale/Charset.hpp>
#include <balor/system/Com.hpp>
#include <balor/system/ComBase.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Flag.hpp>
#include <balor/scopeExit.hpp>
#include <balor/StringBuffer.hpp>


#include <balor/test/Debug.hpp>
#include <balor/gui/MessageBox.hpp>
using namespace balor::test;


namespace balor {
	namespace gui {

using std::move;
using std::string;
using std::vector;
using std::wstring;
using namespace balor::graphics;
using namespace balor::io;
using namespace balor::locale;
using namespace balor::system;


namespace {
static_assert(DragDrop::Effect::none   == DROPEFFECT_NONE, "Innvalid enum value");
static_assert(DragDrop::Effect::copy   == DROPEFFECT_COPY, "Innvalid enum value");
static_assert(DragDrop::Effect::move   == DROPEFFECT_MOVE, "Innvalid enum value");
static_assert(DragDrop::Effect::link   == DROPEFFECT_LINK, "Innvalid enum value");
static_assert(DragDrop::Effect::scroll == DROPEFFECT_SCROLL, "Innvalid enum value");


/// DataObject が持つデータの型を表す
struct Formatetc : public FORMATETC {
	Formatetc() {
		cfFormat = 0;
		ptd = nullptr;
		dwAspect = DVASPECT_CONTENT;
		lindex = -1;
		tymed = TYMED_NULL;
	}
	
	Formatetc(CLIPFORMAT format, DWORD tymed) {
		cfFormat = format;
		ptd = nullptr;
		dwAspect = DVASPECT_CONTENT;
		lindex = -1;
		this->tymed = tymed;
	}
	
	Formatetc& operator=(const FORMATETC& value) {
		*static_cast<FORMATETC*>(this) = value;
		return *this;
	}
};


/// DataObject の持つデータを表す
struct StgMedium : public STGMEDIUM {
	StgMedium() {
		tymed = TYMED_NULL;
		hGlobal = nullptr;
		pUnkForRelease = nullptr;
	}
	StgMedium(StgMedium&& value) {
		tymed = value.tymed;
		hGlobal = value.hGlobal;
		pUnkForRelease = value.pUnkForRelease;

		value.tymed = TYMED_NULL;
		value.hGlobal = nullptr;
		value.pUnkForRelease = nullptr;
	}
	~StgMedium() {
		release();
	}

	StgMedium& operator=(StgMedium&& value) {
		if (this != &value) {
			release();
			*static_cast<STGMEDIUM*>(this) = value;

			value.tymed = TYMED_NULL;
			value.hGlobal = nullptr;
			value.pUnkForRelease = nullptr;
		}
		return *this;
	}
	StgMedium& operator=(const STGMEDIUM& value) {
		if (this != &value) {
			release();
			*static_cast<STGMEDIUM*>(this) = value;
		}
		return *this;
	}

private: // 複製の禁止
	StgMedium(const StgMedium& );

public:

	void release() {
		if (tymed != TYMED_NULL) {
			ReleaseStgMedium(this);
			tymed = TYMED_NULL;
			hGlobal = nullptr;
			pUnkForRelease = nullptr;
		}
	}

	static void copy(STGMEDIUM& target, const STGMEDIUM& source, const FORMATETC& formatetc) {
		target.tymed = source.tymed;
		target.pUnkForRelease = nullptr;
		target.hGlobal = nullptr;
		if (target.tymed != TYMED_NULL) {
			if (target.tymed == TYMED_GDI) {
				Bitmap bitmap = Bitmap::clone(source.hBitmap);
				bitmap.owned(false);
				target.hBitmap = bitmap; // OleDuplicateData ではうまくいかない
			} else {
				target.hGlobal = OleDuplicateData(source.hGlobal, formatetc.cfFormat, GMEM_MOVEABLE);
				if (!target.hGlobal) { // 原因はメモリ不足ではないかもしれないが可能性は高い
					if (source.tymed != TYMED_HGLOBAL || GlobalSize(source.hGlobal)) {
						throw DragDrop::OutOfMemoryException();
					}
				}
			}
		}
	}
};


/// ドラッグドロップするデータをプロセスを超えて持ち運ぶ COM インターフェース
class DataObject : public IDataObject, public IEnumFORMATETC {
	DataObject() : _count(1), _current(0) {
	}
	virtual ~DataObject() {
		assert(_count == 0);
	}

private: // 複製を禁じる
	DataObject(const DataObject& );
	DataObject operator=(const DataObject& );

public:
	static ComPtr<IDataObject> create() {
		ComPtr<IDataObject> ptr;
		*(&ptr) = new DataObject();
		return ptr;
	}

public: // IUnknown
	virtual ULONG STDMETHODCALLTYPE AddRef() {
		return ::InterlockedIncrement(&_count);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id, void** object) {
		if (IsEqualIID(id, IID_IUnknown) || IsEqualIID(id, __uuidof(IDataObject))) {
			*object = this;
			AddRef();
			return S_OK;
		} else if (IsEqualIID(id, __uuidof(IDataObject))) {
			*object = static_cast<IEnumFORMATETC*>(this); // IDataObject* と IEnumFORMATETC* ではアドレスが異なる
			AddRef();
			return S_OK;
		} else {
			*object = NULL;
			return E_NOINTERFACE;
		}
	}

	virtual ULONG STDMETHODCALLTYPE Release() {
		auto newCount = ::InterlockedDecrement(&_count);
		if (!newCount) {
			delete this;
		}
		return newCount;
	}

private:
	long _count;


public: // IDataObject
	virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC* /*formatetc*/, DWORD /*advf*/, IAdviseSink* /*adviseSink*/, DWORD* connection) {
		*connection = 0;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD /*connection*/) {
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA** enumStatData) {
		*enumStatData = nullptr;
		return OLE_E_ADVISENOTSUPPORTED;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD direction, IEnumFORMATETC** enumFormatetc) {
		if (direction == DATADIR_GET) {
			AddRef();
			*enumFormatetc = static_cast<IEnumFORMATETC*>(this); // IDataObject* と IEnumFORMATETC* ではアドレスが異なる
			return S_OK;
		} else {
			return E_NOTIMPL;
		}
	}

	virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC* /*formatetcIn*/, FORMATETC* formatetcOut) {
		if (formatetcOut) {
			memset(formatetcOut, 0, sizeof(*formatetcOut));
		}
		return DATA_S_SAMEFORMATETC;
	}

	virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC* formatetc, STGMEDIUM* medium) {
		auto result = checkFormatetc(*formatetc);
		if (result != S_OK) {
			return result;
		}
		for (auto i = _datas.begin(), end = _datas.end(); i != end; ++i) {
			if (i->formatetc.cfFormat == formatetc->cfFormat && i->formatetc.tymed & formatetc->tymed) {
				assert(formatetc->tymed & i->medium.tymed);
				StgMedium::copy(*medium, i->medium, i->formatetc);
				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}

	/// medium には既にハンドルが割り当てられており、そこにデータを取得する。メモリならば GlobalReAlloc する
	virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC* /*formatetc*/, STGMEDIUM* /*medium*/) {
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC* formatetc) {
		auto result = checkFormatetc(*formatetc);
		if (result != S_OK) {
			return result;
		}
		for (auto i = _datas.begin(), end = _datas.end(); i != end; ++i) {
			if (i->formatetc.cfFormat == formatetc->cfFormat && i->formatetc.tymed & formatetc->tymed) {
				return S_OK;
			}
		}
		return DV_E_FORMATETC;
	}

	virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC* formatetc, STGMEDIUM* medium, BOOL release) {
		auto result = checkFormatetc(*formatetc);
		if (result != S_OK) {
			return result;
		}
		Data* data = nullptr;
		for (auto i = _datas.begin(), end = _datas.end(); i != end; ++i) {
			if (i->formatetc.cfFormat == formatetc->cfFormat && i->formatetc.tymed & formatetc->tymed) {
				data = &(*i);
				break;
			}
		}
		if (!data) {
			_datas.push_back(Data());
			data = &_datas[_datas.size() - 1];
		}

		data->formatetc = *formatetc;
		if (!release) {
			StgMedium::copy(data->medium, *medium, *formatetc);
		} else {
			data->medium = *medium;
		}
		return S_OK;
	}

private:
	HRESULT checkFormatetc(const FORMATETC& formatetc) {
		if (formatetc.dwAspect != DVASPECT_CONTENT) {
			return DV_E_DVASPECT;
		}
		if (!(formatetc.tymed & (TYMED_HGLOBAL | TYMED_ISTREAM | TYMED_GDI | TYMED_MFPICT | TYMED_ENHMF))) {
			return DV_E_TYMED;
		}
		return S_OK;
	}

	struct Data {
		Data() {}
		Data(Data&& value) : formatetc(move(value.formatetc)), medium(move(value.medium)) {
		}

	private:
		Data(const Data& );
		Data operator=(const Data& );

	public:
		Formatetc formatetc;
		StgMedium medium;
	};
	vector<Data> _datas;


public: // IEnumFORMATETC
	virtual HRESULT STDMETHODCALLTYPE Clone(IEnumFORMATETC** enumFormatetc) {
		*enumFormatetc = nullptr;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, FORMATETC* formatetcs, ULONG* fetched) {
		const int size = static_cast<int>(_datas.size());
		auto i = celt;
		while (_current < size && 0 < i) {
			*formatetcs = _datas[_current].formatetc;
			++formatetcs;
			++_current;
			--i;
		}
		if (fetched) {
			*fetched = celt - i;
		}
		return i == 0 ? S_OK : S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE Reset() {
		_current = 0;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) {
		const auto size = _datas.size();
		if (_current + celt < size) {
			_current += celt;
			return S_OK;
		} else {
			_current = size;
			return S_FALSE;
		}
	}

private:
	int _current;
};


struct GlobalLocker {
	GlobalLocker(HGLOBAL handle) : _pointer(static_cast<BYTE*>(GlobalLock(handle))) {
		assert(handle);
		//assert(_pointer);
	}
	~GlobalLocker() {
		verify(GlobalUnlock(_pointer) || GetLastError() == NO_ERROR);
	}

	BYTE* pointer() { return _pointer; }

private:
	GlobalLocker(const GlobalLocker& );
	GlobalLocker& operator=(const GlobalLocker& );

	BYTE* _pointer;
};


HGLOBAL allocateGlobal(int size) {
	auto handle = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!handle) {
		if (GetLastError() == ERROR_NOT_ENOUGH_MEMORY) {
			throw DragDrop::OutOfMemoryException();
		}
		assert("Failed to GlobalAlloc" && false);
	}
	return handle;
}
} // namespace



bool DragDrop::Effect::_validate(Effect value) {
	return (value & ~(
		  none
		| copy
		| move
		| link
		| scroll
		)) == 0;
}



DragDrop::Drop::Drop(Control& sender, const Data& data, Effect allowedEffects, int keyState, const Point& position)
	: Control::Event(sender), _data(data), _allowedEffects(allowedEffects), _keyState(keyState), _position(position), _effect(Effect::move) {
}


DragDrop::Effect DragDrop::Drop::allowedEffects() const { return _allowedEffects; }
bool DragDrop::Drop::alt() const { return (_keyState & MK_ALT) != 0; }
bool DragDrop::Drop::ctrl() const { return (_keyState & MK_CONTROL) != 0; }
const DragDrop::Data& DragDrop::Drop::data() const { return _data; }
DragDrop::Effect DragDrop::Drop::effect() const { return _effect; }
void DragDrop::Drop::effect(DragDrop::Effect value) {
	assert("Invalid DragDrop::Effect" && (value == Effect::none
									   || value == Effect::copy
									   || value == Effect::move
									   || value == Effect::link));
	_effect = value;
}
bool DragDrop::Drop::lButton() const { return (_keyState & MK_LBUTTON) != 0; }
bool DragDrop::Drop::mButton() const { return (_keyState & MK_MBUTTON) != 0; }
const Point& DragDrop::Drop::position() const { return _position; }
bool DragDrop::Drop::rButton() const { return (_keyState & MK_RBUTTON) != 0; }
bool DragDrop::Drop::shift() const { return (_keyState & MK_SHIFT) != 0; }



DragDrop::Feedback::Feedback(Control& sender, Effect effect)
	: Control::Event(sender), _effect(effect), _useDefaultCursor(true) {
}


void DragDrop::Feedback::cursor(HCURSOR value) {
	assert("Null cursor handle" && value);
	Mouse::cursor(value);
	_useDefaultCursor = false;
}


DragDrop::Effect DragDrop::Feedback::effect() const { return _effect; }



DragDrop::QueryContinue::QueryContinue(Control& sender, bool esc, int keyState)
	: Control::Event(sender), _esc(esc), _keyState(keyState), _cancelDrag(false), _drop(false) {
}


bool DragDrop::QueryContinue::alt() const { return (_keyState & MK_ALT) != 0; }
bool DragDrop::QueryContinue::cancelDrag() const { return _cancelDrag; }
void DragDrop::QueryContinue::cancelDrag(bool value) { _cancelDrag = value; _drop = false; }
bool DragDrop::QueryContinue::ctrl() const { return (_keyState & MK_CONTROL) != 0; }
bool DragDrop::QueryContinue::drop() const { return _drop; }
void DragDrop::QueryContinue::drop(bool value) { _drop = value; _cancelDrag = false; }
bool DragDrop::QueryContinue::esc() const { return _esc; }
bool DragDrop::QueryContinue::lButton() const { return (_keyState & MK_LBUTTON) != 0; }
bool DragDrop::QueryContinue::mButton() const { return (_keyState & MK_MBUTTON) != 0; }
bool DragDrop::QueryContinue::rButton() const { return (_keyState & MK_RBUTTON) != 0; }
bool DragDrop::QueryContinue::shift() const { return (_keyState & MK_SHIFT) != 0; }



DragDrop::Data::Data() : _dataObject(DataObject::create()) {
}


DragDrop::Data::Data(Data&& value) : _dataObject(move(value._dataObject)) {
}


DragDrop::Data::Data(HBITMAP bitmap) : _dataObject(DataObject::create()) {
	setBitmap(bitmap);
}


DragDrop::Data::Data(const Bitmap& bitmap) : _dataObject(DataObject::create()) {
	setBitmap(bitmap);
}


DragDrop::Data::Data(int memoryFormat, MemoryStream& stream) : _dataObject(DataObject::create()) {
	setMemory(memoryFormat, stream);
}


DragDrop::Data::Data(const String& text) : _dataObject(DataObject::create()) {
	setText(text);
}


DragDrop::Data::Data(const wchar_t* text) : _dataObject(DataObject::create()) {
	setText(text);
}


DragDrop::Data::Data(const wstring& text) : _dataObject(DataObject::create()) {
	setText(text);
}


DragDrop::Data::Data(::IDataObject* dataObject) {
	assert("Null dataObject" && dataObject);
	*(&_dataObject) = dataObject;
	_dataObject->AddRef();
}


DragDrop::Data::~Data() {
}


DragDrop::Data& DragDrop::Data::operator=(Data&& value) {
	if (this != &value) {
		_dataObject = move(value._dataObject);
	}
	return *this;
}


bool DragDrop::Data::containsBitmap() const {
	Formatetc formatetc(CF_BITMAP, TYMED_GDI);
	return _dataObject->QueryGetData(&formatetc) == S_OK;
}


bool DragDrop::Data::containsDIB() const {
	Formatetc formatetc(CF_DIB, TYMED_HGLOBAL);
	return _dataObject->QueryGetData(&formatetc) == S_OK;
}


bool DragDrop::Data::containsFileDropList() const {
	Formatetc formatetc(CF_HDROP, TYMED_HGLOBAL | TYMED_ISTREAM);
	return _dataObject->QueryGetData(&formatetc) == S_OK;
}


bool DragDrop::Data::containsMemory(int memoryFormat) const {
	assert("Invalid memoryFormat" && memoryFormat);
	Formatetc formatetc(static_cast<CLIPFORMAT>(memoryFormat), TYMED_HGLOBAL);
	return _dataObject->QueryGetData(&formatetc) == S_OK;
}


bool DragDrop::Data::containsText() const {
	Formatetc formatetc(CF_UNICODETEXT, TYMED_HGLOBAL | TYMED_ISTREAM);
	if (_dataObject->QueryGetData(&formatetc) == S_OK) {
		return true;
	}
	formatetc.cfFormat = CF_TEXT;
	return _dataObject->QueryGetData(&formatetc) == S_OK;
}


Bitmap DragDrop::Data::getBitmap() const {
	Formatetc formatetc(CF_BITMAP, TYMED_GDI);
	StgMedium medium;
	if (_dataObject->GetData(&formatetc, &medium) != S_OK) {
		return Bitmap();
	}
	Bitmap bitmap(medium.hBitmap, true);
	medium.tymed = TYMED_NULL; // 所有権の放棄
	return bitmap;
}


Bitmap DragDrop::Data::getDIB() const {
	Formatetc formatetc(CF_DIB, TYMED_HGLOBAL);
	StgMedium medium;
	if (_dataObject->GetData(&formatetc, &medium) != S_OK) {
		return Bitmap();
	}
	return Clipboard::_hgrobalToDIB(medium.hGlobal);
}


vector<String> DragDrop::Data::getFileDropList() const {
	Formatetc formatetc(CF_HDROP, TYMED_HGLOBAL | TYMED_ISTREAM);
	StgMedium medium;
	vector<String> fileDropList;
	if (_dataObject->GetData(&formatetc, &medium) != S_OK) {
		return fileDropList;
	}
	if (medium.tymed == TYMED_ISTREAM && medium.pstm) {
		STATSTG stat;
		stat.cbSize.QuadPart = 0;
		verify(SUCCEEDED(medium.pstm->Stat(&stat, STATFLAG_DEFAULT)));
		const int size = static_cast<int>(stat.cbSize.QuadPart);
		if (!size) {
			return fileDropList;
		}
		StgMedium newMedium;
		newMedium.tymed = TYMED_HGLOBAL;
		newMedium.hGlobal = allocateGlobal(size);
		{
			GlobalLocker lock(newMedium.hGlobal);
			verify(SUCCEEDED(medium.pstm->Read(lock.pointer(), size, nullptr)));
		}
		medium = newMedium;
	}
	if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
		return Clipboard::_hdropToFileDropList(medium.hGlobal);
	}
	return fileDropList;
}


MemoryStream DragDrop::Data::getMemory(int memoryFormat) const {
	assert("Invalid memoryFormat" && memoryFormat);
	Formatetc formatetc(static_cast<CLIPFORMAT>(memoryFormat), TYMED_HGLOBAL);
	StgMedium medium;
	MemoryStream stream;
	if (_dataObject->GetData(&formatetc, &medium) != S_OK) {
		return stream;
	}
	if (medium.tymed != TYMED_HGLOBAL || !medium.hGlobal) {
		return stream;
	}
	GlobalLocker buffer(medium.hGlobal);
	if (buffer.pointer()) { // サイズが０だと pointer() はヌルになる
		stream.write(buffer.pointer(), 0, GlobalSize(medium.hGlobal));
	}
	return stream;
}


String DragDrop::Data::getText() const {
	Formatetc formatetc(CF_UNICODETEXT, TYMED_HGLOBAL | TYMED_ISTREAM);
	StgMedium medium;
	if (_dataObject->GetData(&formatetc, &medium) == S_OK) {
		if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
			const int size = GlobalSize(medium.hGlobal);
			if (size < sizeof(wchar_t)) {
				return String();
			}
			GlobalLocker buffer(medium.hGlobal);
			return String(reinterpret_cast<wchar_t*>(buffer.pointer()), (size / sizeof(wchar_t)) - 1);
		}
		if (medium.tymed == TYMED_ISTREAM && medium.pstm) {
			STATSTG stat;
			stat.cbSize.QuadPart = 0;
			verify(SUCCEEDED(medium.pstm->Stat(&stat, STATFLAG_DEFAULT)));
			const int size = static_cast<int>(stat.cbSize.QuadPart);
			if (size < sizeof(wchar_t)) {
				return String();
			}
			StringBuffer buffer(size / sizeof(wchar_t));
			verify(SUCCEEDED(medium.pstm->Read(buffer.begin(), size, nullptr)));
			buffer.length((size / sizeof(wchar_t)) - 1);
			return move(buffer);
		}
	}
	formatetc.cfFormat = CF_TEXT;
	if (_dataObject->GetData(&formatetc, &medium) == S_OK) {
		if (medium.tymed == TYMED_HGLOBAL && medium.hGlobal) {
			const int size = GlobalSize(medium.hGlobal);
			if (size < sizeof(char)) {
				return String();
			}
			GlobalLocker buffer(medium.hGlobal);
			return Charset::default().decode(reinterpret_cast<char*>(buffer.pointer()));
		}
		if (medium.tymed == TYMED_ISTREAM && medium.pstm) {
			STATSTG stat;
			stat.cbSize.QuadPart = 0;
			verify(SUCCEEDED(medium.pstm->Stat(&stat, STATFLAG_DEFAULT)));
			const int size = static_cast<int>(stat.cbSize.QuadPart);
			if (size < sizeof(char)) {
				return String();
			}
			MemoryStream stream(size);
			verify(SUCCEEDED(medium.pstm->Read(stream.buffer(), size, nullptr)));
			return Charset::default().decode(static_cast<char*>(stream.buffer()));
		}
	}
	return String();
}


int DragDrop::Data::registerMemoryFormat(StringRange memoryFormatName) {
	return Clipboard::registerMemoryFormat(memoryFormatName);
}


void DragDrop::Data::setBitmap(HBITMAP value) {
	assert("Null bitmap" && value);

	Bitmap bitmap = Bitmap::toDDB(value);
	bitmap.owned(false);
	Formatetc formatetc(CF_BITMAP, TYMED_GDI);
	StgMedium medium;
	medium.tymed = TYMED_GDI;
	medium.hBitmap = bitmap;
	if (SUCCEEDED(_dataObject->SetData(&formatetc, &medium, TRUE))) {
		medium.tymed = TYMED_NULL; // 所有権の放棄
	} else {
		assert("Failed to IDataObjct::SetData" && false);
	}
}


void DragDrop::Data::setDIB(HBITMAP value) {
	assert("Null bitmap" && value);

	Formatetc formatetc(CF_DIB, TYMED_HGLOBAL);
	StgMedium medium;
	medium.tymed = TYMED_HGLOBAL;
	medium.hGlobal = Clipboard::_dibToHgrobal(value);
	if (SUCCEEDED(_dataObject->SetData(&formatetc, &medium, TRUE))) {
		medium.tymed = TYMED_NULL; // 所有権の放棄
	} else {
		assert("Failed to IDataObjct::SetData" && false);
	}
}


void DragDrop::Data::setFileDropList(StringRangeArray value) {
	Formatetc formatetc(CF_HDROP, TYMED_HGLOBAL);
	StgMedium medium;
	medium.tymed = TYMED_HGLOBAL;
	medium.hGlobal = Clipboard::_fileDropListToHglobal(value);
	if (SUCCEEDED(_dataObject->SetData(&formatetc, &medium, TRUE))) {
		medium.tymed = TYMED_NULL; // 所有権の放棄
	} else {
		assert("Failed to IDataObjct::SetData" && false);
	}
}


void DragDrop::Data::setMemory(int memoryFormat, Stream& stream) {
	assert("Invalid memoryFormat" && memoryFormat);
	assert("Can't read Stream" && stream.readable());
	Formatetc formatetc(static_cast<CLIPFORMAT>(memoryFormat), TYMED_HGLOBAL);
	StgMedium medium;
	medium.tymed = TYMED_HGLOBAL;
	medium.hGlobal = allocateGlobal(static_cast<int>(stream.length()));
	{
		GlobalLocker buffer(medium.hGlobal);
		if (buffer.pointer()) { // サイズが０だと pointer() はヌルになる
			stream.read(buffer.pointer(), 0, static_cast<int>(stream.length()));
		}
	}
	if (SUCCEEDED(_dataObject->SetData(&formatetc, &medium, TRUE))) {
		medium.tymed = TYMED_NULL; // 所有権の放棄
	} else {
		assert("Failed to IDataObjct::SetData" && false);
	}
}


void DragDrop::Data::setText(StringRange value) {
	const int size = (value.length() + 1);
	Formatetc formatetc(CF_UNICODETEXT, TYMED_HGLOBAL);
	StgMedium medium;
	medium.tymed = TYMED_HGLOBAL;
	medium.hGlobal = allocateGlobal(size * sizeof(wchar_t));
	{
		GlobalLocker buffer(medium.hGlobal);
		String::refer(value).copyTo(reinterpret_cast<wchar_t*>(buffer.pointer()), size);
	}
	verify(SUCCEEDED(_dataObject->SetData(&formatetc, &medium, TRUE)));
	medium.tymed = TYMED_NULL; // 所有権の放棄
}



/// ドラッグドロップを開始する COM インターフェース
class DragDrop::DropSource : public ComBase<IDropSource> {
	DropSource(Control& control) : _owner(&control), _dragImage(false) {
	}
	virtual ~DropSource() {
	}

public:
	static ComPtr<DropSource> create(Control& control) {
		ComPtr<DropSource> ptr;
		*(&ptr) = new DropSource(control);
		return ptr;
	}

public:
	virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) {
		DragDrop::Feedback event(*_owner, static_cast<DragDrop::Effect>(effect));
		_onFeedback(event);
		return event._useDefaultCursor ? DRAGDROP_S_USEDEFAULTCURSORS : S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL esc, DWORD keyState) {
		if (_dragImage) {
			auto position = Mouse::position();
			verify(ImageList_DragMove(position.x, position.y));
		}
		DragDrop::QueryContinue event(*_owner, esc != FALSE ? true : false, keyState);
		if (esc || toFlag(keyState)[MK_LBUTTON | MK_RBUTTON]) { // ESC キーを押したか左右マウスボタン同時押しでキャンセル
			event.cancelDrag(true);
		} else {
			// マウスボタンが離されたときはドロップ
			event.drop(!(keyState & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)));
		}
		_onQueryContinue(event);
		if (event.cancelDrag()) {
			return DRAGDROP_S_CANCEL;
		} else if (event.drop()) {
			return DRAGDROP_S_DROP;
		} else {
			return S_OK;
		}
	}

public:
	Reference<Control> _owner;
	bool _dragImage;
	Listener<DragDrop::Feedback&> _onFeedback;
	Listener<DragDrop::QueryContinue&> _onQueryContinue;
};



DragDrop::Source::Source() {
}


DragDrop::Source::Source(Source&& value) : _dropSource(move(value._dropSource)) {
}


DragDrop::Source::Source(Control& control) : _dropSource(DropSource::create(control)) {
}


DragDrop::Source::~Source() {
}


DragDrop::Source& DragDrop::Source::operator=(Source&& value) {
	if (this != &value) {
		_dropSource = move(value._dropSource);
	}
	return *this;
}


DragDrop::Effect DragDrop::Source::doDragDrop(const DragDrop::Data& data, DragDrop::Effect allowedEffects
	, HIMAGELIST imageList, int imageIndex, int xHotSpot, int yHotSpot) {
	assert("DragDrop::Source not initialized" && _dropSource);
	assert("Invalid DragDropEffect" && Effect::_validate(allowedEffects));

	if (imageList) {
		verify(ImageList_BeginDrag(imageList, imageIndex, xHotSpot, yHotSpot));
		auto position = Mouse::position();
		verify(ImageList_DragEnter(GetDesktopWindow(), position.x, position.y));
		_dropSource->_dragImage = true;
	}

	DWORD effect = Effect::none;
	auto result = DoDragDrop(data._dataObject, _dropSource, allowedEffects, &effect);
	if (result == E_OUTOFMEMORY || result == CO_E_NOTINITIALIZED) {
		if (Com::isMainThread() && !Com::oleInitialized()) {
			Com::oleInitialize();
			result = DoDragDrop(data._dataObject, _dropSource, allowedEffects, &effect);
		}
	}
	assert("Failed to DoDragDrop" && SUCCEEDED(result));

	if (_dropSource->_dragImage) {
		verify(ImageList_DragLeave(GetDesktopWindow()));
		ImageList_EndDrag();
		_dropSource->_dragImage = false;
	}

	return static_cast<Effect>(effect);
}


Listener<DragDrop::Feedback&>& DragDrop::Source::onFeedback() {
	assert("DragDrop::Source not initialized" && _dropSource);
	return _dropSource->_onFeedback;
}


Listener<DragDrop::QueryContinue&>& DragDrop::Source::onQueryContinue() {
	assert("DragDrop::Source not initialized" && _dropSource);
	return _dropSource->_onQueryContinue;
}



/// ドラッグドロップを受け取る COM インターフェース
class DragDrop::DropTarget : public ComBase<IDropTarget> {
	DropTarget(Control& control) : _owner(&control) {
	}
	virtual ~DropTarget() {
	}

public:
	static ComPtr<DropTarget> create(Control& control) {
		ComPtr<DropTarget> ptr;
		*(&ptr) = new DropTarget(control);
		return ptr;
	}

	/// エクスプローラの挙動をまねて操作を決定する
	DragDrop::Effect getEffect(DWORD keyState, DWORD allowedEffects) {
		switch (keyState & (MK_ALT | MK_CONTROL | MK_SHIFT)) {
			case MK_CONTROL | MK_SHIFT :
			case MK_ALT : return DragDrop::Effect::link & static_cast<DragDrop::Effect>(allowedEffects);
			case MK_CONTROL : return DragDrop::Effect::copy & static_cast<DragDrop::Effect>(allowedEffects);
			default : return DragDrop::Effect::move & static_cast<DragDrop::Effect>(allowedEffects);
		}
	}

public:
	virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* dataObject, DWORD keyState, POINTL position, DWORD* allowedEffects) {
		_lastData = DragDrop::Data(dataObject);
		DragDrop::Enter event(*_owner, _lastData, static_cast<DragDrop::Effect>(*allowedEffects), keyState, Point(position.x, position.y));
		event.effect(getEffect(keyState, *allowedEffects));
		_onEnter(event);
		*allowedEffects = event.effect();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD keyState, POINTL position, DWORD* allowedEffects) {
		DragDrop::Move event(*_owner, _lastData, static_cast<DragDrop::Effect>(*allowedEffects), keyState, Point(position.x, position.y));
		event.effect(getEffect(keyState, *allowedEffects));
		_onMove(event);
		*allowedEffects = event.effect();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DragLeave() {
		DragDrop::Leave event(*_owner);
		_onLeave(event);
		_lastData = DragDrop::Data();
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject* dataObject, DWORD keyState, POINTL position, DWORD* allowedEffects) {
		DragDrop::Drop event(*_owner, DragDrop::Data(dataObject), static_cast<DragDrop::Effect>(*allowedEffects), keyState, Point(position.x, position.y));
		event.effect(getEffect(keyState, *allowedEffects));
		_onDrop(event);
		*allowedEffects = event.effect();
		_lastData = DragDrop::Data();
		return S_OK;
	}

public:
	Reference<Control> _owner;
	DragDrop::Data _lastData;
	Listener<DragDrop::Drop&> _onDrop;
	Listener<DragDrop::Enter&> _onEnter;
	Listener<DragDrop::Leave&> _onLeave;
	Listener<DragDrop::Move&> _onMove;
};



DragDrop::Target::Target() {
}


DragDrop::Target::Target(Target&& value) : _dropTarget(move(value._dropTarget)) {
}


#pragma warning (push)
#pragma warning (disable : 4189) //  'result' : ローカル変数が初期化されましたが、参照されていません
DragDrop::Target::Target(Control& control) : _dropTarget(DropTarget::create(control)) {
	auto result = RegisterDragDrop(control, _dropTarget);
	if (result == E_OUTOFMEMORY || result == CO_E_NOTINITIALIZED) {
		if (Com::isMainThread() && !Com::oleInitialized()) {
			Com::oleInitialize();
			result = RegisterDragDrop(control, _dropTarget);
		}
	}
	assert("control not created" && result != DRAGDROP_E_INVALIDHWND);
	assert("DragDrop::Target control already registerd" && result != DRAGDROP_E_ALREADYREGISTERED);
	assert("Failed to RegisterDragDrop" && SUCCEEDED(result));
}
#pragma warning (pop)


DragDrop::Target::~Target() {
	assert("owner window already destroyed. memory leak!" && (!_dropTarget || (_dropTarget->_owner && IsWindow(*_dropTarget->_owner))));
	if (_dropTarget && _dropTarget->_owner) {
		verify(SUCCEEDED(RevokeDragDrop(*_dropTarget->_owner)));
	}
}


DragDrop::Target& DragDrop::Target::operator=(Target&& value) {
	if (this != &value) {
		_dropTarget = move(value._dropTarget);
	}
	return *this;
}


Listener<DragDrop::Drop&>& DragDrop::Target::onDrop() {
	assert("DragDrop::Target not initialized" && _dropTarget);
	return _dropTarget->_onDrop;
}


Listener<DragDrop::Enter&>& DragDrop::Target::onEnter() {
	assert("DragDrop::Target not initialized" && _dropTarget);
	return _dropTarget->_onEnter;
}


Listener<DragDrop::Leave&>& DragDrop::Target::onLeave() {
	assert("DragDrop::Target not initialized" && _dropTarget);
	return _dropTarget->_onLeave;
}


Listener<DragDrop::Move&>& DragDrop::Target::onMove() {
	assert("DragDrop::Target not initialized" && _dropTarget);
	return _dropTarget->_onMove;
}


Size DragDrop::defaultDragSize() {
	return Size(GetSystemMetrics(SM_CXDRAG), GetSystemMetrics(SM_CYDRAG));
}



	}
}