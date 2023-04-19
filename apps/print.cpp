#include <engine/ecs.h>

#include <fmt/core.h>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

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

constexpr int num_entities = 60000;

int main() {
  Registry registry;
  registry.components.register_component<PositionComponent>();
  registry.components.register_component<VelocityComponent>();
  registry.components.register_component<NameComponent>();
  ECS ecs;
  for (int i = 0; i < num_entities; i++) {
    Entity e = ecs.create();
    if (!registry.components.add_component<NameComponent>(
            e, "Entity " + std::to_string(i))) {
      throw std::runtime_error(
          fmt::format("Failed to add name component to {}", i));
    }
    if (i % 2 == 0) {
      if (!registry.components.add_component<PositionComponent>(e, i, i))
        throw std::runtime_error(
            fmt::format("Failed to add position component to {}", i));
    }
    if (i % 3 == 0) {
      if (!registry.components.add_component<VelocityComponent>(e, i + 2,
                                                                i + 1))
        throw std::runtime_error(
            fmt::format("Failed to add velocity component to {}", i));
    }
  }
  // Accessors for the component data
  auto get_position =
      registry.components.component_accessor<PositionComponent>();
  auto get_name = registry.components.component_accessor<NameComponent>();
  auto get_velocity =
      registry.components.component_accessor<VelocityComponent>();
  // Entity groups/views by shared components
  auto positioned_entities =
      registry.components.has_component<NameComponent, PositionComponent>();
  auto moving_entities =
      registry.components
          .has_component<NameComponent, PositionComponent, VelocityComponent>();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  fmt::print("# Entities:\n");
  int sum_x = 0;
  int sum_y = 0;
  for (const auto& id : positioned_entities) {
    PositionComponent& p = get_position(id);
    sum_x += p.x / 2;
    sum_y += p.y / 2;
  }
  fmt::print("Position sum: ({}, {})\n", sum_x, sum_y);

  sum_x = 0;
  sum_y = 0;
  for (const auto& id : moving_entities) {
    VelocityComponent& v = get_velocity(id);
    sum_x += v.x;
    sum_y += v.y;
  }
  fmt::print("Velocity sum: ({}, {})\n", sum_x, sum_y);

  return 0;
}
