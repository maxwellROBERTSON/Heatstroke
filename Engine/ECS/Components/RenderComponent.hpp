#pragma once

#include <string>

#include "Component.hpp"

#include "../EntityManager.hpp"

namespace Engine
{
	class EntityManager;
	class Entity;
}

namespace Engine
{
	class RenderComponent : public Component<RenderComponent>
	{
	public:
		RenderComponent() : entityManager(nullptr), entity(nullptr) {}
		RenderComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity) {}

		virtual void operator=(const RenderComponent& other) override
		{
			this->modelIndex = other.modelIndex;
			this->isActive = other.isActive;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::RENDER; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(modelIndex) + sizeof(isActive); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get model index
		int GetModelIndex() { return modelIndex; }

		// Get is the entity an active render component
		bool GetIsActive() { return isActive; }

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set model index
		void SetModelIndex(int index) { modelIndex = index; SetComponentHasChanged(); }

		// Set is the entity an active render component
		void SetIsActive(int status) { if (status == 0 || status == 1) isActive = status; SetComponentHasChanged(); }

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

	private:
		// EntityManager pointer
		Engine::EntityManager* entityManager;
		// Entity pointer
		Engine::Entity* entity;

		// If component has changed since last network update
		bool hasChanged = false;

		int modelIndex = -1;
		int isActive = 1;
	};
}

//#pragma once
//
//#include <string>
//
//#include "Component.hpp"
//
//#include "../Core/Game.hpp"
//
//namespace Engine
//{
//	class Game;
//}
//
//namespace Engine
//{
//	class RenderComponent : public Component<RenderComponent>
//	{
//	public:
//		RenderComponent() : game(nullptr), entity(nullptr) {}
//		RenderComponent(Engine::Game* game, Engine::Entity* entity) : game(game), entity(entity) {}
//
//		virtual void operator=(const RenderComponent& other) override
//		{
//			this->modelIndex = other.modelIndex;
//			this->isActive = other.isActive;
//		}
//
//		// Getters
//
//		// Static type getter from Component parent
//		ComponentTypes static StaticType() { return ComponentTypes::RENDER; }
//
//		// Static size getter from Component parent
//		size_t static StaticSize() { return sizeof(modelIndex) + sizeof(isActive); }
//
//		// Get component data
//		void GetDataArray(uint8_t*) override;
//
//		// Get model index
//		int GetModelIndex() { return modelIndex; }
//
//		// Setters
//
//		// Set component data
//		void SetDataArray(uint8_t*) override;
//
//		// Set model index
//		void SetModelIndex(int index)
//		{
//			modelIndex = index;
//			if (!hasChanged)
//			{
//				ComponentTypes t = StaticType();
//				game->GetEntityManager().AddChangedComponent(t, entity->GetComponent(t));
//				hasChanged = true;
//			}
//		}
//
//	private:
//		// Game pointer
//		Engine::Game* game;
//		// Entity pointer
//		Engine::Entity* entity;
//
//		// If component has changed since last network update
//		bool hasChanged = false;
//
//		int modelIndex = -1;
//		int isActive = 1;
//	};
//}
