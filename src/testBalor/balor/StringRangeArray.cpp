#include <balor/StringRangeArray.hpp>

#include <array>
#include <cassert>
#include <vector>

#include <balor/io/File.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace testStringRangeArray {

using std::array;
using std::vector;
using std::wstring;
using balor::io::File;


namespace {
void testArray(StringRangeArray array) {
	testAssert(array.length() == 3);
	testAssert(array[0] == L"abc");
	testAssert(array[1] == L"def");
	testAssert(array[2] == L"ghi");
}


template<typename T> void makeVector(T& array) {
	array.push_back(L"abc");
	array.push_back(L"def");
	array.push_back(L"ghi");
}


template<typename T> void makeFile(T& array) {
	array.push_back(File(L"abc"));
	array.push_back(File(L"def"));
	array.push_back(File(L"ghi"));
}


template<typename T> void makeStringBuffer(T& array) {
	array.push_back(StringBuffer(String(L"abc")));
	array.push_back(StringBuffer(String(L"def")));
	array.push_back(StringBuffer(String(L"ghi")));
}
} // namespace



testCase(all) {
	// •¶Žš—ñƒ|ƒCƒ“ƒ^
	wchar_t* charArray[] = {L"abc", L"def", L"ghi"};
	const wchar_t* ccharArray[] = {L"abc", L"def", L"ghi"};

	testArray(charArray);
	testArray(ccharArray);
	testArray(StringRangeArray(charArray, 3));
	testArray(StringRangeArray(ccharArray, 3));

	array<wchar_t*, 3> charStdArray = {L"abc", L"def", L"ghi"};
	array<const wchar_t*, 3> ccharStdArray = {L"abc", L"def", L"ghi"};
	testArray(charStdArray);
	testArray(ccharStdArray);
	testArray(const_cast<const array<wchar_t*, 3>&>(charStdArray));
	testArray(const_cast<const array<const wchar_t*, 3>&>(ccharStdArray));

	vector<wchar_t*> charVector;
	makeVector(charVector);
	testArray(charVector);
	testArray(const_cast<const vector<wchar_t*>&>(charVector));
	vector<const wchar_t*> ccharVector;
	makeVector(ccharVector);
	testArray(ccharVector);
	testArray(const_cast<const vector<const wchar_t*>&>(ccharVector));


	// File
	File fileArray[] = {L"abc", L"def", L"ghi"};
	const File cfileArray[] = {L"abc", L"def", L"ghi"};

	testArray(fileArray);
	testArray(cfileArray);
	testArray(StringRangeArray(fileArray, 3));
	testArray(StringRangeArray(cfileArray, 3));

	array<File, 3> fileStdArray = {L"abc", L"def", L"ghi"};
	array<const File, 3> cfileStdArray = {L"abc", L"def", L"ghi"};
	testArray(fileStdArray);
	testArray(cfileStdArray);
	testArray(const_cast<const array<File, 3>&>(fileStdArray));
	testArray(const_cast<const array<const File, 3>&>(cfileStdArray));

	vector<File> fileVector;
	makeFile(fileVector);
	testArray(fileVector);
	testArray(const_cast<const vector<File>&>(fileVector));
	vector<const File> cfileVector;
	makeFile(cfileVector);
	testArray(cfileVector);
	testArray(const_cast<const vector<const File>&>(cfileVector));


	// String
	String stringArray[] = {L"abc", L"def", L"ghi"};
	const String cstringArray[] = {L"abc", L"def", L"ghi"};

	testArray(stringArray);
	testArray(cstringArray);
	testArray(StringRangeArray(stringArray, 3));
	testArray(StringRangeArray(cstringArray, 3));

	array<String, 3> stringStdArray = {L"abc", L"def", L"ghi"};
	array<const String, 3> cstringStdArray = {L"abc", L"def", L"ghi"};
	testArray(stringStdArray);
	testArray(cstringStdArray);
	testArray(const_cast<const array<String, 3>&>(stringStdArray));
	testArray(const_cast<const array<const String, 3>&>(cstringStdArray));

	vector<String> stringVector;
	makeVector(stringVector);
	testArray(stringVector);
	testArray(const_cast<const vector<String>&>(stringVector));
	vector<const String> cstringVector;
	makeVector(cstringVector);
	testArray(cstringVector);
	testArray(const_cast<const vector<const String>&>(cstringVector));


	// StringBuffer
	StringBuffer stringBufferArray[] = {StringBuffer(String(L"abc")), StringBuffer(String(L"def")), StringBuffer(String(L"ghi"))};
	const StringBuffer cstringBufferArray[] = {StringBuffer(String(L"abc")), StringBuffer(String(L"def")), StringBuffer(String(L"ghi"))};

	testArray(stringBufferArray);
	testArray(cstringBufferArray);
	testArray(StringRangeArray(stringBufferArray, 3));
	testArray(StringRangeArray(cstringBufferArray, 3));

	array<StringBuffer, 3> stringBufferStdArray = {StringBuffer(String(L"abc")), StringBuffer(String(L"def")), StringBuffer(String(L"ghi"))};
	array<const StringBuffer, 3> cstringBufferStdArray = {StringBuffer(String(L"abc")), StringBuffer(String(L"def")), StringBuffer(String(L"ghi"))};
	testArray(stringBufferStdArray);
	testArray(cstringBufferStdArray);
	testArray(const_cast<const array<StringBuffer, 3>&>(stringBufferStdArray));
	testArray(const_cast<const array<const StringBuffer, 3>&>(cstringBufferStdArray));

	vector<StringBuffer> stringBufferVector;
	makeStringBuffer(stringBufferVector);
	testArray(stringBufferVector);
	testArray(const_cast<const vector<StringBuffer>&>(stringBufferVector));
	vector<const StringBuffer> cstringBufferVector;
	makeStringBuffer(cstringBufferVector);
	testArray(cstringBufferVector);
	testArray(const_cast<const vector<const StringBuffer>&>(cstringBufferVector));


	// wstring
	wstring wstringArray[] = {L"abc", L"def", L"ghi"};
	const wstring cwstringArray[] = {L"abc", L"def", L"ghi"};

	testArray(wstringArray);
	testArray(cwstringArray);
	testArray(StringRangeArray(wstringArray, 3));
	testArray(StringRangeArray(cwstringArray, 3));

	array<wstring, 3> wstringStdArray = {L"abc", L"def", L"ghi"};
	array<const wstring, 3> cwstringStdArray = {L"abc", L"def", L"ghi"};
	testArray(wstringStdArray);
	testArray(cwstringStdArray);
	testArray(const_cast<const array<wstring, 3>&>(wstringStdArray));
	testArray(const_cast<const array<const wstring, 3>&>(cwstringStdArray));

	vector<wstring> wstringVector;
	makeVector(wstringVector);
	testArray(wstringVector);
	testArray(const_cast<const vector<wstring>&>(wstringVector));
	vector<const wstring> cwstringVector;
	makeVector(cwstringVector);
	testArray(cwstringVector);
	testArray(const_cast<const vector<const wstring>&>(cwstringVector));
}



	}
}