#include <engine/ecs.h>

#include <fmt/format.h>

#include <iostream>
#include <tuple>
#include <vector>

using namespace engine;

struct PositionComponent {
  int x;
  int y;
};

struct VelocityComponent {
  int x;
  int y;
};

struct NameComponent {
  std::string name{"default"};
};

int main() {
  Registry registry;
  registry.components.register_component<PositionComponent>();
  registry.components.register_component<VelocityComponent>();
  registry.components.register_component<NameComponent>();
  ECS ecs;
  Entity e0 = ecs.create();
  Entity e1 = ecs.create();
  Entity e2 = ecs.create();

  registry.components.add_component<PositionComponent>(e0);
  registry.components.add_component<NameComponent>(e0, "Entity 0");
  registry.components.add_component<PositionComponent>(e1, 10, 10);
  registry.components.add_component<NameComponent>(e1, "Entity 1");
  registry.components.add_component<PositionComponent>(e2, 12, 22);
  registry.components.add_component<VelocityComponent>(e2, 1, 0);
  registry.components.add_component<NameComponent>(e2, "Entity 2");

  auto positioned_entities =
      registry.components.has_component<NameComponent, PositionComponent>();
  auto moving_entities =
      registry.components
          .has_component<NameComponent, PositionComponent, VelocityComponent>();

  auto get_position =
      registry.components.component_accessor<PositionComponent>();
  auto get_name = registry.components.component_accessor<NameComponent>();
  auto get_velocity =
      registry.components.component_accessor<VelocityComponent>();

  fmt::print("# Entities:\n");
  for (const auto& id : positioned_entities) {
    PositionComponent& p = get_position(id);
    fmt::print("{}: Position: ({}, {})\n", get_name(id).name, p.x, p.y);
  }

  fmt::print("# Entities in Motion:\n");
  for (const auto& id : moving_entities) {
    VelocityComponent& v = get_velocity(id);
    fmt::print("{}: Velocity: ({}, {})\n", get_name(id).name, v.x, v.y);
  }
  return 0;
}
