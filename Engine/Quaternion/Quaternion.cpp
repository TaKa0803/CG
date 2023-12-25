#include "Quaternion.h"
#include<cmath>



Quaternion MakeIdentityQuaternion() {

	return {
		0,0,0,1
	};

}

Quaternion Conjugate(const Quaternion& que) {
	Vector3 v3 = { que.x,que.y,que.z };



	Quaternion ans = {
		-v3.x,
		-v3.y,
		-v3.z,
		que.w
	};
	return ans;
}

float Norm(const Quaternion& que) {
	return sqrtf(que.w * que.w + que.x * que.x + que.y * que.y + que.z * que.z);
}

Quaternion Normalize(const Quaternion& que) {
	float norm = Norm(que);

	return {
		que.x / norm,
		que.y / norm,
		que.z / norm,
		que.w / norm,
	};

}

Quaternion Inverse(const Quaternion& que) {
	Quaternion q = Conjugate(que);
	float norm = Norm(que);

	return q * (1.0f / (norm * norm));

}


Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {

	Quaternion ans;

	
	float theta = angle / 2.0f;

	ans = {
		axis.x * std::sin(theta),
		axis.y * std::sin(theta),
		axis.z * std::sin(theta),
		std::cos(theta),
	};

	

	return ans;
}

Vector3 RotateVector(const Vector3& vec, const Quaternion& qua) {

	Quaternion qDash = Conjugate(qua);

	Quaternion r = { vec.x,vec.y,vec.z,0 };

	Quaternion ans=qua * r * qDash;

	return { ans.x,ans.y,ans.z };

}

Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
	return {

		q.w*q.w+q.x*q.x-q.y*q.y-q.z*q.z,2*(q.x*q.y+q.w*q.z),2*(q.x*q.z-q.w*q.y),0,

		2*(q.x*q.y-q.w*q.z),q.w* q.w - q.x * q.x + q.y * q.y - q.z * q.z,2*(q.y*q.z+q.w*q.x),0,

		2*(q.x*q.z+q.w*q.y),2*(q.y*q.z-q.w*q.x),q.w* q.w - q.x * q.x - q.y * q.y + q.z * q.z,0,
		0,0,0,1

	};
}