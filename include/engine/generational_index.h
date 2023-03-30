#import <cstddef>
#import <cstdint>
#import <vector>

namespace engine {
class GenerationalIndex {
public:
  GenerationalIndex() = default;
  GenerationalIndex(std::size_t index, std::uint64_t generation);
  inline std::size_t index() const { return _index; }

  virtual ~GenerationalIndex() = default;

private:
  std::size_t _index;
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
  bool is_live(const GenerationalIndex &index);

private:
  std::vector<AllocatorEntry> _entries;
  std::vector<std::size_t> _free;
};

template <typename T> struct ArrayEntry {
  T value;
  std::uint64_t generation = 0;
};

template <typename T> class GenerationalIndexArray {
public:
  GenerationalIndexArray() = default;
  virtual ~GenerationalIndexArray() = default;

  void set(const GenerationalIndex &index, const T &value);
  const T &get_const(const GenerationalIndex &index) const;
  T &get(const GenerationalIndex &index) const;
};
}; // namespace engine
