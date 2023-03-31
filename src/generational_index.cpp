#include <engine/generational_index.h>

using namespace engine;

GenerationalIndex::GenerationalIndex(std::size_t index,
                                     std::uint64_t generation)
    : _index(index), _generation(generation) {}

GenerationalIndex GenerationalIndexAllocator::allocate() {
  if (_free.empty()) {
    _entries.emplace_back();
    return {_entries.size() - 1, 0};
  }
  auto index = _free.front();
  _free.pop();

  auto &entry = _entries[index];
  entry.generation++;
  entry.is_live = true;
  return {index, entry.generation};
}

bool GenerationalIndexAllocator::deallocate(const GenerationalIndex &index) {
  if (!_entries[index.index()].is_live) {
    return false;
  }

  _free.push(index.index());
  _entries[index.index()].is_live = false;
  return true;
}

template <typename T>
void GenerationalIndexArray<T>::set(const GenerationalIndex &index,
                                    const T &value) {}
