#pragma once

#include <typeindex>

#include "ComponentTypeRegistry.hpp"

template <typename T>
class Component
{
protected:
    Component() : componentId(ComponentTypeRegistry::Get().GetComponentID<T>()) {}

    virtual std::type_index GetType() const { return ComponentTypeRegistry::Get().GetTypeIndex(componentId); }
    virtual int GetTypeId() const { return componentId; }

private:
    int componentId;
};
