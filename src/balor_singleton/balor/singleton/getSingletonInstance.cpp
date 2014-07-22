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

Instances instances; // DLL궕긵깓긜긚궸귺?긞?궠귢귡멟궸룊딖돸궠귢귡궼궦?http://msdn.microsoft.com/ja-jp/library/988ye33t(VS.80).aspx
recursive_mutex instancesMutex; // DLL궕긵깓긜긚궸귺?긞?궠귢귡멟궸룊딖돸궠귢귡궼궦
} // namespace



BALOR_SINGLETON_API void* getSingletonInstance(const type_info& info, void* (*createInstanceFunction)()) {
	recursive_mutex::scoped_lock lock(instancesMutex);
	for (auto i = instances.begin(), end = instances.end(); i != end; ++i) {
		//if (info.hash_code() == i->hashCode) { // 뱋?띙귒귽깛긚?깛긚궕뙥궰궔궯궫
		if (info == *(i->info)) { // 뱋?띙귒귽깛긚?깛긚궕뙥궰궔궯궫
			return i->pointer;
		}
	}
	// 뙥궰궔귞궶궔궯궫궻궳륷궢궋귽깛긚?깛긚귩뱋?
	void* newInstance = (*createInstanceFunction)();
	instances.push_back(Instance(newInstance, info));
	return newInstance;
}



	}
}