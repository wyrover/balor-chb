#include "testSingleton.hpp"

#include <balor/Singleton.hpp>


namespace balor {
	namespace testSingleton {



int& getSingletonInt() {
	return Singleton<int>::get();
}


short& getSingletonShort() {
	return Singleton<short>::get();
}



	}
}