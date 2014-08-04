#pragma once

#include <balor/io/File.hpp>
#include <balor/io/Registry.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>

namespace balor {
/// システム、環境、ＯＳ関連の機能を集めたクラス群（未分類とも言える）
namespace system {
}
}

#include <balor/system/Com.hpp>
//#include <balor/system/ComBase.hpp> // Objbase.h をインクルードしている
#include <balor/system/ComPtr.hpp>
#include <balor/system/Console.hpp>
#include <balor/system/EnvironmentVariable.hpp>
#include <balor/system/FileVersionInfo.hpp>
#include <balor/system/Module.hpp>
#include <balor/system/PerformanceCounter.hpp>
#include <balor/system/Process.hpp>
#include <balor/system/System.hpp>
#include <balor/system/Version.hpp>
//#include <balor/system/windows.hpp> // windows.h のインクルード用

#include <balor/link.hpp>
