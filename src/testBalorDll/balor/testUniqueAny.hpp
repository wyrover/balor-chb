#pragma once

#include <api.hpp>

#include <balor/UniqueAny.hpp>


namespace balor {
	namespace testUniqueAny {


#pragma warning(push)
#pragma warning(disable : 4190) // 'getStringAny' は C リンケージ指定ですが、C と互換性のないユーザー定義の型 'balor::UniqueAny' を返しています。
extern "C" TESTBALORDLL_API UniqueAny getStringAny();

extern "C" TESTBALORDLL_API UniqueAny getListenerAny();

extern "C" TESTBALORDLL_API void setStringAny(UniqueAny& any);

extern "C" TESTBALORDLL_API void setListenerAny(UniqueAny& any);
#pragma warning(pop)


	}
}