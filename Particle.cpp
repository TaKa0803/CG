#include"Particle.h"
#include"RandomNum.h"


Particle MakeNewParticle(const Vector3& spawnPos, const Vector3& emiterSize, const Vector3& maxVelo, const Vector3& minVelo, const Vector4& colorMin, const Vector4& colorMax, const Vector2& lifeTimeminXmaxY) {
	Particle ans;

	ans.world_.translate_ = spawnPos + RandomNumber::Get(-emiterSize / 2, emiterSize / 2);


	ans.velocity = RandomNumber::Get(minVelo, maxVelo);

	ans.color = RandomNumber::Get(colorMin, colorMax);

	ans.lifeTime = RandomNumber::Get(lifeTimeminXmaxY.x, lifeTimeminXmaxY.y);
	ans.currentTime = 0;

	return ans;
}