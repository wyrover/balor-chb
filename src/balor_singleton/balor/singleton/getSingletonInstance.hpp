#pragma once

#if defined(BALOR_SINGLETON_EXPORTS)
#	define BALOR_SINGLETON_API extern "C" __declspec(dllexport)
#else
#	define BALOR_SINGLETON_API extern "C" __declspec(dllimport)
#endif

#include <typeinfo>


namespace balor {
	namespace singleton {


// 싱글톤 인스턴스를 등록, 작성 또는 취득한다. 자세한 것은 balor/Singleton.hpp의 클래스 문서를 참조 
BALOR_SINGLETON_API void* getSingletonInstance(const std::type_info& info, void* (*createInstanceFunction)());



	}
}