#pragma once

#include <vector>
#include <string>
#include <memory>
#include <PxActor.h>
#include <cmath>

#include "Entity.hpp"
#include "Components/PhysicsComponent.hpp"

namespace Engine
{
	using namespace physx;

	class PhysicsComponent;

	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager() {}

		// Getters
		static EntityManager& GetEntityManager() 
		{ 
			static EntityManager instance;
			return instance; 
		}

		// Get total data size
		int GetTotalDataSize();

		// Get total size of all the changed data
		int GetTotalChangedDataSize();

		// Get total number of entities
		int GetNumberOfEntities() { return static_cast<int>(entities.size()); }

		// Get a pointer to an entity, given an id
		Entity* GetEntity(int entityId) { return entities[entityId].get(); }

		// Get the entities with a component type
		std::vector<int> GetEntitiesWithComponent(ComponentTypes type) { return entitiesWithType[type]; }
		
		// Get total number of components of a type
		int GetComponentTypeSize(ComponentTypes type) { return (int)(*componentMap[type]).size(); }

		// Get a pointer to the component of an entity
		ComponentBase* GetComponentOfEntity(int, ComponentTypes);

		// Get a pointer to all the components of a type
		std::vector<std::unique_ptr<ComponentBase>>* GetComponentsOfType(ComponentTypes type);

		// Get component data of all entities for one component type
		std::vector<uint8_t> GetComponentData(ComponentTypes);

		// Get if any changes have been made to the global scene
		bool HasSceneChanged() { if (changedEntitiesAndComponents.size() != 0) { return true; } else { return false; } }

		// Get component data of all entities for all component type
		void GetAllData(uint8_t*);

		// Get number of teams
		int GetNumTeams() { return static_cast<int>(numTeams); }

		// Get reset timer float
		float GetResetTimer() { return resetTimer; }

		// Get reset timer rounded up to an int
		int GetResetTimerInt() { return static_cast<int>(std::ceil(resetTimer)); }

		// Get all changed entity and component data
		void GetAllChangedData(uint8_t*);

		// Get physics components of entites with an updated renderComponent.isActive()
		std::vector<std::pair<PhysicsComponent*, bool>>* GetUpdatedPhysicsComps() { return &updatedPhysicsComponents; }

		// Setters

		// Set component data of all entities for all component type
		void SetAllData(uint8_t*);

		// Set all changed entity and component data
		// If client entity is changed, don't update on client
		void SetAllChangedData(uint8_t*, int);

		// Reset changed entities and components
		void ResetChanged();

		// Add changed entity
		void AddChangedEntity(Entity* entity);

		// Add changed component
		void AddChangedComponent(ComponentTypes type, Entity* entity);

		// Set next network component unassigned to a client
		void AssignNextClient(uint64_t, bool);

		// Set the number of teams (max = 4)
		void SetNumTeams(int t);

		// Set reset timer
		void SetResetTimer(float t);

		// Decrease reset timer by float
		void DecreaseResetTimer(float t);

		// Add an existing entity to the manager
		// ComponentIndexArray overwritten so only use if components not yet initialised
		void AddEntity(Entity*, std::vector<ComponentTypes>);

		// Add an entity with given types
		Entity* MakeNewEntity(std::vector<ComponentTypes>);

		// Add a network component to the queue
		void AddToNetworkComponentQueue(int index) { availableNetworkComponentQueue.push_back(index); }

		// Clears the manager on disconnect
		void ClearManager();

		// Add a physics component of entity with an updated renderComponent.isActive()
		void AddUpdatedPhysicsComp(PhysicsComponent* c, bool b) { updatedPhysicsComponents.emplace_back(std::make_pair(c, b)); }

		// Clear all updated physics components
		void ClearUpdatedPhysicsComps() { updatedPhysicsComponents.clear(); }

	private:
		// Data to be used when updating client server information
		// Vector of changed entities and components
		// first = entity id,
		// second = vector(size TYPE_COUNT + 1) to hold bits for if entity or component data has changed
		std::vector<std::pair<int, std::vector<int>>> changedEntitiesAndComponents = std::vector<std::pair<int, std::vector<int>>>(0);

		// Number of teams for multiplayer mode
		uint8_t numTeams = 1;

		// Reset timer for multiplayer mode
		float resetTimer = 0.f;

		// Private used in AddEntity - doesn't add to entity's
		// vector so mustn't be used except when making a new entity
		int AddComponent(ComponentTypes, Entity*);

		// Vector of entities
		std::vector<std::unique_ptr<Entity>> entities;

		// Vector for each component type
		std::vector<std::unique_ptr<ComponentBase>> audioComponents;
		std::vector<std::unique_ptr<ComponentBase>> cameraComponents;
		std::vector<std::unique_ptr<ComponentBase>> childrenComponents;
		std::vector<std::unique_ptr<ComponentBase>> networkComponents;
		std::vector<std::unique_ptr<ComponentBase>> physicsComponents;
		std::vector<std::unique_ptr<ComponentBase>> renderComponents;

		// Map of component type vectors
		std::map<ComponentTypes, std::vector<std::unique_ptr<ComponentBase>>*> componentMap
		{
			{ ComponentTypes::AUDIO, &audioComponents },
			{ ComponentTypes::CAMERA, &cameraComponents },
			{ ComponentTypes::CHILDREN, &childrenComponents },
			{ ComponentTypes::NETWORK, &networkComponents },
			{ ComponentTypes::PHYSICS, &physicsComponents },
			{ ComponentTypes::RENDER, &renderComponents }
		};

		// For each component type, there is an entry, each holding ids of entities with that component type
		std::vector<std::vector<int>> entitiesWithType = std::vector<std::vector<int>>(TYPE_COUNT);

		// Queue to store the entities with network components unassigned to clients
		std::deque<int> availableNetworkComponentQueue;

		// PhysicsComponents of entites with a recently updated renderComponent.isActive()
		std::vector<std::pair<PhysicsComponent*, bool>> updatedPhysicsComponents;

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

