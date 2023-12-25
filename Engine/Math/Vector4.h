#pragma once

struct Vector4 {
	float x;
	float y;
	float z;
	float w;

	
	friend Vector4 operator-(const Vector4& v) { return { -v.x,-v.y,-v.z,-v.w }; }
};