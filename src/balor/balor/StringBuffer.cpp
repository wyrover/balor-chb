#include "StringBuffer.hpp"

#include <algorithm>
#include <cstring>
#include <string>
#include <utility>

#include <balor/locale/Unicode.hpp>
#include <balor/system/windows.hpp>
#include <balor/test/verify.hpp>
#include <balor/String.hpp>



namespace balor {

using std::max;
using std::min;
using std::move;
using std::swap;
using std::wstring;
using namespace balor::locale;


namespace {
bool isOverlapped(const wchar_t* lhs, int llength, const wchar_t* rhs, int rlength) {
	const wchar_t* lhsend = lhs + llength;
	const wchar_t* rhsend = rhs + rlength;
	return lhs <= rhsend && rhs <= lhsend;
}

wchar_t nullString[] = L""; // DLL‚²‚Æ‚ÉŽÀ‘Ì‚ð‚à‚Á‚Ä‚à‚©‚Ü‚í‚È‚¢
} // namespace



StringBuffer::StringBuffer() : _begin(nullString), _length(0), _capacity(1), _allocatable(true) {
}


StringBuffer::StringBuffer(const StringBuffer& value) : _begin(1 < value._capacity ? new wchar_t[value._capacity] : nullString), _length(value._length), _capacity(value._capacity), _allocatable(true) {
	wmemcpy(_begin, value._begin, _length + 1);
}


StringBuffer::StringBuffer(StringBuffer&& value) : _begin(value._begin), _length(value._length), _capacity(value._capacity), _allocatable(value._allocatable) {
	value._begin = nullString;
	value._length = 0;
	value._capacity = 1;
	value._allocatable = false;
}



StringBuffer::StringBuffer(const String& value) {
	_length = value.length();
	_capacity = _length + 1;
	_allocatable = true;
	if (!_length) {
		_begin = nullString;
	} else {
		_begin = new wchar_t[_capacity];
		wmemcpy(_begin, value.c_str(), _capacity);
	}
}


StringBuffer::StringBuffer(String&& value) {
	_length = value.length();
	_capacity = _length + 1;
	_allocatable = true;
	if (!_length) {
		_begin = nullString;
	} else if (value.referred()) {
		_begin = new wchar_t[_capacity];
		wmemcpy(_begin, value.c_str(), _capacity);
	} else {
		_begin = const_cast<wchar_t*>(value._c_str);
		value._c_str = String().c_str();
		value._length = 0;
	}
}


StringBuffer::StringBuffer(int capacity) : _begin(1 < capacity ? new wchar_t[capacity] : nullString), _length(0), _capacity(capacity), _allocatable(true) {
	assert("Invalid capacity" && 0 < capacity);
	_begin[0] = L'\0';
}


StringBuffer::StringBuffer(wchar_t* buffer, int capacity, int length) : _begin(buffer), _length(length), _capacity(capacity), _allocatable(false) {
	assert("Can't write buffer" && !IsBadWritePtr(buffer, capacity * sizeof(buffer[0])));
	assert("Can't read buffer" && !IsBadReadPtr(buffer, capacity * sizeof(buffer[0])));
	assert("Invalid capacity" && 0 < capacity);
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length < capacity);
	_begin[length] = L'\0';
}


StringBuffer::~StringBuffer() {
	if (allocatable() && _begin != nullString) {
		delete[] _begin;
	}
}


StringBuffer& StringBuffer::operator=(const StringBuffer& value) {
	if (&value != this) {
		length(0);
		*this += value;
	}
	return *this;
}


StringBuffer& StringBuffer::operator=(StringBuffer&& value) {
	swap(_begin, value._begin);
	swap(_length, value._length);
	swap(_capacity, value._capacity);
	swap(_allocatable, value._allocatable);
	return *this;
}


bool StringBuffer::allocatable() const {
	return _allocatable;
}


int StringBuffer::capacity() const {
	return _capacity;
}


void StringBuffer::capacity(int value) {
	assert("capacity is smaller than length" && _length < value);
	assert("Cannot change capacity" && allocatable());
	if (value != capacity()) {
		StringBuffer buffer(value);
		buffer += *this;
		*this = move(buffer);
	}
}


bool StringBuffer::empty() const {
	return !*_begin;
}


void StringBuffer::erase(int beginIndex) {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= _length);
	_begin[beginIndex] = L'\0';
	_length = beginIndex;
}


void StringBuffer::erase(int beginIndex, int length) {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= _length);
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= _length - beginIndex);

	wchar_t* begin = _begin + beginIndex;
	wmemmove(begin, begin + length, _length - length - beginIndex + 1);
	_length -= length;
}


void StringBuffer::expandLength() {
	_length += String::getLength(end());
}


void StringBuffer::expandLength(int delta) {
	assert("Too small delta" && 0 <= _length + delta);
	assert("Too big delta" && _length + delta < capacity());
	_length += delta;
}


void StringBuffer::insert(int beginIndex, wchar_t value, int count) {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= _length);
	assert("Negative count" && 0 <= count);
	const int newLength = _length + count;
	if (newLength < _capacity) {
		wchar_t* i = _begin + beginIndex;
		wmemmove(i + count, i, _length - beginIndex + 1);
		wmemset(i, value, count);
		_length += count;
	} else {
		const int newCapacity = _getNewCapacity(newLength);
		wchar_t* newBegin = new wchar_t[newCapacity];
		wmemcpy(newBegin, _begin, beginIndex);
		wmemset(newBegin + beginIndex, value, count);
		wmemcpy(newBegin + beginIndex + count, _begin + beginIndex, _length - beginIndex + 1);
		if (1 < _capacity) {
			delete [] _begin;
		}
		_begin = newBegin;
		_length += count;
		_capacity = newCapacity;
	}
}


void StringBuffer::insert(int beginIndex, StringRange value) {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= _length);
	assert("value overlapped" && !isOverlapped(_begin + beginIndex, _length - beginIndex + value.length(), value.c_str(), value.length()));

	const int valueLength = value.length();
	const int newLength = _length + valueLength;
	if (newLength < _capacity) {
		wchar_t* i = _begin + beginIndex;
		wmemmove(i + valueLength, i, _length - beginIndex + 1);
		wmemcpy(i, value.c_str(), valueLength);
		_length += valueLength;
	} else {
		const int newCapacity = _getNewCapacity(newLength);
		wchar_t* newBegin = new wchar_t[newCapacity];
		wmemcpy(newBegin, _begin, beginIndex);
		wmemcpy(newBegin + beginIndex, value.c_str(), valueLength);
		wmemcpy(newBegin + beginIndex + valueLength, _begin + beginIndex, _length - beginIndex + 1);
		if (1 < _capacity) {
			delete [] _begin;
		}
		_begin = newBegin;
		_length += valueLength;
		_capacity = newCapacity;
	}
}


void StringBuffer::length(int value) {
	assert("length out of range" && 0 <= value);
	assert("length out of range" && (!_capacity || value < _capacity));
	_begin[value] = L'\0';
	_length = value;
}


void StringBuffer::replace(wchar_t oldValue, wchar_t newValue) {
	for (auto i = _begin, end = _begin + _length; i != end; ++i) {
		*i = *i == oldValue ? newValue : *i;
	}
}


void StringBuffer::replace(StringRange oldValue, StringRange newValue) {
	assert("Empty oldValue" && !oldValue.empty());

	const int oldValueLength = oldValue.length();
	const int newValueLength = newValue.length();
	int pos = 0;
	while (0 <= (pos = String::refer(*this).indexOf(oldValue, pos))) {
		replace(pos, oldValueLength, newValue);
		pos += newValueLength;
	}
}


void StringBuffer::replace(int beginIndex, int length, StringRange value) {
	assert("beginIndex out of range" && 0 <= beginIndex);
	assert("beginIndex out of range" && beginIndex <= _length);
	assert("length out of range" && 0 <= length);
	assert("length out of range" && length <= _length - beginIndex);
	assert("value overlapped" && !isOverlapped(_begin + beginIndex, _length - beginIndex, value.c_str(), value.length()));

	const int valueLength = value.length();
	if (length != valueLength) {
		const int newLength = _length + valueLength - length;
		if (newLength < _capacity) {
			wchar_t* i = _begin + beginIndex;
			wmemmove(i + valueLength, i + length, _length - beginIndex - length + 1);
			wmemcpy(i, value.c_str(), valueLength);
			_length = newLength;
		} else {
			const int newCapacity = _getNewCapacity(newLength);
			wchar_t* newBegin = new wchar_t[newCapacity];
			wmemcpy(newBegin, _begin, beginIndex);
			wmemcpy(newBegin + beginIndex, value.c_str(), valueLength);
			wmemcpy(newBegin + beginIndex + valueLength, _begin + beginIndex + length, _length - beginIndex - length + 1);
			if (1 < _capacity) {
				delete [] _begin;
			}
			_begin = newBegin;
			_length = newLength;
			_capacity = newCapacity;
		}
	} else {
		wmemcpy(_begin + beginIndex, value.c_str(), valueLength);
	}
}


void StringBuffer::reserve(int maxLength) {
	assert("Negative maxLength" && 0 <= maxLength);
	if (_capacity <= maxLength) {
		assert("Not enough fixed capacity" && allocatable());
		capacity(_getNewCapacity(maxLength));
	}
}


void StringBuffer::reserveAdditionally(int additionalLength) {
	reserve(_length + additionalLength);
}


void StringBuffer::trim(StringRange trimChars) {
	const wchar_t* begin = _begin;
	const wchar_t* end = _begin + _length;
	if (trimChars.empty()) {
		while (begin != end) {
			if (!Unicode::isWhiteSpace(*begin)) {
				break;
			}
			++begin;
		}
		while (begin <= --end) {
			if (!Unicode::isWhiteSpace(*end)) {
				break;
			}
		}
		++end;
	} else {
		while (begin != end) {
			bool find = false;
			for (const wchar_t* i = trimChars.c_str(); *i; ++i) {
				if (*begin == *i) {
					find = true;
					break;
				}
			}
			if (!find) {
				break;
			}
			++begin;
		}
		while (begin <= --end) {
			bool find = false;
			for (const wchar_t* i = trimChars.c_str(); *i; ++i) {
				if (*end == *i) {
					find = true;
					break;
				}
			}
			if (!find) {
				break;
			}
		}
		++end;
	}
	erase(end - _begin);
	erase(0, begin - _begin);
}


StringBuffer::operator wstring() const {
	return wstring(_begin, _length);
}


wchar_t& StringBuffer::operator[] (int index) {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= _length);
	return _begin[index];
}


const wchar_t& StringBuffer::operator[] (int index) const {
	assert("index out of range" && 0 <= index);
	assert("index out of range" && index <= _length);
	return _begin[index];
}


StringBuffer& StringBuffer::operator+=(const StringBuffer& rhs) {
	*this += String::refer(rhs);
	return *this;
}


StringBuffer& StringBuffer::operator+=(const String& rhs) {
	assert("rhs overlapped" && !isOverlapped(end(), min(_capacity - 1, rhs.length()), rhs.c_str(), rhs.length()));
	const int rLength = rhs.length();
	const int newLength = _length + rLength;
	if (newLength < _capacity) {
		wmemcpy(_begin + _length, rhs.c_str(), rLength + 1);
	} else {
		int newCapacity = _getNewCapacity(newLength);
		wchar_t* newBegin = new wchar_t[newCapacity];
		wmemcpy(newBegin, _begin, _length);
		wmemcpy(newBegin + _length, rhs.c_str(), rLength + 1);
		if (1 < _capacity) {
			delete [] _begin;
		}
		_begin = newBegin;
		_capacity = newCapacity;
	}
	_length = newLength;
	return *this;
}


StringBuffer& StringBuffer::operator+=(const wstring& rhs) {
	*this += String::refer(rhs);
	return *this;
}


StringBuffer& StringBuffer::operator+=(const wchar_t* rhs) {
	*this += String::refer(rhs);
	return *this;
}


StringBuffer& StringBuffer::operator+=(wchar_t* rhs) {
	*this += String::refer(rhs);
	return *this;
}


StringBuffer& StringBuffer::operator+=(wchar_t rhs) {
	wchar_t buffer[] = {rhs, L'\0'};
	*this += String::refer(buffer, 1);
	return *this;
}


int StringBuffer::_getNewCapacity(int newLength) const {
	assert("Cannot expand capacity" && _allocatable);
	if (_capacity <= 1) {
		return newLength + 1;
	} else {
		int newCapacity = _capacity;
		while (newCapacity <= newLength) {
			newCapacity *= 2;
		}
		return newCapacity;
	}
}



}