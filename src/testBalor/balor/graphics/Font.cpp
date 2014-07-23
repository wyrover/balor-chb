#include <balor/graphics/Font.hpp>

#include <vector>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Brush.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Graphics.hpp>
#include <balor/graphics/Pen.hpp>
#include <balor/io/File.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/io/Resource.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/String.hpp>

#include "../../tools/floatEquals.hpp"


namespace balor {
	namespace graphics {
		namespace testFont {


using std::move;
using namespace balor::io;
using namespace balor::system;
using tools::floatEquals;


namespace {
String getSavePath() {
	return File(Module::current().directory(), L"..\\testBalor\\balor\\graphics\\font\\").path();
}


Bitmap getTestBitmap(const String& name) {
	auto stream = Resource().getRawData(L"balor::graphics::font::" + name);
	return Bitmap(stream);
}


bool makeTestData = false; // 테스트 데이터 작성. 주의! 작성 시에는 꼭 화상을 눈으로 확인할 것 


bool testFontDraw(const Font& font, int width, int height, const String& text, const String& sourceName, int x = 0, int y = 0) {
	Bitmap bitmap(width, height, Bitmap::Format::argb32bpp);
	{
		Graphics graphics(bitmap);
		graphics.brush(Brush::white());
		graphics.pen(Pen::white());
		graphics.drawRectangle(0, 0, width, height);
		graphics.font(font);
		graphics.textColor(Color::black());
		graphics.drawText(text, x, y);
	}
	if (makeTestData) {
		bitmap.save(getSavePath() + L"\\" + sourceName);
		return false;
	}
	return bitmap.equalsBits(getTestBitmap(sourceName), true);
}
} // namespace



testCase(startup) { // 핸들을 처음 호출에서 확보하고 내부에서 계속 유지하는(일견 릭으로 보이는) 함수를 처음부터 호출해 둔다 
	balor::test::UnitTest::ignoreHandleLeakCheck();

	getTestBitmap(L"test00.png"); // GdiplusStartup 함수는 처음 호출에서 핸들을 복수 열고 유지한다

	//Font::names(); // EnumFontFamiliesW 함수는 처음 호출에서 핸들을 복수 열고 유지한다

	Bitmap bitmap = Bitmap::createDDB(100, 100);
	Font font(L"ＭＳ ゴシック", 16);
	Graphics graphics(bitmap);
	graphics.font(font);
	graphics.textColor(Color::black());
	graphics.drawText(L"あい", 0, 0); // TextOutW 함수는 처음 호출에서 핸들을 복수 열고 유지한다
}


testCase(defaultConstruct) {
	Font font;
	testAssert(!font);
}


testCase(rvalueConstruct) {
	Font source(L"MS UI Gothic", 24);
	Font font = move(source);
	testAssert(!source);
	testAssert(font);
	testAssert(font.name() == L"MS UI Gothic");
}


testCase(fontAndHeightAndStyleConstruct) {
	Font source(L"MS UI Gothic", 24, Font::Style::regular);
	{
		Font font(source, 16, Font::Style::bold);
		testAssert(font);
		testAssert(font.name() == L"MS UI Gothic");
		testAssert(font.height() == 16);
		testAssert(font.style() == Font::Style::bold);
	}
	{
		Font font(source, 24, Font::Style::italic);
		testAssert(font.style() == Font::Style::italic);
	}
	{
		Font font(source, 24, Font::Style::strikeout);
		testAssert(font.style() == Font::Style::strikeout);
	}
	{
		Font font(source, 24, Font::Style::underline);
		testAssert(font.style() == Font::Style::underline);
	}
}


testCase(fontFamilyNameConstruct) {
	// 무효한 파라미터
	testAssertionFailed(Font(L"", 1));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 0));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::_enum(-1)));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::_enum(-1)));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof, -3601));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof,  3601));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof,  3600, -3601));
	testAssertionFailed(Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof,  3600,  3601));
	testNoThrow        (Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof,  3600,  3600));
	testNoThrow        (Font(L"ＭＳ ゴシック", 1, Font::Style::regular, Font::Quality::proof, -3600, -3600));

	{// 의미 없는 폰트 이름을 설정하여도 시스템이 어떻게든 해준다
		Font font(L"이상한폰트", 160);
		testAssert(font);
		testAssert(font.name() != L"이상한폰트");
	}

	// name 반영
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::regular, Font::Quality::antialiased), 400, 200, L"あい", L"test00.png"));
	testAssert(testFontDraw(Font(L"ＭＳ Ｐ明朝", 160, Font::Style::regular, Font::Quality::antialiased), 400, 200, L"あい", L"test01.png"));

	// height 반영
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 16, Font::Style::regular, Font::Quality::antialiased), 40, 20, L"あい", L"test02.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 96, Font::Style::regular, Font::Quality::antialiased), 200, 100, L"あい", L"test03.png"));

	// style 반영
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::bold, Font::Quality::antialiased), 400, 200, L"あい", L"test04.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::italic, Font::Quality::antialiased), 400, 200, L"あい", L"test05.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::strikeout, Font::Quality::antialiased), 400, 200, L"あい", L"test06.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::underline, Font::Quality::antialiased), 400, 200, L"あい", L"test07.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::_enum(0x0F), Font::Quality::antialiased), 400, 200, L"あい", L"test08.png"));

	// quality 반영(Caution: antialiased 와 nonantialiased 이외는 OS에 따라서 행동이 다르므로 눈으로 확인)
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::regular, Font::Quality::antialiased), 400, 200, L"あい", L"test09.png"));
	testAssert(testFontDraw(Font(L"ＭＳ ゴシック", 160, Font::Style::regular, Font::Quality::nonantialiased), 400, 200, L"あい", L"test10.png"));

	// 세로 쓰기 테스트
	testAssert(testFontDraw(Font(L"@ＭＳ ゴシック", 160, Font::Style::regular, Font::Quality::antialiased, 2700, 2700), 200, 600, L"궇궆갃", L"test11.png", 160));
}


testCase(destruct) {
	balor::test::HandleLeakChecker checker;
	{// 소유권을 가진 경우
		Font font(L"ＭＳ ゴシック", 16);
		testAssert(checker.getGdiHandleChange() == 1);
	}
	testAssert(checker.getGdiHandleChange() == 0);
	{// 소유권을 가지지 않은 경우
		Font font = Font::defaultGui();
		testAssert(checker.getGdiHandleChange() == 0);
	}
	testAssert(checker.getGdiHandleChange() == 0);
}


testCase(rvalueAssignment) {
	{// 소유권을 가진 경우
		Font source(L"MS UI Gothic", 24);
		Font font;
		font = move(source);
		testAssert(!source);
		testAssert(font);
		testAssert(font.name() == L"MS UI Gothic");

		font = move(font);
		testAssert(font);
	}
	{// 소유권을 가지지 않은 경우
		Font source(L"MS UI Gothic", 24);
		Font source2 = Font((HFONT)source);
		Font font;
		font = move(source2);
		testAssert(!source2);
		testAssert(font);
		testAssert(font.name() == L"MS UI Gothic");
		testAssert((HFONT)font == (HFONT)source);

		font = move(font);
		testAssert(font);
	}
}


testCase(clone) {
	{// 빈 폰트
		Font source;
		Font font = source.clone();
		testAssert(!font);
	}
	{// 소유권을 가진 핸들
		Font source(L"MS UI Gothic", 24);
		Font font = source.clone();
		testAssert(font);
		testAssert((HFONT)font != (HFONT)source);
		testAssert(source.name() == L"MS UI Gothic");
		testAssert(font.name() == source.name());
	}
	{// 소유권을 가지지 않은 핸들 
		Font source(L"MS UI Gothic", 24);
		Font source2 = Font((HFONT)source);
		Font font = source2.clone();
		testAssert(font);
		testAssert((HFONT)font != (HFONT)source2);
		testAssert(source.name() == L"MS UI Gothic");
		testAssert(font.name() == source.name());
	}
}


testCase(cloneWithHFONT) {
	// 무효한 파라미터
	testAssertionFailed(Font::clone(nullptr));

	Font source(L"MS UI Gothic", 24);
	Font font = Font::clone(source);
	testAssert(font);
	testAssert(source.name() == L"MS UI Gothic");
	testAssert(font.escapement()  == source.escapement());
	testAssert(font.height()      == source.height());
	testAssert(font.name()        == source.name());
	testAssert(font.orientation() == source.orientation());
	testAssert(font.quality()     == source.quality());
	testAssert(font.style()       == source.style());
}


testCase(escapement) {
	// 무효한 파라미터
	testAssertionFailed(Font().escapement());

	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::proof).escapement() == 0);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::proof, 2700).escapement() == 2700);
}
//
//
//testCase(fromHdc) {
//	// 무효한 파라미터
//	testAssertionFailed(Font::fromHdc(nullptr));
//
//	Bitmap bitmap(200, 200, Bitmap::PixelFormat::argb32bpp);
//	Graphics graphics(bitmap);
//	Font source(L"MS UI Gothic", 24);
//	testAssert(SelectObject(graphics, source) != nullptr);
//	Font font = Font::fromHdc(graphics);
//	testAssert(font);
//	testAssert(font.name() == L"MS UI Gothic");
//	testAssert((HFONT)font == (HFONT)source);
//}


testCase(fromHandle) {
	Font source(L"MS UI Gothic", 24);
	Font font = Font((HFONT)source);
	testAssert(font);
	testAssert(font.name() == L"MS UI Gothic");
	testAssert((HFONT)font == (HFONT)source);
}


testCase(fromLogFont) {
	Font source(L"MS UI Gothic", 24);
	LOGFONTW logfont;
	memset(&logfont, 0, sizeof(logfont));
	//testThrow(Font::fromLogFont(logfont), Win32Exception); // 성공해버렸다..

	testAssert(GetObjectW(source, sizeof(logfont), &logfont) != 0);
	Font font = Font::fromLogFont(logfont);
	testAssert(font);
	testAssert(font.name() == L"MS UI Gothic");
	testAssert(font.height() == 24);
}


testCase(height) {
	// 무효한 파라미터
	testAssertionFailed(Font().height());

	testAssert(Font(L"ＭＳ ゴシック", 16).height() == 16);
	testAssert(Font(L"ＭＳ ゴシック", 160).height() == 160);
}


testCase(name) {
	// 무효한 파라미터
	testAssertionFailed(Font().name());

	testAssert(Font(L"ＭＳ ゴシック", 16).name() == L"ＭＳ ゴシック");
	testAssert(Font(L"ＭＳ Ｐ明朝", 16).name() == L"ＭＳ Ｐ明朝");
}


testCase(names) {
	auto names = Font::names();
	testAssert(!names.empty());
}


testCase(orientation) {
	// 무효한 파라미터
	testAssertionFailed(Font().orientation());

	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::proof).orientation() == 0);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::proof, 0, 2700).orientation() == 2700);
}


testCase(quality) {
	// 무효한 파라미터
	testAssertionFailed(Font().quality());

	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular).quality() == Font::Quality::proof);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::default         ).quality() == Font::Quality::default);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::draft           ).quality() == Font::Quality::draft);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::proof           ).quality() == Font::Quality::proof);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::nonantialiased  ).quality() == Font::Quality::nonantialiased);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::antialiased     ).quality() == Font::Quality::antialiased);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::clearType       ).quality() == Font::Quality::clearType);
	//testAssert(Font(L"MS UI Gothic", 24, Font::Style::regular, Font::Quality::clearTypeNatural).quality() == Font::Quality::clearTypeNatural);
}


testCase(style) {
	// 무효한 파라미터
	testAssertionFailed(Font().style());

	testAssert(Font(L"MS UI Gothic", 24).style() == Font::Style::regular);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::bold     ).style() == Font::Style::bold);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::italic   ).style() == Font::Style::italic);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::strikeout).style() == Font::Style::strikeout);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::underline).style() == Font::Style::underline);
	testAssert(Font(L"MS UI Gothic", 24, Font::Style::_enum(0x0F)).style() == 0x0F);
}



		}
	}
}