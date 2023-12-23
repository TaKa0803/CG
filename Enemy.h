#pragma once
#include"IGameObject.h"

class Enemy : public IGameObject {

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
