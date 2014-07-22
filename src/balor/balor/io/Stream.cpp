#include "Stream.hpp"


namespace balor {
	namespace io {



Stream::Stream() {
}


Stream::~Stream() {
}


int Stream::read() {
	unsigned char byte;
	if (0 < read(&byte, 0, 1)) {
		return byte;
	} else {
		return -1;
	}
}


void Stream::write(unsigned char value) {
	write(&value, 0, 1);
}



	}
}