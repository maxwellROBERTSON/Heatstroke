#pragma once

#include <typeindex>

#include "ComponentTypeRegistry.hpp"

template <typename T>
class Component
{
protected:
    Component() : componentId(ComponentTypeRegistry::Get().GetComponentID<T>()) {}

    virtual void operator=(const T& other) = 0;

    virtual std::type_index GetType() const { return ComponentTypeRegistry::Get().GetTypeIndex(componentId); }
    virtual int GetTypeId() const { return componentId; }

private:
    int componentId;
};
