#include <catch2/catch_test_macros.hpp>
#include <engine/components.h>
#include <engine/ecs.h>
#include <engine/resources.h>
#include <memory>

using namespace engine;

struct PositionComponent {
  int x;
  int y;
};

TEST_CASE("Entity Create", "[ECS]") {
  Registry registry;
  ECS ecs;
  Entity e0 = ecs.create();
  REQUIRE(registry.components.add_component<PositionComponent>(e0));
  Entity e1 = ecs.create();
  REQUIRE(registry.components.add_component<PositionComponent>(e1, 10, 3));
  REQUIRE_FALSE(registry.components.add_component<PositionComponent>(e1));
}
