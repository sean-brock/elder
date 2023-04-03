#include "engine/components.h"
#include <catch2/catch_test_macros.hpp>
#include <engine/components.h>
#include <engine/ecs.h>

using namespace engine;

TEST_CASE("Entity Create", "[ECS]") {
  Registry registry;
  ECS ecs;
  Entity e = ecs.create();
  registry.components.add_component<UniqueIDComponent>(e);
}
