#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class IgnoreSelfFilterCallback : public PxQueryFilterCallback {
public:
    PxRigidActor* selfActor;

    IgnoreSelfFilterCallback(PxRigidActor* actor) : selfActor(actor) {}

    PxQueryHitType::Enum preFilter(
        const PxFilterData& filterData,
        const PxShape* shape,
        const PxRigidActor* actor,
        PxHitFlags& queryFlags) override {
        if (actor == selfActor) {

            return PxQueryHitType::eNONE;
        }
        return PxQueryHitType::eBLOCK;
    }

    PxQueryHitType::Enum postFilter(
        const PxFilterData& filterData,
        const PxQueryHit& hit,
        const PxShape* shape,
        const PxRigidActor* actor
    ) override {
        return PxQueryHitType::eBLOCK;
    }


};

