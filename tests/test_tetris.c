#include <assert.h>
#include <stdio.h>
#include "../src/brick_game/tetris/tetris.h"

void start_and_spawn(void) {
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  GameInfo g = updateCurrentState();
  assert(g.field[0][4] == 1);
  assert(g.field[3][4] == 1);
}

void move_and_rotate(void) {
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  updateCurrentState();
  userInput(ACT_LEFT);
  GameInfo g = updateCurrentState();
  assert(g.field[1][3] == 1);
  userInput(ACT_ROTATE);
  g = updateCurrentState();
  assert(g.field[3][2] == 1);
  assert(g.field[3][5] == 1);
}

void line_clear(void) {
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j) {
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  }
  userInput(ACT_ROTATE);
  updateCurrentState();
  userInput(ACT_DROP);
  GameInfo g = updateCurrentState();
  int empty = 1;
  for (int j = 0; j < FIELD_WIDTH; ++j) empty &= (g.field[FIELD_HEIGHT - 1][j] == 0);
  assert(empty == 1);
}

void game_over_state(void) {
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();  // move to spawn state
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j) gp->field[0][j] = 7;
  GameInfo g = updateCurrentState();
  assert(g.game_over == 1);
}

void score_and_highscore(void) {
  remove("highscore.dat");
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j) {
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  }
  userInput(ACT_ROTATE);
  updateCurrentState();
  userInput(ACT_DROP);
  GameInfo g = updateCurrentState();
  assert(g.score == 100);
  assert(g.high_score == 100);
  updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  g = updateCurrentState();
  assert(g.score == 0);
  assert(g.high_score == 100);
}

int main(void) {
  start_and_spawn();
  move_and_rotate();
  line_clear();
  game_over_state();
  score_and_highscore();
  return 0;
}
