#pragma once

#include "PhysicsComponent.hpp"

namespace Engine
{
	class PhysicsWorld;

	// Getters

	// Get component data
	void PhysicsComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &type, sizeof(type));
		offset += sizeof(type);
		std::memcpy(data + offset, &translation, sizeof(translation));
		offset += sizeof(translation);
		std::memcpy(data + offset, &scale, sizeof(scale));
		offset += sizeof(scale);
		std::memcpy(data + offset, &rotation, sizeof(rotation));
		offset += sizeof(rotation);
		std::memcpy(data + offset, &isPerson, sizeof(isPerson));
		offset += sizeof(isPerson);
		std::memcpy(data + offset, &entityId, sizeof(entityId));
	}

	// Setters

	// Set component data
	void PhysicsComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy( &type, data + offset, sizeof(type));
		offset += sizeof(type);
		std::memcpy(&translation, data + offset, sizeof(translation));
		offset += sizeof(translation);
		std::memcpy(&scale, data + offset, sizeof(scale));
		offset += sizeof(scale);
		std::memcpy(&rotation, data + offset, sizeof(rotation));
		offset += sizeof(rotation);
		std::memcpy(&isPerson, data + offset, sizeof(isPerson));
		offset += sizeof(isPerson);
		std::memcpy(&entityId, data + offset, sizeof(entityId));
	}

	// Init
	void PhysicsComponent::Init(PhysicsWorld& pworld, PhysicsType physicsType, glm::mat4& transform, int index) {

		entityId = index;

		// parse mat4
		if (!DecomposeTransform(transform, translation, rotation, scale)) {
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}
		scale.x = glm::length(glm::vec3(transform[0]));
		scale.y = glm::length(glm::vec3(transform[1]));
		scale.z = glm::length(glm::vec3(transform[2]));


		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);
		PxMaterial* material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
		material->setRestitution(0.0f);
		type = physicsType;

		switch (type)
		{
		case PhysicsType::STATIC:
		{
			staticBody = pworld.gPhysics->createRigidStatic(pxTransform);
			if (staticBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*staticBody, PxBoxGeometry(scale.x, scale.y, scale.z), *material
				);

				pworld.gScene->addActor(*staticBody);

			}
			break;
		}

		case PhysicsType::DYNAMIC:
		{
			dynamicBody = pworld.gPhysics->createRigidDynamic(pxTransform);

			if (dynamicBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*dynamicBody, PxBoxGeometry(scale.x, scale.y, scale.z), *material
				);

				pworld.gScene->addActor(*dynamicBody);
				pworld.numDynamicRigid++;
			}
			break;
		}

		case PhysicsType::CONTROLLER:
		{
			// set ControllerDescription
			PxCapsuleControllerDesc desc;
			desc.height = 1.f;
			desc.radius = 0.3f;
			desc.stepOffset = 0.1f;
			//desc.contactOffset
			desc.material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
			desc.position = PxExtendedVec3(translation.x, translation.y + desc.height / 2 + desc.radius, translation.z);
			desc.slopeLimit = 0.8f;
			desc.upDirection = PxVec3(0, 1, 0);

			PxCapsuleController* pcontroller = static_cast<PxCapsuleController*>(pworld.gControllerManager->createController(desc));
			controller = pcontroller;
			pworld.controller = pcontroller;

			break;
		}
		}
		//SetComponentHasChanged();
	}

	// Init complex shape
	void PhysicsComponent::InitComplexShape(PhysicsWorld& pWorld, PhysicsType physicsType, Engine::vk::Model& model, glm::mat4& transform, int index) {

		entityId = index;
		type = physicsType;

		// Decompose transform
		if (!DecomposeTransform(transform, translation, rotation, scale)) {
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}

		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);

		PxVec3 pxScale(scale.x, scale.y, scale.z);

		// Iterate over model nodes and primitives to add static rigid bodies
		// based on the model's triangle meshes.
		// Currently adds a static body per primitive mesh, not sure if thats
		// a bad or good thing.
		for (Engine::vk::Node* node : model.nodes) {
			if (node->mesh) {
				for (Engine::vk::Primitive* primitive : node->mesh->primitives) {
					PxMaterial* material = pWorld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

					PxTriangleMeshDesc triMeshDesc;
					triMeshDesc.points.count = primitive->rawData.positions.size();
					triMeshDesc.points.stride = sizeof(glm::vec3);
					triMeshDesc.points.data = primitive->rawData.positions.data();
					triMeshDesc.triangles.count = primitive->rawData.indices.size() / 3;
					triMeshDesc.triangles.stride = sizeof(std::uint32_t) * 3;
					triMeshDesc.triangles.data = primitive->rawData.indices.data();

					assert(triMeshDesc.isValid());

					PxTolerancesScale toleranceScale = pWorld.gPhysics->getTolerancesScale();
					PxCookingParams cookingParams(toleranceScale);
					// If the below line is commented, when cooking the triangle mesh, PhysX will detect 
					// large triangles and output an error in console for primitives that include large 
					// triangles, this is only because it is checking the raw vertex positions and as 
					// such, models like Sponza, which have a large model before scaling, have large 
					// triangles, but when creating the triangle mesh geometry we scale the mesh 
					// appropriately and the resulting mesh geometry used for collisions will have 
					// smaller triangles, hopefully resulting in a stable simulation.
					// There seems to be no difference in disabling cleaning mesh or enabling it,
					// so it is disabled for now.
					cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;

					PxDefaultMemoryOutputStream writeBuffer;
					PxTriangleMeshCookingResult::Enum result;
					bool status = PxCookTriangleMesh(cookingParams, triMeshDesc, writeBuffer, &result);
					if (!status) {
						std::cerr << "Triangle Mesh failed to cook!" << std::endl;
						return;
					}

					PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
					PxTriangleMesh* triMesh = pWorld.gPhysics->createTriangleMesh(readBuffer);
					PxTriangleMeshGeometry triMeshGeometry(triMesh, PxMeshScale(pxScale));

					// Only static triangle meshes are supported for now.
					// Dynamic triangle mesh geometries are possible but are more complicated
					// and I don't believe we have a use case for them just yet.
					if (type != PhysicsType::STATIC)
					{
						std::cerr << "Only static triangle mesh geometries are supported currently!" << std::endl;
						assert(type == PhysicsType::STATIC);
					}
					//assert(physicsType == PhysicsType::STATIC, "Only static triangle mesh geometries are supported currently!");

					switch (type) {
					case PhysicsType::STATIC:
					{
						staticBody = pWorld.gPhysics->createRigidStatic(PxTransform(pxTransform));
						if (staticBody) {
							PxShape* shape = PxRigidActorExt::createExclusiveShape(
								*staticBody, triMeshGeometry, *material
							);

							pWorld.gScene->addActor(*staticBody);
						}
						break;
					}
					}
				}
			}
		}
		//SetComponentHasChanged();
	}

	// Set component has changed in entity manager
	void PhysicsComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}

	bool PhysicsComponent::DecomposeTransform(const glm::mat4& matrix, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		bool success = glm::decompose(matrix, scale, rotation, translation, skew, perspective);
		if (success) {
			rotation = glm::conjugate(rotation);
		}
		return success;
	}
}