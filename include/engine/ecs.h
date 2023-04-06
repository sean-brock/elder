#ifndef ENGINE_ECS_H
#define ENGINE_ECS_H
#include <any>
#include <engine/components.h>
#include <engine/generational_index.h>
#include <engine/type_map.h>
#include <stdexcept>
#include <utility>
namespace engine {

using Entity = GenerationalIndex;
template <typename T> using EntityMap = GenerationalIndexArray<T>;

class ECS {
public:
  inline Entity create() { return entity_allocator.allocate(); }
  inline bool destroy(Entity entity) {
    return entity_allocator.deallocate(entity);
  }

private:
  GenerationalIndexAllocator entity_allocator;
};

class ComponentRegistry {
public:
  ComponentRegistry() = default;

  template <class ComponentType> bool register_component() {
    if (_components.contains<ComponentType>())
      return false;
    _components.put<ComponentType>(EntityMap<ComponentType>());
    return true;
  }

  template <class ComponentType, typename... Args>
  bool add_component(Entity id, Args &&...args) {
    if (!_components.contains<ComponentType>()) {
      throw std::runtime_error(
          "Adding component but component type was not registered.");
    }
    auto &entity_map = std::any_cast<EntityMap<ComponentType> &>(
        _components.find<ComponentType>()->second);
    return entity_map.emplace(id, std::forward<Args>(args)...);
  }

private:
  TypeMap<std::any> _components;
};
/*
class ResourceRegistry {
public:
  ResourceRegistry() = default;

  template <class ResourceType> bool register_resource() {
    if (_resources.contains<ResourceType>())
      return false;
    _resources.put<ResourceType>();
    return true;
  }

  template <class ResourceType>
  bool add_resource(std::unique_ptr<ResourceType> resource) {
    if (_resources.contains<ResourceType>()) {
      return false;
    }
    _resources.put<ResourceType>(std::move(resource));
    return true;
  }

private:
  TypeMap<std::any> _resources;
};
*/
inline ComponentRegistry loadComponentRegistry() {
  ComponentRegistry components;
  components.register_component<UniqueIDComponent>();

  return components;
}
/*
inline ResourceRegistry loadResourceRegistry() {
  ResourceRegistry resources;
  resources.register_resource<TileResource>();
  return resources;
}
*/

struct Registry {
public:
  Registry();
  ComponentRegistry components;
  //  ResourceRegistry resources;
};

}; // namespace engine
#endif
