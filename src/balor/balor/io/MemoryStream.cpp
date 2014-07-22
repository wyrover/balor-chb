#include "MemoryStream.hpp"

#include <algorithm>
#include <limits>
#include <utility>

#include <balor/system/windows.hpp> // IsBadWritePtr, IsBadReadPtr‚Ìassert‚Ìˆ×‚¾‚¯‚É•K—v
#include <balor/test/verify.hpp>
#include <balor/scopeExit.hpp>


namespace balor {
	namespace io {

using std::max;
using std::min;
using std::swap;



MemoryStream::MemoryStream(int capacity) {
	assert("capacity less than 0" && 0 < capacity);

	_first = new unsigned char[capacity];
	_last = _first;
	_end = _first + capacity;
	_current = _first;
	_allocatable = true;
	_writable = true;
}


MemoryStream::MemoryStream(void* buffer, int offset, int bufferSize, bool writable)
	: _first(static_cast<unsigned char*>(buffer) + offset), _last(_first + bufferSize), _end(_last), _current(_first), _allocatable(false), _writable(writable) {
	assert("Null buffer" && buffer);
	assert("Negative offset" && 0 <= offset);
	assert("Negative bufferSize" && 0 <= bufferSize);
	assert("Can't write buffer" && (!_writable || !IsBadWritePtr(_first, bufferSize)));
	assert("Can't read buffer" && !IsBadReadPtr(_first, bufferSize));
}


MemoryStream::MemoryStream(MemoryStream&& stream)
	: _first(stream._first), _last(stream._last), _end(stream._end), _current(stream._current), _allocatable(stream._allocatable), _writable(stream._writable) {
	stream._first = nullptr;
	stream._last = nullptr;
	stream._end = nullptr;
	stream._current = nullptr;
	stream._allocatable = false;
}


MemoryStream::~MemoryStream() {
	if (_allocatable) {
		delete [] _first;
	}
	//_first = nullptr;
	//_last = nullptr;
	//_end = nullptr;
	//_current = nullptr;
	//_allocatable = false;
}


MemoryStream& MemoryStream::operator=(MemoryStream&& stream) {
	if (this != &stream) {
		swap(_first, stream._first);
		swap(_last, stream._last);
		swap(_end, stream._end);
		swap(_current, stream._current);
		swap(_allocatable, stream._allocatable);
		swap(_writable, stream._writable);
	}
	return *this;
}


void* MemoryStream::buffer() {
	return _first;
}


const void* MemoryStream::buffer() const {
	return _first;
}


int MemoryStream::capacity() const {
	return _end - _first;
}


void MemoryStream::flush() {
	// ‰½‚à‚µ‚È‚¢
}


__int64 MemoryStream::length() const {
	return _last - _first;
}


void MemoryStream::length(int value) {
	assert("Invalid MemoryStream" && _first);
	assert("Negative length" && 0 <= value);
	assert("write unsupported" && writable());

	int newCapacity = capacity();
	if (newCapacity < value) {
		assert("Can't allocate new buffer" && _allocatable);
		if (newCapacity == 0) {
			newCapacity = 256;
		}
		while (newCapacity < value) {
			newCapacity *= 2;
		}
		const int oldPosition = _current - _first;
		const int oldLength = _last - _first;
		const unsigned char* oldFirst = _first;
		scopeExit([&] () {
			delete [] oldFirst;
		});
		_first = new unsigned char[newCapacity];
		std::memcpy(_first, oldFirst, oldLength);
		_last = _first + oldLength;
		_end = _first + newCapacity;
		_current = _first + oldPosition;
	}
	_last = _first + value;
}


__int64 MemoryStream::position() const {
	return _current - _first;
}


void MemoryStream::position(__int64 value) {
	assert("Invalid MemoryStream" && _first);
	assert("Negative position" && 0 <= value);
	assert("position overflow" && value <= INT_MAX);

	value = min(max(static_cast<__int64>(0), value), static_cast<__int64>(INT_MAX));
	_current = _first + value;
}


int MemoryStream::read(void* buffer, int offset, int count) {
	assert("Invalid MemoryStream" && _first);
	assert("Null buffer" && buffer);
	assert("Negative offset" && 0 <= offset);
	assert("Negative count" && 0 <= count);
	assert("buffer is bad write pointer" && !IsBadWritePtr(buffer, offset + count));
	assert("read unsupported" && readable());

	const int readCount = std::min(std::max(0,(int)( _last - _current)), count); // _current ‚Í skip ‚É‚æ‚Á‚Ä _last ‚æ‚èŒã‚ë‚És‚­‚±‚Æ‚ª‚Å‚«‚é
	std::memcpy(static_cast<unsigned char*>(buffer) + offset, _current, readCount);
	_current += readCount;
	return readCount;
}


bool MemoryStream::readable() const {
	return true;
}


__int64 MemoryStream::skip(__int64 offset) {
	assert("Invalid MemoryStream" && _first);
	assert("offset overflow" && offset <= INT_MAX);

	__int64 oldPosition = position();
	__int64 newPosition = min(max(static_cast<__int64>(0), oldPosition + offset), static_cast<__int64>(INT_MAX));
	_current = _first + newPosition;
	return newPosition - oldPosition;
}



void MemoryStream::write(const void* buffer, int offset, int count) {
	assert("Invalid MemoryStream" && _first);
	assert("Null buffer" && buffer);
	assert("Negative offset" && 0 <= offset);
	assert("Negative count" && 0 <= count);
	assert("buffer is bad read pointer" && !IsBadReadPtr(buffer, offset + count));
	assert("write unsupported" && writable());

	unsigned char* newCurrent = _current + count;
	if (_last < newCurrent) {
		if (_end < newCurrent && !_allocatable) {
			throw BufferOverrunException();
		}
		length(newCurrent - _first);
	}
	std::memcpy(_current, static_cast<const unsigned char*>(buffer) + offset, count);
	_current += count;
}


bool MemoryStream::writable() const {
	return _writable;
}



	}
}