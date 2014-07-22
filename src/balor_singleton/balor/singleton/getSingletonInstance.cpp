#include "getSingletonInstance.hpp"

#include <vector>

#define BOOST_DATE_TIME_NO_LIB
#define BOOST_THREAD_NO_LIB
#include <boost/thread/recursive_mutex.hpp>


namespace balor {
	namespace singleton {


using namespace std;
using boost::recursive_mutex;



namespace {
struct Instance {
	Instance() {}
	Instance(void* pointer, const type_info& info) : pointer(pointer),/* hashCode(info.hash_code()),*/ info(&info) {}
	void* pointer;
	//size_t hashCode;
	const type_info* info;
};

typedef vector<Instance> Instances;

Instances instances; // DLL이 프로세스에 attach 되기 전에 초기화 되어야 한다 http://msdn.microsoft.com/ja-jp/library/988ye33t(VS.80).aspx
recursive_mutex instancesMutex; // DLL이 프로세스에 attatch 되기 전에 초기화 되어햐 한다
} // namespace



BALOR_SINGLETON_API void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)()) {
	recursive_mutex::scoped_lock lock(instancesMutex);
	for (auto i = instances.begin(), end = instances.end(); i != end; ++i) {
		//if (info.hash_code() == i->hashCode) { // 이미 등록된 인스턴스를 발견하였다
		if (info == *(i->info)) { // 이미 등록된 인스턴스를 발견하였다
			return i->pointer;
		}
	}
	// 발견하지 못해서 새로운 인스턴스 등록 
	void* newInstance = (*createInstanceFunction)();
	instances.push_back(Instance(newInstance, info));
	return newInstance;
}



	}
}