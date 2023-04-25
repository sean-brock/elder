#include <engine/ecs.h>
#include <fmt/core.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
using namespace engine;

enum class Tile { EMPTY, X, O };

constexpr char to_char(const Tile& tile) {
  switch (tile) {
    case Tile::EMPTY:
      return ' ';
    case Tile::X:
      return 'X';
    case Tile::O:
      return 'O';
  }
  return 'Z';
}

constexpr int board_size = 9;

struct GameComponent {
  bool* game_over;
  bool player_won = false;
  bool tie = false;
};

struct BoardComponent {
  std::array<Tile, board_size> state;
  bool player_turn;
};

constexpr std::array<Tile, board_size> empty_board() {
  return {Tile::EMPTY};
}

struct AIComponent {
  std::mt19937 generator;
};

std::vector<int> available_tiles(const BoardComponent& board) {
  std::vector<int> indices;
  for (int i = 0; i < board_size; i++) {
    if (board.state[i] == Tile::EMPTY) {
      indices.push_back(i);
    }
  }
  return indices;
}

int player_input(const std::vector<int>& open_indices) {
  int x = 0;
  bool valid_input = false;
  while (!valid_input) {
    fmt::print("Your move: ");
    std::cin >> x;
    if (x < 0 && x > 8) {
      std::cout << "Input must be in range [0,8]." << std::endl;
    } else if (std::find(std::begin(open_indices), std::end(open_indices), x) ==
               std::end(open_indices)) {
      std::cout << "Input tile is already occupied." << std::endl;
    } else {
      valid_input = true;
    }
  }
  return x;
}

int ai_input(const std::vector<int>& open_indices, std::mt19937& generator) {
  std::uniform_int_distribution<int> dist{
      0, static_cast<int>(open_indices.size()) - 1};
  return open_indices[dist(generator)];
}

void board_turn_system(BoardComponent& board) {
  board.player_turn = !board.player_turn;
}

void input_system(BoardComponent& board, AIComponent& ai) {
  auto tiles = available_tiles(board);
  if (tiles.empty()) {
    throw std::runtime_error("Invalid board state. No available moves.");
  }
  if (board.player_turn) {
    int i = player_input(tiles);
    board.state[i] = Tile::X;
  } else {
    int i = ai_input(tiles, ai.generator);
    board.state[i] = Tile::O;
  }
}

constexpr std::array<std::array<int, 3>, 8> win_conditions = {{{0, 1, 2},
                                                               {3, 4, 5},
                                                               {6, 7, 8},
                                                               {0, 3, 6},
                                                               {1, 4, 7},
                                                               {2, 5, 8},
                                                               {0, 4, 8},
                                                               {2, 4, 6}}};

bool check_win(const std::array<int, 3>& condition,
               const std::array<Tile, board_size>& state, Tile player) {
  for (int i = 0; i < 3; i++) {
    if (state[i] != player) {
      return false;
    }
  }
  return true;
}

bool check_tie(const std::array<Tile, board_size>& state) {
  for (int i = 0; i < board_size; i++) {
    if (state[i] == Tile::EMPTY) {
      return false;
    }
  }
  return true;
}

void winner_system(const BoardComponent& board, GameComponent& game) {
  for (auto& win_state : win_conditions) {
    if (check_tie(board.state)) {
      *game.game_over = true;
      game.tie = true;
    } else if (check_win(win_state, board.state, Tile::X)) {
      *game.game_over = true;
      game.player_won = true;
      game.tie = false;
    } else if (check_win(win_state, board.state, Tile::O)) {
      *game.game_over = true;
      game.player_won = false;
      game.tie = false;
    }
    if (*game.game_over) {
      return;
    }
  }
  *game.game_over = false;
}

void render_system(const BoardComponent& board, const GameComponent& game) {
  auto board_char = [&](int i) {
    return to_char(board.state[i]);
  };
  if (*game.game_over) {
    if (game.tie) {
      std::cout << "Tie!\n" << std::endl;
    } else if (game.player_won) {
      std::cout << "You won!\n" << std::endl;
    } else {
      std::cout << "You lost!\n" << std::endl;
    }
  } else if (board.player_turn) {
    fmt::print(
        "\n\n"
        " {} | {} | {}\n"
        "-----------\n"
        " {} | {} | {}\n"
        "-----------\n"
        " {} | {} | {}\n",
        board_char(0), board_char(1), board_char(2), board_char(3),
        board_char(4), board_char(5), board_char(6), board_char(7),
        board_char(8));
  }
}

void tictac() {
  bool game_over = false;
  Registry registry;
  ECS ecs;
  registry.components.register_component<BoardComponent>();
  registry.components.register_component<AIComponent>();
  registry.components.register_component<GameComponent>();
  // Initialize board and game state
  {
    Entity id = ecs.create();
    std::random_device dev;
    std::mt19937 generator{dev()};
    std::uniform_int_distribution<int> dist{0, 1};

    const bool first_move = dist(generator) > 0;

    registry.components.add_component<BoardComponent>(id, empty_board(),
                                                      first_move);
    registry.components.add_component<AIComponent>(id, generator);
    registry.components.add_component<GameComponent>(id, &game_over);

    fmt::print(
        "tictac\n"
        "------\n"
        "You are X.\n");
  }

  while (!game_over) {
    foreach
      <BoardComponent, AIComponent>(registry.components, input_system);
    foreach
      <BoardComponent>(registry.components, board_turn_system);
    foreach
      <BoardComponent, GameComponent>(registry.components, winner_system);
    foreach
      <BoardComponent, GameComponent>(registry.components, render_system);
  }
}

int main() {
  while (true) {
    tictac();
  }
}
