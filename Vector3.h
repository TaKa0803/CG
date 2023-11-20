#pragma once

class Vector3 {

public:
	float x;
	float y;
	float z;

#pragma region 演算子オーバーロード
	friend Vector3 operator-(const Vector3& v) { return { -v.x,-v.y,-v.z }; }
	friend Vector3 operator+(const Vector3& v) { return v; }

	friend Vector3 operator+(const Vector3& v1, const Vector3& v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }
	friend Vector3 operator-(const Vector3& v1, const Vector3& v2) { return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z }; }
	friend float operator*(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
	friend Vector3 operator*(const float scalar, const Vector3& v) { return { v.x * scalar, v.y * scalar, v.z * scalar }; }
	friend Vector3 operator*(const Vector3& v, const float scalar) { return { v.x * scalar, v.y * scalar, v.z * scalar }; }
	friend Vector3 operator/(const float num, const Vector3& v) { return { v.x * (1.0f / num), v.y * (1.0f / num), v.z * (1.0f / num) }; }
	friend Vector3 operator/(const Vector3& v, const float num) { return { v.x * (1.0f / num), v.y * (1.0f / num), v.z * (1.0f / num) }; }


	Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vector3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
	Vector3& operator+=(Vector3 v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vector3& operator-=(Vector3 v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vector3& operator*=(Vector3 v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

#pragma endregion
public:
#pragma region 計算して反映
	void SetNormalize();

	float GetLength();
#pragma endregion



};

// 内積
float Dot(const Vector3& v1, const Vector3& v2);

// 長さ
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);

//二点間の距離
float Distance(const Vector3& v1,const Vector3& v2);



