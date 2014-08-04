#pragma once

#include <balor/Exception.hpp>


namespace balor {



/// 메모리 부족 예외, 이 예외는 캐치하는 것으로 복원이 가능한 경우에만 던져진다.
class OutOfMemoryException : public Exception {};



}