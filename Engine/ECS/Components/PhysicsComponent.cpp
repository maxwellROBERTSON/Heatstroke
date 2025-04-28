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

		if (std::memcmp(&type, data + offset, sizeof(type)) != 0)
		{
			std::memcpy(&type, data + offset, sizeof(type));
			SetComponentHasChanged();
		}
		offset += sizeof(type);

		if (std::memcmp(&translation, data + offset, sizeof(translation)) != 0)
		{
			std::memcpy(&translation, data + offset, sizeof(translation));
			SetComponentHasChanged();
		}
		offset += sizeof(translation);

		if (std::memcmp(&scale, data + offset, sizeof(scale)) != 0)
		{
			std::memcpy(&scale, data + offset, sizeof(scale));
			SetComponentHasChanged();
		}
		offset += sizeof(scale);

		if (std::memcmp(&rotation, data + offset, sizeof(rotation)) != 0)
		{
			std::memcpy(&rotation, data + offset, sizeof(rotation));
			SetComponentHasChanged();
		}
		offset += sizeof(rotation);

		if (std::memcmp(&isPerson, data + offset, sizeof(isPerson)) != 0)
		{
			std::memcpy(&isPerson, data + offset, sizeof(isPerson));
			SetComponentHasChanged();
		}
		offset += sizeof(isPerson);

		if (std::memcmp(&entityId, data + offset, sizeof(entityId)) != 0)
		{
			std::memcpy(&entityId, data + offset, sizeof(entityId));
			SetComponentHasChanged();
		}
		offset += sizeof(entityId);
	}


	// Init
	// isClient = true if the system using the function is a client (not the server)
	// isLocalPlayer = true if this entity is the local client's entity
	void PhysicsComponent::Init(PhysicsWorld& pworld, PhysicsType physicsType, vk::Model& model, glm::mat4 transform, int index, bool isClient, bool isLocalPlayer)
	{
		entityId = index;

		// parse mat4
		if (!DecomposeTransform(transform, translation, rotation, scale))
		{
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}

		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);

		glm::vec3 glmHalfExtent = (model.bbMax - model.bbMin) / 2.0f;
		PxVec3 halfExtent(std::max(0.001f, glmHalfExtent.x), std::max(0.001f, glmHalfExtent.y), std::max(0.001f, glmHalfExtent.z));
		halfExtent.x *= scale.x;
		halfExtent.y *= scale.y;
		halfExtent.z *= scale.z;

		PxMaterial* material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
		material->setRestitution(0.0f);
		type = physicsType;

		switch (type)
		{
		case PhysicsType::STATIC:
		{
			if (isClient && !isLocalPlayer)
			{
				break;
			}

			staticBody = pworld.gPhysics->createRigidStatic(pxTransform);
			if (staticBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*staticBody, PxBoxGeometry(halfExtent), *material
				);

				//staticBody->setActorFlag(PxActorFlag::eVISUALIZATION, true);
				pworld.gScene->addActor(*staticBody);

			}
			break;
		}

		case PhysicsType::DYNAMIC:
		{
			dynamicBody = pworld.gPhysics->createRigidDynamic(pxTransform);

			if (!dynamicBody) break;

			dynamicBody->setActorFlag(PxActorFlag::eVISUALIZATION, true);

			PxShape* shape = PxRigidActorExt::createExclusiveShape(
				*dynamicBody, PxBoxGeometry(halfExtent), *material
			);

			if (isClient && !isLocalPlayer)
			{
				// Remote entity on client → make kinematic
				dynamicBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			}

			pworld.gScene->addActor(*dynamicBody);

			if (!(isClient && !isLocalPlayer))
			{
				// Only server or local-controlled entities count for simulation stats
				pworld.numDynamicRigid++;
			}

			break;
		}

		case PhysicsType::CONTROLLER:
		{
			if ((!isClient) || (isClient && !isLocalPlayer))
				break;

			// set ControllerDescription
			PxCapsuleControllerDesc desc;
			/*glm::vec3 worldMin = glm::vec3(transform * glm::vec4(model.bbMin, 1.0f));
			glm::vec3 worldMax = glm::vec3(transform * glm::vec4(model.bbMax, 1.0f));*/
			glm::vec3 worldMin = model.bbMin;
			glm::vec3 worldMax = model.bbMax;

			glmHalfExtent = (worldMax - worldMin) / 2.0f;
			halfExtent = PxVec3(std::max(0.001f, glmHalfExtent.x), std::max(0.001f, glmHalfExtent.y), std::max(0.001f, glmHalfExtent.z));
			desc.radius = halfExtent.x > halfExtent.z ? halfExtent.x: halfExtent.z;
			desc.height = halfExtent.y * 2 - (2.0f * desc.radius);
			if (desc.height < 0.0f)
				desc.height = 0.0f;
			desc.stepOffset = 0.1f;
			//desc.contactOffset
			desc.material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
			//desc.position = PxExtendedVec3(translation.x, translation.y + desc.height / 2 + desc.radius, translation.z);
			desc.position = PxExtendedVec3(translation.x, translation.y, translation.z);
			desc.slopeLimit = 0.8f;
			desc.upDirection = PxVec3(0, 1, 0);

			PxCapsuleController* pcontroller = static_cast<PxCapsuleController*>(pworld.gControllerManager->createController(desc));
			controller = pcontroller;
			pworld.controller = pcontroller;

			break;
		}
		}
	}

	// Init complex shape
	void PhysicsComponent::InitComplexShape(PhysicsWorld& pWorld, PhysicsType physicsType, Engine::vk::Model& model, glm::mat4 transform, int index)
	{
		entityId = index;

		// Decompose transform
		if (!DecomposeTransform(transform, translation, rotation, scale)) {
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}

		/*PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);*/

		//PxVec3 pxScale(scale.x, scale.y, scale.z);

		// Iterate over model nodes and primitives to add static rigid bodies
		// based on the model's triangle meshes.
		// Currently adds a static body per primitive mesh, not sure if thats
		// a bad or good thing.
		int count = 0;
		for (Engine::vk::Node* node : model.linearNodes) {
			if (node->mesh) {
				glm::mat4 nodeMatrix = transform * node->getModelMatrix();

				glm::mat3 rotationMatrix = glm::mat3(nodeMatrix);
				glm::vec3 nodeTranslation = glm::vec3(nodeMatrix[3][0], nodeMatrix[3][1], nodeMatrix[3][2]);

				glm::vec3 nodeScale;
				nodeScale.x = glm::length(rotationMatrix[0]);
				nodeScale.y = glm::length(rotationMatrix[1]);
				nodeScale.z = glm::length(rotationMatrix[2]);

				rotationMatrix[0] = rotationMatrix[0] / nodeScale.x;
				rotationMatrix[1] = rotationMatrix[1] / nodeScale.y;
				rotationMatrix[2] = rotationMatrix[2] / nodeScale.z;

				glm::quat nodeRotation = glm::quat_cast(rotationMatrix);

				PxTransform nodePxTransform(
					PxVec3(nodeTranslation.x, nodeTranslation.y, nodeTranslation.z),
					PxQuat(nodeRotation.x, nodeRotation.y, nodeRotation.z, nodeRotation.w)
				);

				PxVec3 nodePxScale(nodeScale.x, nodeScale.y, nodeScale.z);

				//glm::vec3 nodeTranslation, nodeScale;
				//glm::quat nodeRotation;
				//if (!DecomposeTransform(nodeMatrix, nodeTranslation, nodeRotation, nodeScale)) {
				//	std::cerr << "DecomposeTransform failed on node!" << std::endl;
				//	continue;
				//}

				///*nodeTranslation = translation + rotation * nodeTranslation;
				//nodeRotation = rotation * nodeRotation;
				//nodeScale = scale * nodeScale;*/

				//PxTransform nodePxTransform(
				//	PxVec3(nodeTranslation.x, nodeTranslation.y, nodeTranslation.z),
				//	PxQuat(nodeRotation.x, nodeRotation.y, nodeRotation.z, nodeRotation.w)
				//);

				//PxVec3 nodePxScale(nodeScale.x, nodeScale.y, nodeScale.z);

				for (Engine::vk::Primitive* primitive : node->mesh->primitives) {
					PxMaterial* material = pWorld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

					PxTriangleMeshDesc triMeshDesc;
					triMeshDesc.points.count = (PxU32)primitive->rawData.positions.size();
					triMeshDesc.points.stride = sizeof(glm::vec3);
					triMeshDesc.points.data = primitive->rawData.positions.data();
					triMeshDesc.triangles.count = (PxU32)primitive->rawData.indices.size() / 3;
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
					PxTriangleMeshGeometry triMeshGeometry(triMesh, PxMeshScale(nodePxScale));

					// Only static triangle meshes are supported for now.
					// Dynamic triangle mesh geometries are possible but are more complicated
					// and I don't believe we have a use case for them just yet.
					if (physicsType != PhysicsType::STATIC)
					{
						std::cerr << "Only static triangle mesh geometries are supported currently!" << std::endl;
						assert(physicsType == PhysicsType::STATIC);
					}
					//assert(physicsType == PhysicsType::STATIC, "Only static triangle mesh geometries are supported currently!");

					switch (physicsType) {
					case PhysicsType::STATIC:
					{
						staticBody = pWorld.gPhysics->createRigidStatic(PxTransform(nodePxTransform));
						if (staticBody) {
							PxShape* shape = PxRigidActorExt::createExclusiveShape(
								*staticBody, triMeshGeometry, *material
							);

							staticBody->setActorFlag(PxActorFlag::eVISUALIZATION, true);

							pWorld.gScene->addActor(*staticBody);
							count++;
						}
						break;
					}
					}
				}
			}
		}
		std::cout << "Scene actor count: " << count << std::endl;
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