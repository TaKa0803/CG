#pragma once
#include"Instancing/InstancingGameObject.h"

class Enemy : public InstancingGameObject {

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
	void Draw(const Matrix4x4& view);


};
