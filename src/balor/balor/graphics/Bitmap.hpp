#pragma once

#include <balor/ArrayRange.hpp>
#include <balor/Enum.hpp>
#include <balor/NonCopyable.hpp>
#include <balor/OutOfMemoryException.hpp>
#include <balor/StringRange.hpp>

struct HBITMAP__;

namespace std {
template<typename T> class allocator;
template<typename T, typename A> class vector;
}

namespace balor {
	namespace io {
		class Stream;
	}
	class Point;
	class Rectangle;
	class Size;
}


namespace balor {
	namespace graphics {

class Color;



/**
 * GDI 비트맵을 표시한다.
 * 
 * 이미지 복사는 Graphics::copy 함수를 색 취득에는 Graphics::getPixel 함수를 사용할 수 있다. 혹은 pixels 함수로 자력으로 처리한다.
 * 파일이나 스트림으로의 출력은 GDI+을 사용하고, bmp, gif, jpeg, png, tiff 파일 형식 및 알파 텍스쳐 첨부 이미지 형식을 지원한다.
 * 생성자에서 작성한 경우는 보통 DIB 비트맵을 만들 때는 createDDB 또는 toDDB 함수를 사용한다.
 * DDB는 일정 이상의 크기(예를 들면 4096 * 4096)로 만들거나 일정 이상의 용량(그러나 메인 메모리에는 충분히 들어가는)을 넘으면 
 * OutOfMemoryException을 발생시킨다. 이것은 주로 비디오 카드 제한에 의한다. 팔렛을 사용하는 DDB는 지원하지 않는다.
 * 인수로 HBITMAP을 넘기는 함수나 bottomUp, palette, save 함수 등은 HBITMAP 이나 Bitmap이 Graphics 클래스에 참조된채로 있으면 함수가 실패하므로 주의한다.
 * 
 * ※ DDB와는 디스플레이와 같은 픽셀 포맷을 가지고, 고속으로 화면에 렌더링 할수 있지만 내용의 메모리 포인터가 얻을 수 없는 비트맵으로 
 *    DIB는 오리지널 포맷을 가지는 반면, 화면에 렌더링을 하면 내부에서는 교환 처리가 행해지므로 성능이 떨어질 수 있는 비틈맵
 */
class Bitmap : private NonCopyable {
public:
	typedef ::HBITMAP__* HBITMAP;
	typedef ::balor::io::Stream Stream;


	/// 메모리가 충분하지 않다
	struct OutOfMemoryException : public ::balor::OutOfMemoryException {};

	/// 파일 포맷이 틀렸다
	class FileFormatException : public Exception {};


	/// 파일에 보존하는 형식. GDI+ 에서 지원되는 형식만
	struct FileFormat {
		enum _enum {
			bmp,  
			gif,  
			jpeg, 
			png, 
			tiff, 
		};
		BALOR_NAMED_ENUM_MEMBERS(FileFormat);
	};


	/// 비트맵 형식
	struct Format {
		Format();
		/// １ 픽셀 당 비트 수와 RGB 비트마스크로 만든다
		explicit Format(int bitsPerPixel, int rMask = 0, int gMask = 0, int bMask = 0, int aMask = 0);

		/// 모노크롬 팔렛트 비트맵
		static const Format palette1bpp;
		/// 16색 팔렛트 비트맵
		static const Format palette4bpp;
		/// 256색 팔렛트 비트맵
		static const Format palette8bpp;
		/// Format(16, 0x7C00, 0x03E0, 0x001F)인 16비트 이미지
		static const Format rgb16bpp;
		/// Format(16, 0xF800, 0x07E0, 0x001F)인 16비트 이미지
		static const Format rgb16bpp565;
		/// 바이트 배열 열이 B, G, R 인 24비트 이미지
		static const Format rgb24bpp;
		/// Format(32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)인 32비트 이미지
		static const Format argb32bpp;

		bool operator==(const Format& value) const;
		bool operator!=(const Format& value) const;

		/// １ 픽셀 당 비트 수
		int bitsPerPixel;
		/// １ 픽셀로 빨간을 점하는 비트마스크. bitsPerPixel이 16 또는 32인 경우에만 유효. 그 이외에서는 0
		int rMask;
		/// １ 픽셀로 녹색을 점하는 비트마스크. bitsPerPixel이 16 또는 32인 경우에만 유효. 그 이외에서는 0
		int gMask;
		/// １ 픽셀로 파란을 점하는 비트마스크. bitsPerPixel이 16 또는 32인 경우에만 유효. 그 이외에서는 0
		int bMask;
		/// １ 필셀로 알파채널을 점하는 비트마스크. bitsPerPixel이 32인 경우에만 유효. 그 이외에서는 0
		int aMask;
	};


public:
	/// null 핸들로 만든다.
	Bitmap();
	Bitmap(Bitmap&& value);
	/// 핸들에서 만든다. owned가 true 이라면 파괴자에서 핸들을 파괴한다.
	explicit Bitmap(HBITMAP handle, bool owned = false);
	/// 스트림에서 만든다
	explicit Bitmap(Stream& stream, bool bottomUp = true, bool useIcm = false);
	explicit Bitmap(Stream&& stream, bool bottomUp = true, bool useIcm = false);
	/// 파일에서 읽어서 만든다
	explicit Bitmap(StringRange filePath, bool bottomUp = true, bool useIcm = false);
	/// 비트맵의 크기, 포맷을 변경해서 만든다.
	Bitmap(HBITMAP bitmap, const Size& size, Bitmap::Format format, bool bottomUp = true);
	Bitmap(HBITMAP bitmap, int width, int height, Bitmap::Format format, bool bottomUp = true);
	///크기와 포맷, 픽셀의 메모리 데이터에서 만든다.  srcStride는 srcPixels의 1 라인 당 바이트 수
	explicit Bitmap(const Size& size, Bitmap::Format format = Format::argb32bpp, bool bottomUp = true, const void* srcPixels = nullptr, int srcStride = 0, bool srcBottomUp = true);
	Bitmap(int width, int height, Bitmap::Format format = Format::argb32bpp, bool bottomUp = true, const void* srcPixels = nullptr, int srcStride = 0, bool srcBottomUp = true);
	~Bitmap();
	Bitmap& operator=(Bitmap&& value);

public:
	/// １ 픽셀 당 비트 수
	int bitsPerPixel() const;
	/// 픽셀 데이터가 좌하에서 시작하는 어떤지. DDB의 경우는 보통 true를 반환
	bool bottomUp() const;
	/// DIB라면 DIB, DDB라면 DDB인 채로 복제해서 반환한다
	Bitmap clone() const;
	static Bitmap clone(HBITMAP handle);
	/// DDB 비트맵을 만든다
	static Bitmap createDDB(const Size& size);
	static Bitmap createDDB(int width, int height);
	/// 포맷과 픽셀 데이터가 일치하는지 어떤지를 반환. 포맷이 간혹 같아도 DDB와 DIB 비교는 보통 false를 반환
	bool equalsBits(const Bitmap& rhs, bool exceptAlpha = false) const;
	static bool equalsBits(const Bitmap& lhs, const Bitmap& rhs, bool exceptAlpha = false);
	/// 비트맵 형식. 알파 마스크를 취득하는 수단이 없으므로 DIB의 32비트 이미지는 모두 알파마스크 첨부로 보는 것을 주의
	Bitmap::Format format() const;
	/// 이미지의 높이
	int height() const;
	/// DDB인지 어떤지
	bool isDDB() const;
	/// 파괴자에서 핸들을 파괴할지 어떨지. 변경은 요주의
	bool owned() const;
	void owned(bool value);
	/// 팔렛트. 팔렛트 형식의 비트맵이 아니면 조작할 수 없다.
	std::vector<Color, std::allocator<Color> > palette() const;
	void palette(ArrayRange<const Color> value);
	/// 픽셀 데이터의 포인터. 메모리 배치는 bottomUp 함수로 1라인 바이트 수는 stride 함수로 조사되어진다. isDDB()가  true인 경우는 nullptr을 반환.
	/// GDI의 비동기 렌더링과 충돌을 피하기 의해서 메모리에 접근하기 전에 Graphics::flush 함수를 실행하는 쪽이 좋다.
	unsigned char* pixels();
	const unsigned char* pixels() const;
	/// 알파채널을 모두 픽셀레 승산한다. DIB의 32비트 이미지의 경우에만 유효
	/// Graphics::blend 함수로 알파 채널을 사용하는 경우는 처음부터 이 함수를 호출해 둘 필요가 있다.
	void premultiplyAlpha();
	/// 스트림에 보존한다. quality는 jpeg 형식으로 보존하는 경우에만 품질로 0 ～ 100을 설정한다.
	void save(Stream& stream, Bitmap::FileFormat format, int quality = 75) const;
	void save(Stream&& stream, Bitmap::FileFormat format, int quality = 75) const;
	/// 파일에 저장한다. 파일 형식은 확장자에서 판별한다. 판별할 수 없는 경우에는 png 형식으로 보존해둔다. quality는 jpeg 형식의 품질에서 0 ～ 100으로 설정한다.
	void save(StringRange filePath, int quality = 75) const;
	/// 파일로 저장한다. quality는 jpeg 형식의 품질로 0 ～ 100으로 설정한다.
	void save(StringRange filePath, Bitmap::FileFormat format, int quality = 75) const;
	/// 이미지 사이즈
	Size size() const;
	/// 픽셀 데이터의 1 라인이 몇 바이트인지
	int stride() const;
	/// DDB 비트맵으로 복제한다
	Bitmap toDDB() const;
	static Bitmap toDDB(HBITMAP handle);
	/// DIB 비트맵으로 복제한다
	Bitmap toDIB(bool bottomUp = true) const;
	static Bitmap toDIB(HBITMAP handle, bool bottomUp = true);
	/// 이미지의 폭
	int width() const;

public:
	/// HBITMAP으로 자동변환 ＆ null 체크 용
	operator HBITMAP() const { return _handle; }

private:
	HBITMAP _handle;
	bool _owned;
};



	}
}