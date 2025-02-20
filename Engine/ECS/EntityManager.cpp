#include "EntityManager.hpp"

// Public Methods

EntityManager::EntityManager()
{
	entities = std::vector<Entity>();
	for (int i = 0; i < static_cast<int>(ComponentType::Transform) + 1; i++)
	{
		componentList.push_back(std::make_pair(static_cast<ComponentType>(i), std::vector<std::unique_ptr<Component>>()));
		/*switch (static_cast<ComponentType>(i))
		{
			case ComponentType::Render:
			{
				componentList.push_back(std::make_pair(static_cast<ComponentType>(i), std::vector<std::unique_ptr<Component>>()));
			}
		}*/
	}
}

EntityManager::~EntityManager() {};

int EntityManager::GetNumberOfEntities()
{
	return static_cast<int>(entities.size());
}

Entity* EntityManager::AddEntity(ComponentType type[])
{
	bool componentBitmask[sizeof(ComponentType)];
	int componentIndex[sizeof(ComponentType)];
	for (int i = 0; i < sizeof(ComponentType); i++)
	{
		componentBitmask[i] = false;
		componentIndex[i] = -1;
	}
	for (int i = 0; i < sizeof(type); i++)
	{
		switch (type[i])
		{
			case ComponentType::Render:
			{
				int index = static_cast<int>(ComponentType::Render);
				componentBitmask[index] = true;
				componentList[index].second.push_back(std::make_unique<RenderComponent>());
				componentIndex[index] = componentList[index].second.size() - 1;
			}
		}
	}
	Entity entity = Entity(this, static_cast<int>(entities.size()), componentBitmask, componentIndex);
	entities.push_back(entity);
	return &entities[entities.size() - 1];
}

//void EntityManager::RemoveEntity(Entity entity)
//{
//	entities.erase(std::find(entities.begin(), entities.end(), entity));
//}

void EntityManager::Update(float deltaTime)
{
	for (Entity entity : entities)
	{
		entity.Update(deltaTime);
	}
}

//void EntityManager::AddComponent(ComponentType type)
//{
//	// Add new component to list
//	componentList.push_back(MakeComponent(type).release());
//	// Find the type in the type list and add the index to the list of indices
//	for (int i = 0; i < entityList.size(); i++)
//	{
//		if (entityList[i].first == type)
//		{
//			entityList[i].second.push_back(static_cast<int>(componentList.size()));
//		}
//	}
//}

// Private Methods

//std::unique_ptr<Component> EntityManager::MakeComponent(ComponentType type)
//{
//	switch (type)
//	{
//	case ComponentType::Render:
//		return std::make_unique<RenderComponent>();
//	}
//	return nullptr;
//}