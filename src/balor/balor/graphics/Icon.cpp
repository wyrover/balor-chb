#include "Icon.hpp"

#include <utility>
#include <olectl.h>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/ImageList.hpp>
#include <balor/io/File.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>
#include <balor/String.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")


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


Icon fromSystemResource(int id, const Size& size) {
	HICON handle = static_cast<HICON>(LoadImageW(nullptr, MAKEINTRESOURCEW(id), IMAGE_ICON, size.width, size.height, LR_SHARED));
	assert(L"Failed to LoadImageW" && handle);
	return Icon(handle);
}
}



Icon::Icon() : _handle(nullptr), _owned(false) {
}


Icon::Icon(Icon&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Icon::Icon(HICON handle, bool owned) : _handle(handle), _owned(owned) {
}


Icon::Icon(HBITMAP bitmap, HBITMAP mask) : _owned(true) {
	assert(L"Null bitmap handle" && bitmap);
	assert(L"Null mask bitmap handle" && mask);
	ICONINFO info;
	info.fIcon = TRUE;
	info.hbmColor = bitmap;
	info.hbmMask  = mask;
	info.xHotspot = 0;
	info.yHotspot = 0;
	_handle = CreateIconIndirect(&info);
	assert("Faied to CreateIconIndirect" && _handle);
}


Icon::Icon(HBITMAP bitmap, const Color& colorKey) : _handle(nullptr), _owned(true) {
	assert(L"Null bitmap handle" && bitmap);
	Bitmap temp(bitmap);
	ImageList list(temp.size(), ImageList::getFormatFromBitsPerPixel(temp.bitsPerPixel()));
	list.add(temp, colorKey);
	*this = list.getIcon(0);
}


//// どうにもおかしい。LoadImage関数を使った場合と結果が違う。
//Icon::Icon(Stream& stream) : _handle(nullptr), _owned(false) {
//	assert("stream read unsupported" && stream.canRead());
//
//	{
//		StreamBridge bridge(stream);
//		IPicture* picture = nullptr;
//		const HRESULT result = OleLoadPicture(&bridge, 0, FALSE, IID_IPicture, reinterpret_cast<LPVOID*>(&picture));
//		if (!FAILED(result)) {
//			auto releaser = scopeExit([&] () { picture->Release(); });
//			SHORT type = 0;
//			verify(SUCCEEDED(picture->get_Type(&type)));
//			if (type != PICTYPE_ICON) {
//				throw FileFormatException(L"failed to load Icon");
//			}
//			HICON icon = nullptr;
//			verify(SUCCEEDED(picture->get_Handle(reinterpret_cast<OLE_HANDLE*>(&icon))));// このハンドルはpictureが破棄される時に一緒に破棄されるからコピーが必要（メモリ消費二倍）
//			*this = Icon(icon);
//		}
//		if (result == E_OUTOFMEMORY) {
//			throw OutOfMemoryException(L"failed to OleLoadPicture");
//		}
//	}
//}


Icon::Icon(StringRange filePath, const Size& size) : _handle(nullptr), _owned(false) {
	assert("Empty filePath" && !filePath.empty());

	if (!File::exists(filePath)) {
		throw NotFoundException();
	}

	const HINSTANCE instance = GetModuleHandleW(nullptr);
	assert(instance);
	/// サイズにもっとも近いアイコンを取ってさらに拡縮する
	_handle = static_cast<HICON>(LoadImageW(instance, filePath.c_str(), IMAGE_ICON, size.width, size.height, LR_LOADFROMFILE));
	if (!_handle) { // FileFormatException を検出したいが GetLastError は 0 が返るようだ。
		throw LoadFailedException();
	}
	_owned = true;
}


Icon::Icon(StringRange filePath, int width, int height) : _handle(nullptr), _owned(false) {
	*this = Icon(filePath, Size(width, height));
}


Icon::~Icon() {
	if (_handle && _owned) {
		verify(DestroyIcon(_handle));
	}
	//_owned = false;
	//_handle = nullptr;
}


Icon& Icon::operator=(Icon&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


Bitmap Icon::bitmap() const {
	assert("Null Icon handle" && *this);
	IconInfo info(*this);
	Bitmap bitmap(info.hbmColor, true);
	info.hbmColor = nullptr;
	return bitmap;
}


Icon Icon::clone() const {
	if (!*this) {
		return Icon();
	}
	return clone(_handle);
}


Icon Icon::clone(HICON handle) {
	assert("Null Icon handle" && handle);
	Icon icon(static_cast<HICON>(CopyImage(handle, IMAGE_ICON, 0, 0, 0)), true);
	assert("failed to CopyImage" && icon);
	return icon;
}


Size Icon::largeSize() {
	return Size(GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
}


Bitmap Icon::mask() const {
	assert("Null Icon handle" && *this);
	IconInfo info(*this);
	Bitmap bitmap(info.hbmMask, true);
	info.hbmMask = nullptr;
	return bitmap;
}


bool Icon::owned() const {
	return _owned;
}


void Icon::owned(bool value) {
	_owned = value;
}


void Icon::save(Stream&& stream) const {
	save(stream);
}


void Icon::save(Stream& stream) const {
	save(ArrayRange<const Icon>(this, 1), stream);
}


void Icon::save(StringRange filePath) const {
	save(ArrayRange<const Icon>(this, 1), filePath);
}


void Icon::save(ArrayRange<const Icon> icons, Stream&& stream) {
	save(icons, stream);
}


void Icon::save(ArrayRange<const Icon> icons, Stream& stream) {
	assert("Empty icons" && !icons.empty());
	assert("Can't write stream" && stream.writable());

	// アイコン（カーソル）が n 個含まれるアイコン（カーソル）ファイルの構造は以下の通り。 
	//
	// IconDir + IconDirEntry * n + DIB ビットマップデータ * n
	// 
	// DIB ビットマップデータは BITMAPINFOHEADER から始まって RGBQUAD のパレットテーブル、ビクセルデータと続く
	// 一般的な DIB データ構造だが、BITMAPINFOHEADER のメンバは biSize, biWidth, biHeight, biPlanes, biBitCount, biSizeImage 以外全て０であり、
	// またピクセルデータの後に同じ解像度を持ったマスク用の１ビットのモノクロ画像のピクセルデータがくっついている。
	// この為 BITMAPINFOHEADER の biHeight メンバは二倍したものになっている。
	// http://msdn.microsoft.com/en-us/library/ms997538.aspx

#pragma pack(push)
#pragma pack(2)
	struct IconDir {
		WORD reserved; // 0
		WORD type;     // 1 = アイコン, 2 = カーソル
		WORD count;    // アイコンの数
	};

	struct IconDirEntry {
		BYTE width;        // アイコンの幅
		BYTE height;       // アイコンの高さ
		BYTE paletteCount; // パレットの色の数
		BYTE reserved;     // 0
		WORD planes;       // 1（カーソルの場合はホットスポットのｘ座標）
		WORD bitCount;     // ピクセルのビット深度（カーソルの場合はホットスポットのｙ座標）
		DWORD imageSize;   // 画像のデータサイズ
		DWORD imageOffset; // ファイル先頭から画像データまでのバイト数
	};
#pragma pack(pop)

	bool isIcon = true;
	{// IconDir
		IconInfo info(icons[0]);
		isIcon = info.fIcon != FALSE;
		IconDir iconDir;
		iconDir.reserved = 0;
		iconDir.type = isIcon ? 1 : 2;
		iconDir.count = static_cast<WORD>(icons.size());

		stream.write(&iconDir, 0, sizeof(iconDir));
	}
	int offset = sizeof(IconDir) + sizeof(IconDirEntry) * icons.size();
	for (int i = 0, end = icons.size(); i < end; ++i) { // IconDirEntry
		assert("Null Icon" && icons[i]);
		IconInfo info(icons[i]);
		assert("Icon and Cursor mixed" && isIcon == (info.fIcon != FALSE));
		BITMAP bitmap;
		verify(GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap));
		assert("Too big bitmap width " && bitmap.bmWidth  < 256);
		assert("Too big bitmap height" && bitmap.bmHeight < 256);
		const int maskStride = ((((bitmap.bmWidth * 1 + 7) / 8) + 3) / 4) * 4;

		IconDirEntry entry;
		entry.width        = static_cast<BYTE>(bitmap.bmWidth );
		entry.height       = static_cast<BYTE>(bitmap.bmHeight);
		entry.paletteCount = bitmap.bmBitsPixel <= 8 ? (1 << bitmap.bmBitsPixel) : 0;
		entry.reserved     = 0;
		entry.planes       = static_cast<WORD>(isIcon ? 1                  : info.xHotspot);
		entry.bitCount     = static_cast<WORD>(isIcon ? bitmap.bmBitsPixel : info.yHotspot);
		entry.imageSize    = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * entry.paletteCount + (bitmap.bmWidthBytes + maskStride) * bitmap.bmHeight;
		entry.imageOffset  = offset;
		offset += entry.imageSize;

		stream.write(&entry, 0, sizeof(entry));
	}
	for (int i = 0, end = icons.size(); i < end; ++i) { // アイコン画像データ
		IconInfo info(icons[i]);
		Bitmap bitmap(info.hbmColor);
		if (bitmap.isDDB()) {
			bitmap = bitmap.toDIB();
		}
		BITMAP bitmapInfo;
		verify(GetObjectW(bitmap, sizeof(bitmapInfo), &bitmapInfo));
		const int maskStride = ((((bitmapInfo.bmWidth * 1 + 7) / 8) + 3) / 4) * 4;

		BITMAPINFOHEADER header;
		ZeroMemory(&header, sizeof(header));
		header.biSize = sizeof(header);
		header.biWidth = bitmapInfo.bmWidth;
		header.biHeight = bitmapInfo.bmHeight * 2;
		header.biPlanes = 1;
		header.biBitCount = bitmapInfo.bmBitsPixel;
		header.biSizeImage = (bitmapInfo.bmWidthBytes + maskStride) * bitmapInfo.bmHeight;

		stream.write(&header, 0, sizeof(header));

		if (bitmapInfo.bmBitsPixel <= 8) {
			RGBQUAD palette[256];
			ZeroMemory(palette, sizeof(palette));
			int paletteCount = 1 << bitmapInfo.bmBitsPixel;
			verify(0 < GetDIBColorTable(Graphics(bitmap), 0, paletteCount, palette));

			stream.write(palette, 0, sizeof(RGBQUAD) * paletteCount);
		}

		const int bitmapSize = bitmapInfo.bmWidthBytes * bitmapInfo.bmHeight;

		stream.write(bitmapInfo.bmBits, 0, bitmapSize);

		Bitmap mask(info.hbmMask);
		assert("Invalid mask width " && mask.width()  == bitmapInfo.bmWidth );
		assert("Invalid mask height" && mask.height() == bitmapInfo.bmHeight);
		if (mask.isDDB()) {
			mask = mask.toDIB();
		}
		BITMAP maskInfo;
		verify(GetObjectW(mask, sizeof(maskInfo), &maskInfo));
		const int maskSize = maskStride * bitmapInfo.bmHeight;

		stream.write(maskInfo.bmBits, 0, maskSize);
	}
}


void Icon::save(ArrayRange<const Icon> icons, StringRange filePath) {
	FileStream stream(filePath, FileStream::Mode::create, FileStream::Access::write);
	save(icons, stream);
}


Size Icon::size() const {
	assert("Null Icon _handle" && *this);
	IconInfo info(*this);
	return Bitmap(info.hbmColor).size();
}


Size Icon::smallSize() {
	return Size(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
}


Icon Icon::application(const Size& size) { return fromSystemResource((int)IDI_APPLICATION, size); }
Icon Icon::asterisk(const Size& size)    { return fromSystemResource((int)IDI_ASTERISK   , size); }
Icon Icon::error(const Size& size)       { return fromSystemResource((int)IDI_ERROR      , size); }
Icon Icon::exclamation(const Size& size) { return fromSystemResource((int)IDI_EXCLAMATION, size); }
Icon Icon::hand(const Size& size)        { return fromSystemResource((int)IDI_HAND       , size); }
Icon Icon::information(const Size& size) { return fromSystemResource((int)IDI_INFORMATION, size); }
Icon Icon::question(const Size& size)    { return fromSystemResource((int)IDI_QUESTION   , size); }
Icon Icon::warning(const Size& size)     { return fromSystemResource((int)IDI_WARNING    , size); }
Icon Icon::windowsLogo(const Size& size) { return fromSystemResource((int)IDI_WINLOGO    , size); }



	}
}