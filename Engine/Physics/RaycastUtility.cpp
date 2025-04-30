#include "RaycastUtility.hpp"
#include <stdexcept>

using namespace physx;

bool RaycastUtility::SingleHit(
	PxScene* scene,
	const PxVec3& origin,
	const PxVec3& direction,
	float maxDistance,
	PxRaycastHit& outHit,
	PxQueryFilterCallback* fiilterCallback = nullptr)
{
	PxRaycastBuffer hit;
	PxQueryFilterData filterData;
	filterData.flags |= PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC;
	if (fiilterCallback) {
		filterData.flags |= PxQueryFlag::ePREFILTER;
	}
	bool status = scene->raycast(
		origin,
		direction,
		maxDistance,
		hit,
		PxHitFlag::eDEFAULT,
		filterData,
		fiilterCallback,
		nullptr,
		PxGeometryQueryFlag::eDEFAULT
	);;

	if (status && hit.hasBlock) {
		outHit = hit.block;
		return true;
	}

	return false;
}

std::vector<PxRaycastHit> RaycastUtility::MultiHit(PxScene* scene,
	const PxVec3& origin,
	const PxVec3& direction,
	float maxDistance,
	int maxHits)

{
	//std::vector<PxRaycastHit> results;


	//PxRaycastBufferN<10> hitBuffer;

	//bool status = scene->raycast(origin, direction, maxDistance, PxHitFlag::eDEFAULT, hitBuffer);

	//if (status) {
	//    for (PxU32 i = 0; i < hitBuffer.nbTouches; ++i) {
	//        results.push_back(hitBuffer.touches[i]);
	//    }
	//}

	//return results;

	// TODO
	throw std::runtime_error("RaycastUtility::MultiHit is not implemented yet.");
}