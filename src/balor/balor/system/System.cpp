#include "System.hpp"

#include <vector>

#include <balor/io/File.hpp>
#include <balor/locale/Charset.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/FileVersionInfo.hpp>
#include <balor/system/Version.hpp>
#define  SECURITY_WIN32
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>

#include <Lmcons.h> // for UNLEN
#include <Security.h>
#pragma comment (lib, "secur32.lib")
#include <shellapi.h>
#include <ShlObj.h>

#include <powrprof.h>
#pragma comment(lib, "PowrProf.lib")


namespace balor {
	namespace system {


using std::move;
using std::vector;
using namespace balor::io;
using namespace balor::locale;



bool System::OsKind::_validate(OsKind value) {
	return unknown <= value && value <= windows7;
}


System::BootMode System::bootMode() {
	return static_cast<BootMode>(GetSystemMetrics(SM_CLEANBOOT));
}


int System::commandLineArgCount() {
	return __argc;
}


String System::computerName() {
	wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
	buffer[0] = 0;
	DWORD size =  MAX_COMPUTERNAME_LENGTH + 1;
	verify(GetComputerNameW(buffer, &size));
	return buffer;
}


bool System::dbcsEnabled() {
	return GetSystemMetrics(SM_DBCSENABLED) != 0;
}


bool System::debugOS() {
	return GetSystemMetrics(SM_DEBUG) != 0;
}


bool System::dragFullWindows() {
	BOOL result = FALSE;
	verify(SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &result, 0));
	return result != FALSE;
}


String System::getCommandLineArg(int index) {
	assert("command line arg index out of range" && 0 <= index);
	assert("command line arg index out of range" && index < commandLineArgCount());
	if (__wargv) {
		return String::refer(__wargv[index]);
	} else {
		return Charset::default().decode(__argv[index]);
	}
}


bool System::is64BitOs() {
	SYSTEM_INFO info;
	ZeroMemory(&info, sizeof(info));
	GetNativeSystemInfo(&info); // XPà»ç~Ç»ÇÁÇ±ÇÃä÷êîÇÕÇ†ÇÈ
	bool result = false;
	switch (info.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_AMD64 : result = true; break;
		case PROCESSOR_ARCHITECTURE_IA64  : result = true; break;
		case PROCESSOR_ARCHITECTURE_INTEL : break;
		default : assert(false); break; // ïsñæÇ»ÇbÇoÇtÉAÅ[ÉLÉeÉNÉ`ÉÉ
	}
	return result;
}


bool System::is64BitProcess() {
	if (!is64BitOs()) {
		return false;
	}
	//const HMODULE module = GetModuleHandleW(L"kernel32");
	//assert(module);
	//auto isWow64Process = reinterpret_cast<decltype(IsWow64Process)*>(GetProcAddress(module, "IsWow64Process"));
	//if (!isWow64Process) {
	//	return false;
	//}
	BOOL result = FALSE;
	const HANDLE process = GetCurrentProcess();
	assert(process);
	verify(IsWow64Process(process, &result)); // XPà»ç~Ç»ÇÁÇ±ÇÃä÷êîÇÕÇ†ÇÈ
	return result == FALSE;
}


bool System::network() {
	return (GetSystemMetrics(SM_NETWORK) & 1) != 0;
}


System::OsKind System::osKind() {
	OSVERSIONINFOW info;
	info.dwOSVersionInfoSize = sizeof(info);
	verify(GetVersionExW(&info));
	if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		switch (info.dwMajorVersion) {
			case 4 : {
				switch (info.dwMinorVersion) {
					case  0 : return OsKind::windows95;
					case 10 : return OsKind::windows98;
					case 90 : return OsKind::windowsME;
				}
			} break;
		}
	} else if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		OSVERSIONINFOEXW infoEx;
		infoEx.dwOSVersionInfoSize = sizeof(infoEx);
		verify(GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&infoEx)));
		switch (infoEx.dwMajorVersion) {
			case 4 : {
				switch (infoEx.dwMinorVersion) {
					case  0 : return OsKind::windowsNT4;
				}
			} break;
			case 5 : {
				switch (infoEx.dwMinorVersion) {
					case  0 : return OsKind::windows2000;
					case  1 : return OsKind::windowsXP  ;
					case  2 : {
						if (infoEx.wProductType == VER_NT_WORKSTATION) {
							return OsKind::windowsXP;
						}
						if (infoEx.wSuiteMask & VER_SUITE_WH_SERVER) {
							return OsKind::windowsHomeServer;
						}
						if (!GetSystemMetrics(SM_SERVERR2)) {
							return OsKind::windowsServer2003;
						} else {
							return OsKind::windowsServer2003R2;
						}
					} break;
				}
			} break;
			case 6 : {
				switch (infoEx.dwMinorVersion) {
					case  0 : {
						if (infoEx.wProductType == VER_NT_WORKSTATION) {
							return OsKind::windowsVista;
						} else {
							return OsKind::windowsServer2008;
						}
					} break;
					case  1 : {
						if (infoEx.wProductType == VER_NT_WORKSTATION) {
							return OsKind::windows7;
						} else {
							return OsKind::windowsServer2008R2;
						}
					} break;
				}
			} break;
		}
	} else { // infoEx.dwPlatformId == VER_PLATFORM_WIN32s Ç‹ÇΩÇÕêVÇµÇ¢Ç»Ç…Ç©
	}
	return OsKind::unknown;
}


String System::osServicePack() {
	OSVERSIONINFOW info;
	info.dwOSVersionInfoSize = sizeof(info);
	verify(GetVersionExW(&info));
	return info.szCSDVersion;
}


Version System::osVersion() {
	OSVERSIONINFOW info;
	info.dwOSVersionInfoSize = sizeof(info);
	verify(GetVersionExW(&info));
	if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		OSVERSIONINFOEXW infoEx;
		infoEx.dwOSVersionInfoSize = sizeof(infoEx);
		verify(GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&infoEx)));
		return Version(infoEx.dwMajorVersion, infoEx.dwMinorVersion, infoEx.dwBuildNumber, (infoEx.wServicePackMajor << 16) | infoEx.wServicePackMinor);
	} else {
		return Version(info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, 0);
	}
}


int System::processorCount() {
	SYSTEM_INFO info;
	ZeroMemory(&info, sizeof(info));
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
}


bool System::suspend(bool hibernate, bool force, bool disableWakeEvent) {
	return SetSuspendState(hibernate, force, disableWakeEvent) != 0;
}


String System::userName() {
	wchar_t buffer[UNLEN + 1];
	DWORD size = UNLEN + 1;
	verify(GetUserNameW(buffer, &size));
	return buffer;
}


bool System::visualStyleEnabled() {
	static int comCtl32Version = 0; // DLL Ç≤Ç∆Ç…é¿ëÃÇéùÇ¡ÇƒÇ‡Ç©Ç‹ÇÌÇ»Ç¢ÇµèàóùÇ™èdï°ÇµÇƒÇ‡Ç©Ç‹ÇÌÇ»Ç¢ÅB
	if (!comCtl32Version) {
		Module comCtrl = Module::find(L"ComCtl32.dll");
		if (!comCtrl) {
			comCtl32Version = -1;
		} else {
			FileVersionInfo versionInfo(comCtrl.file());
			comCtl32Version = versionInfo.fileVersion().major();
		}
	}
	return 6 <= comCtl32Version;
}



	}
}