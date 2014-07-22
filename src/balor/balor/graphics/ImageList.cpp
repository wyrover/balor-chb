#include "ImageList.hpp"

#include <utility>

#include <balor/graphics/Bitmap.hpp>
#include <balor/graphics/Color.hpp>
#include <balor/graphics/Icon.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>
#include <balor/Size.hpp>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <Shellapi.h>


namespace balor {
	namespace graphics {

using std::move;
using std::swap;


namespace {
static_assert(ImageList::Format::palette4bpp == ILC_COLOR4 , "Invalid enum value");
static_assert(ImageList::Format::palette8bpp == ILC_COLOR8 , "Invalid enum value");
static_assert(ImageList::Format::rgb16bpp    == ILC_COLOR16, "Invalid enum value");
static_assert(ImageList::Format::rgb24bpp    == ILC_COLOR24, "Invalid enum value");
static_assert(ImageList::Format::argb32bpp   == ILC_COLOR32, "Invalid enum value");

static_assert(ImageList::State::normal   == ILD_NORMAL, "Invalid enum value");
static_assert(ImageList::State::drawMask == ILD_MASK, "Invalid enum value");
static_assert(ImageList::State::unmasked == ILD_IMAGE, "Invalid enum value");
static_assert(ImageList::State::focused  == ILD_FOCUS, "Invalid enum value");
static_assert(ImageList::State::selected == ILD_SELECTED, "Invalid enum value");

static_assert(ImageList::SystemResource::smallStandardImages == IDB_STD_SMALL_COLOR , "Invalid enum value");
static_assert(ImageList::SystemResource::largeStandardImages == IDB_STD_LARGE_COLOR , "Invalid enum value");
static_assert(ImageList::SystemResource::smallViewImages     == IDB_VIEW_SMALL_COLOR, "Invalid enum value");
static_assert(ImageList::SystemResource::largeViewImages     == IDB_VIEW_LARGE_COLOR, "Invalid enum value");
static_assert(ImageList::SystemResource::smallExplorerImages == IDB_HIST_SMALL_COLOR, "Invalid enum value");
static_assert(ImageList::SystemResource::largeExplorerImages == IDB_HIST_LARGE_COLOR, "Invalid enum value");

static_assert(ImageList::StandardImage::cut          == STD_CUT       , "Invalid enum value");
static_assert(ImageList::StandardImage::copy         == STD_COPY      , "Invalid enum value");
static_assert(ImageList::StandardImage::paste        == STD_PASTE     , "Invalid enum value");
static_assert(ImageList::StandardImage::undo         == STD_UNDO      , "Invalid enum value");
static_assert(ImageList::StandardImage::redo         == STD_REDOW     , "Invalid enum value");
static_assert(ImageList::StandardImage::remove       == STD_DELETE    , "Invalid enum value");
static_assert(ImageList::StandardImage::fileNew      == STD_FILENEW   , "Invalid enum value");
static_assert(ImageList::StandardImage::fileOpen     == STD_FILEOPEN  , "Invalid enum value");
static_assert(ImageList::StandardImage::fileSave     == STD_FILESAVE  , "Invalid enum value");
static_assert(ImageList::StandardImage::printPreview == STD_PRINTPRE  , "Invalid enum value");
static_assert(ImageList::StandardImage::properties   == STD_PROPERTIES, "Invalid enum value");
static_assert(ImageList::StandardImage::help         == STD_HELP      , "Invalid enum value");
static_assert(ImageList::StandardImage::find         == STD_FIND      , "Invalid enum value");
static_assert(ImageList::StandardImage::replace      == STD_REPLACE   , "Invalid enum value");
static_assert(ImageList::StandardImage::print        == STD_PRINT     , "Invalid enum value");

static_assert(ImageList::ViewImage::largeIcons    == VIEW_LARGEICONS   , "Invalid enum value");
static_assert(ImageList::ViewImage::smallIcons    == VIEW_SMALLICONS   , "Invalid enum value");
static_assert(ImageList::ViewImage::list          == VIEW_LIST         , "Invalid enum value");
static_assert(ImageList::ViewImage::details       == VIEW_DETAILS      , "Invalid enum value");
static_assert(ImageList::ViewImage::sortName      == VIEW_SORTNAME     , "Invalid enum value");
static_assert(ImageList::ViewImage::sortSize      == VIEW_SORTSIZE     , "Invalid enum value");
static_assert(ImageList::ViewImage::sortDate      == VIEW_SORTDATE     , "Invalid enum value");
static_assert(ImageList::ViewImage::sortType      == VIEW_SORTTYPE     , "Invalid enum value");
static_assert(ImageList::ViewImage::parentFolder  == VIEW_PARENTFOLDER , "Invalid enum value");
static_assert(ImageList::ViewImage::netConnect    == VIEW_NETCONNECT   , "Invalid enum value");
static_assert(ImageList::ViewImage::netDisconnect == VIEW_NETDISCONNECT, "Invalid enum value");
static_assert(ImageList::ViewImage::newFolder     == VIEW_NEWFOLDER    , "Invalid enum value");
static_assert(ImageList::ViewImage::viewMenu      == VIEW_VIEWMENU     , "Invalid enum value");

static_assert(ImageList::ExplorerImage::back           == HIST_BACK          , "Invalid enum value");
static_assert(ImageList::ExplorerImage::forward        == HIST_FORWARD       , "Invalid enum value");
static_assert(ImageList::ExplorerImage::favorites      == HIST_FAVORITES     , "Invalid enum value");
static_assert(ImageList::ExplorerImage::addToFavorites == HIST_ADDTOFAVORITES, "Invalid enum value");
static_assert(ImageList::ExplorerImage::viewTree       == HIST_VIEWTREE      , "Invalid enum value");


Size getSystemResourceIconSize(ImageList::SystemResource systemResource) {
	switch (systemResource) {
		case ImageList::SystemResource::smallStandardImages :
		case ImageList::SystemResource::smallViewImages     :
		case ImageList::SystemResource::smallExplorerImages : return Size(16, 16);
	}
	return Size(24, 24);
}
} // namespace


bool ImageList::Format::_validate(Format value) {
	switch (value) {
		case palette4bpp :
		case palette8bpp :
		case rgb16bpp    :
		case rgb24bpp    :
		case argb32bpp   : return true;
		default          : return false;
	}
}


bool ImageList::State::_validate(State value) {
	switch (value) {
		case normal   :
		case drawMask :
		case unmasked :
		case focused  :
		case selected : return true;
		default       : return false;
	}
}


bool ImageList::SystemResource::_validate(SystemResource value) {
	switch (value) {
		case smallStandardImages :
		case largeStandardImages :
		case smallViewImages     :
		case largeViewImages     :
		case smallExplorerImages :
		case largeExplorerImages : return true;
		default                  : return false;
	}
}


ImageList::ImageList() : _handle(nullptr), _owned(false) {
}


ImageList::ImageList(ImageList&& value) : _handle(value._handle), _owned(value._owned) {
	value._handle = nullptr;
	value._owned = false;
}


ImageList::ImageList(HIMAGELIST handle, bool owned) : _handle(handle), _owned(owned) {
}


ImageList::ImageList(const Size& imageSize, ImageList::Format format, bool masked) : _owned(true) {
	assert("Invalid ImageList::Format" && Format::_validate(format));
	InitCommonControls();
	_handle = ImageList_Create(imageSize.width, imageSize.height, format | (masked ? ILC_MASK : 0), 0, 4);
	assert("Failed to ImageList_Create" && _handle);
}


ImageList::ImageList(int width, int height, ImageList::Format format, bool masked) : _handle(nullptr), _owned(false) {
	*this = ImageList(Size(width, height), format, masked);
}


ImageList::ImageList(ImageList::SystemResource systemResource) : _handle(nullptr), _owned(true) {
	assert("Invalid ImageList::SystemResource" && SystemResource::_validate(systemResource));

	INITCOMMONCONTROLSEX init = {sizeof(init), ICC_BAR_CLASSES};
	verify(InitCommonControlsEx(&init));
	auto toolBar = CreateWindowExW(WS_EX_CONTROLPARENT, TOOLBARCLASSNAMEW, nullptr
		, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | CCS_NOPARENTALIGN | CCS_NORESIZE
		, 0, 0, 0, 0
		, HWND_MESSAGE, nullptr,nullptr, nullptr);
	assert("Failed to CreateWindowExW for toolBar" && toolBar);
	scopeExit([&] () {
		verify(DestroyWindow(toolBar));
	});
	ImageList imageList(getSystemResourceIconSize(systemResource));
	SendMessageW(toolBar, TB_SETIMAGELIST, 0, (LPARAM)(HIMAGELIST)imageList);
	SendMessageW(toolBar, TB_LOADIMAGES, systemResource, (LPARAM)HINST_COMMCTRL);

	*this = ImageList((HIMAGELIST)SendMessageW(toolBar, TB_GETIMAGELIST, 0, 0)).clone();
}


ImageList::~ImageList() {
	if (_handle && _owned) {
		verify(ImageList_Destroy(_handle));
	}
	//_owned = false;
	//_handle = nullptr;
}


ImageList& ImageList::operator=(ImageList&& value) {
	swap(_handle, value._handle);
	swap(_owned, value._owned);
	return *this;
}


void ImageList::add(HBITMAP bitmap, HBITMAP mask) {
	assert("Null ImageList handle" && *this);
	assert("Null bitmap" && bitmap);
	assert("Too small bitmap width" && imageSize().width <= Bitmap(bitmap).width());
	verify(0 <= ImageList_Add(*this, bitmap, mask));
}


void ImageList::add(HBITMAP bitmap, const Color& colorKey) {
	assert("Null ImageList handle" && *this);
	assert("Null bitmap" && bitmap);
	assert("Too small bitmap width" && imageSize().width <= Bitmap(bitmap).width());
	verify(0 <= ImageList_AddMasked(*this, bitmap, colorKey.toCOLORREF()));
}


void ImageList::add(HICON icon) {
	assert("Null ImageList handle" && *this);
	assert("Null icon" && icon);
	verify(0 <= ImageList_ReplaceIcon(*this, -1, icon));
}


void ImageList::clear() {
	erase(-1);
}


ImageList ImageList::clone() const {
	if (!*this) {
		return ImageList();
	}
	return clone(*this);
}


ImageList ImageList::clone(HIMAGELIST handle) {
	assert("Null ImageList handle" && handle);
	ImageList list(ImageList_Duplicate(handle), true);
	assert("Failed to ImageList_Duplicate" && list);
	return list;
}


int ImageList::count() const {
	assert("Null ImageList handle" && *this);
	return ImageList_GetImageCount(*this);
}


//void ImageList::count(int value) {
//	assert("Null ImageList handle" && *this);
//	assert("count out of range" && 0 <= value);
//	assert("count out of range" && value < count());
//	verify(ImageList_SetImageCount(*this, value));
//}


void ImageList::erase(int index) {
	assert("Null ImageList handle" && *this);
	assert("index out of range" && -1 <= index);
	assert("index out of range" && index < count());
	verify(ImageList_Remove(*this, index));
}


ImageList::Format ImageList::format() {
	assert("Null ImageList handle" && *this);
	bool needDummyImage = !count();
	if (needDummyImage) {
		add(Icon::application());
	}
	IMAGEINFO info;
	verify(ImageList_GetImageInfo(*this, 0, &info));
	BITMAP bitmap;
	verify(GetObjectW(info.hbmImage, sizeof(bitmap), &bitmap));
	Format format = getFormatFromBitsPerPixel(bitmap.bmBitsPixel);
	if (needDummyImage) {
		clear();
	}
	return format;
}


//ImageList ImageList::fromSystemIcons() {
//	SHFILEINFO info;
//	ImageList list(reinterpret_cast<HIMAGELIST>(SHGetFileInfoW(L"", 0, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_LARGEICON)));
//	return list;
//}


ImageList::Format ImageList::getFormatFromBitsPerPixel(int bitsPerPixel) {
	Format format = Format::argb32bpp;
	switch (bitsPerPixel) {
		case  4 : format = Format::palette4bpp; break;
		case  8 : format = Format::palette8bpp; break;
		case 16 : format = Format::rgb16bpp; break;
		case 24 : format = Format::rgb24bpp; break;
		case 32 : format = Format::argb32bpp; break;
		default : assert("Invalid format" && false); break;
	}
	return format;
}


Icon ImageList::getIcon(int index) const {
	assert("Null ImageList handle" && *this);
	assert("index out of range" && -1 <= index);
	assert("index out of range" && index < count());
	return Icon(ImageList_GetIcon(*this, index, ILD_TRANSPARENT));
}


Size ImageList::imageSize() const {
	assert("Null ImageList handle" && *this);
	Size size(0, 0);
	verify(ImageList_GetIconSize(*this, &size.width, &size.height));
	return size;
}


bool ImageList::owned() const {
	return _owned;
}


void ImageList::owned(bool value) {
	_owned = value;
}


void ImageList::replace(int index, HBITMAP bitmap, HBITMAP mask) {
	assert("Null ImageList handle" && *this);
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	assert("Null bitmap" && bitmap);
	assert("Too small bitmap width" && imageSize().width <= Bitmap(bitmap).width());
	verify(ImageList_Replace(*this, index, bitmap, mask));
}


void ImageList::replace(int index, HBITMAP bitmap, const Color& colorKey) {
	assert("Null ImageList handle" && *this);
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	assert("Null bitmap" && bitmap);
	assert("Too small bitmap width" && imageSize().width <= Bitmap(bitmap).width());
	auto count = this->count();
	verify(0 <= ImageList_AddMasked(*this, bitmap, colorKey.toCOLORREF()));
	verify(ImageList_Copy(*this, index, *this, count, ILCF_MOVE));
	verify(ImageList_SetImageCount(*this, count));
}


void ImageList::replace(int index, HICON icon) {
	assert("Null ImageList handle" && *this);
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index < count());
	assert("Null icon" && icon);
	verify(ImageList_ReplaceIcon(*this, index, icon) != -1);
}



	}
}