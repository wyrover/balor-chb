#include <balor/io/Resource.hpp>

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Cursor.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/graphics/Pen.hpp>
#include <balor/io/File.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/system/Module.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/Rectangle.hpp>


namespace balor {
	namespace io {
		namespace testResource {

using std::move;
using namespace balor::graphics;
using namespace balor::io;
using namespace balor::locale;
using namespace balor::system;


namespace {
Bitmap getTestBitmap(Bitmap&& bitmap) { // 잘 보이고, 경계와 반전 체크를 할 수 있도록 화상을 생성한다.
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
	graphics.pen(Color::blue());
	graphics.drawRectangle( rect);
	rect.grow(-1, -1);
	graphics.brush(Color::green());
	graphics.pen(Color::green());
	graphics.drawRectangle(rect);
	graphics.setPixel(0, 0, Color::green());
	graphics.setPixel(bitmap.width() - 1, 0, Color::black());
	graphics.setPixel(0, bitmap.height() - 1, Color::yellow());
	graphics.setPixel(bitmap.width() - 1, bitmap.height() - 1, Color::aqua());
	Graphics::flush();
	return move(bitmap);
}
} // namespace



testCase(startup) {
	balor::test::UnitTest::ignoreHandleLeakCheck();
	Resource resource(Locale(L"en-US"));
	auto stream = resource.getRawData(L"balor::io::resource::test00.png");
	Bitmap bitmap(stream); // XP에서 여기 처리를 한번 통과해 두지 않으면 핸들 릭이 검출 된다.
}


testCase(construct) {
	{// 기본 생성자
		testNoThrow(Resource resource);
	}
}


testCase(getBitmap) {
	Resource resource;

	// 발견되지 않은 리소스
	testThrow(resource.getBitmap(L"balor::io::resource::lajeoiajfo0830203"), Resource::NotFoundException);

	{// RCDATA 리소스
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test00.png");
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(30, 40, Bitmap::Format::rgb24bpp))));
	}
	{// BITMAP 리소스
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test01.bmp");
		testAssert(!bitmap.isDDB());
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(30, 40, Bitmap::Format::palette8bpp))));
	}
	resource = Resource(Locale(L"en-US"));
	{// 로컬라이즈 리소스
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test02.bmp");
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::palette4bpp))));
	}
	{// 로컬라이즈 리소스를 찾을 수 없다
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test03.bmp");
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::argb32bpp)), true));
	}
	{// 로컬라이즈 리소스(RCDATA)
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test04.png");
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::palette4bpp))));
	}
	{// 로컬라이즈 리소스를 찾을 수 없다(RCDATA)
		Bitmap bitmap = resource.getBitmap(L"balor::io::resource::test05.png");
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::argb32bpp))));
	}

	// TODO: DLL 리소스에 대해서는 미 테스트 
}


testCase(getCursor) {
	balor::test::UnitTest::ignoreHandleLeakCheck(); // 콘솔 리소스는 일단 로드 되었다면 해제하여도 프로세스에 계속 남는 것 같다
	Resource resource;

	// 찾을 수 없는 리소스
	testThrow(resource.getCursor(L"balor::io::resource::la0r9840avfjoasdjifao0"), Resource::NotFoundException);

	{// 로컬라이즈 없음
		Cursor cursor = resource.getCursor(L"balor::io::resource::test08.ico");
		testAssert(cursor.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(32, 32, Bitmap::Format::argb32bpp))));
	}

	resource = Resource(Locale(L"en-US"));
	{// 로컬라이즈 리소스
		Cursor cursor = resource.getCursor(L"balor::io::resource::test09.ico");
		testAssert(cursor.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(32, 32, Bitmap::Format::argb32bpp))));
	}
	{// 로컬라이즈 리소스를 찾을 수 없다
		Cursor cursor = resource.getCursor(L"balor::io::resource::test08.ico");
		testAssert(cursor.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(32, 32, Bitmap::Format::argb32bpp))));
	}
}


testCase(getIcon) {
	balor::test::UnitTest::ignoreHandleLeakCheck(); // 콘솔 리소스는 일단 로드 되었다면 해제하여도 프로세스에 계속 남는 것 같다
	Resource resource;

	// 찾을 수 없는 리소스
	testThrow(resource.getIcon(L"balor::io::resource::la0r9840avfjoasdjifao0"), Resource::NotFoundException);

	{// 로컬라이즈 없음
		Icon icon = resource.getIcon(L"balor::io::resource::test06.ico");
		testAssert(icon.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(32, 32, Bitmap::Format::argb32bpp))));
	}

	resource = Resource(Locale(L"en-US"));
	{// 로컬라이즈 리소스
		Icon icon = resource.getIcon(L"balor::io::resource::test07.ico", 16, 16);
		testAssert(icon.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(16, 16, Bitmap::Format::argb32bpp))));
	}
	{// 로컬라이즈 리소스를 찾을 수 없다
		Icon icon = resource.getIcon(L"balor::io::resource::test06.ico");
		testAssert(icon.bitmap().toDIB().equalsBits(getTestBitmap(Bitmap(32, 32, Bitmap::Format::argb32bpp))));
	}
}


testCase(getRawData) {
	Resource resource;

	// 찾을 수 없는 리소스
	testThrow(resource.getRawData(L"balor::io::resource::la0r9840avfjoasdjifao0"), Resource::NotFoundException);

	{// getBitmap에서 데이터를 빌린다
		auto stream = resource.getRawData(L"balor::io::resource::test00.png");
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(30, 40, Bitmap::Format::rgb24bpp))));
	}

	resource = Resource(Locale(L"en-US"));
	{// 로컬라이즈 리소스
		auto stream = resource.getRawData(L"balor::io::resource::test04.png");
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::palette4bpp))));
	}
	{// 로컬라이즈 리소스를 찾을 수 없다
		auto stream = resource.getRawData(L"balor::io::resource::test05.png");
		Bitmap bitmap(stream);
		testAssert(bitmap.equalsBits(getTestBitmap(Bitmap(40, 50, Bitmap::Format::argb32bpp))));
	}
}



		}
	}
}