#pragma once

//#include <functional>

namespace balor {
/// ユニットテストやデバッグ関連のクラス群
namespace test {
}
}

#include <balor/test/Debug.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/InstanceTracer.hpp>
#include <balor/test/noMacroAssert.hpp>
#include <balor/test/UnhandledException.hpp>
//#include <balor/test/UnitTest.hpp> // マクロを含む
//#include <balor/test/verify.hpp> // マクロを含む

#include <balor/link.hpp>
