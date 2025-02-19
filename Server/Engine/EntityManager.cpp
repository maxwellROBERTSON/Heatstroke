#include "EntityManager.hpp"

// Public Methods

EntityManager::EntityManager()
{
	entities = std::vector<Entity*>();
	entityList = std::vector<std::pair<ComponentType, std::vector<int>>>();
	componentList = std::vector<Component*>();
}

//EntityManager* EntityManager::get()
//{
//	static EntityManager* instance = new EntityManager();
//	return instance;
//}

int EntityManager::GetNumberOfEntities()
{
	return entities.size();
}

void EntityManager::AddEntity(Entity* entity)
{
	entities.push_back(entity);
}

//void EntityManager::RemoveEntity(Entity entity)
//{
//	entities.erase(std::find(entities.begin(), entities.end(), entity));
//}

void EntityManager::Update(float deltaTime)
{
	for (Entity* entity : entities)
	{
		entity->Update(deltaTime);
	}
}

void EntityManager::AddComponent(ComponentType type)
{
	// Add new component to list
	componentList.push_back(MakeComponent(type).release());
	// Find the type in the type list and add the index to the list of indices
	for (int i = 0; i < entityList.size(); i++)
	{
		if (entityList[i].first == type)
		{
			entityList[i].second.push_back(componentList.size());
		}
	}
}

// Private Methods

std::unique_ptr<Component> EntityManager::MakeComponent(ComponentType type)
{
	switch (type)
	{
	case ComponentType::Render:
		return std::make_unique<RenderComponent>();
	}
}