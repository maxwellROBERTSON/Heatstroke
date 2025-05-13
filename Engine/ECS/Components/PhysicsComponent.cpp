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
		offset += sizeof(reset);
		std::memcpy(data + offset, &reset, sizeof(reset));
	}

	// Get this components physx actor
	PxActor* PhysicsComponent::GetComponentActor()
	{
		switch (type)
		{
		case PhysicsComponent::PhysicsType::STATIC:
			return GetStaticBody();
		case PhysicsComponent::PhysicsType::DYNAMIC:
			return GetDynamicBody();
		case PhysicsComponent::PhysicsType::CONTROLLER:
			return GetController()->getActor();
		default:
			return nullptr;
		}
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

		if (std::memcmp(&reset, data + offset, sizeof(reset)) != 0)
		{
			std::memcpy(&reset, data + offset, sizeof(reset));
			if (reset)
			{
				entity->ResetToSpawnState();
				glm::vec3 pos = entity->GetPosition();
				GetController()->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
				reset = false;
				std::cout << "Reset" << std::endl;
			}
		}
		offset += sizeof(reset);
	}

	// Init
	// isClient = true if the system using the function is a client (not the server)
	// isLocalPlayer = true if this entity is the local client's entity
	void PhysicsComponent::Init(PhysicsWorld& pworld, PhysicsType physicsType, vk::Model& model, glm::mat4 transform, int index, bool isClient, bool isLocalPlayer)
	{
		entityId = index;

		type = physicsType;

		if (!isClient)
		{
			simulation = PhysicsSimulation::NOTUPDATED;
		}
		else if (isClient && !isLocalPlayer)
		{
			simulation = PhysicsSimulation::LOCALLYUPDATED;
		}
		else if (isClient && isLocalPlayer)
		{
			simulation = PhysicsSimulation::LOCALLYSIMULATED;
		}

		// parse mat4
		if (!DecomposeTransform(transform, translation, rotation, scale))
		{
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}

		glm::mat4 transformNoTranslation = transform;
		transformNoTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);

		glm::vec3 worldSpaceMin = glm::vec3(FLT_MAX);
		glm::vec3 worldSpaceMax = glm::vec3(-FLT_MAX);

		for (Engine::vk::Node* node : model.linearNodes) {
			if (node->mesh) {
				glm::mat4 nodeMatrix = transformNoTranslation * node->getModelMatrix();

				glm::vec3 localMin = node->bbMin;
				glm::vec3 localMax = node->bbMax;

				// Generate all 8 corners of the AABB
				glm::vec3 corners[8] = {
					node->bbMin,
					glm::vec3(localMin.x, localMin.y, localMax.z),
					glm::vec3(localMin.x, localMax.y, localMin.z),
					glm::vec3(localMin.x, localMax.y, localMax.z),
					glm::vec3(localMax.x, localMin.y, localMin.z),
					glm::vec3(localMax.x, localMin.y, localMax.z),
					glm::vec3(localMax.x, localMax.y, localMin.z),
					node->bbMax
				};

				for (int i = 0; i < 8; ++i) {
					glm::vec3 worldCorner = glm::vec3(nodeMatrix * glm::vec4(corners[i], 1.0f));
					worldSpaceMin = glm::min(worldSpaceMin, worldCorner);
					worldSpaceMax = glm::max(worldSpaceMax, worldCorner);
				}
			}
		}

		glm::vec3 glmHalfExtent = (worldSpaceMax - worldSpaceMin) / 2.0f;
		PxVec3 halfExtent(std::max(0.001f, glmHalfExtent.x), std::max(0.001f, glmHalfExtent.y), std::max(0.001f, glmHalfExtent.z));
		// std::cout << "Max: " << worldSpaceMax.x << " " << worldSpaceMax.y << " " << worldSpaceMax.z << std::endl;
		// std::cout << "Min: " << worldSpaceMin.x << " " << worldSpaceMin.y << " " << worldSpaceMin.z << std::endl;
		// std::cout << "glmHalf: " << glmHalfExtent.x << " " << glmHalfExtent.y << " " << glmHalfExtent.z << std::endl;
		// std::cout << "halfExtent: " << halfExtent.x << " " << halfExtent.y << " " << halfExtent.z << std::endl;

		PxMaterial* material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
		material->setRestitution(0.0f);

		switch (type)
		{
		case PhysicsType::STATIC:
		{
			staticBody = pworld.gPhysics->createRigidStatic(pxTransform);
			if (staticBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*staticBody, PxBoxGeometry(halfExtent), *material
				);

				staticBody->setActorFlag(PxActorFlag::eVISUALIZATION, true);
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
			if ((!isClient))
				break;

			float radius = halfExtent.x > halfExtent.z ? halfExtent.x : halfExtent.z;
			float height = halfExtent.y * 2.0f - (2.0f * radius);
			if (height <= 0.0f)
				height = 0.01f;
			
			PxCapsuleControllerDesc desc;
			desc.radius = radius;
			desc.height = height;
			desc.stepOffset = 0.1f;
			desc.scaleCoeff = 1.0f;
			desc.contactOffset = 0.001f * radius;
			desc.material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
			desc.position = PxExtendedVec3(translation.x, translation.y + (height / 2 + radius), translation.z);
			desc.slopeLimit = 0.3f;
			desc.upDirection = PxVec3(0, 1, 0);

			PxCapsuleController* pcontroller = static_cast<PxCapsuleController*>(pworld.gControllerManager->createController(desc));
			controller = pcontroller;

			if (isLocalPlayer)
			{
				pworld.controller = pcontroller;
				pworld.setControllerEntity(entity);
				pworld.setControllerHeight(halfExtent.y * 2 * 0.9);
			}

			break;
		}
		}
	}

	void PhysicsComponent::InitComplexShape(const char* name, PhysicsWorld& pWorld, PhysicsType physicsType, Engine::vk::Model& model, glm::mat4 transform, int index)
	{
		entityId = index;

		glm::vec3 worldSpaceMin = glm::vec3(FLT_MAX);
		glm::vec3 worldSpaceMax = glm::vec3(-FLT_MAX);

		// Iterate over model nodes and primitives to add static rigid bodies
		// based on the model's triangle meshes.
		// Currently adds a static body per primitive mesh, not sure if thats
		// a bad or good thing.
		for (Engine::vk::Node* node : model.linearNodes) {
			if (node->mesh) {
				glm::mat4 nodeMatrix = transform * node->getModelMatrix();

				glm::vec3 translation;
				glm::quat rotation;
				glm::vec3 scale;

				// Decompose transform
				if (!DecomposeTransform(nodeMatrix, translation, rotation, scale)) {
					std::cout << "DecomposeTransform failed!" << std::endl;
					return;
				}

				PxTransform nodePxTransform(
					PxVec3(translation.x, translation.y, translation.z),
					PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
				);

				PxVec3 nodePxScale(scale.x, scale.y, scale.z);

				if (physicsType == PhysicsType::STATICBOUNDED)
				{
					glm::vec3 localMin = node->bbMin;
					glm::vec3 localMax = node->bbMax;

					// Generate all 8 corners of the AABB
					glm::vec3 corners[8] = {
						node->bbMin,
						glm::vec3(localMin.x, localMin.y, localMax.z),
						glm::vec3(localMin.x, localMax.y, localMin.z),
						glm::vec3(localMin.x, localMax.y, localMax.z),
						glm::vec3(localMax.x, localMin.y, localMin.z),
						glm::vec3(localMax.x, localMin.y, localMax.z),
						glm::vec3(localMax.x, localMax.y, localMin.z),
						node->bbMax
					};

					for (int i = 0; i < 8; ++i) {
						glm::vec3 worldCorner = glm::vec3(nodeMatrix * glm::vec4(corners[i], 1.0f));
						worldSpaceMin = glm::min(worldSpaceMin, worldCorner);
						worldSpaceMax = glm::max(worldSpaceMax, worldCorner);
					}
				}
				
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
					PxTriangleMeshGeometry triMeshGeometry(triMesh, PxMeshScale(nodePxScale), PxMeshGeometryFlag::eDOUBLE_SIDED);

					// Only static triangle meshes are supported for now.
					// Dynamic triangle mesh geometries are possible but are more complicated
					// and I don't believe we have a use case for them just yet.
					if (physicsType != PhysicsType::STATIC && physicsType != PhysicsType::STATICBOUNDED)
					{
						std::cerr << "Only static or static bounded triangle mesh geometries are supported currently!" << std::endl;
						assert(false); // or assert with a more specific condition if desired
					}

					staticBody = pWorld.gPhysics->createRigidStatic(PxTransform(nodePxTransform));
					if (staticBody) {
						PxShape* shape = PxRigidActorExt::createExclusiveShape(
							*staticBody, triMeshGeometry, *material 
						);

						staticBody->setName(name);
						pWorld.gScene->addActor(*staticBody);
					}
				}
			}
		}

		if (physicsType == PhysicsType::STATICBOUNDED)
			AddBoundingBox(pWorld, worldSpaceMin, worldSpaceMax, transform);
	}

	void PhysicsComponent::AddBoundingBox(PhysicsWorld& pWorld, glm::vec3 bbMin, glm::vec3 bbMax, glm::mat4 transform)
	{
		const float wallThickness = 0.1f;

		glm::vec3 size = bbMax - bbMin;
		glm::vec3 center = (bbMin + bbMax) / 2.0f;

		// Create a material
		PxMaterial* material = pWorld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

		auto addWall = [&](glm::vec3 pos, glm::vec3 halfExtents)
			{
				glm::vec3 worldPos = glm::vec3(transform * glm::vec4(pos, 1.0f));
				glm::mat3 rotationMatrix = glm::mat3(transform);
				glm::quat rotationQuat = glm::normalize(glm::quat_cast(rotationMatrix));
				PxTransform wallTransform(PxVec3(worldPos.x, worldPos.y, worldPos.z),
					PxQuat(rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w));

				PxBoxGeometry boxGeom(PxVec3(halfExtents.x, halfExtents.y, halfExtents.z));
				PxRigidStatic* wall = pWorld.gPhysics->createRigidStatic(wallTransform);
				if (!wall) {
					std::cerr << "Failed to create wall actor!" << std::endl;
					return;
				}

				PxShape* shape = PxRigidActorExt::createExclusiveShape(*wall, boxGeom, *material);
				if (!shape) {
					std::cerr << "Failed to create wall shape!" << std::endl;
					return;
				}

				wall->setName("levelBounds");
				pWorld.gScene->addActor(*wall);
			};

		// Floor
		addWall(glm::vec3(center.x, bbMin.y - wallThickness * 0.5f, center.z),
			glm::vec3(size.x * 0.5f, wallThickness * 0.5f, size.z * 0.5f));
		// Ceiling
		addWall(glm::vec3(center.x, bbMax.y + wallThickness * 0.5f, center.z),
			glm::vec3(size.x * 0.5f, wallThickness * 0.5f, size.z * 0.5f));
		// Back
		addWall(glm::vec3(center.x, center.y, bbMin.z - wallThickness * 0.5f),
			glm::vec3(size.x * 0.5f, size.y * 0.5f, wallThickness * 0.5f));
		// Front
		addWall(glm::vec3(center.x, center.y, bbMax.z + wallThickness * 0.5f),
			glm::vec3(size.x * 0.5f, size.y * 0.5f, wallThickness * 0.5f));
		// Left
		addWall(glm::vec3(bbMin.x - wallThickness * 0.5f, center.y, center.z),
			glm::vec3(wallThickness * 0.5f, size.y * 0.5f, size.z * 0.5f));
		// Right
		addWall(glm::vec3(bbMax.x + wallThickness * 0.5f, center.y, center.z),
			glm::vec3(wallThickness * 0.5f, size.y * 0.5f, size.z * 0.5f));
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
			// For some reason this conjuagte breaks the rotation for
			// some nodes in the warehouse map, commenting it out fixes it.
			//rotation = glm::conjugate(rotation);
		}
		return success;
	}
}