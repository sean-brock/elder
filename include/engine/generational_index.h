#include <array>
#include <bitset>
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

  void set(const GenerationalIndex &index, const T &value);
  const T &get(const GenerationalIndex &index) const;
  T &get(const GenerationalIndex &index);
  inline bool contains(const GenerationalIndex &index) const {
    return _index_live[index.index()];
  }

private:
  std::array<SparseArrayIndexType, max_generational_index_array_size> _indices;
  std::bitset<max_generational_index_array_size> _index_live;
  std::vector<GenerationalIndex> _data_ids;
  std::vector<T> _data;
};
}; // namespace engine
