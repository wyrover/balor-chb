#include "PerformanceCounter.hpp"

#include <Pdh.h>
#pragma comment(lib, "pdh.lib")

#include <balor/test/verify.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {
	namespace system {


//using std::move;



#pragma warning(push)
#pragma warning(disable : 4189) // 'status' : ローカル変数が初期化されましたが、参照されていません
PerformanceCounter::PerformanceCounter(StringRange categoryName, StringRange counterName, StringRange instanceName) {
	StringBuffer buffer;
	buffer += L"\\";
	buffer += String::refer(categoryName);
	if (!instanceName.empty()) {
		buffer += L"(";
		buffer += String::refer(instanceName);
		buffer += L")";
	}
	buffer += L"\\";
	buffer += String::refer(counterName);

	verify(PdhOpenQueryW(nullptr, 0, &_query) == ERROR_SUCCESS);
	PDH_STATUS status = PdhAddCounterW(_query, buffer.begin(), 0, &_counter);
	assert(status == ERROR_SUCCESS);
	status = PdhCollectQueryData(_query);
	assert(status == ERROR_SUCCESS);
}
#pragma warning(pop)


PerformanceCounter::~PerformanceCounter() {
	if (_query) {
		verify(PdhCloseQuery(_query) == ERROR_SUCCESS);
		//_query = nullptr;
		//_counter = nullptr;
	}
}


template<> double PerformanceCounter::nextValue<double>() const {
	verify(PdhCollectQueryData(_query) == ERROR_SUCCESS);
	PDH_FMT_COUNTERVALUE value;
	verify(PdhGetFormattedCounterValue(_counter, PDH_FMT_DOUBLE, nullptr, &value) == ERROR_SUCCESS);
	return value.doubleValue;
}


template<> int PerformanceCounter::nextValue<int>() const {
	verify(PdhCollectQueryData(_query) == ERROR_SUCCESS);
	PDH_FMT_COUNTERVALUE value;
	verify(PdhGetFormattedCounterValue(_counter, PDH_FMT_LONG, nullptr, &value) == ERROR_SUCCESS);
	return value.longValue;
}



	}
}