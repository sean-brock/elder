#include <cassert>
#include <engine/generational_index.h>
#include <memory>
#include <stdexcept>

using namespace engine;

GenerationalIndex::GenerationalIndex(GenerationalIndexType index,
                                     std::uint64_t generation)
    : _index(index), _generation(generation) {}

GenerationalIndex GenerationalIndexAllocator::allocate() {
  if (_free.empty()) {
    _entries.emplace_back();
    return {static_cast<GenerationalIndexType>(_entries.size()) - 1, 0};
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
                                    const T &value) {
  if (!contains(index)) {
    // not live yet
    _index_live[index.index()] = true;
    // add to end of packed array
    _data_ids.push_back(index);
    _data.push_back(value);
    // map end of packedarray to this index
    _indices[index.index()] = _data.size() - 1;
  } else {
    auto packed_array_index = _indices[index.index()];
    _data[packed_array_index] = value;
    // index should match, otherwise indices and data_ids are out of sync
    assert(_data_ids[packed_array_index].index() == index.index());
    // Update generation
    _data_ids[packed_array_index] = index;
  }
}

template <typename T>
const T &GenerationalIndexArray<T>::get(const GenerationalIndex &index) const {
  if (!contains(index)) {
    throw std::out_of_range(
        "GenerationalIndexArray accessed non-existent index.");
  }
  auto packed_array_index = _indices[index.index()];
  auto &stored_index = _data_ids[packed_array_index];
  if (stored_index.index() != index.index()) {
    throw std::runtime_error("Stored id does not match requested id.");
  }
  if (stored_index.generation() != index.generation()) {
    throw std::runtime_error("Stored id does not match requested id.");
  }
  return _data[packed_array_index];
}

template <typename T>
T &GenerationalIndexArray<T>::get(const GenerationalIndex &index) {
  return const_cast<T &>(get(index));
}
