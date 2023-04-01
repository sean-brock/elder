#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace engine {

using GenerationalIndexType = std::uint32_t;

class GenerationalIndex {
public:
  GenerationalIndex() = default;
  GenerationalIndex(GenerationalIndexType index, std::uint64_t generation);
  inline GenerationalIndexType index() const { return _index; }
  inline std::uint64_t generation() const { return _generation; }

  virtual ~GenerationalIndex() = default;

private:
  GenerationalIndexType _index;
  std::uint64_t _generation;
};

struct AllocatorEntry {
  std::uint64_t generation = 0;
  bool is_live = true;
};

class GenerationalIndexAllocator {
public:
  GenerationalIndexAllocator() = default;
  virtual ~GenerationalIndexAllocator() = default;

  GenerationalIndex allocate();
  bool deallocate(const GenerationalIndex &index);
  inline bool is_live(const GenerationalIndex &index) const {
    return _entries[index.index()].is_live;
  }
  inline std::size_t allocated() const { return _entries.size(); }
  inline std::size_t free() const { return _free.size(); }

private:
  std::vector<AllocatorEntry> _entries;
  std::queue<GenerationalIndexType> _free;
};

constexpr std::size_t max_generational_index_array_size = 60000;
using SparseArrayIndexType = std::uint16_t;

template <typename T> class GenerationalIndexArray {
public:
  GenerationalIndexArray() = default;
  virtual ~GenerationalIndexArray() = default;

  void set(const GenerationalIndex &index, const T &value) {
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

  const T &get(const GenerationalIndex &index) const {
    auto packed_array_index = check_and_translate_index(index);
    return _data[packed_array_index];
  }

  inline bool contains(const GenerationalIndex &index) const {
    return _index_live[index.index()];
  }

  void remove(const GenerationalIndex &index) {
    auto remove_id = check_and_translate_index(index);
    // if removing only item or last item in data
    if (remove_id == _data_ids.size() - 1) {
      _index_live[index.index()] = false;
    } else {
      auto swap_id = _data.size() - 1;
      auto swap_index = _data_ids[swap_id].index();
      _indices[swap_index] = index.index();
      std::swap(_data[remove_id], _data[swap_id]);
      std::swap(_data_ids[remove_id], _data_ids[swap_id]);

      // invalidate the indicies entry, mapping index to data
      _index_live[index.index()] = false;
    }
    // Remove last item, it is the index to be removed
    _data_ids.pop_back();
    _data.pop_back();
  }

private:
  SparseArrayIndexType
  check_and_translate_index(const GenerationalIndex &index) const {
    if (!contains(index)) {
      throw std::out_of_range(
          "GenerationalIndexArray accessed non-existent index.");
    }
    SparseArrayIndexType packed_array_index{_indices[index.index()]};
    auto &stored_index = _data_ids[packed_array_index];
    if (stored_index.index() != index.index()) {
      throw std::runtime_error("Stored id does not match requested id.");
    }
    if (stored_index.generation() != index.generation()) {
      throw std::runtime_error(
          "Stored generation does not match requested id.");
    }

    return packed_array_index;
  }

  std::array<SparseArrayIndexType, max_generational_index_array_size> _indices;
  std::bitset<max_generational_index_array_size> _index_live;
  std::vector<GenerationalIndex> _data_ids;
  std::vector<T> _data;
};
}; // namespace engine
