#ifndef ENGINE_COMPONENTS_H
#define ENGINE_COMPONENTS_H

#include <atomic>
namespace engine {

class UniqueIDComponent {
public:
  UniqueIDComponent() : id(++_next_id) {}
  int id;

private:
  inline static std::atomic_int _next_id = 0;
};
}; // namespace engine

#endif
