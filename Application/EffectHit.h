#pragma once
#include"Model/Model.h"
#include"WorldTransform/WorldTransform.h"
#include"Camera/Camera.h"
#include<list>

class EffectHit {
public:
	static EffectHit* GetInstance();

private:
	EffectHit() = default;
	~EffectHit() = default;
	EffectHit(const EffectHit& o) = delete;
	const EffectHit& operator=(const EffectHit& o) = delete;

public:

	

	void Initialize(const Camera* camera);

	void Update();

	void Draw();

	void Finalize();

	void SpawnE(const WorldTransform& world,int num);
public:

	const Camera* camera_;

	int texture_;

	

	//WorldTransform world_;


	struct deadTime {
		WorldTransform world_;
		Vector3 velo_;
		int maxTime_;
		int time_;
		bool isdead_;
	};

	std::vector<std::pair<Model*, deadTime>> effects_;

	//max描画数
	int num = 12;
};

