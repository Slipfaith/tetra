#include "tetris.h"
#include <string.h>

void init_game(GameInfo *game) {
  if (game) {
    memset(game->field, 0, sizeof(game->field));
  }
}

void step_game(GameInfo *game) {
  // Placeholder for game logic
  (void)game;
}
