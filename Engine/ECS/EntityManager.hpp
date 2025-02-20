#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Entity.hpp"
#include "Component.hpp"
#include "RenderComponent.hpp"

class Entity;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	//static EntityManager* get();
	int GetNumberOfEntities();
	Entity* AddEntity(ComponentType type[]);
	//void RemoveEntity(Entity);
	void Update(float);
	//void AddComponent(ComponentType);

private:
	/*std::unique_ptr<Component> MakeComponent(ComponentType);
	Component* GetComponent(ComponentType);*/

	std::vector<Entity> entities;

	// List of component types, each item in the list holds a pair of type and
	// a list of indices used for the component list,
	// each entity has an index in the list (for each type)
	std::vector<std::pair<ComponentType, std::vector<std::unique_ptr<Component>>>> componentList;

	//// List of all components for all entities
	//std::vector<Component*> componentList;
};

