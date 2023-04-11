#ifndef ENGINE_ECS_H
#define ENGINE_ECS_H
#include <engine/generational_index.h>
#include <engine/type_map.h>
#include <algorithm>
#include <any>
#include <functional>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace engine {

using Entity = GenerationalIndex;
template <typename T>
using EntityMap = GenerationalIndexArray<T>;
using AnyMap = TypeMap<std::any>;

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
  inline void assert_registered() {
    if (!_components.contains<ComponentType>()) {
      throw std::runtime_error(
          "Adding component but component type was not registered.");
    }
  }

  template <class ComponentType>
  bool register_component() {
    if (_components.contains<ComponentType>())
      return false;
    _components.put<ComponentType>(EntityMap<ComponentType>());
    return true;
  }

  template <class ComponentType, typename... Args>
  bool add_component(Entity id, Args&&... args) {
    assert_registered<ComponentType>();
    auto& entity_map = std::any_cast<EntityMap<ComponentType>&>(
        _components.find<ComponentType>()->second);
    return entity_map.emplace(id, std::forward<Args>(args)...);
  }

  template <class ComponentType>
  std::vector<Entity> has_component() {
    assert_registered<ComponentType>();
    EntityMap<ComponentType> entity_map =
        std::any_cast<EntityMap<ComponentType>&>(
            _components.find<ComponentType>()->second);
    return entity_map.indices();
  }

  template <class FirstComponentType, class SecondComponentType,
            class... RestComponentType>
  std::vector<Entity> has_component() {
    constexpr std::size_t num_component_types =
        2 + sizeof...(RestComponentType);
    std::array<std::vector<GenerationalIndex>, num_component_types>
        entity_lists;
    for (int i = 0; i < 2; i++) {
      entity_lists[i] = has_component<FirstComponentType>();
      if (entity_lists[i].empty())
        return {};
    }
    if constexpr (num_component_types > 2) {
      // detect if empty entity list for at least one component
      bool empty_entity_list = false;
      int i = 2;
      (
          [&] {
            if (!empty_entity_list) {
              entity_lists[i] = has_component<RestComponentType>();
              if (entity_lists[i].empty())
                empty_entity_list = true;
            }
            i++;
          }(),
          ...);
      if (empty_entity_list)
        return {};
    }

    // Init result vector. Max size is where every entity is in every component list.
    const std::size_t max_entities = [&] {
      std::size_t sum = 0;
      for (auto& e : entity_lists)
        sum += e.size();
      return sum;
    }();

    std::vector<Entity> unique_entities{max_entities};
    // intersect on first 2
    std::vector<Entity>::iterator it = std::set_intersection(
        entity_lists[0].begin(), entity_lists[0].end(), entity_lists[1].begin(),
        entity_lists[1].end(), unique_entities.begin());
    // Compute intersect on rest of component types
    if constexpr (num_component_types > 2) {
      for (std::size_t i = 2; i < num_component_types; i++) {
        it = std::set_intersection(
            unique_entities.begin(), unique_entities.end(),
            entity_lists[i].begin(), entity_lists[i].end(),
            unique_entities.begin());
      }
    }
    unique_entities.resize(it - unique_entities.begin());

    return unique_entities;
  }

 private:
  AnyMap _components;
};

class ResourceRegistry {
 public:
  ResourceRegistry() = default;

  template <class ResourceType, typename... Args>
  bool register_resource(Args&&... args) {
    return _resources.emplace<ResourceType>(std::forward<Args>(args)...);
  }

 private:
  AnyMap _resources;
};

struct Registry {
  ComponentRegistry components;
  ResourceRegistry resources;
};
};  // namespace engine
#endif
