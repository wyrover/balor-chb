#include <balor/graphics/Color.hpp>

#include <balor/system/windows.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

#include <balor/test/UnitTest.hpp>
#include "../../tools/floatEquals.hpp"


namespace balor {
	namespace graphics {
		namespace testColor {

using tools::floatEquals;



testCase(construct) {
	{
		testNoThrow(Color color);
	}
	{
		Color color(1, 2, 3);
		testAssert(color.r == 1);
		testAssert(color.g == 2);
		testAssert(color.b == 3);
		testAssert(color.a == 255);
	}
	{
		Color color(1, 2, 3, 4);
		testAssert(color.r == 1);
		testAssert(color.g == 2);
		testAssert(color.b == 3);
		testAssert(color.a == 4);
	}
}


testCase(fromCOLORREFToCOLORREF) {
	testAssert(sizeof(Color::COLORREF) == sizeof(COLORREF));
	unsigned long colorref = 0x01020304;
	Color color = Color::fromCOLORREF(colorref);
	testAssert(color.r == 4);
	testAssert(color.g == 3);
	testAssert(color.b == 2);
	testAssert(color.a == 255);
	testAssert(color.toCOLORREF() == 0x00020304);
}


testCase(operatorEqualAndNotEqual) {
	testAssert(!(Color(1, 0, 0, 0) == Color(0, 0, 0, 0)));
	testAssert(!(Color(0, 1, 0, 0) == Color(0, 0, 0, 0)));
	testAssert(!(Color(0, 0, 1, 0) == Color(0, 0, 0, 0)));
	testAssert(!(Color(0, 0, 0, 1) == Color(0, 0, 0, 0)));
	testAssert(  Color(0, 0, 0, 0) == Color(0, 0, 0, 0) );

	testAssert(  Color(0, 0, 0, 1) != Color(0, 0, 0, 0) );
	testAssert(!(Color(0, 0, 0, 0) != Color(0, 0, 0, 0)));
}


//testCase(staticColors) {
//	testAssert(Color::aliceBlue()            .toArgb() == 0xFFF0F8FF);
//	testAssert(Color::antiqueWhite()         .toArgb() == 0xFFFAEBD7);
//	testAssert(Color::aqua()                 .toArgb() == 0xFF00FFFF);
//	testAssert(Color::aquamarine()           .toArgb() == 0xFF7FFFD4);
//	testAssert(Color::azure()                .toArgb() == 0xFFF0FFFF);
//	testAssert(Color::beige()                .toArgb() == 0xFFF5F5DC);
//	testAssert(Color::bisque()               .toArgb() == 0xFFFFE4C4);
//	testAssert(Color::black()                .toArgb() == 0xFF000000);
//	testAssert(Color::blanchedAlmond()       .toArgb() == 0xFFFFEBCD);
//	testAssert(Color::blue()                 .toArgb() == 0xFF0000FF);
//	testAssert(Color::blueViolet()           .toArgb() == 0xFF8A2BE2);
//	testAssert(Color::brown()                .toArgb() == 0xFFA52A2A);
//	testAssert(Color::burlyWood()            .toArgb() == 0xFFDEB887);
//	testAssert(Color::cadetBlue()            .toArgb() == 0xFF5F9EA0);
//	testAssert(Color::chartreuse()           .toArgb() == 0xFF7FFF00);
//	testAssert(Color::chocolate()            .toArgb() == 0xFFD2691E);
//	testAssert(Color::coral()                .toArgb() == 0xFFFF7F50);
//	testAssert(Color::cornflowerBlue()       .toArgb() == 0xFF6495ED);
//	testAssert(Color::cornsilk()             .toArgb() == 0xFFFFF8DC);
//	testAssert(Color::crimson()              .toArgb() == 0xFFDC143C);
//	testAssert(Color::cyan()                 .toArgb() == 0xFF00FFFF);
//	testAssert(Color::darkBlue()             .toArgb() == 0xFF00008B);
//	testAssert(Color::darkCyan()             .toArgb() == 0xFF008B8B);
//	testAssert(Color::darkGoldenrod()        .toArgb() == 0xFFB8860B);
//	testAssert(Color::darkGray()             .toArgb() == 0xFFA9A9A9);
//	testAssert(Color::darkGreen()            .toArgb() == 0xFF006400);
//	testAssert(Color::darkKhaki()            .toArgb() == 0xFFBDB76B);
//	testAssert(Color::darkMagenta()          .toArgb() == 0xFF8B008B);
//	testAssert(Color::darkOliveGreen()       .toArgb() == 0xFF556B2F);
//	testAssert(Color::darkOrange()           .toArgb() == 0xFFFF8C00);
//	testAssert(Color::darkOrchid()           .toArgb() == 0xFF9932CC);
//	testAssert(Color::darkRed()              .toArgb() == 0xFF8B0000);
//	testAssert(Color::darkSalmon()           .toArgb() == 0xFFE9967A);
//	testAssert(Color::darkSeaGreen()         .toArgb() == 0xFF8FBC8F);
//	testAssert(Color::darkSlateBlue()        .toArgb() == 0xFF483D8B);
//	testAssert(Color::darkSlateGray()        .toArgb() == 0xFF2F4F4F);
//	testAssert(Color::darkTurquoise()        .toArgb() == 0xFF00CED1);
//	testAssert(Color::darkViolet()           .toArgb() == 0xFF9400D3);
//	testAssert(Color::deepPink()             .toArgb() == 0xFFFF1493);
//	testAssert(Color::deepSkyBlue()          .toArgb() == 0xFF00BFFF);
//	testAssert(Color::dimGray()              .toArgb() == 0xFF696969);
//	testAssert(Color::dodgerBlue()           .toArgb() == 0xFF1E90FF);
//	testAssert(Color::fireBrick()            .toArgb() == 0xFFB22222);
//	testAssert(Color::floralWhite()          .toArgb() == 0xFFFFFAF0);
//	testAssert(Color::forestGreen()          .toArgb() == 0xFF228B22);
//	testAssert(Color::fuchsia()              .toArgb() == 0xFFFF00FF);
//	testAssert(Color::gainsboro()            .toArgb() == 0xFFDCDCDC);
//	testAssert(Color::ghostWhite()           .toArgb() == 0xFFF8F8FF);
//	testAssert(Color::gold()                 .toArgb() == 0xFFFFD700);
//	testAssert(Color::goldenrod()            .toArgb() == 0xFFDAA520);
//	testAssert(Color::gray()                 .toArgb() == 0xFF808080);
//	testAssert(Color::green()                .toArgb() == 0xFF008000);
//	testAssert(Color::greenYellow()          .toArgb() == 0xFFADFF2F);
//	testAssert(Color::honeydew()             .toArgb() == 0xFFF0FFF0);
//	testAssert(Color::hotPink()              .toArgb() == 0xFFFF69B4);
//	testAssert(Color::indianRed()            .toArgb() == 0xFFCD5C5C);
//	testAssert(Color::indigo()               .toArgb() == 0xFF4B0082);
//	testAssert(Color::ivory()                .toArgb() == 0xFFFFFFF0);
//	testAssert(Color::khaki()                .toArgb() == 0xFFF0E68C);
//	testAssert(Color::lavender()             .toArgb() == 0xFFE6E6FA);
//	testAssert(Color::lavenderBlush()        .toArgb() == 0xFFFFF0F5);
//	testAssert(Color::lawnGreen()            .toArgb() == 0xFF7CFC00);
//	testAssert(Color::lemonChiffon()         .toArgb() == 0xFFFFFACD);
//	testAssert(Color::lightBlue()            .toArgb() == 0xFFADD8E6);
//	testAssert(Color::lightCoral()           .toArgb() == 0xFFF08080);
//	testAssert(Color::lightCyan()            .toArgb() == 0xFFE0FFFF);
//	testAssert(Color::lightGoldenrodYellow() .toArgb() == 0xFFFAFAD2);
//	testAssert(Color::lightGreen()           .toArgb() == 0xFF90EE90);
//	testAssert(Color::lightGray()            .toArgb() == 0xFFD3D3D3);
//	testAssert(Color::lightPink()            .toArgb() == 0xFFFFB6C1);
//	testAssert(Color::lightSalmon()          .toArgb() == 0xFFFFA07A);
//	testAssert(Color::lightSeaGreen()        .toArgb() == 0xFF20B2AA);
//	testAssert(Color::lightSkyBlue()         .toArgb() == 0xFF87CEFA);
//	testAssert(Color::lightSlateGray()       .toArgb() == 0xFF778899);
//	testAssert(Color::lightSteelBlue()       .toArgb() == 0xFFB0C4DE);
//	testAssert(Color::lightYellow()          .toArgb() == 0xFFFFFFE0);
//	testAssert(Color::lime()                 .toArgb() == 0xFF00FF00);
//	testAssert(Color::limeGreen()            .toArgb() == 0xFF32CD32);
//	testAssert(Color::linen()                .toArgb() == 0xFFFAF0E6);
//	testAssert(Color::magenta()              .toArgb() == 0xFFFF00FF);
//	testAssert(Color::maroon()               .toArgb() == 0xFF800000);
//	testAssert(Color::mediumAquamarine()     .toArgb() == 0xFF66CDAA);
//	testAssert(Color::mediumBlue()           .toArgb() == 0xFF0000CD);
//	testAssert(Color::mediumOrchid()         .toArgb() == 0xFFBA55D3);
//	testAssert(Color::mediumPurple()         .toArgb() == 0xFF9370DB);
//	testAssert(Color::mediumSeaGreen()       .toArgb() == 0xFF3CB371);
//	testAssert(Color::mediumSlateBlue()      .toArgb() == 0xFF7B68EE);
//	testAssert(Color::mediumSpringGreen()    .toArgb() == 0xFF00FA9A);
//	testAssert(Color::mediumTurquoise()      .toArgb() == 0xFF48D1CC);
//	testAssert(Color::mediumVioletRed()      .toArgb() == 0xFFC71585);
//	testAssert(Color::midnightBlue()         .toArgb() == 0xFF191970);
//	testAssert(Color::mintCream()            .toArgb() == 0xFFF5FFFA);
//	testAssert(Color::mistyRose()            .toArgb() == 0xFFFFE4E1);
//	testAssert(Color::moccasin()             .toArgb() == 0xFFFFE4B5);
//	testAssert(Color::navajoWhite()          .toArgb() == 0xFFFFDEAD);
//	testAssert(Color::navy()                 .toArgb() == 0xFF000080);
//	testAssert(Color::oldLace()              .toArgb() == 0xFFFDF5E6);
//	testAssert(Color::olive()                .toArgb() == 0xFF808000);
//	testAssert(Color::oliveDrab()            .toArgb() == 0xFF6B8E23);
//	testAssert(Color::orange()               .toArgb() == 0xFFFFA500);
//	testAssert(Color::orangeRed()            .toArgb() == 0xFFFF4500);
//	testAssert(Color::orchid()               .toArgb() == 0xFFDA70D6);
//	testAssert(Color::paleGoldenrod()        .toArgb() == 0xFFEEE8AA);
//	testAssert(Color::paleGreen()            .toArgb() == 0xFF98FB98);
//	testAssert(Color::paleTurquoise()        .toArgb() == 0xFFAFEEEE);
//	testAssert(Color::paleVioletRed()        .toArgb() == 0xFFDB7093);
//	testAssert(Color::papayaWhip()           .toArgb() == 0xFFFFEFD5);
//	testAssert(Color::peachPuff()            .toArgb() == 0xFFFFDAB9);
//	testAssert(Color::peru()                 .toArgb() == 0xFFCD853F);
//	testAssert(Color::pink()                 .toArgb() == 0xFFFFC0CB);
//	testAssert(Color::plum()                 .toArgb() == 0xFFDDA0DD);
//	testAssert(Color::powderBlue()           .toArgb() == 0xFFB0E0E6);
//	testAssert(Color::purple()               .toArgb() == 0xFF800080);
//	testAssert(Color::red()                  .toArgb() == 0xFFFF0000);
//	testAssert(Color::rosyBrown()            .toArgb() == 0xFFBC8F8F);
//	testAssert(Color::royalBlue()            .toArgb() == 0xFF4169E1);
//	testAssert(Color::saddleBrown()          .toArgb() == 0xFF8B4513);
//	testAssert(Color::salmon()               .toArgb() == 0xFFFA8072);
//	testAssert(Color::sandyBrown()           .toArgb() == 0xFFF4A460);
//	testAssert(Color::seaGreen()             .toArgb() == 0xFF2E8B57);
//	testAssert(Color::seashell()             .toArgb() == 0xFFFFF5EE);
//	testAssert(Color::sienna()               .toArgb() == 0xFFA0522D);
//	testAssert(Color::silver()               .toArgb() == 0xFFC0C0C0);
//	testAssert(Color::skyBlue()              .toArgb() == 0xFF87CEEB);
//	testAssert(Color::slateBlue()            .toArgb() == 0xFF6A5ACD);
//	testAssert(Color::slateGray()            .toArgb() == 0xFF708090);
//	testAssert(Color::snow()                 .toArgb() == 0xFFFFFAFA);
//	testAssert(Color::springGreen()          .toArgb() == 0xFF00FF7F);
//	testAssert(Color::steelBlue()            .toArgb() == 0xFF4682B4);
//	testAssert(Color::tan()                  .toArgb() == 0xFFD2B48C);
//	testAssert(Color::teal()                 .toArgb() == 0xFF008080);
//	testAssert(Color::thistle()              .toArgb() == 0xFFD8BFD8);
//	testAssert(Color::tomato()               .toArgb() == 0xFFFF6347);
//	testAssert(Color::turquoise()            .toArgb() == 0xFF40E0D0);
//	testAssert(Color::violet()               .toArgb() == 0xFFEE82EE);
//	testAssert(Color::wheat()                .toArgb() == 0xFFF5DEB3);
//	testAssert(Color::white()                .toArgb() == 0xFFFFFFFF);
//	testAssert(Color::whiteSmoke()           .toArgb() == 0xFFF5F5F5);
//	testAssert(Color::yellow()               .toArgb() == 0xFFFFFF00);
//	testAssert(Color::yellowGreen()          .toArgb() == 0xFF9ACD32);
//}


		}
	}
}