#include <balor/graphics/Bitmap.hpp>

#include <algorithm>
#include <vector>

#include <windows.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#undef min
#undef max

#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/GdiplusInitializer.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/graphics/Pen.hpp>
#include <balor/io/File.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/system/Process.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/test/verify.hpp>
#include <balor/ArrayRange.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/scopeExit.hpp>
#include <balor/Size.hpp>


namespace balor {
	namespace graphics {
		namespace testBitmap {


using std::move;
using std::vector;
using namespace balor::io;
using namespace balor::test;


namespace {
const wchar_t testDirectoryName[] = L"testBalor_graphics_Bitmap_laj34af4trgadljk5a7al0894s1";
File getTestDirectory() {
	File dir(File::getSpecial(File::Special::temporary), testDirectoryName);
	if (dir.exists()) {
		dir.remove(true);
	}
	dir.createDirectory();
	return dir;
}


void removeTestDirectory() {
	File(File::getSpecial(File::Special::temporary), testDirectoryName).remove(true);
}


Bitmap getTestBitmap(Bitmap&& bitmap) { 
	if (bitmap.bitsPerPixel() == 1) {
		Color colors[] = {Color::red(), Color::yellow()};
		bitmap.palette(colors);
		Graphics graphics(bitmap);
		Rectangle rect(0, 0, bitmap.width(), bitmap.height());
		graphics.brush(Brush::hollow());
		graphics.pen(Color::red());
		graphics.drawRectangle(rect);
		rect.grow(-1, -1);
		graphics.brush(Color::yellow());
		graphics.pen(Color::yellow());
		graphics.drawRectangle(rect);
		graphics.setPixel(0, 0, Color::yellow());
		graphics.setPixel(bitmap.width() - 2, 0, Color::yellow());
		graphics.setPixel(2, bitmap.height() - 1, Color::yellow());
		graphics.setPixel(bitmap.width() - 4, bitmap.height() - 1, Color::yellow());
	} else {
		if (bitmap.bitsPerPixel() <= 8) {
			Color colors[] = {Color::red(), Color::blue(), Color::green(), Color::white(), Color::black(), Color::yellow(), Color::aqua()};
			bitmap.palette(colors);
		}
		Graphics graphics(bitmap);
		Rectangle rect(0, 0, bitmap.width(), bitmap.height());
		graphics.brush(Brush::hollow());
		graphics.pen(Color::red());
		graphics.drawRectangle(rect);
		rect.grow(-1, -1);
		graphics.brush(Brush::hollow());
		graphics.pen(Color::blue());
		graphics.drawRectangle(rect);
		rect.grow(-1, -1);
		graphics.brush(Color::green());
		graphics.pen(Color::green());
		graphics.drawRectangle(rect);
		graphics.setPixel(0, 0, Color::white());
		graphics.setPixel(bitmap.width() - 1, 0, Color::black());
		graphics.setPixel(0, bitmap.height() - 1, Color::yellow());
		graphics.setPixel(bitmap.width() - 1, bitmap.height() - 1, Color::aqua());
	}
	Graphics::flush();
	return move(bitmap);
}


struct AnalyzeResult {
	int maxError;
	int totalError;
	int averageError;
};


AnalyzeResult analyze(const Bitmap& lhs, const Bitmap& rhs) {
	assert(!lhs.isDDB());
	assert(!rhs.isDDB());
	assert(lhs.size() == rhs.size());
	assert(lhs.format() == rhs.format());
	assert(lhs.bottomUp() == rhs.bottomUp());
	assert(8 < lhs.bitsPerPixel());

	AnalyzeResult result;
	result.maxError = 0;
	result.totalError = 0;

	auto format = lhs.format();
	int lstride = lhs.stride();
	int rstride = rhs.stride();
	const BYTE* lbuffer = reinterpret_cast<const BYTE*>(lhs.pixels());
	const BYTE* rbuffer = reinterpret_cast<const BYTE*>(rhs.pixels());
	int width = lhs.width();
	int height = lhs.height();
	for (int y = 0; y < height; ++y) {
		int l;
		int r;
		int diff;
		if (format == Bitmap::Format::rgb16bpp) {
			const WORD* lptr = reinterpret_cast<const WORD*>(lbuffer);
			const WORD* rptr = reinterpret_cast<const WORD*>(rbuffer);
			for (int x = 0; x < width; ++x) {
				DWORD lpixel = lptr[x];
				DWORD rpixel = rptr[x];
				l = (lpixel >> 10) & 0x1F;
				r = (rpixel >> 10) & 0x1F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = (lpixel >> 5) & 0x1F;
				r = (rpixel >> 5) & 0x1F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = (lpixel >> 0) & 0x1F;
				r = (rpixel >> 0) & 0x1F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
			}
		} else if (format == Bitmap::Format::rgb16bpp565) {
			const WORD* lptr = reinterpret_cast<const WORD*>(lbuffer);
			const WORD* rptr = reinterpret_cast<const WORD*>(rbuffer);
			for (int x = 0; x < width; ++x) {
				DWORD lpixel = lptr[x];
				DWORD rpixel = rptr[x];
				l = (lpixel >> 11) & 0x1F;
				r = (rpixel >> 11) & 0x1F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = (lpixel >> 5) & 0x3F;
				r = (rpixel >> 5) & 0x3F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = (lpixel >> 0) & 0x1F;
				r = (rpixel >> 0) & 0x1F;
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
			}
		} else if (format == Bitmap::Format::rgb24bpp) {
			for (int x = 0; x < width; ++x) {
				const BYTE* lpixel = lbuffer + (x * 3);
				const BYTE* rpixel = rbuffer + (x * 3);
				l = lpixel[0];
				r = rpixel[0];
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = lpixel[1];
				r = rpixel[1];
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
				l = lpixel[2];
				r = rpixel[2];
				diff = std::abs(l - r);
				result.maxError = std::max(diff, result.maxError);
				result.totalError += diff;
			}
		} else if (format == Bitmap::Format::argb32bpp) {
				const DWORD* lptr = reinterpret_cast<const DWORD*>(lbuffer);
				const DWORD* rptr = reinterpret_cast<const DWORD*>(rbuffer);
				for (int x = 0; x < width; ++x) {
					DWORD lpixel = lptr[x];
					DWORD rpixel = rptr[x];
					l = (lpixel >> 16) & 0xFF;
					r = (rpixel >> 16) & 0xFF;
					diff = std::abs(l - r);
					result.maxError = std::max(diff, result.maxError);
					result.totalError += diff;
					l = (lpixel >> 8) & 0xFF;
					r = (rpixel >> 8) & 0xFF;
					diff = std::abs(l - r);
					result.maxError = std::max(diff, result.maxError);
					result.totalError += diff;
					l = (lpixel >> 0) & 0xFF;
					r = (rpixel >> 0) & 0xFF;
					diff = std::abs(l - r);
					result.maxError = std::max(diff, result.maxError);
					result.totalError += diff;
				}
		}
		lbuffer += lstride;
		rbuffer += rstride;
	}
	result.averageError = result.totalError / (width * height * 3);
	return result;
}
} // namespace



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	balor::test::UnitTest::ignoreHandleLeakCheck();

	GdiplusInitializer::initialize(); // Gdiplus::GdiplusStartup 함수는 처음 호출에서 핸들을 복수 열고 유지한다

	Bitmap bitmap = Bitmap::createDDB(10, 10);
	Gdiplus::Bitmap gpBitmap(10, 10);
	Graphics graphics(bitmap);
	Gdiplus::Graphics gpGraphics(graphics);
	gpGraphics.DrawImage(&gpBitmap, 0, 0); // 이유를 잘 모르겠지만 이 함수는 처음 1회 호출만으로 GDI 핸들을 1개 릭 한다

	Bitmap bitmap2(10, 10);
	bitmap2 = bitmap2.clone(); // CreateDIBSection 가 처음 1회만 핸들 릭?

	Sleep(1000); // GDI+ 가 어떤 처리를 다른 스레드에서 하고 있어서 기다리지 않으면 불확정하게 다른 테스트 케이스에서 핸들 릭이 검출 
}



testCase(fileFormatValidate) {
	testAssert(!Bitmap::FileFormat::_validate(static_cast<Bitmap::FileFormat>(Bitmap::FileFormat::bmp - 1)));
	testAssert(Bitmap::FileFormat::_validate(Bitmap::FileFormat::bmp));
	testAssert(!Bitmap::FileFormat::_validate(static_cast<Bitmap::FileFormat>(Bitmap::FileFormat::tiff + 1)));
	testAssert(Bitmap::FileFormat::_validate(Bitmap::FileFormat::tiff));
}



testCase(formatDefaultConstruct) {
	testNoThrow(Bitmap::Format format);
}


testCase(formatConstruct) {
	// 유효한 bitsPerPixel
	testAssertionFailed(Bitmap::Format(0));
	testNoThrow(Bitmap::Format( 1));
	testNoThrow(Bitmap::Format( 4));
	testNoThrow(Bitmap::Format( 8));
	testNoThrow(Bitmap::Format(16));
	testNoThrow(Bitmap::Format(24));
	testNoThrow(Bitmap::Format(32));

	// rgbMask 를 설정할 수 있는 bitsPerPixel 은 16 과 32 뿐
	testAssertionFailed(Bitmap::Format(1, 1, 0, 0));
	testAssertionFailed(Bitmap::Format(1, 0, 1, 0));
	testAssertionFailed(Bitmap::Format(1, 1, 0, 1));
	testNoThrow(Bitmap::Format(1, 0, 0, 0));
	testNoThrow(Bitmap::Format(16, 0x7C00, 0x03E0, 0x001F));
	testNoThrow(Bitmap::Format(32, 0x7C00, 0x03E0, 0x001F));

	// rgbMask 는 비트가 중복하지 않는다
	testAssertionFailed(Bitmap::Format(32, 0x00FF0000, 0x0001FF00, 0x000000FF));
	testAssertionFailed(Bitmap::Format(32, 0x00FF0000, 0x000000FF, 0x0001FF00));
	testAssertionFailed(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000001FF));
	testNoThrow(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF));

	// rgbMask 가 bitsPerPixel 범위를 오버하고 있다 
	testAssertionFailed(Bitmap::Format(16, 0x0001F000, 0x00000F00, 0x000000F0));
	testNoThrow(Bitmap::Format(16, 0x0000F000, 0x00000F00, 0x000000F0));
}


testCase(formatConsts) {
	testAssert(Bitmap::Format::palette1bpp == Bitmap::Format( 1, 0, 0, 0));
	testAssert(Bitmap::Format::palette4bpp == Bitmap::Format( 4, 0, 0, 0));
	testAssert(Bitmap::Format::palette8bpp == Bitmap::Format( 8, 0, 0, 0));
	testAssert(Bitmap::Format::rgb16bpp    == Bitmap::Format(16, 0x7C00, 0x03E0, 0x001F));
	testAssert(Bitmap::Format::rgb16bpp565 == Bitmap::Format(16, 0xF800, 0x07E0, 0x001F));
	testAssert(Bitmap::Format::rgb24bpp    == Bitmap::Format(24, 0, 0, 0));
	testAssert(Bitmap::Format::argb32bpp    == Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000));
}


testCase(formatEqualOperator) {
	testAssert(Bitmap::Format(16, 0, 0, 0) != Bitmap::Format(24, 0, 0, 0));
	testAssert(Bitmap::Format(16, 0, 0, 0) == Bitmap::Format(16, 0, 0, 0));
	testAssert(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF) != Bitmap::Format(32, 0x007F0000, 0x0000FF00, 0x000000FF));
	testAssert(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF) != Bitmap::Format(32, 0x00FF0000, 0x00007F00, 0x000000FF));
	testAssert(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF) != Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x0000007F));
	testAssert(Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF) == Bitmap::Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF));
}



testCase(defaultConstruct) {
	Bitmap empty;
	testAssert(!empty);
	testAssert(!empty.owned());
}


testCase(moveConstruct) {
	Bitmap sourceSource = getTestBitmap(Bitmap(21, 13, Bitmap::Format::rgb24bpp));
	Bitmap source = sourceSource.clone();
	Bitmap bitmap = move(source);
	testAssert(!source);
	testAssert(!source.owned());
	testAssert(bitmap);
	testAssert(bitmap.owned());
	testAssert(bitmap.equalsBits(sourceSource));
}



testCase(streamConstruct) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	
	{// 무효한 파라미터
		File file0(dir, L"file0.bmp");
		FileStream stream(file0.path(), FileStream::Mode::create, FileStream::Access::write, FileStream::Share::none, FileStream::Options::removeOnClose);
		testAssertionFailed(Bitmap bitmap(stream));
	}

	{// bmp의 각 화상 포맷 읽기
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette1bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette4bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette8bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{// 어떤 이유인지 565 형식은 32bit 화상으로 읽혀진다. OleLoadPicture에서도 555로서 읽혀진다
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp565));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);

		// 덤으로 변환 오차가 붙는다 
		Bitmap source32 = getTestBitmap(Bitmap(source.width(), source.height(), Bitmap::Format::argb32bpp));
		auto result = analyze(bitmap, source32);
		testAssert(result.averageError == 0);
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source, true));
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(stream, false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source, true));
	}

	{// gif 각 화상 포맷 읽기
		Bitmap source = getTestBitmap(Bitmap(30, 30, Bitmap::Format::palette1bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::gif);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.format() == Bitmap::Format::palette8bpp);
		Bitmap source32(source, source.width(), source.height(), Bitmap::Format::argb32bpp);
		Bitmap bitmap32(bitmap, source.width(), source.height(), Bitmap::Format::argb32bpp);
		testAssert(bitmap32.equalsBits(source32));
	}
	
	{
		Bitmap source = getTestBitmap(Bitmap(30, 30, Bitmap::Format::palette8bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::gif);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(30, 30, Bitmap::Format::palette8bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::gif);
		stream.position(0);
		Bitmap bitmap(stream, false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}
	
	{// jpg 각 화상 포맷 읽기
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette8bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		Bitmap source24(source, source.width(), source.height(), Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.equalsBits(source24));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		Bitmap source24(source, source.width(), source.height(), Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.equalsBits(source24));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp565));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		Bitmap source24(source, source.width(), source.height(), Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.equalsBits(source24));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg, 100);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.equalsBits(source));
		auto result = analyze(bitmap, source);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		Bitmap source24(source, source.width(), source.height(), Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.equalsBits(source24));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::jpeg);
		stream.position(0);
		Bitmap bitmap(stream, false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		Bitmap source24(source, source.width(), source.height(), Bitmap::Format::rgb24bpp, false);
		testAssert(!bitmap.equalsBits(source24));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 4); // jpg는 까다로운 부류의 화상이므로 조금 느슨하게
	}

	{// png 각 화상 포맷 읽기
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette1bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette4bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette8bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{// 555 형식은 32bit 화상으로 저장된다
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);

		// 덤으로 변환 오차가 붙는다
		Bitmap source32 = getTestBitmap(Bitmap(source.width(), source.height(), Bitmap::Format::argb32bpp));
		auto result = analyze(bitmap, source32);
		testAssert(result.averageError < 2);
	}
	{// 565 형식은 32bit 화상으로 저장된다 
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp565));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);

		// 덤으로 변환 오차가 붙는다
		Bitmap source32 = getTestBitmap(Bitmap(source.width(), source.height(), Bitmap::Format::argb32bpp));
		auto result = analyze(bitmap, source32);
		testAssert(result.averageError == 0);
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		stream.position(0);
		Bitmap bitmap(stream, false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}

	{// tiff 각 화상 포맷 읽기
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette1bpp));
		// tiff의 모노크롬 화상은 흑과 백색만 허용한다
		Color colors[] = {Color::black(), Color::white()};
		source.palette(colors);
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette4bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::palette8bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{// 555 형식은 24bit 화상으로 저장된다
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);

		// 덤으로 변환 오차가 붙는다
		Bitmap source24 = getTestBitmap(Bitmap(source.width(), source.height(), Bitmap::Format::rgb24bpp));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError < 2);
	}
	{// 565 형식은 24bit 화상으로 저장된다
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb16bpp565));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);

		// 덤으로 변환 오차가 붙는다
		Bitmap source24 = getTestBitmap(Bitmap(source.width(), source.height(), Bitmap::Format::rgb24bpp));
		auto result = analyze(bitmap, source24);
		testAssert(result.averageError == 0);
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(source));
	}
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::tiff);
		stream.position(0);
		Bitmap bitmap(stream, false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}


}


testCase(rvalueStreamConstruct) {
	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(MemoryStream(stream.buffer(), 0, static_cast<int>(stream.length())));
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.equalsBits(source, true));
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::argb32bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::bmp);
		stream.position(0);
		Bitmap bitmap(MemoryStream(stream.buffer(), 0, static_cast<int>(stream.length())), false);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source, true));
	}
}


testCase(stringConstruct) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	{
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		File file1(dir, L"file1.bmp");
		stream.position(0);
		file1.create().write(stream.buffer(), 0, static_cast<int>(stream.length()));
		Bitmap bitmap(file1.openRead());
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
	}
	{// topDown
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp, false));
		MemoryStream stream;
		source.save(stream, Bitmap::FileFormat::png);
		File file1(dir, L"file1.bmp");
		stream.position(0);
		file1.create().write(stream.buffer(), 0, static_cast<int>(stream.length()));
		Bitmap bitmap(file1.openRead(), false);
		testAssert(!bitmap.bottomUp());
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
	}
}


testCase(HBITMAPAndWidthAndHeightAndFormatConstruct) {
	// 무효한 파라미터
	testAssertionFailed(Bitmap(nullptr, 1, 1));
	testAssertionFailed(Bitmap(Bitmap(10, 10, Bitmap::Format::palette8bpp), 1, 1, Bitmap::Format::palette4bpp));

	{// DDB
		Bitmap source = getTestBitmap(Bitmap::createDDB(300, 200));
		Bitmap bitmap(source, source.width() * 2, source.height() * 2, source.format());
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.size() == source.size() * 2);
		testAssert(!bitmap.isDDB());
		//testAssert(bitmap.equalsBits(source));
		bitmap = Bitmap(bitmap, source.width(), source.height(), source.format());
		source = Bitmap::toDIB(source);
		auto result = analyze(bitmap, source);
		testAssert(result.averageError == 0); // 축소 하프톤 처리로 오차
		bitmap = Bitmap(bitmap, source.width(), source.height(), source.format(), false);
		testAssert(!bitmap.bottomUp());
	}
	{// DIB
		Bitmap source = getTestBitmap(Bitmap(300, 200, Bitmap::Format::rgb24bpp));
		Bitmap bitmap(source, source.width() * 2, source.height() * 2, Bitmap::Format::rgb24bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.size() == source.size() * 2);
		testAssert(!bitmap.isDDB());
		//testAssert(bitmap.equalsBits(source));
		bitmap = Bitmap(bitmap, source.width(), source.height(), Bitmap::Format::rgb24bpp);
		auto result = analyze(bitmap, source);
		testAssert(result.averageError == 0); // 축소 하프톤 처리로 오차
		bitmap = Bitmap(bitmap, source.width(), source.height(), Bitmap::Format::rgb24bpp, false);
		testAssert(!bitmap.bottomUp());
	}
	{// 팔레트 없음
		Bitmap source = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		Bitmap bitmap(source, 10, 10, Bitmap::Format::argb32bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.size() == Size(10, 10));
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);
		testAssert(Bitmap(bitmap, 10, 10, Bitmap::Format::rgb24bpp).equalsBits(source));
		bitmap = Bitmap(bitmap, source.width(), source.height(), Bitmap::Format::argb32bpp, false);
		testAssert(!bitmap.bottomUp());
	}
	{// 긬깒긞긣궇귟
		Bitmap source = getTestBitmap(Bitmap(10, 10, Bitmap::Format::palette4bpp));
		Bitmap bitmap(source, 10, 10, Bitmap::Format::palette8bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.size() == Size(10, 10));
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.format() == Bitmap::Format::palette8bpp);
		bitmap = Bitmap(bitmap, source.width(), source.height(), Bitmap::Format::palette8bpp, false);
		testAssert(!bitmap.bottomUp());
	}

	{// Size 버전
		Bitmap source = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		Bitmap bitmap(source, source.size(), Bitmap::Format::argb32bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.size() == Size(10, 10));
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);
		testAssert(Bitmap(bitmap, 10, 10, Bitmap::Format::rgb24bpp).equalsBits(source));
		bitmap = Bitmap(bitmap, source.width(), source.height(), Bitmap::Format::argb32bpp, false);
		testAssert(!bitmap.bottomUp());
	}
}


testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) {
	{// 무효한 파라미터
		char buffer[3] = {0};
		testAssertionFailed(Bitmap bitmap(0, 1, Bitmap::Format::rgb16bpp, true, nullptr, 0));
		testAssertionFailed(Bitmap bitmap(1, 0, Bitmap::Format::rgb16bpp, true, nullptr, 0));
		testNoThrow        (Bitmap bitmap(1, 1, Bitmap::Format::rgb16bpp, true, nullptr, 0));
		testAssertionFailed(Bitmap bitmap(1, 1, Bitmap::Format::rgb24bpp, true, buffer , 2));
		testNoThrow        (Bitmap bitmap(1, 1, Bitmap::Format::rgb24bpp, true, buffer , 3));
	}

	{// DIB 라면 거대 사이즈 작성도 OK
		testNoThrow(Bitmap bitmap(10000, 10000, Bitmap::Format::argb32bpp, nullptr, 0));
		// 라고 하지만 40GB 화상은 만들 수 옶다
		//testThrow(Bitmap bitmap(100000, 100000, 0, Bitmap::Format::argb32bpp, nullptr), OutOfMemoryException); // OutoOfMemory가 보고 되지도 않고, 불안정. 메모리가 부족하면 불안정 하는 듯
	}

	// 각 픽셀 포맷의 DIB 작성 검즘
	{// Bitmap::Format::palette1bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::palette1bpp, false));
		Bitmap bitmap(17, 40, Bitmap::Format::palette1bpp, false, source.pixels(), source.stride(), false);
		bitmap.palette(source.palette());
		testAssert(bitmap.equalsBits(source));
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::palette1bpp);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 1);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Color palette[] = {Color::red(), Color::blue()};
		bitmap.palette(palette);
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(0, 0, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x / 8)) == 0x80);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x / 8)) == 0x80);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x / 8)) == 0x80);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x / 8)) == 0x80);
	}
	{// Bitmap::Format::palette4bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::palette4bpp, false));
		Bitmap bitmap(17, 40, Bitmap::Format::palette4bpp, true, source.pixels(), source.stride(), false);
		bitmap.palette(source.palette());
		testAssert(bitmap.equalsBits(Bitmap(source, source.size(), source.format())));
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::palette4bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 4);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Color palette[] = {Color::red(), Color::blue(), Color::lime(), Color::white(), Color::black()};
		bitmap.palette(palette);
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int right = bitmap.width() - 1;
		int bottom = bitmap.height() - 1;
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, bottom) == Color::red());
		graphics.setPixel(0, bottom, Color::blue());
		testAssert(graphics.getPixel(x, bottom) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x10);
		x = right;
		y = 0;
		testAssert(graphics.getPixel(x, bottom) == Color::red());
		graphics.setPixel(x, bottom, Color::lime());
		testAssert(graphics.getPixel(x, bottom) == Color::lime());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x20);
		x = 0;
		y = bottom;
		testAssert(graphics.getPixel(x, 0) == Color::red());
		graphics.setPixel(x, 0, Color::white());
		testAssert(graphics.getPixel(x, 0) == Color::white());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x30);
		x = right;
		y = bottom;
		testAssert(graphics.getPixel(x, 0) == Color::red());
		graphics.setPixel(x, 0, Color::black());
		testAssert(graphics.getPixel(x, 0) == Color::black());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x40);
	}
	{// Bitmap::Format::palette8bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::palette8bpp, true));
		Bitmap bitmap(17, 40, Bitmap::Format::palette8bpp, false, source.pixels(), source.stride(), true);
		bitmap.palette(source.palette());
		testAssert(bitmap.equalsBits(Bitmap(source, source.size(), source.format(), false)));
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::palette8bpp);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 8);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Color palette[] = {Color::red(), Color::blue(), Color::lime(), Color::white(), Color::black()};
		bitmap.palette(palette);
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(0, 0, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x1);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x2);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x3);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::red());
		graphics.setPixel(x, y, Color::black());
		testAssert(graphics.getPixel(x, y) == Color::black());
		Graphics::flush();
		testAssert(*(bits + stride * y + (x * bitCount / 8)) == 0x4);
	}
	{// Bitmap::Format::rgb16bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::rgb16bpp, true));
		Bitmap bitmap(17, 40, Bitmap::Format::rgb16bpp, true, source.pixels(), source.stride(), true);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::rgb16bpp);
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 16);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int right = bitmap.width() - 1;
		int bottom = bitmap.height() - 1;
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, bottom) == Color::black());
		graphics.setPixel(0, bottom, Color::red());
		testAssert(graphics.getPixel(x, bottom) == Color::red());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x7C00);
		x = right;
		y = 0;
		testAssert(graphics.getPixel(x, bottom) == Color::black());
		graphics.setPixel(x, bottom, Color::lime());
		testAssert(graphics.getPixel(x, bottom) == Color::lime());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x03E0);
		x = 0;
		y = bottom;
		testAssert(graphics.getPixel(x, 0) == Color::black());
		graphics.setPixel(x, 0, Color::blue());
		testAssert(graphics.getPixel(x, 0) == Color::blue());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x001F);
		x = right;
		y = bottom;
		testAssert(graphics.getPixel(x, 0) == Color::black());
		graphics.setPixel(x, 0, Color::white());
		testAssert(graphics.getPixel(x, 0) == Color::white());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x7FFF);
	}
	{// Bitmap::Format::rgb16bpp565
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::rgb16bpp565, false));
		Bitmap bitmap(17, 40, Bitmap::Format::rgb16bpp565, false, source.pixels(), source.stride(), false);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::rgb16bpp565);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 16);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(0, 0, Color::red());
		testAssert(graphics.getPixel(x, y) == Color::red());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0xF800);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x07E0);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0x001F);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned short*>(bits + stride * y + (x * bitCount / 8)) == 0xFFFF);
	}
	{// Bitmap::Format::rgb24bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::rgb24bpp, false));
		Bitmap bitmap(17, 40, Bitmap::Format::rgb24bpp, false, source.pixels(), source.stride(), false);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::rgb24bpp);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 24);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(0, 0, Color::red());
		testAssert(graphics.getPixel(x, y) == Color::red());
		Graphics::flush();
		unsigned char* color = bits + stride * y + (x * bitCount / 8);
		testAssert(color[0] == 0x00 && color[1] == 0x00 && color[2] == 0xFF);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		color = bits + stride * y + (x * bitCount / 8);
		testAssert(color[0] == 0x00 && color[1] == 0xFF && color[2] == 0x00);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		color = bits + stride * y + (x * bitCount / 8);
		testAssert(color[0] == 0xFF && color[1] == 0x00 && color[2] == 0x00);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		color = bits + stride * y + (x * bitCount / 8);
		testAssert(color[0] == 0xFF && color[1] == 0xFF && color[2] == 0xFF);
	}
	{// Bitmap::Format::argb32bpp
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::argb32bpp, false));
		Bitmap bitmap(17, 40, Bitmap::Format::argb32bpp, false, source.pixels(), source.stride(), false);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 32);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(0, 0, Color::red());
		testAssert(graphics.getPixel(x, y) == Color::red());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x00FF0000);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x0000FF00);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x000000FF);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x00FFFFFF);
	}

	{// Size 버전
		Bitmap source = getTestBitmap(Bitmap(17, 40, Bitmap::Format::argb32bpp, false));
		Bitmap bitmap(Size(17, 40), Bitmap::Format::argb32bpp, false, source.pixels(), source.stride(), false);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == Bitmap::Format::argb32bpp);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 32);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(0, 0, Color::red());
		testAssert(graphics.getPixel(x, y) == Color::red());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x00FF0000);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x0000FF00);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x000000FF);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x00FFFFFF);
	}

	{// 컬러 마스크 지정
		Bitmap::Format format(32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
		Bitmap source = getTestBitmap(Bitmap(17, 40, format, false));
		Bitmap bitmap(17, 40, format, false, source.pixels(), source.stride(), false);
		testAssert(bitmap.width() == 17);
		testAssert(bitmap.height() == 40);
		testAssert(bitmap.size() == Size(17, 40));
		testAssert(bitmap.format() == format);
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.owned());
		testAssert(bitmap.bitsPerPixel() == 32);
		testAssert(!bitmap.isDDB());
		testAssert(((bitmap.width() * bitmap.bitsPerPixel() + 7) / 8) <= bitmap.stride());

		// 메모리 배치 검증
		Graphics::flush();
		unsigned char* bits = reinterpret_cast<unsigned char*>(bitmap.pixels());
		std::memset(bits, 0, bitmap.stride() * bitmap.height());
		int stride = bitmap.stride();
		int bitCount = bitmap.bitsPerPixel();
		int x = 0;
		int y = 0;
		Graphics graphics(bitmap);
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(0, 0, Color::red());
		testAssert(graphics.getPixel(x, y) == Color::red());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0xFF000000);
		x = bitmap.width() - 1;
		y = 0;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::lime());
		testAssert(graphics.getPixel(x, y) == Color::lime());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x00FF0000);
		x = 0;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::blue());
		testAssert(graphics.getPixel(x, y) == Color::blue());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0x0000FF00);
		x = bitmap.width() - 1;
		y = bitmap.height() - 1;
		testAssert(graphics.getPixel(x, y) == Color::black());
		graphics.setPixel(x, y, Color::white());
		testAssert(graphics.getPixel(x, y) == Color::white());
		Graphics::flush();
		testAssert(*reinterpret_cast<unsigned int*>(bits + stride * y + (x * bitCount / 8)) == 0xFFFFFF00);
	}
}


testCase(destruct) {
	balor::test::HandleLeakChecker checker;
	{// 빈 Bitmap
		Bitmap bitmap;
		testAssert(checker.getGdiHandleChange() == 0);
	}
	testAssert(checker.getGdiHandleChange() == 0);
	{// 핸들 소유
		Bitmap bitmap = Bitmap::createDDB(1, 1);
		testAssert(checker.getGdiHandleChange() == 1);
	}
	testAssert(checker.getGdiHandleChange() == 0);
	{// 핸들 미 소유 
		Bitmap bitmap = Bitmap::createDDB(1, 1);
		testAssert(checker.getGdiHandleChange() == 1);
		{
			Bitmap refer(static_cast<HBITMAP>(bitmap));
			testAssert(checker.getGdiHandleChange() == 1);
		}
		testAssert(checker.getGdiHandleChange() == 1);
	}
	testAssert(checker.getGdiHandleChange() == 0);
}


testCase(operatorRvalueAssignment) {
	{
		Bitmap source;
		Bitmap bitmap;
		Bitmap& result = bitmap = move(source);
		testAssert(&result == &bitmap);
		testAssert(!bitmap);
		testAssert(!bitmap.owned());
		testAssert(!source);
		testAssert(!source.owned());
	}
	{
		Bitmap source = getTestBitmap(Bitmap(450, 601, Bitmap::Format::rgb24bpp));
		Bitmap bitmap;
		Bitmap& result = bitmap = move(source);
		testAssert(&result == &bitmap);
		testAssert(bitmap);
		testAssert(bitmap.owned());
		testAssert(!source);
		testAssert(!source.owned());
		Bitmap source2 = getTestBitmap(Bitmap(450, 601, Bitmap::Format::rgb24bpp));
		testAssert(bitmap.equalsBits(source2));
	}
}


testCase(bitsPerPixel) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.bottomUp());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct)을 참조
}


testCase(bottomUp) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.bitsPerPixel());

	testAssert( Bitmap(10, 10, Bitmap::Format::palette1bpp       ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::palette1bpp, false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::palette4bpp       ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::palette4bpp, false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::palette8bpp       ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::palette8bpp, false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::rgb16bpp          ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::rgb16bpp   , false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::rgb16bpp565       ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::rgb16bpp565, false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::rgb24bpp          ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::rgb24bpp   , false).bottomUp());
	testAssert( Bitmap(10, 10, Bitmap::Format::argb32bpp          ).bottomUp());
	testAssert(!Bitmap(10, 10, Bitmap::Format::argb32bpp   , false).bottomUp());
}


testCase(clone) {
	{
		Bitmap source;
		Bitmap bitmap = source.clone();
		testAssert(!bitmap);
	}
	{// DDB는 DDB로
		Bitmap source = getTestBitmap(Bitmap::createDDB(10, 10));
		Bitmap bitmap = source.clone();
		testAssert(source);
		testAssert(bitmap);
		testAssert(bitmap.equalsBits(source));
		testAssert(source.isDDB());
		testAssert(bitmap.isDDB());
		testAssert(bitmap.owned());
	}
	{// DIB는 DIB로
		Bitmap source = getTestBitmap(Bitmap(11, 14, Bitmap::Format::argb32bpp));
		Bitmap bitmap = source.clone();
		testAssert(bitmap.equalsBits(source));
		testAssert(!source.isDDB());
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
	}
	{// DIB는 팔레트도 복사한다
		Bitmap source = getTestBitmap(Bitmap(13, 15, Bitmap::Format::palette4bpp, false));
		Bitmap bitmap = source.clone();
		testAssert(bitmap.equalsBits(source));
		testAssert(!bitmap.isDDB());
		testAssert(!bitmap.bottomUp());
	}
	{// 핸들 참조 복사 
		Bitmap sourceSource = getTestBitmap(Bitmap(11, 14, Bitmap::Format::argb32bpp));
		Bitmap source(static_cast<HBITMAP>(sourceSource));
		Bitmap bitmap = source.clone();
		testAssert(bitmap.equalsBits(source));
		testAssert((HBITMAP)bitmap != (HBITMAP)source);
		testAssert(bitmap.owned());
	}
}


testCase(cloneWithHBITMAP) {
	// 무효한 파라미터
	testAssertionFailed(Bitmap::clone(nullptr));

	{// DDB > DDB
		Bitmap source = getTestBitmap(Bitmap::createDDB(101, 99));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::clone(sourceHandle);
		testAssert(bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
		testAssert(bitmap != sourceHandle);
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
	}
	{// DIB > DIB
		Bitmap source = getTestBitmap(Bitmap(102, 11, Bitmap::Format::rgb16bpp, false));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::clone(sourceHandle);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
		testAssert(bitmap != sourceHandle);
		testAssert(bitmap.owned());
		testAssert(!bitmap.bottomUp());
	}
	{// DIB > DIB 팔레트 복사
		Bitmap source = getTestBitmap(Bitmap(102, 11, Bitmap::Format::palette8bpp));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::clone(sourceHandle);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(source));
		testAssert(bitmap != sourceHandle);
		testAssert(bitmap.bottomUp());
	}
}


testCase(createDDB) {
	// 무효한 파라미터
	testAssertionFailed(Bitmap::createDDB(0, 1));
	testAssertionFailed(Bitmap::createDDB(1, 0));
	testNoThrow(Bitmap::createDDB(1, 1));

	// 만약 모니터가 256색 모드를 선택할 수 있는 레거시 하드라면 assert에 실패하는 것을 테스트 할 수 있다
	// DDB의 팔레트 모드는 지원하지 않는다
	{
		Bitmap bitmap = Bitmap::createDDB(101, 203);
		testAssert(bitmap.size() == Size(101, 203));
		testAssert(bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
	}

	{// Size 버전
		Bitmap bitmap = Bitmap::createDDB(Size(101, 203));
		testAssert(bitmap.size() == Size(101, 203));
		testAssert(bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
	}
}


testCase(equalsBits) {
	{// 무효한 파라미터
		Bitmap bitmap = getTestBitmap(Bitmap::createDDB(10, 10));
		testAssertionFailed(bitmap.equalsBits(Bitmap()));
		testAssertionFailed(Bitmap().equalsBits(bitmap));
	}
	{// 사이즈 불일치 
		Bitmap bitmap = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		testAssert(!Bitmap::equalsBits(bitmap, Bitmap(bitmap, bitmap.width() - 1, bitmap.height()    , bitmap.format())));
		testAssert(!Bitmap::equalsBits(bitmap, Bitmap(bitmap, bitmap.width()    , bitmap.height() - 1, bitmap.format())));
		testAssert( Bitmap::equalsBits(bitmap, Bitmap(bitmap, bitmap.width()    , bitmap.height()    , bitmap.format())));
	}
	{// 포맷 불일치
		Bitmap bitmap = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		testAssert(!Bitmap::equalsBits(bitmap, Bitmap(bitmap, bitmap.width(), bitmap.height(), Bitmap::Format::argb32bpp)));
		testAssert( Bitmap::equalsBits(bitmap, Bitmap(bitmap, bitmap.width(), bitmap.height(), bitmap.format()         )));
	}
	{// DDB와 DIB 불일치
		Bitmap ddb0 = getTestBitmap(Bitmap::createDDB(10, 10));
		Bitmap ddb1 = ddb0.clone();
		Bitmap dib0 = Bitmap::toDIB(ddb0);
		Bitmap dib1 = Bitmap::toDIB(ddb0);
		testAssert(!ddb0.equalsBits(dib0));
		testAssert(!dib0.equalsBits(ddb0));
		testAssert( ddb0.equalsBits(ddb1));
		testAssert( dib0.equalsBits(dib1));
	}
	{// 팔레트 불일치
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::palette1bpp));
		Bitmap rhs = lhs.clone();
		Color palette0[] = {Color::black(), Color::red()};
		Color palette1[] = {Color::black(), Color::white()};
		lhs.palette(palette0);
		rhs.palette(palette0);
		testAssert( lhs.equalsBits(rhs));
		lhs.palette(palette1);
		testAssert(!lhs.equalsBits(rhs));
		lhs.palette(palette0);
		testAssert( lhs.equalsBits(rhs));
	}
	{// 보톰업 불일치 
		Bitmap bitmap0 = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		Bitmap bitmap1 = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp, false));
		Bitmap bitmap2 = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		Bitmap bitmap3 = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp, false));
		testAssert(!bitmap0.equalsBits(bitmap1));
		testAssert( bitmap0.equalsBits(bitmap2));
		testAssert( bitmap1.equalsBits(bitmap3));
	}
	// 픽셀 불일치 화소 4 구석에 대해서도 체크 
	{// DDB
		Bitmap lhs = getTestBitmap(Bitmap::createDDB(10, 10));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format1bppIndexed
		Bitmap lhs(10, 10, Bitmap::Format::palette1bpp);
		Color palette[] = {Color::black(), Color::blue()};
		lhs.palette(palette);
		{
			Graphics g(lhs);
			g.pen(Color::black());
			g.brush(Color::black());
			g.drawRectangle(0, 0, 10, 10);
		}
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format4bppIndexed
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::palette4bpp));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format8bppIndexed
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::palette8bpp));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format16bppRgb555
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb16bpp));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format16bppRgb565
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb16bpp565));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format24bppRgb
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb24bpp));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}
	{// format32bppRgb
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::argb32bpp));
		Bitmap rhs = lhs.clone();
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 0, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 0, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(0, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(0, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
		Graphics(lhs).setPixel(9, 9, Color::blue());
		testAssert(!lhs.equalsBits(rhs));
		Graphics(rhs).setPixel(9, 9, Color::blue());
		testAssert( lhs.equalsBits(rhs));
	}

	// 마스크 비트 검증
	{// format1bppIndexed
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::palette1bpp));
		Bitmap rhs = lhs.clone();
		unsigned char* lbits = reinterpret_cast<unsigned char*>(lhs.pixels());
		unsigned char* rbits = reinterpret_cast<unsigned char*>(rhs.pixels());
		std::memset(lbits, 0, lhs.stride() * 10);
		std::memset(rbits, 0, rhs.stride() * 10);
		testAssert( lhs.equalsBits(rhs));
		lbits[1] |= 0x01;
		testAssert( lhs.equalsBits(rhs));
		lbits[1] |= 0x20;
		testAssert( lhs.equalsBits(rhs));
		lbits[1] |= 0x40;
		testAssert(!lhs.equalsBits(rhs));
	}
	{// format16bppRgb555
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::rgb16bpp));
		Bitmap rhs = lhs.clone();
		unsigned short* lbits = reinterpret_cast<unsigned short*>(lhs.pixels());
		unsigned short* rbits = reinterpret_cast<unsigned short*>(rhs.pixels());
		std::memset(lbits, 0, lhs.stride() * 10);
		std::memset(rbits, 0, rhs.stride() * 10);
		testAssert( lhs.equalsBits(rhs));
		lbits[0] |= 0x8000;
		testAssert( lhs.equalsBits(rhs));
		lbits[0] |= 0x4000;
		testAssert(!lhs.equalsBits(rhs));
	}
	{// format32bppRgb
		Bitmap lhs = getTestBitmap(Bitmap(10, 10, Bitmap::Format::argb32bpp));
		Bitmap rhs = lhs.clone();
		unsigned int* lbits = reinterpret_cast<unsigned int*>(lhs.pixels());
		unsigned int* rbits = reinterpret_cast<unsigned int*>(rhs.pixels());
		std::memset(lbits, 0, lhs.stride() * 10);
		std::memset(rbits, 0, rhs.stride() * 10);
		testAssert( lhs.equalsBits(rhs, true));
		lbits[0] |= 0xFF000000;
		testAssert( lhs.equalsBits(rhs, true));
		lbits[0] |= 0x00800000;
		testAssert(!lhs.equalsBits(rhs, true));
	}
}


testCase(format) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.format());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}

testCase(height) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.height());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}


testCase(owned) {
	Bitmap bitmap = Bitmap::createDDB(10, 10);
	testAssert(bitmap.owned());
	bitmap.owned(false);
	testAssert(!bitmap.owned());
	HBITMAP handle = bitmap;
	scopeExit([&] () {
		verify(DeleteObject(handle) != 0);
	});
}


#pragma warning (push)
#pragma warning (disable : 4189)
testCase(palette) {
	{// 무효한 파라미터
		Bitmap emptyBitmap;
		Bitmap ddbBitmap = Bitmap::createDDB(10, 10);
		Bitmap bitmap2(10, 10, Bitmap::Format::palette1bpp);
		Bitmap bitmap8(10, 10, Bitmap::Format::palette8bpp);
		Bitmap bitmap16(10, 10, Bitmap::Format::rgb16bpp);
		Color palette2[] = {Color::black(), Color::white()};
		Color palette3[] = {Color::black(), Color::white(), Color::red()};
		// getter
		testAssertionFailed(emptyBitmap.palette());
		testAssertionFailed(ddbBitmap.palette());
		testAssertionFailed(bitmap16.palette());
		testNoThrow        (bitmap8.palette());
		// setter
		testAssertionFailed(emptyBitmap.palette(palette2));
		testAssertionFailed(ddbBitmap.palette(palette2));
		testAssertionFailed(bitmap16.palette(palette2));
		testNoThrow        (bitmap8.palette(palette2));
		testAssertionFailed(bitmap2.palette(vector<Color>()));
		testAssertionFailed(bitmap2.palette(palette3));
		testNoThrow        (bitmap2.palette(palette2));
	}

	{
		Bitmap bitmap(10, 10, Bitmap::Format::palette1bpp);

		// 설정 하지 않아도 팔레트는 가지고 있다
		testAssert(bitmap.palette().size() == 2);

		// 설정한 팔레트를 올바르게 얻을 수 있다
		Color sourcePalette[] = {Color::black(), Color::white()};
		bitmap.palette(sourcePalette);
		auto palette = bitmap.palette();
		testAssert(palette.size() == 2);
		testAssert(palette[0] == sourcePalette[0]);
		testAssert(palette[1] == sourcePalette[1]);

		// 비트 수 보다 적은 팔레트를 설정하여도 수는 줄어들지 않는다
		Color sourcePalette2[] = {Color::red()};
		bitmap.palette(sourcePalette2);
		palette = bitmap.palette();
		testAssert(palette.size() == 2);
		testAssert(palette[0] == sourcePalette2[0]);
		testAssert(palette[1] == sourcePalette[1]);
	}

	// 그 외 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}
#pragma warning (pop)


#pragma warning (push)
#pragma warning (disable : 4189)
testCase(pixels) {
	// 무효한 파라미터
	Bitmap bitmap;
	const Bitmap& constBitmap = bitmap;
	testAssertionFailed(bitmap.pixels());
	testAssertionFailed(constBitmap.pixels());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}
#pragma warning (pop)


testCase(saveStreamAndFormatAndQuality) {
	{// 무효한 파라미터
		Bitmap bitmap = Bitmap::createDDB(10, 10);
		MemoryStream stream;
		char buffer[1] = {0};
		MemoryStream cantWriteStream(buffer, false);
		testAssertionFailed(Bitmap().save(stream, Bitmap::FileFormat::bmp));
		testAssertionFailed(bitmap.save(cantWriteStream, Bitmap::FileFormat::bmp));
		testAssertionFailed(bitmap.save(stream, Bitmap::FileFormat::_enum(-1)));
		testAssertionFailed(bitmap.save(stream, Bitmap::FileFormat::bmp, -1));
		testNoThrow        (bitmap.save(stream, Bitmap::FileFormat::bmp, 0));
		testAssertionFailed(bitmap.save(stream, Bitmap::FileFormat::bmp, 101));
		testNoThrow        (bitmap.save(stream, Bitmap::FileFormat::bmp, 100));
	}

	

	{// quality 검증
		Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
		MemoryStream stream000, stream030, stream100;
		source.save(stream000, Bitmap::FileFormat::jpeg,   0);
		source.save(stream030, Bitmap::FileFormat::jpeg,  30);
		source.save(stream100, Bitmap::FileFormat::jpeg, 100);
		testAssert(stream000.length() < stream030.length());
		testAssert(stream030.length() < stream100.length());
		stream000.position(0);
		stream030.position(0);
		stream100.position(0);
		Bitmap bitmap000(stream000);
		Bitmap bitmap030(stream030);
		Bitmap bitmap100(stream100);
		auto result000 = analyze(bitmap000, source);
		auto result030 = analyze(bitmap030, source);
		auto result100 = analyze(bitmap100, source);
		testAssert(result100.averageError < result030.averageError);
		testAssert(result030.averageError < result000.averageError);
	}
}


testCase(saveRvalueStreamAndFormatAndQuality) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 무효한 파라미터
	testAssertionFailed(Bitmap().save(MemoryStream(), Bitmap::FileFormat::bmp));

	File file(dir, L"file.png");
	Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
	source.save(file.create(), Bitmap::FileFormat::png);
	Bitmap bitmap(file.openRead());
	testAssert(bitmap.equalsBits(source));
}


testCase(saveFileNameAndQuality) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	{// 무효한 파라미터
		Bitmap bitmap = Bitmap::createDDB(10, 10);
		testAssertionFailed(Bitmap().save(L" "));
		testAssertionFailed(bitmap.save(L""));
	}

	// 확장자로 각 화상 포맷 판별을 검증
	{
		Bitmap bitmap = getTestBitmap(Bitmap::createDDB(100, 100));
		File file(dir, L"file.bmp");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::bmp);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::palette8bpp));
		File file(dir, L"file.gif");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::gif);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::rgb24bpp));
		File file(dir, L"file.jpg");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::jpeg);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::rgb24bpp));
		File file(dir, L"file.jpeg");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::jpeg);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::rgb24bpp));
		File file(dir, L"file.jpe");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::jpeg);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::argb32bpp));
		File file(dir, L"file.png");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::png);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::palette1bpp));
		File file(dir, L"file.tif");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::tiff);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::palette1bpp));
		File file(dir, L"file.tiff");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::tiff);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}

	// 뭔지 알 수 없는 확장자는 png가 된다
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::rgb24bpp));
		File file(dir, L"file.wst");
		bitmap.save(file.path());
		testAssert(file.exists());
		MemoryStream stream0;
		MemoryStream stream1;
		bitmap.save(stream0, Bitmap::FileFormat::png);
		{
			auto fileStream = file.openRead();
			stream1.length(static_cast<int>(fileStream.length()));
			fileStream.read(stream1.buffer(), 0, stream1.capacity());
		}
		testAssert(stream0.length() == stream1.length());
		testAssert(std::memcmp(stream0.buffer(), stream1.buffer(), static_cast<size_t>(stream0.length())) == 0);
	}

	// quality 검증
	{
		Bitmap bitmap = getTestBitmap(Bitmap(120, 120, Bitmap::Format::rgb24bpp));
		File file0(dir, L"file0.jpeg");
		File file1(dir, L"file1.jpeg");
		bitmap.save(file0.path(), 10);
		bitmap.save(file1.path(), 100);
		testAssert(file0.exists());
		testAssert(file1.exists());
		testAssert(file0.openRead().length() < file1.openRead().length());
	}
}


testCase(saveFileNameAndFormatAndQuality) {
	scopeExit(&removeTestDirectory);
	File dir = getTestDirectory();

	// 무효한 파라미터
	testAssertionFailed(Bitmap().save(L" ", Bitmap::FileFormat::bmp));

	File file(dir, L"file.png");
	Bitmap source = getTestBitmap(Bitmap(300, 300, Bitmap::Format::rgb24bpp));
	source.save(file.path(), Bitmap::FileFormat::png);
	Bitmap bitmap(file.openRead());
	testAssert(bitmap.equalsBits(source));
}


testCase(size) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.size());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}


testCase(stride) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.stride());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}


testCase(toDDB) {
	// 무효한 파라미터
	testAssertionFailed(Bitmap::toDDB(nullptr));

	{// DDB > DDB
		Bitmap source = getTestBitmap(Bitmap::createDDB(101, 99));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDDB(sourceHandle);
		testAssert(bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.equalsBits(source));
	}
	{// DIB > DDB
		Bitmap source = getTestBitmap(Bitmap(500, 255, Bitmap::Format::rgb16bpp565));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDDB(sourceHandle);
		testAssert(bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(!bitmap.equalsBits(source));
		testAssert(bitmap.size() == source.size());
	}
}


testCase(toDIB) {
	// 무효한 파라미터
	testAssertionFailed(Bitmap::toDIB(nullptr));

	{// DDB > DIB
		Bitmap source = getTestBitmap(Bitmap::createDDB(101, 99));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDIB(sourceHandle);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
		testAssert(!bitmap.equalsBits(source));
		testAssert(bitmap.size() == source.size());
		//testAssert(bitmap.format() == source.format());
	}
	{// DIB > DIB
		Bitmap source = getTestBitmap(Bitmap(5, 25, Bitmap::Format::rgb24bpp));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDIB(sourceHandle);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}
	{// DIB > DIB 팔레트 복사
		Bitmap source = getTestBitmap(Bitmap(433, 351, Bitmap::Format::palette1bpp));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDIB(sourceHandle);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}
	{// 탑 다운
		Bitmap source = getTestBitmap(Bitmap(5, 25, Bitmap::Format::rgb24bpp, false));
		HBITMAP sourceHandle = source;
		Bitmap bitmap = Bitmap::toDIB(sourceHandle, false);
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.owned());
		testAssert(!bitmap.bottomUp());
		testAssert(bitmap.equalsBits(source));
	}
}


testCase(width) {
	// 무효한 파라미터
	const Bitmap bitmap;
	testAssertionFailed(bitmap.width());

	// 정상 케이스에 대해서는 testCase(widthAndHeightAndFormatAndPixelsAndStrideConstruct) 를 참조
}



		}
	}
}