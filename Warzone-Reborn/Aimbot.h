#pragma once
#include "Vec.h"

namespace Aimbot
{
	struct Aim
	{
		Aim();
		~Aim();

		float	fInitCenterDist;
		float	fMinCenterDist;
		Vector2 pLockPosition;
	};

	void ResetLock();
	bool IsKeyDown(int vk_key);
	void GetTargetInFOV(Vector2 pTargetPos);
	void AimLockOnTarget();
	void AimbotInit(int playerId, float Distance, Vector3 LocalPlayerPos, Vector3 PlayerPos, int Stance);
}