#include <engine/ecs.h>
#include <catch2/catch_test_macros.hpp>
#include <memory>

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

struct TextResource {
  std::vector<std::string> text;
};

struct CounterResource {
  int counter;
};

TEST_CASE("Entity Create", "[ECS]") {
  Registry registry;
  registry.components.register_component<PositionComponent>();
  registry.components.register_component<VelocityComponent>();
  ECS ecs;
  Entity e0 = ecs.create();
  Entity e1 = ecs.create();
  Entity e2 = ecs.create();

  REQUIRE(registry.components.add_component<PositionComponent>(e0));
  REQUIRE(registry.components.add_component<PositionComponent>(e1, 10, 3));
  REQUIRE_FALSE(registry.components.add_component<PositionComponent>(e1));
  REQUIRE(registry.components.add_component<VelocityComponent>(e1, 2, 3));
  REQUIRE(registry.components.add_component<VelocityComponent>(e2, 2, 5));
}

TEST_CASE("Resource Create", "[ECS]") {
  Registry registry;
  REQUIRE(registry.resources.register_resource<CounterResource>(15));
  /*
  REQUIRE(registry.resources.register_resource<TextResource>(
      std::vector<std::string>{"String1", "String2"}));
  REQUIRE_FALSE(registry.resources.register_resource<TextResource>(
      std::vector<std::string>{"String3"}));
    */
}

TEST_CASE("View", "[ECS]") {
  Registry registry;
  registry.components.register_component<PositionComponent>();
  registry.components.register_component<VelocityComponent>();
  registry.components.register_component<NameComponent>();
  ECS ecs;
  Entity e0 = ecs.create();
  Entity e1 = ecs.create();
  Entity e2 = ecs.create();

  REQUIRE(registry.components.add_component<PositionComponent>(e0));
  REQUIRE(registry.components.add_component<PositionComponent>(e1, 10, 3));

  REQUIRE(registry.components.has_component<PositionComponent>().size() == 2);
  REQUIRE(registry.components.has_component<VelocityComponent>().empty());
  REQUIRE(
      registry.components.has_component<VelocityComponent, PositionComponent>()
          .empty());

  REQUIRE(registry.components.add_component<VelocityComponent>(e1));
  REQUIRE(
      registry.components.has_component<VelocityComponent, PositionComponent>()
          .size() == 1);

  REQUIRE(registry.components.add_component<NameComponent>(e1, "text"));
  REQUIRE(
      registry.components.has_component<VelocityComponent, PositionComponent>()
          .size() == 1);
  REQUIRE(
      registry.components
          .has_component<VelocityComponent, PositionComponent, NameComponent>()
          .size() == 1);

  foreach
    <NameComponent>(registry.components, [](const auto& name) {
      fmt::print("Viewed name: {}\n", name.name);
    });
}

constexpr void update_position(PositionComponent& p,
                               const VelocityComponent& v) {
  p.x += v.x;
  p.y += v.y;
}

TEST_CASE("Iterate View", "[ECS]") {
  Registry registry;
  registry.components.register_component<PositionComponent>();
  registry.components.register_component<VelocityComponent>();
  ECS ecs;
  constexpr int num_entities = 10000;
  for (int i = 0; i < num_entities; i++) {
    Entity e = ecs.create();
    registry.components.add_component<PositionComponent>(e, 0, 0);
    registry.components.add_component<VelocityComponent>(e, 1, 1);
  }

  constexpr int num_updates = 1000;
  for (int i = 0; i < num_updates; i++) {
    foreach
      <PositionComponent, VelocityComponent>(registry.components,
                                             update_position);
  }
  foreach
    <PositionComponent>(registry.components, [](const PositionComponent& p) {
      REQUIRE(p.x == num_updates);
      REQUIRE(p.y == num_updates);
    });
}
