#pragma once

#if defined(BALOR_SINGLETON_EXPORTS)
#	define BALOR_SINGLETON_API extern "C" __declspec(dllexport)
#else
#	define BALOR_SINGLETON_API extern "C" __declspec(dllimport)
#endif

#include <typeinfo>


namespace balor {
	namespace singleton {



BALOR_SINGLETON_API void* getSingletonInstance(const std::type_info& info, void* (*createInstanceFunction)());



	}
}