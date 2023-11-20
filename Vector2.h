#pragma once


class Vector2 {
public:
	float x;
	float y;	

	Vector2& operator+(Vector2 v) { x += v.x; y += v.y; return *this; }
	Vector2& operator-(Vector2 v) { x -= v.x; y -= v.y; return *this; }
	Vector2& operator*(Vector2 v) { x *= v.x; y *= v.y; return *this; }

	friend Vector2 operator+(const Vector2& a, const Vector2& b) { return Vector2( a.x + b.x,a.y +b.y ); }

	friend Vector2 operator-(const Vector2& a, const Vector2& b) { return Vector2(a.x - b.x, a.y - b.y); }

	friend float operator*(const Vector2& a, const Vector2& b) { return a.x * b.x+ a.y * b.y; }

};




