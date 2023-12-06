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
