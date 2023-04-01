#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <engine/generational_index.h>
#include <unordered_set>

using namespace engine;

TEST_CASE("Set Get", "[GenerationalIndex]") {
  GenerationalIndex index{1, 44};
  REQUIRE(index.index() == 1);
  REQUIRE(index.generation() == 44);
}

TEST_CASE("Allocation", "[GenerationalIndexAllocator]") {
  GenerationalIndexAllocator alloc;
  REQUIRE(alloc.allocated() == 0);
  std::unordered_set<GenerationalIndexType> indices;
  GenerationalIndex last;
  for (int i = 0; i < 5; i++) {
    last = alloc.allocate();
    indices.insert(last.index());
    REQUIRE(alloc.is_live(last));
  }
  REQUIRE(alloc.free() == 0);
  REQUIRE(alloc.allocated() == 5);
  REQUIRE(indices.size() == 5);
  // Free one
  alloc.deallocate(last);
  REQUIRE(alloc.allocated() == 5);
  REQUIRE(alloc.free() == 1);
  // Realloc the same freed one
  auto realloc = alloc.allocate();
  REQUIRE(realloc.index() == last.index());
  // Generation should be diff
  REQUIRE(realloc.generation() != last.generation());
}
