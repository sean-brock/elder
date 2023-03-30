#include <engine/generational_index.h>

using namespace engine;

GenerationalIndex::GenerationalIndex(std::size_t index,
                                     std::uint64_t generation)
    : _index(index), _generation(generation) {}

GenerationalIndex GenerationalIndexAllocator::allocate() {
  if (_free.empty()) {
    _entries.emplace_back();
    return GenerationalIndex{_entries.size() - 1, 0};
  } else {
  }
}
