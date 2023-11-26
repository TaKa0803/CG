#include"Vector3.h"
#include<math.h>



void Vector3::SetNormalize() {

	float length = sqrtf(x * x + y * y + z * z);
	Vector3 num = *this /= length;
	x = num.x;
	y = num.y;
	z = num.z;

}

float Vector3::GetLength() {
	return sqrtf(x * x + y * y + z * z);
}


// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	float Answer = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return Answer;
}

// 長さ
float Length(const Vector3& v) {
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return length;
}



// 正規化
Vector3 Normalize(const Vector3& v) {
	float length = Length(v);
	Vector3 Answer = {
		.x = (v.x / length),
		.y = (v.y / length),
		.z = (v.z / length),
	};
	return Answer;
}




float Distance(const Vector3& v1, const Vector3& v2) {
	Vector3 v = {
		v2.x - v1.x,
		v2.y - v1.y,
		v2.z - v2.z
	};

	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}


