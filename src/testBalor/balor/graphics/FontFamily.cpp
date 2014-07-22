#include <balor/graphics/FontFamily.hpp>

#include <vector>

#include <balor/graphics/GdiplusInitializer.hpp>
#include <balor/ArgumentException.hpp>
#include <balor/scopeExit.hpp>
#include <balor/test/UnitTest.hpp>
#define ENABLE_WIN32_LEAN_AND_MEAN
#define ENABLE_MINMAX
#include <balor/system/windows.hpp> // gdiplus.hが中でmin, maxマクロを使っている！
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")


namespace balor {
	namespace graphics {
		namespace testFontFamily {


using std::move;



testCase(startup) { // ハンドルを最初の呼び出しで確保し、内部で保持し続ける（一見リークに見える）関数をあらかじめ呼んでおく
	balor::test::UnitTest::ignoreHandleLeakCheck();

	GdiplusInitializer::initialize(); // Gdiplus::GdiplusStartup関数は最初の呼び出しでだけハンドルを複数開いて保持する

	{
		FontFamily family(L"Arial"); // EnumFontFamiliesExW 関数は最初の呼び出しでだけハンドルを複数開いて保持する
		Gdiplus::FontFamily gdiFamily(L"Arial"); // この関数は最初の呼び出しでだけハンドルを複数開いて保持する
	}
}


testCase(defaultConstruct) {
	FontFamily family;
	testAssert(!family);
	testAssert(family.name() == L"");
}


testCase(rvalueConstruct) {
	FontFamily source(L"MS UI Gothic");
	FontFamily family = move(source);
	testAssert(!source);
	testAssert(family);
	testAssert(family.name() == L"MS UI Gothic");
}


testCase(nameConstruct) {
	// 無効なパラメータ
	testAssertionFailed(FontFamily(L""));

	// 見つからないフォント名
	testThrow(FontFamily(L"ありえないフォント名"), ArgumentException);

	FontFamily family(L"MS UI Gothic");
	testAssert(family);
	testAssert(family.name() == L"MS UI Gothic");
}


testCase(rvalueAssignment) {
	FontFamily source(L"MS UI Gothic");
	FontFamily family;
	family = move(source);
	testAssert(!source);
	testAssert(family);
	testAssert(family.name() == L"MS UI Gothic");

	family = move(family);
	testAssert(family);
}


testCase(getMetrics) {
	// 計算方法があてずっぽうなのでとにかく色々比較してみる
	//auto families = FontFamily::families(); // 縦書き用と、.NETでは列挙されない海外のキャラクターセットのフォントと、ＭＳ ＰＲ２ゴシック以外ではうまくいった
	//for (auto i = families.begin(), end = families.end(); i != end; ++i) {
	//const wchar_t* names[] = {L"ＭＳ ＰＲ２ゴシック"}; // これはなぜかregularスタイルをサポートしないと誤判断される。それでGdiplusの取得値が０になりうまくいかない
	const wchar_t* names[] = {L"Arial", L"MS UI Gothic", L"ＭＳ ゴシック", L"ＭＳ Ｐゴシック", L"ＭＳ 明朝", L"ＭＳ Ｐ明朝", L"メイリオ"};
	const int styles[] = {FontStyle::regular, FontStyle::bold, FontStyle::italic, FontStyle::bold | FontStyle::italic};
	for (int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
		for (int j = 0; j < sizeof(styles) / sizeof(styles[0]); ++j) {
			FontFamily family(names[i]);
			//FontFamily& family = *i;
			Gdiplus::FontFamily gdiFamily(family.name().c_str());
			const int style = styles[j];
			if (gdiFamily.GetLastStatus() != Gdiplus::Ok) {
				balor::test::UnitTest::write(L"invalid family name:");
				balor::test::UnitTest::write(family.name().c_str());
				balor::test::UnitTest::write(L"\n");
			} else {
				testAssert(family.getCellAscent(style)  == gdiFamily.GetCellAscent(style));
				testAssert(family.getCellDescent(style) == gdiFamily.GetCellDescent(style));
				testAssert(family.getEmHeight(style)    == gdiFamily.GetEmHeight(style));
				testAssert(family.getLineSpacing(style) == gdiFamily.GetLineSpacing(style));

				//auto a = family.getCellAscent(style);
				//auto b = gdiFamily.GetCellAscent(style);
				//if (gdiFamily.IsStyleAvailable(style) && a != b) {
				//	a = a;
				//	balor::test::UnitTest::write(L"error family name:");
				//	balor::test::UnitTest::write(family.name().c_str());
				//	balor::test::UnitTest::write(L"\n");
				//}
			}
		}
	}
}


testCase(families) {
	auto families = FontFamily::families();
	testAssert(!families.empty());
}


testCase(genericFamilies) {
	testAssert(FontFamily::genericMonospace().name() == L"Courier New");
	testAssert(FontFamily::genericSansSerif().name() == L"Microsoft Sans Serif");
	testAssert(FontFamily::genericSerif().name() == L"Times New Roman");
}


//testCase(name) { // testCase(construct), testCase(nameConstruct) でテスト済み
//}



		}
	}
}