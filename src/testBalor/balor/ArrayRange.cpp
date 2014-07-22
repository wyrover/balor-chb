#include <balor/ArrayRange.hpp>

#include <vector>

#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testArrayRange {


using std::vector;


namespace {
vector<wchar_t> stringToVector(const wchar_t* chars) {
	vector<wchar_t> array;
	for (const wchar_t* i = chars; *i != 0; ++i) {
		array.push_back(*i);
	}
	array.push_back(0);
	return move(array);
}


vector<wchar_t> rangeToVector(ArrayRange<const wchar_t> range) {
	return vector<wchar_t>(range.begin(), range.end());
}
} // namespace



testCase(constructAndAccess) {
	{ // 포인터 1개 버전
		wchar_t array[] = L"abc";
		ArrayRange<wchar_t> range = array;
		testAssert(range.begin() == array);
		testAssert(range.end() == (array + 4));
		testNoThrow(*range.begin() = L'0');
	}
	{ // 포인터와 길이 버전
		wchar_t array[] = L"abc";
		ArrayRange<wchar_t> range(array, 4);
		testAssert(range.begin() == array);
		testAssert(range.end() == (array + 4));
		testNoThrow(*range.begin() = L'0');
	}
	{ // vector<wchar_t> 버전
		vector<wchar_t> array = stringToVector(L"abc");
		ArrayRange<wchar_t> range = array;
		testAssert(range.begin() == &array.front());
		testAssert(range.end() == (&array.front() + 4));
		testNoThrow(*range.begin() = L'0');
	}

	// const 판
	{ // 포인터 1개 버전
		const wchar_t array[] = L"abc";
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.begin() == array);
		testAssert(range.end() == (array + 4));
	}
	{ // 포인터와 길이 버전
		const wchar_t array[] = L"abc";
		const ArrayRange<const wchar_t> range(array, 4);
		testAssert(range.begin() == array);
		testAssert(range.end() == (array + 4));
	}
	{ // vector<wchar_t> 버전
		const vector<wchar_t> array = stringToVector(L"abc");
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.begin() == &array.front());
		testAssert(range.end() == (&array.front() + 4));
	}
}


testCase(empty) {
	{ // 비지 않은 리터럴
		const wchar_t array[] = L"abc";
		const ArrayRange<const wchar_t> range = array;
		testAssert(!range.empty());
	}
	{ // 포인터와 0으로 초기화
		const ArrayRange<const wchar_t> range(nullptr, 0);
		testAssert(range.empty());
	}
	{ // 빈 vector<wchar_t>
		const vector<wchar_t> array;
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.empty());
	}
	{ // 비지 않은 vector<wchar_t>
		const vector<wchar_t> array = stringToVector(L"abc");
		const ArrayRange<const wchar_t> range = array;
		testAssert(!range.empty());
	}
}


testCase(length) {
	{ // 비지 않은 리터럴
		const wchar_t array[] = L"abc";
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.length() == 4);
	}
	{ // 포인터와 길이로 초기화
		const ArrayRange<const wchar_t> range(nullptr, 0);
		testAssert(range.length() == 0);
	}
	{ // 빈 vector<wchar_t>
		const vector<wchar_t> array;
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.length() == 0);
	}
	{ // 비지 않은 vector<wchar_t>
		const vector<wchar_t> array = stringToVector(L"abc");;
		const ArrayRange<const wchar_t> range = array;
		testAssert(range.length() == 4);
	}
}

		
testCase(functionArgument) {
	testAssert(rangeToVector(L"abc") == stringToVector(L"abc"));
	testAssert(rangeToVector(stringToVector(L"aaaa")) == stringToVector(L"aaaa"));
}



	}
}