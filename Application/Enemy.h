#pragma once
#include"IGameObject/IGameObject.h"

class Enemy : public GameObject {

public:
	/// <summary>
	/// 
	/// </summary>
	void Initialize(const Vector3& position);

	/// <summary>
	/// 
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	void Draw(const Matrix4x4& viewP);


};
