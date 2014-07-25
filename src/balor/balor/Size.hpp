#pragma once


namespace balor {

class Point;



/**
 * 폭과 높이에 의한 수치를 나타내는 구조체
 */
class Size {
public:
	
	Size() {}
	Size(const Size& size);
	/// Point 구조체로 초기화
	Size(const Point& point);
	/// 폭과 높이로 초기화
	Size(int width, int height);

public:
	/// 요소마다 최소 값을 구한다
	static Size minimize(const Size& lhs, const Size& rhs);
	/// 요소마다 최대 값을 구한다
	static Size maximize(const Size& lhs, const Size& rhs);
	/// 요소에 마이너스 값이 포함 되었는지 조사
	bool negative() const;

public:
	/// Point 구조체로 변환
	operator Point() const;
	Size& operator += (const Size& rhs);
	friend Size operator + (const Size& lhs, const Size& rhs);
	Size& operator -= (const Size& rhs);
	friend Size operator - (const Size& lhs, const Size& rhs);
	Size& operator *= (int rhs);
	friend Size operator * (const Size& lhs, int rhs);
	Size& operator /= (int rhs);
	friend Size operator / (const Size& lhs, int rhs);
	bool operator == (const Size& rhs) const;
	bool operator != (const Size& rhs) const;

public:
	int width;
	int height;
};



}