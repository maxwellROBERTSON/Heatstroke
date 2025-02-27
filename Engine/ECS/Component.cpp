#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

//// Define the template constructor outside the header
//template <typename T>
//Component<T>::Component()
//{
//    componentId = ComponentTypeRegistry::Get().GetComponentID<T>();
//}
//
//// Define the template functions outside the header
//template <typename T>
//std::type_index Component<T>::GetType() const
//{
//    return ComponentTypeRegistry::Get().GetTypeIndex(componentId);
//}
//
//template <typename T>
//int Component<T>::GetTypeId() const
//{
//    return componentId;
//}