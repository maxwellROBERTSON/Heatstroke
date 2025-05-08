#include "ChildrenComponent.hpp"

namespace Engine
{
	// Getters

	// Get component data
	void ChildrenComponent::GetDataArray(uint8_t* data)
	{
		uint32_t offset = 0;

		std::memcpy(data + offset, &childrenEntityIds, sizeof(childrenEntityIds));
		offset += sizeof(childrenEntityIds);
	}

	// Get entityIds of children
	std::vector<int> ChildrenComponent::GetChildrenEntityIds()
	{
		std::vector<int> children;
		for (int i = 0; i < numChildren; i++)
		{
			children.emplace_back(childrenEntityIds.at(i));
		}

		return children;
	}

	// Set component data
	void ChildrenComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		if (std::memcmp(&childrenEntityIds, data + offset, sizeof(childrenEntityIds)) != 0)
		{
			std::memcpy(&childrenEntityIds, data + offset, sizeof(childrenEntityIds));
			SetComponentHasChanged();
		}
		offset += sizeof(childrenEntityIds);
	}

	// Set component has changed in entity manager
	void ChildrenComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}

	// Add a child of this entity
	void ChildrenComponent::AddChild(int entityId)
	{
		childrenEntityIds.at(numChildren) = entityId;
		numChildren++;
	}
}
