#include "StringRangeArray.hpp"

#include <string>

#include <balor/io/File.hpp>
#include <balor/String.hpp>
#include <balor/StringBuffer.hpp>


namespace balor {

using std::wstring;
using namespace balor::io;



StringRange StringRangeArray::_charPointerAccessor(const void* begin, int index) {
	return reinterpret_cast<const wchar_t**>(const_cast<void*>(begin))[index];
}


StringRange StringRangeArray::_fileAccessor(const void* begin, int index) {
	return reinterpret_cast<const File*>(begin)[index];
}


StringRange StringRangeArray::_stringAccessor(const void* begin, int index) {
	return reinterpret_cast<const String*>(begin)[index];
}


StringRange StringRangeArray::_stringBufferAccessor(const void* begin, int index) {
	return reinterpret_cast<const StringBuffer*>(begin)[index];
}


StringRange StringRangeArray::_stdStringAccessor(const void* begin, int index) {
	return reinterpret_cast<const wstring*>(begin)[index];
}



}