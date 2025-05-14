#include "ChildrenComponent.hpp"

#include "../../Core/Log.hpp"

namespace Engine
{
	// Getters

	// Get component data
	void ChildrenComponent::GetDataArray(uint8_t* data)
	{
		uint32_t offset = 0;

		std::memcpy(data + offset, &numChildren, sizeof(numChildren));
		offset += sizeof(numChildren);
		std::memcpy(data + offset, childrenEntityIds.data(), sizeof(int) * childrenEntityIds.size());
	}

	// Get entityIds of children
	std::vector<int> ChildrenComponent::GetChildrenEntityIds()
	{
		std::vector<int> children(numChildren);
		for (int i = 0; i < numChildren; i++)
		{
			children[i] = childrenEntityIds[i];
		}

		return children;
	}

	// Set component data
	void ChildrenComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		if (std::memcmp(&numChildren, data + offset, sizeof(numChildren)) != 0)
		{
			std::memcpy(&numChildren, data + offset, sizeof(numChildren));
			SetComponentHasChanged();
		}
		offset += sizeof(numChildren);
		if (std::memcmp(childrenEntityIds.data(), data + offset, sizeof(int) * childrenEntityIds.size()) != 0)
		{
			std::memcpy(childrenEntityIds.data(), data + offset, sizeof(int) * childrenEntityIds.size());
			SetComponentHasChanged();
		}
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
		if (numChildren == childrenEntityIds.size())
		{
			DLOG("Max children reached.");
			return;
		}
		childrenEntityIds.at(numChildren) = entityId;
		numChildren++;
		SetComponentHasChanged();
	}
}
