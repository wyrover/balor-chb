#pragma once

#include <algorithm>
#include <cmath>
#include <limits>


namespace tools {



/// 부동소스점 오차를 고려한 등가 판정
inline bool floatEquals(float lhs, float rhs) {
	float d = std::max(std::abs(lhs), std::abs(rhs));
	return (d == 0.0f ? 0.0f : std::abs(lhs - rhs) / d) <= std::numeric_limits<float>::epsilon();
}


/// 부동소수점 오차를 고려한 등가 판정
inline bool floatEquals(double lhs, double rhs) {
	double d = std::max(std::abs(lhs), std::abs(rhs));
	return (d == 0.0 ? 0.0 : std::abs(lhs - rhs) / d) <= std::numeric_limits<double>::epsilon();
}



}