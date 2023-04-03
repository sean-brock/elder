#include <catch2/catch_test_macros.hpp>
#include <engine/generational_index.h>
#include <unordered_map>
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

TEST_CASE("Array insert and remove", "[GenerationalIndexArray]") {
  GenerationalIndexAllocator alloc;
  GenerationalIndexArray<int> ints;
  std::unordered_map<GenerationalIndexType, int> values;
  std::vector<GenerationalIndex> gen_indices;

  auto input_transform = [](int x) { return x * 100; };

  // add 5 values
  for (int i = 0; i < 5; i++) {
    GenerationalIndex index = alloc.allocate();
    auto value = input_transform(i);
    ints.set(index, value);
    values[index.index()] = value;
    gen_indices.push_back(index);
  }

  for (int i = 0; i < 5; i++) {
    GenerationalIndex index = gen_indices[i];
    REQUIRE(ints.get(index) == values[index.index()]);
  }
  // remove 1 value in middle
  ints.remove(gen_indices[3]);
  alloc.deallocate(gen_indices[3]);
  gen_indices.erase(gen_indices.begin() + 3);
  for (int i = 0; i < 4; i++) {
    GenerationalIndex index = gen_indices[i];
    REQUIRE(ints.get(index) == values[index.index()]);
  }

  // add it back
  GenerationalIndex new_index = alloc.allocate();
  int new_val = input_transform(new_index.index());
  gen_indices.push_back(new_index);
  ints.set(new_index, new_val);
  values[new_index.index()] = new_val;
  for (int i = 0; i < 5; i++) {
    GenerationalIndex index = gen_indices[i];
    REQUIRE(ints.get(index) == values[index.index()]);
  }
}
