#include "UnitTest.hpp"

#include <map>
#include <regex>
#include <string>
#include <utility>
#include <vector>
#define BOOST_DATE_TIME_NO_LIB
#define BOOST_THREAD_NO_LIB
#include <boost/thread/mutex.hpp>
#include <signal.h>

#include <balor/system/PerformanceCounter.hpp>
#include <balor/system/Process.hpp>
#include <balor/system/Windows.hpp>
#include <balor/test/Debug.hpp>
#include <balor/test/HandleLeakChecker.hpp>
#include <balor/test/UnhandledException.hpp>
#include <balor/Singleton.hpp>


namespace balor {
	namespace test {


using std::map;
using std::move;
using std::string;
using std::vector;
using boost::mutex;


namespace {
void registerSignal();

void onAbort(int ) {
	registerSignal();
	throw balor::test::UnitTest::AssertionFailedException();
}

void registerSignal() {
 	signal(SIGABRT, onAbort);
}

struct TestCase {
	TestCase() : functionName(nullptr), function(nullptr) {}
	TestCase(const char* functionName, void (*function)()) : functionName(functionName), function(function) {}
	const char* functionName;
	void (*function)();
};

class Global {
	friend Singleton<Global>;

	Global() : reportTotalCount(0), reportFailedCount(0), currentTestCase(nullptr), unitTestIsRunning(false) {
	}
	~Global() {
	}

public:
	map<string, map<int, TestCase> > functionMap;
	int reportTotalCount;
	int reportFailedCount;
	const char* currentFileName;
	const char* currentTestCase;
	int currentTestCaseLine;
	bool unitTestIsRunning;
	vector<string> reports;
	bool ignoreHandleLeackCheck;
	mutex reportMutex;
};
} // namespace



UnitTest::FunctionRegister::FunctionRegister(void (*function) (), const char* functionName, const char* fileName, int line) {
	UnitTest::registerTestCase(function, functionName, fileName, line);
};


void UnitTest::ignoreHandleLeakCheck() {
	Singleton<Global>::get().ignoreHandleLeackCheck = true;
}


bool UnitTest::isRunning() {
	return Singleton<Global>::get().unitTestIsRunning;
}


void UnitTest::registerTestCase(void (*function) (), const char* functionName, const char* fileName, int line) {
	Singleton<Global>::get().functionMap[fileName][line] = TestCase(functionName, function); // 不思議なことに Debug と Release で __FILE__ の値が小文字のみだったり大文字まじりだったりする。
}


void UnitTest::report(bool result, const char* expression, int line) {
	Global& global = Singleton<Global>::get();
	mutex::scoped_lock lock(global.reportMutex);
	++global.reportTotalCount;
	if (!result) {
		string text;
		text += "::failed case file: ";
		text += global.currentFileName;
		char buffer[256];
		wsprintfA(buffer, " (line: %d)\n", line);
		text += buffer;
		text += "         text case: ";
		text += global.currentTestCase;
		text += "\n        expression: ";
		text += expression;
		global.reports.push_back(text);
		Debug::write(text.c_str());
		Debug::write("\n\n");
		++global.reportFailedCount;
	}
}


void UnitTest::run() {
	run("");
}


void UnitTest::run(const char* pettern) {
	// assert マクロでメッセージボックスを出さずに AssertionFailedException を投げるようにする
	_set_error_mode(_OUT_TO_STDERR);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
	registerSignal();


	Global& global = Singleton<Global>::get();
	std::regex regexPettern;
	if (pettern && *pettern != L'\0') {
		regexPettern = std::regex(pettern, std::regex::icase);
	}
	Debug::write("begin unit test.\n\n\n");
	global.unitTestIsRunning = true;
	global.reports.clear();
	int totalCount = 0;
	int failedCount = 0;
	for (auto i = global.functionMap.begin(), end = global.functionMap.end(); i != end; ++i) {
		if (pettern && *pettern != L'\0') {
			if (!std::regex_search(i->first, regexPettern)) {
				continue;
			}
		}
		Debug::write("@begin test file : ");
		Debug::write(i->first.c_str());
		Debug::write("\n");
		global.reportTotalCount = 0;
		global.reportFailedCount = 0;
		global.currentFileName = i->first.c_str();
		for (auto j = i->second.begin(), end = i->second.end(); j != end; ++j) {
			global.currentTestCase = j->second.functionName;
			global.currentTestCaseLine = j->first;
			try {
				global.ignoreHandleLeackCheck = false;
				HandleLeakChecker checker;
				j->second.function();
				if (!global.ignoreHandleLeackCheck) {
					char buffer[256];
					wsprintfA(buffer, "System handle leak detected :%d\n\n", checker.getSystemHandleChange());
					report(checker.getSystemHandleChange() <= 0, buffer, global.currentTestCaseLine);
					wsprintfA(buffer, "GDI handle leak detected :%d\n\n", checker.getGdiHandleChange());
					report(checker.getGdiHandleChange()    <= 0, buffer, global.currentTestCaseLine);
					wsprintfA(buffer, "User handle leak detected :%d\n\n", checker.getUserHandleChange());
					report(checker.getUserHandleChange()   <= 0, buffer, global.currentTestCaseLine);
				}
			} catch (UnhandledException& ) {
				report(false, "unhandled exception occurred!", global.currentTestCaseLine);
				Debug::enableCrashDumpHandler(L"", false);
			}
		}
		char buffer[256];
		wsprintfA(buffer, "@end test file : run = %d, failures = %d\n\n", global.reportTotalCount, global.reportFailedCount);
		Debug::write(buffer);
		totalCount += global.reportTotalCount;
		failedCount += global.reportFailedCount;
	}

	Debug::write("end unit test.\n");
	char buffer[256];
	wsprintfA(buffer, "\ntest results : run = %d, failures = %d\n\n\n", totalCount, failedCount);
	Debug::write(buffer);
	if (0 < failedCount) {
		Debug::write("!!!FAILURES!!!\n\n");
		for (auto i = global.reports.begin(), end = global.reports.end(); i != end; ++i) {
			Debug::write(i->c_str());
			Debug::write("\n\n");
		}
	}
	global.unitTestIsRunning = false;
}



	}
}