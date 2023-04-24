#include <engine/ecs.h>
#include <fmt/core.h>
#include <iostream>

using namespace engine;

enum class Tile { EMPTY, X, O };

struct BoardComponent {
  std::array<Tile, 9> state;
};

int player_input() {
  int x = 0;
  bool valid_input = false;
  while (!valid_input) {
    std::cin >> x;
    if (x >= 0 && x <= 8) {
      valid_input = true;
      std::cout << "Input must be in range [0,8]." << std::endl;
    }
  }
  return x;
}

void game_logic_system(BoardComponent& board) {}

void render_system(const BoardComponent& board) {}

void tictac() {
  bool game_over = false;
  Registry registry;
  registry.components.register_component<BoardComponent>();

  while (!game_over) {
    foreach
      <BoardComponent>(registry.components, game_logic_system);
    foreach
      <BoardComponent>(registry.components, render_system);
  }
}

int main() {
  while (true) {
    tictac();
  }
}
