#include "RandomNum.h"


std::mt19937 RandomNumber::randomEngine_;

void RandomNumber::RandomNumberProcessInitialize() {
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	randomEngine_ = randomEngine;
}

float RandomNumber::GetRandomNum(const float minNum, const float maxNum) {
	std::uniform_real_distribution<float>distribution(minNum, maxNum);

	return distribution(randomEngine_);
}

Vector2 RandomNumber::GetRandomNum(const Vector2 minNum, const Vector2 maxNum) {
	std::uniform_real_distribution<float>distributionx(minNum.x, maxNum.x);
	std::uniform_real_distribution<float>distributiony(minNum.y, maxNum.y);
	
	Vector2 ans={
		distributionx(randomEngine_),
		distributiony(randomEngine_)
	};

	return ans;
}

Vector3 RandomNumber::GetRandomNum(const Vector3& minNum, const Vector3& maxNum) {
	std::uniform_real_distribution<float>distributionx(minNum.x, maxNum.x);
	std::uniform_real_distribution<float>distributiony(minNum.y, maxNum.y);
	std::uniform_real_distribution<float>distributionz(minNum.z, maxNum.z);

	return {
		distributionx(randomEngine_),
		distributiony(randomEngine_),
		distributionz(randomEngine_)
	};
}