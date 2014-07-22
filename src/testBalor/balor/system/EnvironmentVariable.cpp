#include <balor/system/EnvironmentVariable.hpp>

#include <balor/io/Registry.hpp>
#include <balor/test/UnitTest.hpp>
#include <balor/String.hpp>


namespace balor {
	namespace system {
		namespace testEnvironmentVariable {

using namespace balor::io;



testCase(expandStrings) {
	testAssert(EnvironmentVariable::expandStrings(L"") == L"");
	testAssert(0 < EnvironmentVariable::expandStrings(L"%PATH%").length());
	testAssert(EnvironmentVariable::expandStrings(L"%PATH%") == EnvironmentVariable::get(L"PATH"));
}


testCase(get) {
	// 무효한 파라미터
	testAssertionFailed(EnvironmentVariable::get(L""));
	testAssert(EnvironmentVariable::get(L"jldfjaldjfaldjalkdja01841978943") == L"");
	testAssert(EnvironmentVariable::get(L"PATH") == EnvironmentVariable::expandStrings(L"%PATH%")); // 
}


testCase(set) {
	// 무효한 파라미터
	testAssertionFailed(EnvironmentVariable::remove(L""));

	testAssertionFailed(EnvironmentVariable::set(L"", L""));

	const wchar_t valueName[] = L"testBalor_environment_name_82304839057ofjal90q84rq0w";
	EnvironmentVariable::remove(valueName);
	testAssert(EnvironmentVariable::get(valueName) == L"");
	EnvironmentVariable::set(valueName, L"%PATH%");
	testAssert(EnvironmentVariable::get(valueName) == L"%PATH%");
	EnvironmentVariable::remove(valueName);
	testAssert(EnvironmentVariable::get(valueName) == L"");
}


testCase(systemRegistry) {
	auto registry = EnvironmentVariable::systemRegistry();
	testAssert(registry);
}


testCase(updateAllProcess) {
	//auto registry = EnvironmentVariable::userRegistry();
	testNoThrow(EnvironmentVariable::updateAllProcess());

	// TODO: 반영되었지만 테스트 할 수 없었다 
}


testCase(userRegistry) {
	auto registry = EnvironmentVariable::userRegistry();
	testAssert(registry);
}


testCase(variablesBegin) {
	bool result = true;
	for (auto i = EnvironmentVariable::variablesBegin(); i; ++i) {
		if (i.value() != EnvironmentVariable::expandStrings(L"%" + i.name() + L"%")) {
			result = false;
		}
	}
	testAssert(result);
}



		}
	}
}