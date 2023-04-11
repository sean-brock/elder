#include <engine/generational_index.h>
#include <algorithm>
#include <stdexcept>

using namespace engine;

GenerationalIndex::GenerationalIndex(GenerationalIndexType index,
                                     std::uint32_t generation)
    : _index(index), _generation(generation) {}

GenerationalIndex GenerationalIndexAllocator::allocate() {
  if (_free.empty()) {
    _entries.emplace_back();
    return {static_cast<GenerationalIndexType>(_entries.size()) - 1, 0};
  }
  auto index = _free.front();
  _free.pop();

  auto& entry = _entries[index];
  entry.generation++;
  entry.is_live = true;
  return {index, entry.generation};
}

bool GenerationalIndexAllocator::deallocate(const GenerationalIndex& index) {
  if (!_entries[index.index()].is_live) {
    return false;
  }

  _free.push(index.index());
  _entries[index.index()].is_live = false;
  return true;
}
