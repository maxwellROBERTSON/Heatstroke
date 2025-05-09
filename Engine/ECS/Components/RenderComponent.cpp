#include "RenderComponent.hpp"
#include "PhysicsComponent.hpp"

#include <cstring>

namespace Engine
{
	// Getters

	// Get component data
	void RenderComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &modelIndex, sizeof(modelIndex));
		offset += sizeof(modelIndex);
		std::memcpy(data + offset, &isActive, sizeof(isActive));
	}

	// Setters

	// Set component data
	void RenderComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		if (std::memcmp(&modelIndex, data + offset, sizeof(modelIndex)) != 0)
		{
			std::memcpy(&modelIndex, data + offset, sizeof(modelIndex));
			SetComponentHasChanged();
		}
		offset += sizeof(modelIndex);

		if (std::memcmp(&isActive, data + offset, sizeof(isActive)) != 0)
		{
			std::memcpy(&isActive, data + offset, sizeof(isActive));
			SetComponentHasChanged();

			PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(entityManager->GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
			PhysicsComponent::PhysicsType type = physicsComponent->GetPhysicsType();
			
			switch (type)
			{
			case PhysicsComponent::PhysicsType::STATIC:
				if (physicsComponent->GetStaticBody() != NULL)
					entityManager->AddChangedActor(physicsComponent->GetStaticBody(), !isActive);
				break;
			case PhysicsComponent::PhysicsType::DYNAMIC:
				if (physicsComponent->GetDynamicBody() != NULL)
					entityManager->AddChangedActor(physicsComponent->GetDynamicBody(), !isActive);
				break;
			case PhysicsComponent::PhysicsType::CONTROLLER:
				if (physicsComponent->GetController() != NULL)
					entityManager->AddChangedActor(physicsComponent->GetController()->getActor(), !isActive);
				break;
			}
		}
		offset += sizeof(isActive);
	}

	// Set component has changed in entity manager
	void RenderComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}
}