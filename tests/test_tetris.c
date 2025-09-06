#include <stdio.h>
#include "../src/brick_game/tetris/tetris.h"

// Minimal test framework ----------------------------------------------------
static int tests_failed = 0;

static void report_failure(const char *file, int line, const char *msg, int a,
                           int b) {
  fprintf(stderr, "%s:%d: %s (%d vs %d)\n", file, line, msg, a, b);
  tests_failed++;
}

#define ck_assert_int_eq(a, b)                                                \
  do {                                                                        \
    int _a = (a), _b = (b);                                                   \
    if (_a != _b)                                                             \
      report_failure(__FILE__, __LINE__, #a " != " #b, _a, _b);              \
  } while (0)

#define ck_assert_int_le(a, b)                                                \
  do {                                                                        \
    int _a = (a), _b = (b);                                                   \
    if (_a > _b)                                                              \
      report_failure(__FILE__, __LINE__, #a " <= " #b, _a, _b);              \
  } while (0)

#define START_TEST(name) static void name(void)
#define END_TEST

// Helper to return the game to its initial state
static void reset_state(void) {
  userInput(Terminate, false);
  updateCurrentState();
  userInput(Terminate, false);
  updateCurrentState();
}

START_TEST(start_and_spawn) {
  reset_state();
  userInput(Start, false);
  GameInfo_t g = updateCurrentState();
  ck_assert_int_eq(g.field[0][4], 1);
  ck_assert_int_eq(g.field[3][4], 1);
}
END_TEST

START_TEST(move_and_rotate) {
  reset_state();
  userInput(Start, false); updateCurrentState();
  userInput(Left, false);
  GameInfo_t g = updateCurrentState();
  ck_assert_int_eq(g.field[1][3], 1);
  userInput(Action, false);
  g = updateCurrentState();
  ck_assert_int_eq(g.field[1][2], 1);
  ck_assert_int_eq(g.field[1][5], 1);
}
END_TEST

START_TEST(line_clear) {
  reset_state();
  userInput(Start, false); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j)
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  userInput(Action, false); updateCurrentState();
  userInput(Down, false);
  GameInfo_t g = updateCurrentState();
  int empty = 1;
  for (int j = 0; j < FIELD_WIDTH; ++j)
    empty &= (g.field[FIELD_HEIGHT - 1][j] == 0);
  ck_assert_int_eq(empty, 1);
}
END_TEST

START_TEST(game_over_state) {
  reset_state();
  userInput(Start, false); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j) gp->field[0][j] = 7;
  updateCurrentState();
  ck_assert_int_eq(getGame()->game_over, 1);
}
END_TEST

START_TEST(score_and_highscore) {
  reset_state();
  remove("highscore.dat");
  userInput(Start, false); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j)
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  userInput(Action, false); updateCurrentState();
  userInput(Down, false);
  GameInfo_t g = updateCurrentState();
  ck_assert_int_eq(g.score, 100);
  ck_assert_int_eq(g.high_score, 100);
  updateCurrentState();
  userInput(Terminate, false); updateCurrentState();
  userInput(Start, false); updateCurrentState();
  g = updateCurrentState();
  ck_assert_int_eq(g.score, 0);
  ck_assert_int_eq(g.high_score, 100);
}
END_TEST

START_TEST(pause_functionality) {
  reset_state();
  userInput(Start, false); updateCurrentState();
  GameInfo_t g = updateCurrentState();
  GameInfo *gs = getGame();
  int x = gs->current_x;
  int y = gs->current_y;
  userInput(Pause, false); updateCurrentState();
  userInput(Left, false);
  g = updateCurrentState();
  gs = getGame();
  ck_assert_int_eq(g.pause, 1);
  ck_assert_int_eq(gs->current_x, x);
  ck_assert_int_eq(gs->current_y, y);
  userInput(Pause, false); updateCurrentState();
  userInput(Down, true);
  g = updateCurrentState();
  gs = getGame();
  ck_assert_int_eq(g.pause, 0);
  ck_assert_int_eq(gs->current_y, y + 1);
}
END_TEST

START_TEST(level_and_speed) {
  reset_state();
  userInput(Start, false); updateCurrentState();
  GameInfo *gp = getGame();
  int start_y = gp->current_y;
  for (int i = 0; i < 19; ++i) updateCurrentState();
  GameInfo g = *getGame();
  ck_assert_int_eq(g.current_y, start_y);

  gp->score = 600;
  updateCurrentState();
  int y_after_level = gp->current_y;
  for (int i = 0; i < 18; ++i) updateCurrentState();
  g = *getGame();
  ck_assert_int_eq(g.level, 2);
  ck_assert_int_eq(g.current_y, y_after_level + 1);

  gp->score = 6000;
  updateCurrentState();
  g = *getGame();
  ck_assert_int_le(g.level, 10);
}
END_TEST

int main(void) {
  start_and_spawn();
  move_and_rotate();
  line_clear();
  game_over_state();
  score_and_highscore();
  pause_functionality();
  level_and_speed();
  if (tests_failed) {
    fprintf(stderr, "%d test(s) failed\n", tests_failed);
    return 1;
  }
  printf("All tests passed\n");
  return 0;
}

