#pragma once
#include<random>
#include"Vector2.h"
#include"Vector3.h"

class RandomNumber {
public:

	//エンジンで呼び出しているのでほかで使う必要なし
	static void RandomNumberProcessInitialize();

	/// <summary>
	/// ランダムな数字を取得
	/// </summary>
	/// <param name="minNum">最小値</param>
	/// <param name="maxNum">最大値</param>
	static float GetRandomNum(const float minNum, const float maxNum);


	static Vector2 GetRandomNum(const Vector2 minNum, const Vector2 maxNum);

	static Vector3 GetRandomNum(const Vector3 &minNum, const Vector3& maxNum);


private:
	
	static std::mt19937 randomEngine_;
};


