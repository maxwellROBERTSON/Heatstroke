#pragma once

// When you add more component types:
// 1. Add name enum class ComponentType
// 2. Define the GetType() method in the derived class to return this enum
// 3. Add an include for the new component in EntityManager.hpp
// 4. Add a new case to the switch statement in EntityManager::MakeComponent()

enum class ComponentType
{
    Render,
    Audio,
    Physics,
    Transform
	// Add more component types here
};

class Component
{
public:
    virtual ~Component() = default;  // Ensure proper polymorphic behavior
    virtual ComponentType GetType() const = 0;  // Force derived classes to specify type
};
