#include <balor/io/Stream.hpp>

#include <utility>

#include <balor/String.hpp>
#include <balor/io/MemoryStream.hpp>
#include <balor/test/UnitTest.hpp>


namespace balor {
	namespace io {
		namespace testStream {


using std::move;
using namespace balor::io;



testCase(readByteAndWriteByte) {
	MemoryStream stream;
	stream.write('a');
	testAssert(stream.length() == 1);
	stream.position(0);
	testAssert(stream.read() == 'a');
	testAssert(stream.read() == -1);
}



		}
	}
}