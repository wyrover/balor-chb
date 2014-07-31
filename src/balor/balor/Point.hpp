#pragma once

struct tagPOINT;


namespace balor {



	/**
	* 평면 좌표를 표시하는 구조체.
	*
	* 각종연산자를 서포트하는 것 외에, WIN32의 POINT 구조체와 상호 교환 가능하다.
	*/
	class Point {
	public:
		typedef ::tagPOINT POINT;

	public:
		/// 초기화 하지 않고 작성
		Point() {}
		Point(const Point& value);
		/// POINT 구조체로 초기화.
		Point(const POINT& point);
		/// 좌표로 초기화.
		Point(int x, int y);

	public:
		/// 요소마다 최대값을 구한다.
		static Point maximize(const Point& lhs, const Point& rhs);
		/// 요소마다 최소값을 구한다.
		static Point minimize(const Point& lhs, const Point& rhs);

	public:
		/// POINT 구조체로의 변환.
		operator POINT() const;
		Point operator-() const;
		Point& operator += (const Point& rhs);
		friend Point operator + (const Point& lhs, const Point& rhs);
		Point& operator -= (const Point& rhs);
		friend Point operator - (const Point& lhs, const Point& rhs);
		bool operator == (const Point& rhs) const;
		bool operator != (const Point& rhs) const;

	public:
		int x;
		int y;
	};



}