#include "Monitor.hpp"

#include <vector>

#include <balor/graphics/Graphics.hpp>
#include <balor/test/verify.hpp>
#include <balor/system/windows.hpp>
#include <balor/Point.hpp>
#include <balor/Rectangle.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace gui {

using std::move;
using std::vector;
using namespace balor::graphics;


namespace {
static_assert(Monitor::Orientation::angle0   == DMDO_DEFAULT, "Invalid enum value");
static_assert(Monitor::Orientation::angle90  == DMDO_90     , "Invalid enum value");
static_assert(Monitor::Orientation::angle180 == DMDO_180    , "Invalid enum value");
static_assert(Monitor::Orientation::angle270 == DMDO_270    , "Invalid enum value");


struct MonitorInfo : public MONITORINFOEXW {
	MonitorInfo(HMONITOR handle) {
		assert("Null Monitor handle" && handle);
		cbSize = sizeof(MONITORINFOEXW);
		verify(GetMonitorInfoW(handle, this));
	}
};


struct DeviceMode : public DEVMODEW {
	DeviceMode(HMONITOR handle) {
		dmSize = sizeof(DEVMODEW);
		verify(EnumDisplaySettingsW(handle ? MonitorInfo(handle).szDevice : nullptr, ENUM_CURRENT_SETTINGS, this));
	}
};
} // namnespace



Monitor::Monitor(HMONITOR handle) : _handle(handle) {
}


Monitor::Monitor(Monitor&& value) : _handle(value._handle) {
}


Monitor& Monitor::operator=(Monitor&& value) {
	_handle = value._handle;
	return *this;
}


int Monitor::bitsPerPixel() const {
	//HDC hdc = CreateDCW(_handle ? deviceName().c_str() : L"DISPLAY", nullptr, nullptr, nullptr);
	//verify(hdc);
	//int bitsPixel = GetDeviceCaps(hdc, BITSPIXEL);
	//int planes = GetDeviceCaps(hdc, PLANES);
	//verify(DeleteDC(hdc));
	//return bitsPixel * planes;

	DeviceMode mode(_handle);
	return (mode.dmFields & DM_BITSPERPEL) != 0 ? mode.dmBitsPerPel : 0;
}


Rectangle Monitor::bounds() const {
	if (_handle) {
		return MonitorInfo(_handle).rcMonitor;
	} else {
		return Rectangle(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
}


String Monitor::deviceName() const {
	if (_handle) {
		return MonitorInfo(_handle).szDevice;
	} else {
		return primaryMonitor().deviceName();
	}
}


Monitor Monitor::fromControl(HWND control) {
	return Monitor(MonitorFromWindow(control, MONITOR_DEFAULTTONEAREST));
}


Monitor Monitor::fromPoint(const Point& point) {
	return Monitor(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST));
}


Monitor Monitor::fromRectangle(const Rectangle& rect) {
	RECT winRect = rect;
	return Monitor(MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST));
}


std::vector<Monitor, std::allocator<Monitor> > Monitor::monitors() {
	struct Enumerator {
		vector<Monitor> screens;
		static BOOL CALLBACK procedure(HMONITOR _handle, HDC , LPRECT , LPARAM lparam) {
			Enumerator* enumerator = reinterpret_cast<Enumerator*>(lparam);
			enumerator->screens.push_back(Monitor(_handle));
			return TRUE;
		}
	} enumerator;
	verify(EnumDisplayMonitors(Graphics::fromScreen(), nullptr, Enumerator::procedure, (LPARAM)&enumerator));
	return move(enumerator.screens);
}


bool Monitor::monitorsHasSameBitsPerPixel() {
	return GetSystemMetrics(SM_SAMEDISPLAYFORMAT) != 0;
}


Monitor::Orientation Monitor::orientation() const {
	DeviceMode mode(_handle);
	return (mode.dmFields & DM_DISPLAYORIENTATION) != 0 ? static_cast<Orientation>(mode.dmDisplayOrientation) : Orientation::angle0;
}


bool Monitor::primary() const {
	if (_handle) {
		return (MonitorInfo(_handle).dwFlags & MONITORINFOF_PRIMARY) != 0;
	} else {
		return true;
	}
}


Monitor Monitor::primaryMonitor() {
	struct Enumerator {
		HMONITOR handle;
		static BOOL CALLBACK procedure(HMONITOR handle, HDC , LPRECT , LPARAM lparam) {
			if (Monitor(handle).primary()) {
				reinterpret_cast<Enumerator*>(lparam)->handle = handle;
				return FALSE;
			}
			return TRUE;
		}
	} enumerator;
	enumerator.handle = nullptr;
	EnumDisplayMonitors(Graphics::fromScreen(), nullptr, Enumerator::procedure, (LPARAM)&enumerator);
	assert("Not found primaryMonitor" && enumerator.handle);
	return Monitor(enumerator.handle);
}


int Monitor::refreshRate() const {
	DeviceMode mode(_handle);
	if ((mode.dmFields & DM_DISPLAYFREQUENCY) != 0) {
		return mode.dmDisplayFrequency;
	}
	return 0;
}


Rectangle Monitor::virtualMonitorBounds() {
	return Rectangle(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
}


int Monitor::visibleMonitorsCount() {
	return GetSystemMetrics(SM_CMONITORS);
}


Rectangle Monitor::workingArea() const {
	if (_handle) {
		return MonitorInfo(_handle).rcWork;
	} else {
		RECT rect = {0, 0, 0, 0};
		verify(SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0));
		return rect;
	}
}



	}
}