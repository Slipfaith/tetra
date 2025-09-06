#include "tetris.h"

#include <stdlib.h>
#include <string.h>

static GameInfo game;
static UserAction last_action = ACT_NONE;
static GameState state = STATE_START;
static int next_shape = 0;

// Definitions of all tetromino shapes in 4 rotations
static const int shapes[7][4][4][4] = {
    // I
    {
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    },
    // J
    {
        {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    },
    // L
    {
        {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    },
    // O
    {
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    },
    // S
    {
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
    },
    // T
    {
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    },
    // Z
    {
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
    },
};

static void copy_next_preview(void) {
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      game.next[i][j] = shapes[next_shape][0][i][j];
}

static void reset_game(void) {
  memset(&game, 0, sizeof(game));
  next_shape = 0;
  copy_next_preview();
}

static int check_collision(int x, int y, int shape, int rot) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (shapes[shape][rot][i][j]) {
        int nx = x + j;
        int ny = y + i;
        if (nx < 0 || nx >= FIELD_WIDTH || ny < 0 || ny >= FIELD_HEIGHT)
          return 1;
        if (game.field[ny][nx]) return 1;
      }
    }
  }
  return 0;
}

static void place_piece(void) {
  const int (*shape)[4] = shapes[game.current_shape][game.rotation];
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (shape[i][j]) game.field[game.current_y + i][game.current_x + j] = game.current_shape + 1;
}

static void remove_piece(void) {
  const int (*shape)[4] = shapes[game.current_shape][game.rotation];
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      if (shape[i][j]) game.field[game.current_y + i][game.current_x + j] = 0;
}

static void spawn_piece(void) {
  game.current_shape = next_shape;
  next_shape = (next_shape + 1) % 7;
  copy_next_preview();
  game.rotation = 0;
  game.current_x = FIELD_WIDTH / 2 - 2;
  game.current_y = 0;
  if (check_collision(game.current_x, game.current_y, game.current_shape, game.rotation)) {
    game.game_over = 1;
    state = STATE_GAMEOVER;
    return;
  }
  place_piece();
}

static int try_move(int dx, int dy) {
  remove_piece();
  int new_x = game.current_x + dx;
  int new_y = game.current_y + dy;
  if (!check_collision(new_x, new_y, game.current_shape, game.rotation)) {
    game.current_x = new_x;
    game.current_y = new_y;
    place_piece();
    return 1;
  }
  place_piece();
  return 0;
}

static void try_rotate(void) {
  remove_piece();
  int new_rot = (game.rotation + 1) % 4;
  if (!check_collision(game.current_x, game.current_y, game.current_shape, new_rot))
    game.rotation = new_rot;
  place_piece();
}

static void hard_drop(void) {
  remove_piece();
  while (!check_collision(game.current_x, game.current_y + 1, game.current_shape, game.rotation))
    game.current_y++;
  place_piece();
}

static void clear_lines(void) {
  for (int i = FIELD_HEIGHT - 1; i >= 0; --i) {
    int full = 1;
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      if (game.field[i][j] == 0) {
        full = 0;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; --k)
        memcpy(game.field[k], game.field[k - 1], sizeof(game.field[0]));
      memset(game.field[0], 0, sizeof(game.field[0]));
      i++;  // recheck same row after shifting
    }
  }
}

void userInput(UserAction action) { last_action = action; }

GameInfo updateCurrentState(void) {
  switch (state) {
    case STATE_START:
      if (last_action == ACT_START) {
        reset_game();
        state = STATE_SPAWN;
      }
      break;
    case STATE_SPAWN:
      spawn_piece();
      if (state != STATE_GAMEOVER) state = STATE_MOVE;
      break;
    case STATE_MOVE:
      if (last_action == ACT_LEFT) {
        try_move(-1, 0);
      } else if (last_action == ACT_RIGHT) {
        try_move(1, 0);
      } else if (last_action == ACT_ROTATE) {
        try_rotate();
      } else if (last_action == ACT_DOWN) {
        try_move(0, 1);
      } else if (last_action == ACT_DROP) {
        hard_drop();
      } else if (last_action == ACT_QUIT) {
        game.game_over = 1;
        state = STATE_GAMEOVER;
      }
      if (!try_move(0, 1)) {
        place_piece();
        clear_lines();
        if (state != STATE_GAMEOVER) state = STATE_SPAWN;
      }
      break;
    case STATE_GAMEOVER:
      if (last_action == ACT_START) {
        reset_game();
        state = STATE_SPAWN;
        game.game_over = 0;
      }
      break;
  }
  last_action = ACT_NONE;
  return game;
}

__attribute__((constructor)) static void init_module(void) { reset_game(); }

GameInfo *getGame(void) { return &game; }
