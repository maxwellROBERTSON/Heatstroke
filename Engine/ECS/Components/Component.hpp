#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <cstdint>

namespace Engine
{
    enum ComponentTypes
	{
		CAMERA,
		NETWORK,
		PHYSICS,
		RENDER,
        TYPE_COUNT
	};

    extern std::map<ComponentTypes, uint8_t> ComponentSizes;

    class ComponentBase
    {
    public:
        // Virtual methods for children
        virtual ~ComponentBase() = default;
        virtual ComponentTypes GetType() const = 0;
        virtual size_t GetSize() const = 0;
        virtual void GetDataArray(uint8_t*) = 0;
    };

    template <typename T>
    class Component : public ComponentBase
    {
    protected:
        Component() {}

        // Virtual override of the assignment operator
        virtual void operator=(const T& other) = 0;

        // Getters

        // Get type size of a type
        ComponentTypes GetType() const { return T::StaticType(); }

        // Type getter in child classes
        ComponentTypes StaticType() { return ComponentTypes::TYPE_COUNT; }

        // Get size of a type
        size_t GetSize() const { return T::StaticSize(); }

        // Size getter in child classes
        virtual size_t StaticSize() { return 0; }

        // Get component data
        virtual void GetDataArray(uint8_t*) = 0;

        // Setters

        // Set component data
        virtual void SetDataArray(uint8_t* data) = 0;

        // Set component has changed in entity manager
        virtual void SetComponentHasChanged() = 0;

    private:
        int componentId;
    };
}

