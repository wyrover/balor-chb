#pragma once

#include <api.hpp>

#include <balor/UniqueAny.hpp>


namespace balor {
	namespace testUniqueAny {


#pragma warning(push)
#pragma warning(disable : 4190) // 'getStringAny'는 C 링케이지 지정이지만 C 와 호환성 없는 유저 정의 형 'balor::UniqueAny'를 반환하고 있다.
extern "C" TESTBALORDLL_API UniqueAny getStringAny();

extern "C" TESTBALORDLL_API UniqueAny getListenerAny();

extern "C" TESTBALORDLL_API void setStringAny(UniqueAny& any);

extern "C" TESTBALORDLL_API void setListenerAny(UniqueAny& any);
#pragma warning(pop)


	}
}