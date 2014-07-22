#pragma once

#include <api.hpp>


namespace balor {
	namespace testSingleton {



extern "C" TESTBALORDLL_API int& getSingletonInt();

extern "C" TESTBALORDLL_API short& getSingletonShort();


	}
}