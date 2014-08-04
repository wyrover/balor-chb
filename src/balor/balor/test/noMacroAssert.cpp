#include "noMacroAssert.hpp"

#include <cassert>


namespace balor {
	namespace test {



#pragma warning(push)
#pragma warning(disable : 4100) // 'expression' : 引数は関数の本体部で 1 度も参照されません。
void noMacroAssert(bool expression) {
	assert(expression);
}
#pragma warning(pop)



	}
}