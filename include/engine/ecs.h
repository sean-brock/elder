#ifndef ENGINE_ECS_H
#define ENGINE_ECS_H
#include <engine/generational_index.h>
#include <engine/type_map.h>
#include <any>
#include <stdexcept>
#include <utility>

namespace engine {

using Entity = GenerationalIndex;
template <typename T>
using EntityMap = GenerationalIndexArray<T>;

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

  template <class ComponentType>
  bool register_component() {
    if (_components.contains<ComponentType>())
      return false;
    _components.put<ComponentType>(EntityMap<ComponentType>());
    return true;
  }

  template <class ComponentType, typename... Args>
  bool add_component(Entity id, Args&&... args) {
    if (!_components.contains<ComponentType>()) {
      throw std::runtime_error(
          "Adding component but component type was not registered.");
    }
    auto& entity_map = std::any_cast<EntityMap<ComponentType>&>(
        _components.find<ComponentType>()->second);
    return entity_map.emplace(id, std::forward<Args>(args)...);
  }

 private:
  TypeMap<std::any> _components;
};

class ResourceRegistry {
 public:
  ResourceRegistry() = default;

  template <class ResourceType, typename... Args>
  bool register_resource(Args&&... args) {
    return _resources.emplace<ResourceType>(std::forward<Args>(args)...);
  }

 private:
  TypeMap<std::any> _resources;
};

struct Registry {
  ComponentRegistry components;
  ResourceRegistry resources;
};

};  // namespace engine
#endif
