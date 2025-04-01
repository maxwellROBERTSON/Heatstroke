#pragma once

#include <string>

#include "Component.hpp"

namespace Engine
{
	class RenderComponent : public Component<RenderComponent>
	{
	public:
		RenderComponent() {};

		virtual void operator=(const RenderComponent& other) override
		{
			this->modelIndex = other.modelIndex;
			this->isActive = other.isActive;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::RENDER; }

		// Get component data
		void GetDataArray(std::vector<uint8_t>&) override;

		// Get model index
		int GetModelIndex() { return modelIndex; }

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set model index
		void SetModelIndex(int index) { modelIndex = index; }

	private:
		int modelIndex = -1;
		int isActive = true;
	};
}
