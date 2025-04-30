#pragma once
#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

class RaycastUtility {
public:
	static bool SingleHit(
		PxScene* scene,
		const PxVec3& origin,
		const PxVec3& direction,
		float maxDistance,
		PxRaycastHit& outHit,
		PxQueryFilterCallback* fiilterCallback
	);

	static std::vector<PxRaycastHit> MultiHit(
		PxScene* scene,
		const PxVec3& origin,
		const PxVec3& direction,
		float maxDistance,
		int maxHits = 10
	);
};
