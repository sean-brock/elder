#ifndef ENGINE_PACKED_ARRAY_H
#define ENGINE_PACKED_ARRAY_H

#include <vector>

template <typename T> class PackedArray {
public:
  PackedArray() = default;
  T &get(std::size_t index);
  void set(const T &value);
  inline bool contains(std::size_t index) const { return }

private:
  std::vector<T> _data;
  std::vector<T> _indices;
};

#endif
