#pragma once

struct tagPOINT;


namespace balor {



/**
* ��� ��ǥ�� ǥ���ϴ� ����ü.
*
* ���������ڸ� ����Ʈ�ϴ� �� �ܿ�, WIN32�� POINT ����ü�� ��ȣ ��ȯ �����ϴ�.
*/
class Point {
public:
	typedef ::tagPOINT POINT;

public:
	/// �ʱ�ȭ ���� �ʰ� �ۼ�
	Point() {}
	Point(const Point& value);
	/// POINT ����ü�� �ʱ�ȭ.
	Point(const POINT& point);
	/// ��ǥ�� �ʱ�ȭ.
	Point(int x, int y);

public:
	/// ��Ҹ��� �ִ밪�� ���Ѵ�.
	static Point maximize(const Point& lhs, const Point& rhs);
	/// ��Ҹ��� �ּҰ��� ���Ѵ�.
	static Point minimize(const Point& lhs, const Point& rhs);

public:
	/// POINT ����ü���� ��ȯ.
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