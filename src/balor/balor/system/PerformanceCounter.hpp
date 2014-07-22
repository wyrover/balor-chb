#pragma once

#include <balor/NonCopyable.hpp>
#include <balor/StringRange.hpp>


namespace balor {
	namespace system {



/**
 * パフォーマンスカウンタ暫定実装。
 */
class PerformanceCounter : private NonCopyable {
private:
	typedef void* HQUERY;
	typedef void* HCOUNTER;

public:
	PerformanceCounter(StringRange categoryName, StringRange counterName, StringRange instanceName = L"");
	~PerformanceCounter();

public:
	/// 指定した型で値を取得する。指定できる型は int, long long, double。
	template<typename T> T nextValue() const;

private:
	HQUERY _query;
	HCOUNTER _counter;
};



	}
}