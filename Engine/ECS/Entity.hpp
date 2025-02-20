#pragma once

#include <string>
#include <vector>

#include "EntityManager.hpp"
#include "Component.hpp"
#include "../gltf/glTF.hpp"

class EntityManager;

class Entity
{
public:
	Entity(EntityManager*, int, bool[], int[]);
	~Entity() {};

	int HasComponent(ComponentType);
	int GetNumberOfComponents();

	//void AddComponent(ComponentType);
	void Update(float);

private:
	void LoadEntity();

	EntityManager* entityManager;
	int entityId;
	// Holds a list of components with the type and
	// their id in that types list in EntityManager
	std::vector<std::pair<ComponentType, int>> componentListId;

};
