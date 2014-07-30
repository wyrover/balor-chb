#include "Cursor.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Size.hpp>


namespace balor {
	namespace graphics {

using std::move;
using std::swap;
using namespace balor::io;



namespace {
struct IconInfo : public ICONINFO {
	IconInfo(HICON handle) {
		verify(GetIconInfo(handle, this));
	}
	~IconInfo() {
		if (hbmColor) {
			verify(DeleteObject(hbmColor));
		}
		if (hbmMask) {
			verify(DeleteObject(hbmMask));
		}
	}
};


Cursor fromSystemResource(int id) {
	auto _handle = static_cast<HICON>(LoadImageW(nullptr, MAKEINTRESOURCEW(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	assert(L"Failed to LoadImageW" && _handle);
	return Cursor(_handle);
}
} // namespace


Cursor::Cursor() : _handle(nullptr), _owned(false) {
}


Cursor::Cursor(Cursor&& cursor) : _handle(cursor._handle), _owned(cursor._owned) {
	cursor._handle = nullptr;
	cursor._owned = false;
}


Cursor::Cursor(HCURSOR handle, bool owned) : _handle(handle), _owned(owned) {
}


Cursor::Cursor(HBITMAP bitmap, HBITMAP mask, int xHotSpot, int yHotSpot) : _owned(true) {
	assert(L"Null bitmap handle" && bitmap);
	assert(L"Null mask bitmap handle" && mask);
	ICONINFO info;
	info.fIcon = FALSE;
	info.hbmColor = bitmap;
	info.hbmMask  = mask;
	info.xHotspot = xHotSpot;
	info.yHotspot = yHotSpot;
	_handle = CreateIconIndirect(&info);
	assert("Faied to CreateIconIndirect" && _handle);
}


Cursor::Cursor(HBITMAP bitmap, const Color& colorKey, int xHotSpot, int yHotSpot) : _handle(nullptr), _owned(true) {
	assert(L"Null bitmap handle" && bitmap);
	Bitmap temp(bitmap);
	ImageList list(temp.size(), ImageList::getFormatFromBitsPerPixel(temp.bitsPerPixel()));
	list.add(temp, colorKey);
	IconInfo info(list.getIcon(0));
	info.fIcon = FALSE;
	info.xHotspot = xHotSpot;
	info.yHotspot = yHotSpot;
	_handle = CreateIconIndirect(&info);
	assert("Faied to CreateIconIndirect" && _handle);
}


Cursor::Cursor(StringRange filePath): _handle(nullptr), _owned(false) {
	assert("Empty filePath" && !filePath.empty());

	if (!File::exists(filePath)) {
		throw NotFoundException();
	}
	const HINSTANCE instance = GetModuleHandleW(nullptr);
	assert(instance);
	_handle = static_cast<HCURSOR>(LoadImageW(instance, filePath.c_str(), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE));
	if (!_handle) { // FileFormatException を検出したいが GetLastError は 0 が帰るようだ
		throw LoadFailedException();
	}
	_owned = true;
}


Cursor::~Cursor() {
	if (_handle && _owned) {
		verify(DestroyCursor(_handle));
	}
	//_owned = false;
	//_handle = nullptr;
}


Cursor& Cursor::operator=(Cursor&& cursor) {
	swap(_handle, cursor._handle);
	swap(_owned, cursor._owned);
	return *this;
}


Bitmap Cursor::bitmap() const {
	assert("Null Cursor handle" && *this);
	IconInfo info(*this);
	Bitmap bitmap(info.hbmColor, true);
	info.hbmColor = nullptr;
	return bitmap;
}


Cursor Cursor::clone() const {
	if (!*this) {
		return Cursor();
	}
	return clone(*this);
}


Cursor Cursor::clone(HCURSOR handle) {
	assert("Null handle" && handle);
	Cursor cursor(static_cast<HCURSOR>(CopyImage(handle, IMAGE_CURSOR, 0, 0, 0)), true);
	assert("Failed to CopyImage" && cursor);
	return cursor;
}


Size Cursor::defaultSize() {
	return Size(GetSystemMetrics(SM_CXCURSOR), GetSystemMetrics(SM_CYCURSOR));
}


Point Cursor::hotSpot() {
	assert("Null Cursor handle" && *this);
	IconInfo info(*this);
	return Point(info.xHotspot, info.yHotspot);
}


Bitmap Cursor::mask() const {
	assert("Null Cursor handle" && *this);
	IconInfo info(*this);
	Bitmap bitmap(info.hbmMask, true);
	info.hbmMask = nullptr;
	return bitmap;
}


bool Cursor::owned() const {
	return _owned;
}


void Cursor::owned(bool value) {
	_owned = value;
}


void Cursor::save(Stream&& stream) const {
	save(stream);
}


void Cursor::save(Stream& stream) const {
	save(ArrayRange<const Cursor>(this, 1), stream);
}


void Cursor::save(StringRange filePath) const {
	save(ArrayRange<const Cursor>(this, 1), filePath);
}


void Cursor::save(ArrayRange<const Cursor> cursors, Stream&& stream) {
	save(cursors, stream);
}


void Cursor::save(ArrayRange<const Cursor> cursors, Stream& stream) {
	assert("Empty cursors" && !cursors.empty());
	assert("Can't write stream" && stream.writable());
	const Icon* const begin = reinterpret_cast<const Icon*>(cursors.begin()); // やだ超強引！メンバ拡張は要注意
	Icon::save(ArrayRange<const Icon>(begin, cursors.size()), stream);
}


void Cursor::save(ArrayRange<const Cursor> cursors, StringRange filePath) {
	FileStream stream(filePath, FileStream::Mode::create, FileStream::Access::write);
	save(cursors, stream);
}


Size Cursor::size() {
	assert("Null Cursor handle" && *this);
	IconInfo info(*this);
	return Bitmap(info.hbmColor).size();
}


// Control クラス内においてマウス移動のたびに呼ばれる可能性があるのでキャッシュする。
// キャッシュはシステム共有リソースなので重複初期化されても DLL ごとに実体をもっても問題ない
Cursor Cursor::appStarting() { static HCURSOR cache = fromSystemResource(OCR_APPSTARTING); return Cursor(cache); }
Cursor Cursor::default()     { static HCURSOR cache = fromSystemResource(OCR_NORMAL     ); return Cursor(cache); }
Cursor Cursor::cross()       { static HCURSOR cache = fromSystemResource(OCR_CROSS      ); return Cursor(cache); }
Cursor Cursor::hand()        { static HCURSOR cache = fromSystemResource(OCR_HAND       ); return Cursor(cache); }
Cursor Cursor::help()        { static HCURSOR cache = fromSystemResource(32651          ); return Cursor(cache); }
Cursor Cursor::iBeam()       { static HCURSOR cache = fromSystemResource(OCR_IBEAM      ); return Cursor(cache); }
Cursor Cursor::no()          { static HCURSOR cache = fromSystemResource(OCR_NO         ); return Cursor(cache); }
Cursor Cursor::sizeAll()     { static HCURSOR cache = fromSystemResource(OCR_SIZEALL    ); return Cursor(cache); }
Cursor Cursor::sizeNESW()    { static HCURSOR cache = fromSystemResource(OCR_SIZENESW   ); return Cursor(cache); }
Cursor Cursor::sizeNS()      { static HCURSOR cache = fromSystemResource(OCR_SIZENS     ); return Cursor(cache); }
Cursor Cursor::sizeNWSE()    { static HCURSOR cache = fromSystemResource(OCR_SIZENWSE   ); return Cursor(cache); }
Cursor Cursor::sizeWE()      { static HCURSOR cache = fromSystemResource(OCR_SIZEWE     ); return Cursor(cache); }
Cursor Cursor::upAllow()     { static HCURSOR cache = fromSystemResource(OCR_UP         ); return Cursor(cache); }
Cursor Cursor::wait()        { static HCURSOR cache = fromSystemResource(OCR_WAIT       ); return Cursor(cache); }



	}
}