#include "Clipboard.hpp"

#include <vector>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>

#include <ShellAPI.h>
#include <ShlObj.h>


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::graphics;
using namespace balor::io;



namespace {
struct ClipboardOpener {
	ClipboardOpener() {
		if (!OpenClipboard(nullptr)) {
			//const DWORD errorCode = GetLastError();
			//if (errorCode == ERROR_ACCESS_DENIED) {
			//	throw Clipboard::AccessDeniedException(); // 握りつぶさないと爆弾になるか？
			//}
			assert(L"Failed to OpenClipboard" && false);
		}
	}
	~ClipboardOpener() {
		verify(CloseClipboard());
	}
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
	auto const handle = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!handle) {
		if (GetLastError() == ERROR_NOT_ENOUGH_MEMORY) {
			throw Clipboard::OutOfMemoryException();
		}
		assert("Failed to GlobalAlloc" && false);
	}
	return handle;
}
} // namespace



void Clipboard::clear() {
	ClipboardOpener opener;
	verify(EmptyClipboard());
}


bool Clipboard::containsBitmap() {
	return IsClipboardFormatAvailable(CF_BITMAP) != 0;
}


bool Clipboard::containsDIB() {
	UINT formats[] = {CF_DIB, CF_DIBV5};
	const int cf = GetPriorityClipboardFormat(formats, sizeof(formats) / sizeof(formats[0]));
	return 0 < cf;
}


bool Clipboard::containsFileDropList() {
	return IsClipboardFormatAvailable(CF_HDROP) != 0;
}


bool Clipboard::containsMemory(int memoryFormat) {
	return IsClipboardFormatAvailable(memoryFormat) != 0;
}


bool Clipboard::containsText() {
	return IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
}


Bitmap Clipboard::getBitmap() {
	if (!containsBitmap()) {
		return Bitmap();
	}
	ClipboardOpener opener;
	HBITMAP const handle = static_cast<HBITMAP>(GetClipboardData(CF_BITMAP));
	if (!handle) {
		return Bitmap();
	}
	return Bitmap::clone(handle);
}


Bitmap Clipboard::getDIB() {
	UINT formats[] = {CF_DIBV5, CF_DIB, CF_BITMAP};
	const int cf = GetPriorityClipboardFormat(formats, sizeof(formats) / sizeof(formats[0]));
	if (cf <= 0 || cf == CF_BITMAP) {
		return Bitmap();
	}
	ClipboardOpener opener;
	HGLOBAL const global = static_cast<HGLOBAL>(GetClipboardData(CF_DIB)); // CF_DIBV5 でなくともアルファチャンネルは取り出せるので CF_DIB だけで良い
	return _hgrobalToDIB(global);
}


vector<String> Clipboard::getFileDropList() {
	vector<String> list;
	if (!containsFileDropList()) {
		return vector<String>();
	}
	ClipboardOpener opener;
	return _hdropToFileDropList(GetClipboardData(CF_HDROP));
}


MemoryStream Clipboard::getMemory(int memoryFormat) {
	assert("Invalid memoryFormat" && memoryFormat);
	MemoryStream stream;
	if (!containsMemory(memoryFormat)) {
		return stream;
	}
	ClipboardOpener opener;
	HGLOBAL const global = GetClipboardData(memoryFormat);
	if (!global) {
		return stream;
	}
	GlobalLocker lock(global);
	if (lock.pointer()) { // サイズが０だと pointer() はヌルになる
		stream.write(lock.pointer(), 0, GlobalSize(global));
		stream.position(0);
	}
	return stream;
}


String Clipboard::getText() {
	if (!containsText()) {
		return String();
	}
	ClipboardOpener opener;
	HGLOBAL const global = GetClipboardData(CF_UNICODETEXT);
	if (!global) {
		return String();
	}
	const auto size = GlobalSize(global);
	if (size < sizeof(wchar_t)) {
		return String();
	}
	GlobalLocker lock(global);
	return String(reinterpret_cast<const wchar_t*>(lock.pointer()), size / sizeof(wchar_t) - 1);
}


int Clipboard::registerMemoryFormat(StringRange memoryFormatName) {
	assert("Empty memoryFormatName" && !memoryFormatName.empty());
	auto format = RegisterClipboardFormatW(memoryFormatName.c_str());
	assert("Failed to RegisterClipboardFormatW" && format);
	return format;
}


void Clipboard::setBitmap(HBITMAP value) {
	assert("Null bitmap" && value);
	Bitmap bitmap = Bitmap::toDDB(value);
	ClipboardOpener opener;
	verify(EmptyClipboard());
	if (SetClipboardData(CF_BITMAP, bitmap)) {
		bitmap.owned(false); // クリップボードに所有権が移ったのでハンドルの所有権を放棄
	} else {
		assert("Failed to SetClipboardData" && false);
	}
}


void Clipboard::setDIB(HBITMAP value) {
	assert("Null bitmap" && value);
	HGLOBAL global = _dibToHgrobal(value);
	ClipboardOpener opener;
	verify(EmptyClipboard());
	if (!SetClipboardData(CF_DIB, global)) {
		verify(!GlobalFree(global));
		assert("Failed to SetClipboardData" && false);
	}
}


void Clipboard::setFileDropList(StringRangeArray value) {
	HGLOBAL global = _fileDropListToHglobal(value);
	ClipboardOpener opener;
	verify(EmptyClipboard());
	if (!SetClipboardData(CF_HDROP, global)) {
		verify(!GlobalFree(global));
		assert("Failed to SetClipboardData" && false);
	}
}


void Clipboard::setMemory(int memoryFormat, Stream& stream) {
	assert("Invalid memoryFormat" && memoryFormat);
	assert("Can't read Stream" && stream.readable());
	HGLOBAL const global = allocateGlobal(static_cast<int>(stream.length()));
	{
		GlobalLocker lock(global);
		if (lock.pointer()) { // サイズが０だと pointer() はヌルになる
			stream.read(lock.pointer(), 0, static_cast<int>(stream.length()));
		}
	}
	ClipboardOpener opener;
	verify(EmptyClipboard());
	if (!SetClipboardData(memoryFormat, global)) {
		verify(!GlobalFree(global));
		assert("Failed to SetClipboardData" && false);
	}
}


void Clipboard::setText(StringRange text) {
	const auto size = (text.length() + 1) * sizeof(wchar_t);
	HGLOBAL const global = allocateGlobal(size);
	{
		GlobalLocker lock(global);
		wchar_t* const pointer = reinterpret_cast<wchar_t*>(lock.pointer());
		String::refer(text).copyTo(pointer, text.length() + 1);
	}
	ClipboardOpener opener;
	verify(EmptyClipboard());
	if (!SetClipboardData(CF_UNICODETEXT, global)) {
		verify(!GlobalFree(global));
		assert("Failed to SetClipboardData" && false);
	}
}


void* Clipboard::_dibToHgrobal(HBITMAP bitmap) {
	Bitmap temp(bitmap);
	if (temp.isDDB()) {
		temp = temp.toDIB();
	}
	DIBSECTION dib;
	verify(GetObjectW(temp, sizeof(dib), &dib));
	const bool bottomUp = temp.bottomUp();
	const int imageSize = dib.dsBm.bmWidthBytes * dib.dsBm.bmHeight;
	int paletteCount = 0;
	if (dib.dsBm.bmBitsPixel <= 8) {
		paletteCount = 1 << dib.dsBm.bmBitsPixel;
	} else if (dib.dsBmih.biCompression == BI_BITFIELDS) {
		paletteCount = 3;
	}
	const int offset = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * paletteCount;
	HGLOBAL const global = allocateGlobal(offset + imageSize);
	{
		GlobalLocker lock(global);
		BITMAPINFOHEADER& header = *reinterpret_cast<BITMAPINFOHEADER*>(lock.pointer());
		ZeroMemory(&header, sizeof(header));
		header.biSize = sizeof(header);
		header.biWidth  = dib.dsBm.bmWidth;
		header.biHeight = bottomUp ? dib.dsBm.bmHeight : -dib.dsBm.bmHeight;
		header.biPlanes = 1;
		header.biBitCount = static_cast<WORD>(dib.dsBm.bmBitsPixel);
		header.biCompression = dib.dsBmih.biCompression;
		header.biSizeImage = imageSize;
		header.biXPelsPerMeter = dib.dsBmih.biXPelsPerMeter;
		header.biYPelsPerMeter = dib.dsBmih.biYPelsPerMeter;
		header.biClrUsed = 0;
		header.biClrImportant = dib.dsBmih.biClrImportant;
		if (paletteCount) {
			if (dib.dsBm.bmBitsPixel <= 8) {
				RGBQUAD* const palette = reinterpret_cast<RGBQUAD*>(lock.pointer() + header.biSize);
				verify(0 < GetDIBColorTable(Graphics(bitmap), 0, paletteCount, palette));
			} else {
				DWORD* const masks = reinterpret_cast<DWORD*>(lock.pointer() + header.biSize);
				masks[0] = dib.dsBitfields[0];
				masks[1] = dib.dsBitfields[1];
				masks[2] = dib.dsBitfields[2];
			}
		}
		memcpy(lock.pointer() + offset, dib.dsBm.bmBits, imageSize);
	}
	return global;
}


void* Clipboard::_fileDropListToHglobal(StringRangeArray fileDropList) {
	auto size = sizeof(DROPFILES) + sizeof(wchar_t);
	for (auto i = 0, end = fileDropList.length(); i != end; ++i) {
		size += (fileDropList[i].length() + 1) * sizeof(wchar_t);
	}
	HGLOBAL const global = allocateGlobal(size);
	{
		GlobalLocker lock(global);
		DROPFILES* dropFiles = reinterpret_cast<DROPFILES*>(lock.pointer());
		dropFiles->pFiles = sizeof(DROPFILES);
		dropFiles->pt.x = 0;
		dropFiles->pt.y = 0;
		dropFiles->fNC = FALSE;
		dropFiles->fWide = TRUE;
		wchar_t* fileNames = reinterpret_cast<wchar_t*>(lock.pointer() + sizeof(DROPFILES));
		for (auto i = 0, end = fileDropList.length(); i != end; ++i) {
			auto file = fileDropList[i];
			auto length = file.length();
			String::refer(file.c_str(), length).copyTo(fileNames, length + 1);
			fileNames += length + 1;
		}
		*fileNames = L'\0';
	}
	return global;
}


vector<String> Clipboard::_hdropToFileDropList(void* drop) {
	vector<String> list;
	const UINT count = DragQueryFileW(static_cast<HDROP>(drop), 0xFFFFFFFF, nullptr, 0);
	for (UINT i = 0; i < count; ++i) {
		wchar_t buffer[MAX_PATH];
		buffer[0] = 0;
		DragQueryFileW(static_cast<HDROP>(drop), i, buffer, MAX_PATH);
		list.push_back(buffer);
	}
	return list;
}


Bitmap Clipboard::_hgrobalToDIB(void* global) {
	if (!global) {
		return Bitmap();
	}
	GlobalLocker lock(global);
	const BITMAPINFOHEADER& header = *reinterpret_cast<BITMAPINFOHEADER*>(lock.pointer());
	int paletteCount = 0;
	if (header.biClrUsed) {
		paletteCount = header.biClrUsed;
	} else if (header.biBitCount <= 8) {
		paletteCount = 1 << header.biBitCount;
	}
	Bitmap::Format format(header.biBitCount);
	switch (header.biCompression) {
		case BI_BITFIELDS : {
			const DWORD* const masks = reinterpret_cast<DWORD*>(lock.pointer() + header.biSize);
			format.rMask = masks[0];
			format.gMask = masks[1];
			format.bMask = masks[2];
			if (format.bitsPerPixel == 32) {
				format.aMask = ~(format.rMask | format.gMask | format.bMask);
			}
			paletteCount += 3; // BI_BITFIELDS と 私的パレットが同時の場合は加算で良いのだろうか？
		} break;
		case BI_RGB : {
			switch (format.bitsPerPixel) {
				case 16 : format = Bitmap::Format::rgb16bpp; break;
				case 32 : format = Bitmap::Format::argb32bpp; break;
			}
		} break;
		default : { // 圧縮形式は未対応
			return Bitmap();
		} break;
	}
	const bool bottomUp = 0 <= header.biHeight;
	const int offset = header.biSize + sizeof(RGBQUAD) * paletteCount;
	const int stride = ((((header.biWidth * header.biBitCount + 7) / 8) + 3) / 4) * 4;
	Bitmap bitmap(header.biWidth, abs(header.biHeight), format, bottomUp, lock.pointer() + offset, stride, bottomUp);
	if (header.biBitCount <= 8) {
		const RGBQUAD* const palette = reinterpret_cast<RGBQUAD*>(lock.pointer() + header.biSize);
		verify(SetDIBColorTable(Graphics(bitmap), 0, paletteCount, palette));
	}
	return bitmap;
}



	}
}