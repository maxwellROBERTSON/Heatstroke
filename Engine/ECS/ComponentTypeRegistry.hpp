#pragma once

#include <unordered_map>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <iostream>

class ComponentTypeRegistry
{
public:
    // Getters
    static ComponentTypeRegistry& Get()
    {
        static ComponentTypeRegistry instance;
        return instance;
    }

    template <typename T>
    int GetComponentID()
    {
        std::type_index typeIndex(typeid(T));
        auto it = typeToId.find(typeIndex);
        if (it != typeToId.end())
        {
            return it->second;
        }
        else
        {
            RegisterComponentType<T>();
            return nextId - 1;
        }
    }

    std::type_index GetTypeIndex(int id)
    {
        auto it = idToType.find(id);
        if (it != idToType.end())
        {
            return it->second;
        }
        return std::type_index(typeid(void));  // Invalid type
    }

    int GetNumberOfComponentTypes() const
    {
        return static_cast<int>(typeToId.size());
    }

   /* std::vector<std::type_index> GetComponentTypes()
    {

    }*/

	// Setters
	template <typename ... Types>
	void RegisterComponentTypes()
	{
		(RegisterComponentType<Types>(), ...);
	}

    template <typename T>
    void RegisterComponentType()
    {
        std::type_index typeIndex(typeid(T));
        int newId = nextId++;
		typeToId.insert({ typeIndex, newId });
		idToType.insert({ newId, typeIndex });
    }

private:
    std::unordered_map<std::type_index, int> typeToId;  // Maps type to ID
    std::unordered_map<int, std::type_index> idToType;  // Maps ID to type
    int nextId = 0;
};