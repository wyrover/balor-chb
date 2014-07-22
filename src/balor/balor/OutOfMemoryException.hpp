#pragma once

#include <balor/Exception.hpp>


namespace balor {



/// メモリ不足例外。この例外はキャッチすることで修復可能な場合のみ投げられる。
class OutOfMemoryException : public Exception {};



}