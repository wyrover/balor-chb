#pragma once

#include <algorithm>
#include <cmath>
#include <limits>


namespace tools {



/// 浮動少数点数の誤差を考慮した等値判定
inline bool floatEquals(float lhs, float rhs) {
	float d = std::max(std::abs(lhs), std::abs(rhs));
	return (d == 0.0f ? 0.0f : std::abs(lhs - rhs) / d) <= std::numeric_limits<float>::epsilon();
}


/// 浮動少数点数の誤差を考慮した等値判定
inline bool floatEquals(double lhs, double rhs) {
	double d = std::max(std::abs(lhs), std::abs(rhs));
	return (d == 0.0 ? 0.0 : std::abs(lhs - rhs) / d) <= std::numeric_limits<double>::epsilon();
}



}