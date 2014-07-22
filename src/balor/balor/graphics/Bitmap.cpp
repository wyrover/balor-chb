#include "Bitmap.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include <balor/graphics/Color.hpp>
#include <balor/graphics/GdiplusInitializer.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/io/File.hpp>
#include <balor/io/FileStream.hpp>
#include <balor/io/StreamToIStream.hpp>
#include <balor/test/verify.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/scopeExit.hpp>
#include <balor/Size.hpp>
#include <balor/String.hpp>

#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

#undef max
#undef min


namespace balor {
	namespace graphics {


using std::move;
using std::swap;
using std::unique_ptr;
using std::vector;
using namespace balor::io;


namespace {
bool isDIB(const BITMAP& info) {
	return info.bmBits != nullptr;
}


bool isDIB(HBITMAP bitmap) {
	BITMAP info;
	if (!GetObjectW(bitmap, sizeof(info), &info)) { // 스크린 HDC를 상대로 하면 실패할 수도 있다.
		return false;
	}
	return isDIB(info);
}


struct BitmapInfo : public BITMAP {
	BitmapInfo(HBITMAP handle) {
		assert("Null bitmap handle" && handle);
		verify(GetObjectW(handle, sizeof(BITMAP), this));
	}
};
} // namespace



bool Bitmap::FileFormat::_validate(FileFormat value) {
	return bmp <= value && value <= tiff;
}



Bitmap::Format::Format() {
}


Bitmap::Format::Format(int bitsPerPixel , int rMask, int gMask, int bMask, int aMask)
	: bitsPerPixel(bitsPerPixel)
	, rMask(rMask)
	, gMask(gMask)
	, bMask(bMask)
	, aMask(aMask)
	{
	assert("Unsupported bitsPerPixel" && (bitsPerPixel == 1 || bitsPerPixel == 4 || bitsPerPixel == 8 || bitsPerPixel == 16 || bitsPerPixel == 24 || bitsPerPixel == 32));
	assert("rgbMasks can set only if bitsPerPixel is 16 or 32" && ((bitsPerPixel == 16 || bitsPerPixel == 32) || (rMask == 0 && gMask == 0 && bMask == 0)));
	assert("aMask can set only if bitsPerPixel is 32" && (bitsPerPixel == 32 || aMask == 0));
	assert("Overlapped rgMasks" && !(rMask & gMask));
	assert("Overlapped rbMasks" && !(rMask & bMask));
	assert("Overlapped raMasks" && !(rMask & aMask));
	assert("Overlapped gbMasks" && !(gMask & bMask));
	assert("Overlapped gaMasks" && !(gMask & aMask));
	assert("Overlapped baMasks" && !(bMask & aMask));
	assert("rgbMasks overflow" && !(bitsPerPixel == 16 && (rMask | gMask | bMask) & 0xFFFF0000));
}


const Bitmap::Format Bitmap::Format::palette1bpp( 1);
const Bitmap::Format Bitmap::Format::palette4bpp( 4);
const Bitmap::Format Bitmap::Format::palette8bpp( 8);
const Bitmap::Format Bitmap::Format::rgb16bpp(16, 0x7C00, 0x03E0, 0x001F);
const Bitmap::Format Bitmap::Format::rgb16bpp565(16, 0xF800, 0x07E0, 0x001F);
const Bitmap::Format Bitmap::Format::rgb24bpp(24);
const Bitmap::Format Bitmap::Format::argb32bpp(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);


bool Bitmap::Format::operator==(const Format& value) const {
	return bitsPerPixel == value.bitsPerPixel
		&& rMask == value.rMask
		&& gMask == value.gMask
		&& bMask == value.bMask
		&& aMask == value.aMask
		;
}


bool Bitmap::Format::operator!=(const Format& value) const {
	return !(*this == value);
}



Bitmap::Bitmap() : _handle(nullptr), _owned(false) {
}


Bitmap::Bitmap(Bitmap&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


Bitmap::Bitmap(HBITMAP handle, bool owned) : _handle(handle), _owned(owned) {
}


Bitmap::Bitmap(Stream& stream, bool bottomUp, bool useIcm) : _handle(nullptr), _owned(true) {
	assert("stream read unsupported" && stream.readable());

	GdiplusInitializer::initialize();

	auto istream = StreamToIStream::create(stream);
	Gdiplus::Bitmap gdiplusBitmap(istream, useIcm);
	const Gdiplus::Status status = gdiplusBitmap.GetLastStatus();
	if (status != Gdiplus::Ok) {
		switch (status) {
			case Gdiplus::OutOfMemory        : throw OutOfMemoryException();
			case Gdiplus::UnknownImageFormat : throw FileFormatException();
			case Gdiplus::InvalidParameter   : throw FileFormatException(); // 아마 포맷이 틀린 경우일 것이다.
			default                          : assert("Failed to construct Gdiplus::Bitmap" && false); return;
		}
	}
	// Gdiplus::Bitmap::GetHBITMAP은 원래 픽셀 포맷에 관계 없이 32비트 화상을 반환하므로 사용할 수 없다.
	Bitmap bitmap;
	Format format;
	bool canScanCopy = true;
	Format rgb32bpp = Format::argb32bpp;
	rgb32bpp.aMask = 0;
	switch (gdiplusBitmap.GetPixelFormat()) {
		case PixelFormat1bppIndexed    : format = Format::palette1bpp; break;
		case PixelFormat4bppIndexed    : format = Format::palette4bpp; break;
		case PixelFormat8bppIndexed    : format = Format::palette8bpp; break;
		case PixelFormat16bppGrayScale : format = rgb32bpp; canScanCopy = false; break;
		case PixelFormat16bppRGB555    : format = Format::rgb16bpp   ; break;
		case PixelFormat16bppRGB565    : format = Format::rgb16bpp565; break;
		case PixelFormat16bppARGB1555  : format = Format::rgb16bpp   ; break;
		case PixelFormat24bppRGB       : format = Format::rgb24bpp   ; break;
		case PixelFormat32bppRGB       : format = rgb32bpp; break;
		case PixelFormat32bppARGB      : format = Format::argb32bpp  ; break;
		case PixelFormat32bppPARGB     : format = Format::argb32bpp  ; break; // premultiplyAlpha 함수를 실행한 후의 argb32bpp 화상
		case PixelFormat48bppRGB       : 
		case PixelFormat64bppARGB      : 
		case PixelFormat64bppPARGB     : 
		case PixelFormat32bppCMYK      : 
		default                        : format = rgb32bpp; canScanCopy = false; break;
	}
	if (canScanCopy) {
		Gdiplus::BitmapData data;
		const Gdiplus::Rect rect(0, 0, gdiplusBitmap.GetWidth(), gdiplusBitmap.GetHeight());
		verify(gdiplusBitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, gdiplusBitmap.GetPixelFormat(), &data) == Gdiplus::Ok);
		scopeExit([&] () {
			verify(gdiplusBitmap.UnlockBits(&data) == Gdiplus::Ok);
		});
		bitmap = Bitmap(gdiplusBitmap.GetWidth(), gdiplusBitmap.GetHeight(), format, bottomUp, data.Scan0, data.Stride, false);
	} else {
		bitmap = Bitmap(gdiplusBitmap.GetWidth(), gdiplusBitmap.GetHeight(), format, bottomUp);
	}
	if (format.bitsPerPixel <= 8) {
		struct GdiPlusPalette {
			UINT flags;
			UINT count;
			RGBQUAD entries[256];
		} palette;
		const UINT size = gdiplusBitmap.GetPaletteSize();
		assert("Failed to Gdiplus::Bitmap::GetPaletteSize" && size != 0);
		assert(size <= sizeof(palette));
		verify(gdiplusBitmap.GetPalette(reinterpret_cast<Gdiplus::ColorPalette*>(&palette), size) == Gdiplus::Ok);
		verify(SetDIBColorTable(Graphics(bitmap), 0, palette.count, palette.entries));
	}
	if (!canScanCopy) { // 이쪽만으로도 좋지만 특정 포맷(팔렛트계 및 16비트 포맷)에서 왜인지 그물 모양이 발생하는 경우가 있다.
		Graphics graphics(bitmap);
		Gdiplus::Graphics gdiplusGraphics(graphics);
		//gdiplusGraphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
		verify(gdiplusGraphics.DrawImage(&gdiplusBitmap, 0, 0) == Gdiplus::Ok);
	}
	*this = move(bitmap);
}


Bitmap::Bitmap(Stream&& stream, bool bottomUp, bool useIcm) : _handle(nullptr), _owned(true) {
	*this = Bitmap(stream, bottomUp, useIcm);
}


Bitmap::Bitmap(StringRange filePath, bool bottomUp, bool useIcm) : _handle(nullptr), _owned(true) {
	FileStream stream(filePath, FileStream::Mode::open, FileStream::Access::read);
	*this = Bitmap(stream, bottomUp, useIcm);
}


Bitmap::Bitmap(HBITMAP bitmap, const Size& size, Bitmap::Format format, bool bottomUp) : _handle(nullptr), _owned(true) {
	*this = Bitmap(bitmap, size.width, size.height, format, bottomUp);
}


Bitmap::Bitmap(HBITMAP bitmap, int width, int height, Bitmap::Format format, bool bottomUp) : _handle(nullptr), _owned(true) {
	assert("Null bitmap" && bitmap);
	assert("too small format palette size" && !(format.bitsPerPixel <= 8 && format.bitsPerPixel < Bitmap(bitmap).bitsPerPixel())); // 팔레트 감색 처리가 필요한 포맷은 지정할 수 없다 

	*this = Bitmap(width, height, format, bottomUp);
	Graphics src(bitmap);
	Graphics dst(_handle);
	if (format.bitsPerPixel <= 8) {
		RGBQUAD rgbs[256];
		UINT count = GetDIBColorTable(src, 0, 256, rgbs);
		assert(0 < count);
		verify(SetDIBColorTable(dst, 0, count, rgbs));
	}
	dst.copyMode(Graphics::CopyMode::deleteScans); // CopyMode::halftone 라면 알파채널이 이상하게 된다.
	dst.copy(0, 0, width, height, src);
}


Bitmap::Bitmap(const Size& size, Bitmap::Format format, bool bottomUp, const void* srcPixels, int srcStride, bool srcBottomUp) : _handle(nullptr), _owned(true) {
	*this = Bitmap(size.width, size.height, format, bottomUp, srcPixels, srcStride, srcBottomUp);
}


Bitmap::Bitmap(int width, int height, Format format, bool bottomUp, const void* srcPixels, int srcStride, bool srcBottomUp) : _handle(nullptr), _owned(true) {
	assert("width less tha 0" && 0 < width);
	assert("height less than 0" && 0 < height);
	assert("Too small srcStride" && (!srcPixels || ((width * format.bitsPerPixel + 7) / 8) <= srcStride));
	assert("srcPixels is bad read pointer" && (!srcPixels || !IsBadReadPtr(srcPixels, srcStride * height)));

	struct BitmapInfo {
		BITMAPV5HEADER header;
		DWORD palette[256];
	} bitmapInfo;
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));

	BITMAPV5HEADER& header = bitmapInfo.header;
	header.bV5Size = sizeof(header);
	header.bV5Width  = width ;
	header.bV5Height = bottomUp ? height : -height;
	header.bV5Planes = 1;
	header.bV5BitCount = static_cast<WORD>(format.bitsPerPixel);
	header.bV5Compression = BI_RGB;
	header.bV5SizeImage = 0;
	header.bV5XPelsPerMeter = 3780; // 96dpi
	header.bV5YPelsPerMeter = 3780; // 96dpi
	header.bV5ClrUsed = 0;
	header.bV5ClrImportant = 0;
	header.bV5CSType = LCS_WINDOWS_COLOR_SPACE; // 읽기 방법이 없으므로 설정해도 의미 없지만
	header.bV5Intent = LCS_GM_GRAPHICS;
	if ((format.bitsPerPixel == 16 && format != Format::rgb16bpp && format != Format(16))
	 || (format.bitsPerPixel == 32)) { // 독자 컬러 마스크를 지정할 수 있을지 어떨지 
		header.bV5Compression = BI_BITFIELDS;
		header.bV5RedMask   = format.rMask;
		header.bV5GreenMask = format.gMask;
		header.bV5BlueMask  = format.bMask;
		if (format.bitsPerPixel == 32) {
			if (format.aMask) {
				header.bV5AlphaMask = format.aMask;
			} else { // 알파 채널이 없는 경우라도 강제적으로 알파를 붙인다 
				header.bV5AlphaMask = ~(format.rMask | format.gMask | format.bMask);
			}
		}
	}
	void* dstPixels = nullptr;
	_handle = CreateDIBSection(nullptr, reinterpret_cast<const BITMAPINFO*>(&header), DIB_RGB_COLORS, &dstPixels, nullptr, 0);
	if (!_handle) {
		const DWORD errorCode = GetLastError();
		switch (errorCode) {
			case ERROR_NOT_ENOUGH_MEMORY : throw OutOfMemoryException();
			default : assert("Failed to CreateDIBSection" && false); return;
		}
	}

	if (srcPixels) {
		int dstStride = this->stride();
		const int minStride = std::min(dstStride, srcStride);
		const BYTE* src = reinterpret_cast<const BYTE*>(srcPixels);
		BYTE* dst = reinterpret_cast<BYTE*>(dstPixels);
		if (bottomUp != srcBottomUp) {
			dst += (height - 1) * dstStride;
			dstStride = -dstStride;
		}
		for (int y = 0; y < height; ++y) {
			memcpy(dst, src, minStride);
			if (format.bitsPerPixel == 32 && !format.aMask) { // 알파 채널이 없는 경우는 불투명으로 초기화 한다
				DWORD* pixels = reinterpret_cast<DWORD*>(dst);
				for (int x = 0; x < width; ++x, ++pixels) {
					*pixels |= header.bV5AlphaMask;
				}
			}
			src += srcStride;
			dst += dstStride;
		}
	}
}


Bitmap::~Bitmap() {
	if (_handle && _owned) {
		verify(DeleteObject(_handle)); // Graphics에 보존하기 때문에 소거할 수 없을까? 또는 기존의 유효한 핸들이 아니다.
	}
}


Bitmap& Bitmap::operator=(Bitmap&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


int Bitmap::bitsPerPixel() const {
	return BitmapInfo(*this).bmBitsPixel;
}


bool Bitmap::bottomUp() const {
	assert("Null Bitmap handle" && *this);
	BitmapInfo info(*this);
	if (!isDIB(info)) {
		return true;
	}
	// 처음 메모리 픽셀 색을 0 -> 1로 변화 시켜서 좌표(0,0)의 색이 변화하는지 어떤지 본다
	Graphics graphics(*this);
	if (info.bmBitsPixel <= 8) {
		BYTE* const buffer = static_cast<BYTE*>(info.bmBits);
		RGBQUAD paletteBackup[2];
		verify(GetDIBColorTable(graphics, 0, 2, paletteBackup) == 2);
		RGBQUAD palette[2] = {{0, 0, 0, 0}, {255, 255, 255, 255}};
		verify(SetDIBColorTable(graphics, 0, 2, palette)); // 다른 색 팔레트로 해서 변화를 검출할 수 있도록 한다
		BYTE clearMask = 0;
		BYTE oneFlag = 0;
		switch (info.bmBitsPixel) {
			case  1 : clearMask = 0x7F; oneFlag = 0x80; break;
			case  4 : clearMask = 0x0F; oneFlag = 0x10; break;
			case  8 : clearMask = 0x00; oneFlag = 0x01; break;
		}
		const BYTE pixelBackup = *buffer;
		*buffer = *buffer & clearMask;
		const Color oldColor = graphics.getPixel(0, 0);
		*buffer = *buffer | oneFlag;
		const Color newColor = graphics.getPixel(0, 0);
		*buffer = pixelBackup;
		verify(SetDIBColorTable(graphics, 0, 2, paletteBackup));
		return oldColor == newColor;
	}
	switch (info.bmBitsPixel) {
		case 16 : {
			WORD* const buffer = static_cast<WORD*>(info.bmBits);
			const WORD pixelBackup = *buffer;
			*buffer = 0;
			const Color oldColor = graphics.getPixel(0, 0);
			*buffer = 0xFFFF;
			const Color newColor = graphics.getPixel(0, 0);
			*buffer = pixelBackup;
			return oldColor == newColor;
		} break;
		case 24 : {
			BYTE* const buffer = static_cast<BYTE*>(info.bmBits);
			const BYTE pixelBackup = *buffer;
			*buffer = 0;
			const Color oldColor = graphics.getPixel(0, 0);
			*buffer = 255;
			const Color newColor = graphics.getPixel(0, 0);
			*buffer = pixelBackup;
			return oldColor == newColor;
		} break;
		case 32 : {
			DWORD* const buffer = static_cast<DWORD*>(info.bmBits);
			const DWORD pixelBackup = *buffer;
			*buffer = 0;
			const Color oldColor = graphics.getPixel(0, 0);
			*buffer = 0xFFFFFFFF;
			const Color newColor = graphics.getPixel(0, 0);
			*buffer = pixelBackup;
			return oldColor == newColor;
		} break;
	}
	return true;
}


Bitmap Bitmap::clone() const {
	if (!*this) {
		return Bitmap();
	}
	return clone(_handle);
}


Bitmap Bitmap::clone(HBITMAP handle) {
	assert("Null handle" && handle);
	if (isDIB(handle)) {
		return toDIB(handle, Bitmap(handle).bottomUp());
	} else {
		return toDDB(handle);
	}
}


Bitmap Bitmap::createDDB(const Size& size) {
	return createDDB(size.width, size.height);
}


Bitmap Bitmap::createDDB(int width, int height) {
	assert("width less tha 0" && 0 < width);
	assert("height less than 0" && 0 < height);

	Bitmap bitmap(CreateCompatibleBitmap(Graphics::fromScreen(), width, height), true);
	if (!bitmap) {
		const DWORD errorCode = GetLastError();
		switch (errorCode) {
			case ERROR_NOT_ENOUGH_MEMORY : throw OutOfMemoryException();
			default : assert("Failed to CreateCompatibleBitmap" && false); break;
		}
	}
	assert("palette DDB Bitmap unsupported" && 8 < bitmap.bitsPerPixel());
	return bitmap;
}


bool Bitmap::equalsBits(const Bitmap& rhs, bool exceptAlpha) const {
	return equalsBits(*this, rhs, exceptAlpha);
}


bool Bitmap::equalsBits(const Bitmap& lhs, const Bitmap& rhs, bool exceptAlpha) {
	assert("Null bitmap handle" && lhs);
	assert("Null bitmap handle" && rhs);

	const Size lSize = lhs.size();
	const Size rSize = rhs.size();
	if (lSize != rSize) { // 크기 비교
		return false;
	}
	const Format lFormat = lhs.format();
	const Format rFormat = rhs.format();
	if (lFormat != rFormat) { // 포맷 비교
		return false;
	}
	const bool lIsDDB = lhs.isDDB();
	const bool rIsDDB = rhs.isDDB();
	if (lIsDDB != rIsDDB) {
		return false; // DIB 이거나 DDB 인지 일치하지 않으면 비교하지 않는다.
	}
	if (lIsDDB && rIsDDB) { // DDB 경우는 DIB 로 변환해서 
		Bitmap ldib = lIsDDB ? Bitmap::toDIB(lhs) : Bitmap(static_cast<HBITMAP>(lhs));
		Bitmap rdib = rIsDDB ? Bitmap::toDIB(rhs) : Bitmap(static_cast<HBITMAP>(rhs));
		return equalsBits(ldib, rdib);
	}
	if (lFormat.bitsPerPixel <= 8) { // 팔레트 비교
		const auto lpalette = lhs.palette();
		const auto rpalette = rhs.palette();
		if (lpalette != rpalette) {
			return false;
		}
	}
	if (lhs.bottomUp() != rhs.bottomUp()) { // 보텀-업 불이치
		return false;
	}
	// 필셀 비교 
	Graphics::flush();
	const int bitCount = lFormat.bitsPerPixel;
	const int width = lSize.width;
	DWORD mask = 0xffffffff;
	if (bitCount == 16 || bitCount == 32) {
		mask = lFormat.rMask | lFormat.gMask | lFormat.bMask;
		if (bitCount == 32 && !exceptAlpha) {
			mask |= lFormat.aMask;
		}
	} else if (bitCount == 1 || bitCount == 4) {
		int overBitCount = 8 - ((width * bitCount) % 8);
		if (overBitCount != 0 && overBitCount != 8) {
			mask = 0xffffffff ^ ((1 << overBitCount) - 1);
		}
	}
	const int lstride = lhs.stride();
	const int rstride = rhs.stride();
	const int byteWidth = (width * bitCount + 7) / 8;
	const BYTE* lbuffer = reinterpret_cast<const BYTE*>(lhs.pixels());
	const BYTE* rbuffer = reinterpret_cast<const BYTE*>(rhs.pixels());
	for (int y = 0; y < lSize.height; ++y) {
		switch (bitCount) {
			case  1 :
			case  4 : { // 최후 바이트만 마스크 
				if ( 1 < byteWidth) {
					if (std::memcmp(lbuffer, rbuffer, byteWidth - 1) != 0) {
						return false;
					}
				}
				const DWORD lpixel = lbuffer[byteWidth - 1] & mask;
				const DWORD rpixel = rbuffer[byteWidth - 1] & mask;
				if (lpixel != rpixel) {
					return false;
				}
			} break;
			case  8 :
			case 24 : { // 모든 바이트 유효
				if (std::memcmp(lbuffer, rbuffer, byteWidth) != 0) {
					return false;
				}
			} break;
			case 16 : { // 마스크 바이트만 유효
				const WORD* const lptr = reinterpret_cast<const WORD*>(lbuffer);
				const WORD* const rptr = reinterpret_cast<const WORD*>(rbuffer);
				for (int x = 0; x < width; ++x) {
					const DWORD lpixel = lptr[x] & mask;
					const DWORD rpixel = rptr[x] & mask;
					if (lpixel != rpixel) {
						return false;
					}
				}
			} break;
			case 32 : { // 마스크 바이트만 유효 
				const DWORD* const lptr = reinterpret_cast<const DWORD*>(lbuffer);
				const DWORD* const rptr = reinterpret_cast<const DWORD*>(rbuffer);
				for (int x = 0; x < width; ++x) {
					const DWORD lpixel = lptr[x] & mask;
					const DWORD rpixel = rptr[x] & mask;
					if (lpixel != rpixel) {
						return false;
					}
				}
			} break;
		}
		lbuffer += lstride;
		rbuffer += rstride;
	}
	return true;
}


Bitmap::Format Bitmap::format() const {
	assert("Null Bitmap handle" && *this);
	BitmapInfo info(*this);
	switch (info.bmBitsPixel) {
		case  1 : return Format::palette1bpp;
		case  4 : return Format::palette4bpp;
		case  8 : return Format::palette8bpp;
		case 24 : return Format::rgb24bpp;
		case 16 :
		case 32 : {
			Format format;
			DWORD compression;
			DIBSECTION dib;
			if (GetObjectW(*this, sizeof(dib), &dib) && dib.dsBm.bmBits) {
				compression = dib.dsBmih.biCompression;
				format = Format(dib.dsBmih.biBitCount, dib.dsBitfields[0], dib.dsBitfields[1], dib.dsBitfields[2]);
				if (format.bitsPerPixel == 32) { // 32 비트라면 강제적으로 알파 채널을 붙인다 
					format.aMask = ~(format.rMask | format.gMask | format.bMask);
				}
			} else { // DDB 경우는 GetDIBits 를 사용한다
				struct BitmapInfo {
					BITMAPINFOHEADER header; // 예를들면 BITMAPV5HEADER 로 CreateDIBSection 하였더라도 GetDIBits 는 보통 BITMAPINFOHEADER 를 상정하고 있다 
					DWORD masks[256];
				} bitmapInfo;
				ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
				bitmapInfo.header.biSize = sizeof(bitmapInfo.header);
				Graphics graphics = Graphics::fromScreen();
				// First call to GetDIBits will fill in the optimal biBitCount.  http://support.microsoft.com/kb/230492/ja
				verify(GetDIBits(graphics, *this, 0, 1, nullptr, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS));
				// Second call to GetDIBits will get the optimal color table, or the optimal bitfields values.
				verify(GetDIBits(graphics, *this, 0, 1, nullptr, reinterpret_cast<BITMAPINFO*>(&bitmapInfo), DIB_RGB_COLORS));
				compression = bitmapInfo.header.biCompression;
				format = Format(bitmapInfo.header.biBitCount, bitmapInfo.masks[0], bitmapInfo.masks[1], bitmapInfo.masks[2]);
			}
			switch (compression) {
				case BI_BITFIELDS : return format;
				case BI_RGB       : return format.bitsPerPixel == 16 ? Format::rgb16bpp : Format::argb32bpp;
				default           : assert("Unsupported bitmap format"); return Format::argb32bpp; // 아마 압축 형식 
			}
		}
		default : assert("Unsupported bitsPerPixel"); return Format::argb32bpp;
	}
}


int Bitmap::height() const {
	return BitmapInfo(*this).bmHeight;
}


bool Bitmap::isDDB() const {
	return !isDIB(_handle);
}


bool Bitmap::owned() const {
	return _owned;
}


void Bitmap::owned(bool value) {
	_owned = value;
}


vector<Color> Bitmap::palette() const {
	assert("Null Bitmap handle" && *this);
	assert("DDB bitmap can not have palette" && isDIB(_handle));
	assert("Can't have palette" && bitsPerPixel() <= 8);

	RGBQUAD rgbs[256];
	const UINT count = GetDIBColorTable(Graphics(*this), 0, 256, rgbs);
	assert(0 < count);
	vector<Color> colors;
	colors.reserve(count);
	for (UINT i = 0; i < count; ++i) {
		colors.push_back(Color(rgbs[i].rgbRed, rgbs[i].rgbGreen, rgbs[i].rgbBlue));
	}
	return colors;
}


void Bitmap::palette(ArrayRange<const Color> value) {
	assert("Null Bitmap handle" && *this);
	assert("DDB bitmap can not have palette" && isDIB(_handle));
	assert("Can't have palette" && bitsPerPixel() <= 8);
	assert("Empty value" && !value.empty());
	assert("Too large value length" && value.length() <= (1 << bitsPerPixel()));

	RGBQUAD rgbs[256];
	RGBQUAD* rgb = rgbs;
	for (auto i = value.begin(), end = value.end(); i != end; ++i, ++rgb) {
		rgb->rgbRed   = i->r;
		rgb->rgbGreen = i->g;
		rgb->rgbBlue  = i->b;
	}
	verify(SetDIBColorTable(Graphics(*this), 0, value.length(), rgbs));
}


unsigned char* Bitmap::pixels() {
	return const_cast<unsigned char*>(const_cast<const Bitmap*>(this)->pixels());
}


const unsigned char* Bitmap::pixels() const {
	return reinterpret_cast<unsigned char*>(BitmapInfo(*this).bmBits); // DIB 섹션 이외에서는 0 로 된다
}


void Bitmap::premultiplyAlpha() {
	assert("Can't premultiply alpha to DDB bitmap" && !isDDB());
	assert("Can't premultiply alpha to non 32bit bitmap" && bitsPerPixel() == 32);

	struct NTZ { // http://www.nminoru.jp/~nminoru/programming/bitcount.html  Henry S. Warren    Hacker's Delight 
		static long get(long value) { // get Number of Training Zero
			value = (value & (-value)) - 1;
			
			value = (value & 0x55555555) + (value >> 1 & 0x55555555);
			value = (value & 0x33333333) + (value >> 2 & 0x33333333);
			value = (value & 0x0f0f0f0f) + (value >> 4 & 0x0f0f0f0f);
			value = (value & 0x00ff00ff) + (value >> 8 & 0x00ff00ff);
			return  (value & 0x0000ffff) + (value >>16 & 0x0000ffff);
		}
	};
	BitmapInfo info(*this);
	if (!info.bmBits || info.bmBitsPixel != 32) {
		return;
	}
	const auto format = this->format();
	const DWORD masks[] = {format.rMask, format.gMask, format.bMask};
	const DWORD shifts[] = {NTZ::get(format.rMask), NTZ::get(format.gMask), NTZ::get(format.bMask)};
	const DWORD aShift = NTZ::get(format.aMask);
	const DWORD aMax = static_cast<DWORD>(format.aMask) >> aShift;
	auto buffer = static_cast<BYTE*>(info.bmBits);
	for (LONG y = 0; y < info.bmHeight; ++y) {
		auto pixels = reinterpret_cast<DWORD*>(buffer);
		for (LONG x = 0; x < info.bmWidth; ++x, ++pixels) {
			DWORD pixel = *pixels & format.aMask;
			const DWORD alpha = pixel >> aShift;
			for (int channel = 0; channel < 3; ++channel) {
				const DWORD shift = shifts[channel];
				DWORD c = *pixels & masks[channel];
				c >>= shift;
				c = (c * alpha) / aMax;
				c <<= shift;
				pixel |= c;
			}
			*pixels = pixel;
		}
		buffer += info.bmWidthBytes;
	}
}


void Bitmap::save(Stream& stream, Bitmap::FileFormat format, int quality) const {
	assert("Null Bitmap handle" && *this);
	assert("stream write unsupported" && stream.writable());
	assert("Invalid FileFormat" && FileFormat::_validate(format));
	assert("quality out of range" && 0 <= quality);
	assert("quality out of range" && quality <= 100);

	GdiplusInitializer::initialize();

	Gdiplus::Bitmap* bitmap = nullptr;
	scopeExit([&] () {
		delete bitmap;
	});
	unique_ptr<BYTE[]> buffer;
	BitmapInfo info(*this);
	if (info.bmBitsPixel == 32 && info.bmBits) { // 32비트 경우는 앙파 채널도 보존한다 
		if (bottomUp()) {
			buffer.reset(new BYTE[info.bmWidthBytes * info.bmHeight]);
			const BYTE* src = reinterpret_cast<BYTE*>(info.bmBits) + (info.bmHeight - 1) * info.bmWidthBytes;
			BYTE* dst = buffer.get();
			for (int y = 0; y < info.bmHeight; ++y) {
				memcpy(dst, src, info.bmWidthBytes);
				src -= info.bmWidthBytes;
				dst += info.bmWidthBytes;
			}
			bitmap = new Gdiplus::Bitmap(info.bmWidth, info.bmHeight, info.bmWidthBytes, PixelFormat32bppARGB, buffer.get());
		} else {
			bitmap = new Gdiplus::Bitmap(info.bmWidth, info.bmHeight, info.bmWidthBytes, PixelFormat32bppARGB, static_cast<BYTE*>(info.bmBits));
		}
	} else {
		bitmap = new Gdiplus::Bitmap(*this, nullptr);
	}
	
	Gdiplus::Status status = bitmap->GetLastStatus();
	if (status != Gdiplus::Ok) {
		switch (status) {
			case Gdiplus::OutOfMemory : throw OutOfMemoryException();
			default : assert("Failed to construct Gdiplus::Bitmap" && false); return;
		}
	}
	auto istream = StreamToIStream::create(stream);
	CLSID encoderId = CLSID();
	{
		const wchar_t* mime = L"";
		switch (format) {
			case Bitmap::FileFormat::bmp  : mime = L"image/bmp" ; break;
			case Bitmap::FileFormat::gif  : mime = L"image/gif" ; break;
			case Bitmap::FileFormat::jpeg : mime = L"image/jpeg"; break;
			case Bitmap::FileFormat::png  : mime = L"image/png" ; break;
			case Bitmap::FileFormat::tiff : mime = L"image/tiff"; break;
		}

		UINT num = 0;
		UINT size = 0;
		verify(Gdiplus::GetImageEncodersSize(&num, &size) == Gdiplus::Ok);
		unique_ptr<BYTE[]> infoBuffer(new BYTE[size]);
		Gdiplus::ImageCodecInfo* info = reinterpret_cast<Gdiplus::ImageCodecInfo*>(infoBuffer.get());
		bool find = false;
		verify(Gdiplus::GetImageEncoders(num, size, info) == Gdiplus::Ok);
		for (UINT i = 0; i < num; ++i) {
			if (String::equals(info[i].MimeType, mime)) {
				encoderId = info[i].Clsid;
				find = true;
			}
		}
		assert("Mime not found" && find);
	}
	Gdiplus::EncoderParameters parameters;
	ULONG qualityValue = quality;
	parameters.Count = 1;
	parameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
	parameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
	parameters.Parameter[0].NumberOfValues = 1;
	parameters.Parameter[0].Value = &qualityValue;
	status = bitmap->Save(istream, &encoderId, &parameters);
	if (status != Gdiplus::Ok) {
		switch (status) {
			case Gdiplus::OutOfMemory : throw OutOfMemoryException();
			default : assert("Failed to Gdiplus::Bitmap::Save" && false); return;
		}
	}
}


void Bitmap::save(Stream&& stream, Bitmap::FileFormat format, int quality) const {
	save(stream, format, quality);
}


void Bitmap::save(StringRange filePath, int quality) const {
	assert("Null Bitmap handle" && *this);
	assert("Empty filePath" && !filePath.empty());

	String ext = File(filePath).extension();
	FileFormat format = FileFormat::png;
	if (ext == L".bmp") {
		format = FileFormat::bmp;
	} else if (ext == L".gif") {
		format = FileFormat::gif;
	} else if (ext == L".jpg" || ext == L".jpeg" || ext == L".jpe") {
		format = FileFormat::jpeg;
	} else if (ext == L".png") {
		format = FileFormat::png;
	} else if (ext == L".tif" || ext == L".tiff") {
		format = FileFormat::tiff;
	}
	save(filePath, format, quality);
}


void Bitmap::save(StringRange filePath, Bitmap::FileFormat format, int quality) const {
	assert("Null Bitmap handle" && *this);
	FileStream stream(filePath, FileStream::Mode::create, FileStream::Access::write);
	save(stream, format, quality);
}


Size Bitmap::size() const {
	BitmapInfo info(*this);
	return Size(info.bmWidth, info.bmHeight);
}


int Bitmap::stride() const {
	return BitmapInfo(*this).bmWidthBytes;
	//return ((((width() * bitsPerPixel() + 7) / 8) + 3) / 4) * 4;
}


Bitmap Bitmap::toDDB() const {
	return toDDB(*this);
}


Bitmap Bitmap::toDDB(HBITMAP handle) {
	assert("Null handle" && handle);
	BITMAP info;
	verify(GetObjectW(handle, sizeof(info), &info)); // 실패한다면 _handle 핸들이 유효하지 않다.
	Bitmap bitmap= createDDB(info.bmWidth, info.bmHeight);	// 당초는 CopyImage 를 사용하였지만 큰 사이즈의 화상을 넘기면 에러가 발생해서 그만두었다 
	Graphics dst(bitmap);
	Graphics src(handle);
	dst.copy(0, 0, src, 0, 0, info.bmWidth, info.bmHeight);
	return bitmap;
}


Bitmap Bitmap::toDIB(bool bottomUp) const {
	return toDIB(*this, bottomUp);
}


Bitmap Bitmap::toDIB(HBITMAP handle, bool bottomUp) {
	assert("Null handle" && handle);
	BitmapInfo info(handle);
	Bitmap bitmap = Bitmap(info.bmWidth, info.bmHeight, Bitmap(handle).format(), bottomUp);
	Graphics dst(bitmap);
	Graphics src(handle);
	if (info.bmBitsPixel <= 8) {
		RGBQUAD rgbs[256];
		const UINT count = GetDIBColorTable(src, 0, 256, rgbs);
		assert(0 < count);
		verify(SetDIBColorTable(dst, 0, count, rgbs));
	}
	dst.copy(0, 0, src, 0, 0, info.bmWidth, info.bmHeight);
	return bitmap;
}


int Bitmap::width() const {
	return BitmapInfo(*this).bmWidth;
}



	}
}