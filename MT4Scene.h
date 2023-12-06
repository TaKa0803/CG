#pragma once
#include"Quaternion.h"

class MT4Scene {
public:
	void Initialize();

	void Update();

	void Draw();

	void Finalize();

private:

	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };

	Quaternion identity = MakeIdentityQuaternion();

	Quaternion conj = Conjugate(q1);

	Quaternion inv = Inverse(q1);

	Quaternion normal = Normalize(q1);

	Quaternion mul1 = q1 * q2;

	Quaternion mul2 = q2 * q1;

	float norm =Norm(q1);

};

