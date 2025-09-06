#include "tetris.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static GameInfo game;
static UserAction last_action = ACT_NONE;
static GameState state = STATE_START;
static int next_shape = 0;
static const char *score_file = "highscore.dat";
static int base_fall_delay = 20;
static int fall_delay = 20;
static int fall_counter = 0;
static const int level_speedup = 2;

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

void setFallSpeed(int delay) {
  if (delay > 0) {
    base_fall_delay = delay;
    fall_delay =
        base_fall_delay - (game.level > 0 ? (game.level - 1) * level_speedup : 0);
    if (fall_delay < 1) fall_delay = 1;
  }
}

static void load_high_score(void) {
  FILE *f = fopen(score_file, "r");
  if (f) {
    if (fscanf(f, "%d", &game.high_score) != 1) game.high_score = 0;
    fclose(f);
  } else {
    game.high_score = 0;
  }
}

static void save_high_score(void) {
  FILE *f = fopen(score_file, "w");
  if (f) {
    fprintf(f, "%d", game.high_score);
    fclose(f);
  }
}

static void reset_game(void) {
  memset(&game, 0, sizeof(game));
  load_high_score();
  next_shape = 0;
  copy_next_preview();
  game.level = 1;
  fall_delay = base_fall_delay;
  fall_counter = 0;
  game.paused = 0;
}

static void update_level(void) {
  int new_level = game.score / 600 + 1;
  if (new_level > 10) new_level = 10;
  if (new_level != game.level) {
    game.level = new_level;
    fall_delay = base_fall_delay - (game.level - 1) * level_speedup;
    if (fall_delay < 1) fall_delay = 1;
    fall_counter = 0;
  }
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
  fall_counter = 0;
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

static int clear_lines(void) {
  int cleared = 0;
  for (int i = FIELD_HEIGHT - 1; i >= 0; --i) {
    int full = 1;
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      if (game.field[i][j] == 0) {
        full = 0;
        break;
      }
    }
    if (full) {
      ++cleared;
      for (int k = i; k > 0; --k)
        memcpy(game.field[k], game.field[k - 1], sizeof(game.field[0]));
      memset(game.field[0], 0, sizeof(game.field[0]));
      i++;  // recheck same row after shifting
    }
  }
  if (cleared == 1)
    game.score += 100;
  else if (cleared == 2)
    game.score += 300;
  else if (cleared == 3)
    game.score += 700;
  else if (cleared == 4)
    game.score += 1500;
  if (game.score > game.high_score) {
    game.high_score = game.score;
    save_high_score();
  }
  return cleared;
}

void userInput(UserAction action) { last_action = action; }

GameInfo updateCurrentState(void) {
  update_level();
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
        if (!try_move(0, 1)) {
          place_piece();
          clear_lines();
          if (state != STATE_GAMEOVER) state = STATE_SPAWN;
        }
        fall_counter = 0;
        break;
      } else if (last_action == ACT_DROP) {
        hard_drop();
        clear_lines();
        if (state != STATE_GAMEOVER) state = STATE_SPAWN;
        fall_counter = 0;
        break;
      } else if (last_action == ACT_PAUSE) {
        state = STATE_PAUSE;
        break;
      } else if (last_action == ACT_QUIT) {
        game.game_over = 1;
        state = STATE_GAMEOVER;
        break;
      }

      fall_counter++;
      if (fall_counter >= fall_delay) {
        if (!try_move(0, 1)) {
          place_piece();
          clear_lines();
          if (state != STATE_GAMEOVER) state = STATE_SPAWN;
        }
        fall_counter = 0;
      }
      break;
    case STATE_PAUSE:
      if (last_action == ACT_PAUSE) {
        state = STATE_MOVE;
        fall_counter = 0;
      } else if (last_action == ACT_QUIT) {
        game.game_over = 1;
        state = STATE_GAMEOVER;
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
  game.paused = (state == STATE_PAUSE);
  last_action = ACT_NONE;
  return game;
}

__attribute__((constructor)) static void init_module(void) { reset_game(); }

GameInfo *getGame(void) { return &game; }
