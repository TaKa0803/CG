#include"Vector2.h"
#include<math.h>


// 長さ
float Length(const Vector2& v) {
	float length = sqrtf(v.x * v.x + v.y * v.y);
	return length;
}

Vector2 Normalize(const Vector2& v) {
	float length = Length(v);

	if (length != 0) {
		Vector2 Answer = {
			.x = (v.x / length),
			.y = (v.y / length),
		};
		return Answer;
	}
	return{ 0,0 };
}