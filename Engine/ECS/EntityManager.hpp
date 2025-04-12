#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Entity.hpp"
#include "Components/Component.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/NetworkComponent.hpp"
#include "Components/PhysicsComponent.hpp"
#include "Components/RenderComponent.hpp"

namespace Engine
{
	class EntityManager
	{
	public:
		EntityManager()
		{
			if (componentMap.size() != ComponentTypes::TYPE_COUNT)
			{
				throw std::runtime_error("Size mismatch between componentMap and ComponentTypes");
			}
		};
		~EntityManager() {}

		// Getters

		// Get total data size
		size_t GetTotalDataSize();

		// Get total number of entities
		int GetNumberOfEntities() { return static_cast<int>(entities.size()); }

		// Get a pointer to an entity, given an id
		Entity* GetEntity(int entityId) { return &entities[entityId]; }

		// Get the entities with a component type
		std::vector<int> GetEntitiesWithComponent(ComponentTypes type) { return entitiesWithType[type]; }
		
		// Get total number of components of a type
		int GetComponentTypeSize(ComponentTypes type) { return (*componentMap[type]).size(); }

		// Get a pointer to the component of an entity
		ComponentBase* GetComponentOfEntity(int, ComponentTypes);

		// Get a pointer to all the components of a type
		std::vector<std::unique_ptr<ComponentBase>>* GetComponentsOfType(ComponentTypes type) { return componentMap[type]; }

		// Get component data of all entities for one component type
		std::vector<uint8_t> GetComponentData(ComponentTypes);

		// Get component data of all entities for all component type
		void GetAllData(uint8_t*);

		// Setters

		// Set component data of all entities for all component type
		void SetAllData(uint8_t*);

		// Add an entity with given types
		Entity* AddEntity(std::vector<ComponentTypes>);

		// Clears the manager on disconnect
		void ClearManager();

	private:
		// Private used in AddEntity - doesn't add to entity's
		// vector so mustn't be used except when making a new entity
		int AddComponent(ComponentTypes);

		// Vector of entities
		std::vector<Entity> entities;

		// Vector for each component type
		std::vector<std::unique_ptr<ComponentBase>> cameraComponents;
		std::vector<std::unique_ptr<ComponentBase>> networkComponents;
		std::vector<std::unique_ptr<ComponentBase>> physicsComponents;
		std::vector<std::unique_ptr<ComponentBase>> renderComponents;
		std::vector<std::unique_ptr<ComponentBase>> audioComponents;


		// Map of component type vectors
		std::map<ComponentTypes, std::vector<std::unique_ptr<ComponentBase>>*> componentMap
		{
			{ ComponentTypes::CAMERA, &cameraComponents },
			{ ComponentTypes::NETWORK, &networkComponents },
			{ ComponentTypes::PHYSICS, &physicsComponents },
			{ ComponentTypes::RENDER, &renderComponents },
			{ ComponentTypes::AUDIO, &renderComponents }
		};

		// For each component type, there is an entry, each holding ids of entities with that component type
		std::vector<std::vector<int>> entitiesWithType = std::vector<std::vector<int>>(TYPE_COUNT);

		// For each entity:
		// Component vector of int = registry.GetNumComponents * sizeof(int)
		// Matrix = 16 * sizeof(float)
		// If rendercomp:
		//  Model index = sizeof(int)
		// If camera:
		// fov + near + far + vec3(pos) + vec3(front_dir) = 9 * float
		// If network:
		// clientid = int

	};
}

