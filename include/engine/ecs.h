#ifndef ENGINE_ECS_H
#define ENGINE_ECS_H
#include <any>
#include <engine/components.h>
#include <engine/generational_index.h>
#include <engine/type_map.h>
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

/*
struct Resource {
  virtual ~Resource() = 0;
};
*/

class ComponentRegistry {
public:
  ComponentRegistry() = default;

  template <class ComponentType> bool register_component() {
    if (_components.contains<ComponentType>())
      return false;
    _components.put<ComponentType>(EntityMap<ComponentType>());
    return true;
  }

  template <class ComponentType> bool add_component(Entity id) {
    EntityMap<ComponentType> entity_map =
        std::any_cast<EntityMap<ComponentType>>(
            _components.find<ComponentType>()->second);
    if (entity_map.contains(id))
      return false;
    entity_map.set(id, ComponentType());
    return true;
  }

private:
  TypeMap<std::any> _components;
};

/*
class ResourceRegistry {
public:
  ResourceRegistry() = default;
  template <class ResourceType> bool register_resource();

private:
  TypeMap<std::unique_ptr<EntityMap<Resource>>> _resources;
};
*/
inline ComponentRegistry loadComponentRegistry() {
  ComponentRegistry components;
  components.register_component<UniqueIDComponent>();

  return components;
}
// ResourceRegistry loadResourceRegistry();

struct Registry {
public:
  Registry();
  ComponentRegistry components;
  //  ResourceRegistry resources;
};

}; // namespace engine
#endif
