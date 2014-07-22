#include "Reference.hpp"



namespace balor {



ReferenceBase::ReferenceBase() : _pointer(nullptr), _next(nullptr) {
}


ReferenceBase::ReferenceBase(const ReferenceBase& value) {
	_attach(value._pointer);
}


ReferenceBase::ReferenceBase(Referenceable* pointer) {
	_attach(pointer);
}


ReferenceBase::~ReferenceBase() {
	_detach();
}


void ReferenceBase::_attach(Referenceable* pointer) {
	_pointer = pointer;
	_next = nullptr;
	if (_pointer) {
		auto backup = _pointer->_first;
		_pointer->_first = this;
		_next = backup;
	}
}


void ReferenceBase::_detach() {
	if (_pointer) {
		if (_pointer->_first == this) {
			_pointer->_first = _next;
		} else {
			for (auto i = _pointer->_first; i; i = i->_next) {
				if (i->_next == this) {
					i->_next = _next;
					break;
				}
			}
		}
	}
}



Referenceable::Referenceable() : _first(nullptr) {
}


Referenceable::Referenceable(Referenceable&& value) : _first(value._first) {
	for (auto i = _first; i; i = i->_next) {
		i->_pointer = this;
	}
	value._first = nullptr;
}


Referenceable::~Referenceable() {
	for (auto i = _first; i; i = i->_next) {
		i->_pointer = nullptr;
	}
}


Referenceable& Referenceable::operator=(Referenceable&& value) {
	for (auto i = _first; i; i = i->_next) {
		i->_pointer = nullptr;
	}
	_first = value._first;
	for (auto i = _first; i; i = i->_next) {
		i->_pointer = this;
	}
	value._first = nullptr;
	return *this;
}



}
