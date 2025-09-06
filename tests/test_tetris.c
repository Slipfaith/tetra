#include <check.h>
#include <stdio.h>
#include "../src/brick_game/tetris/tetris.h"

// Helper to return the game to its initial state
static void reset_state(void) {
  userInput(ACT_QUIT);
  updateCurrentState();
  userInput(ACT_QUIT);
  updateCurrentState();
}

START_TEST(start_and_spawn) {
  reset_state();
  userInput(ACT_START);
  GameInfo g = updateCurrentState();
  ck_assert_int_eq(g.field[0][4], 1);
  ck_assert_int_eq(g.field[3][4], 1);
}
END_TEST

START_TEST(move_and_rotate) {
  reset_state();
  userInput(ACT_START); updateCurrentState();
  userInput(ACT_LEFT);
  GameInfo g = updateCurrentState();
  ck_assert_int_eq(g.field[1][3], 1);
  userInput(ACT_ROTATE);
  g = updateCurrentState();
  ck_assert_int_eq(g.field[1][2], 1);
  ck_assert_int_eq(g.field[1][5], 1);
}
END_TEST

START_TEST(line_clear) {
  reset_state();
  userInput(ACT_START); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j)
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  userInput(ACT_ROTATE); updateCurrentState();
  userInput(ACT_DROP);
  GameInfo g = updateCurrentState();
  int empty = 1;
  for (int j = 0; j < FIELD_WIDTH; ++j)
    empty &= (g.field[FIELD_HEIGHT - 1][j] == 0);
  ck_assert_int_eq(empty, 1);
}
END_TEST

START_TEST(game_over_state) {
  reset_state();
  userInput(ACT_START); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j) gp->field[0][j] = 7;
  GameInfo g = updateCurrentState();
  ck_assert_int_eq(g.game_over, 1);
}
END_TEST

START_TEST(score_and_highscore) {
  reset_state();
  remove("highscore.dat");
  userInput(ACT_START); updateCurrentState();
  GameInfo *gp = getGame();
  for (int j = 0; j < FIELD_WIDTH; ++j)
    if (j < 3 || j > 6) gp->field[FIELD_HEIGHT - 1][j] = 7;
  userInput(ACT_ROTATE); updateCurrentState();
  userInput(ACT_DROP);
  GameInfo g = updateCurrentState();
  ck_assert_int_eq(g.score, 100);
  ck_assert_int_eq(g.high_score, 100);
  updateCurrentState();
  userInput(ACT_QUIT); updateCurrentState();
  userInput(ACT_START); updateCurrentState();
  g = updateCurrentState();
  ck_assert_int_eq(g.score, 0);
  ck_assert_int_eq(g.high_score, 100);
}
END_TEST

START_TEST(pause_functionality) {
  reset_state();
  userInput(ACT_START); updateCurrentState();
  GameInfo g = updateCurrentState();
  int x = g.current_x;
  int y = g.current_y;
  userInput(ACT_PAUSE); updateCurrentState();
  userInput(ACT_LEFT);
  g = updateCurrentState();
  ck_assert_int_eq(g.paused, 1);
  ck_assert_int_eq(g.current_x, x);
  ck_assert_int_eq(g.current_y, y);
  userInput(ACT_PAUSE); updateCurrentState();
  userInput(ACT_DOWN);
  g = updateCurrentState();
  ck_assert_int_eq(g.paused, 0);
  ck_assert_int_eq(g.current_y, y + 1);
}
END_TEST

START_TEST(level_and_speed) {
  reset_state();
  userInput(ACT_START); updateCurrentState();
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

Suite *tetris_suite(void) {
  Suite *s = suite_create("tetris");
  TCase *tc = tcase_create("core");
  tcase_add_checked_fixture(tc, reset_state, NULL);
  tcase_add_test(tc, start_and_spawn);
  tcase_add_test(tc, move_and_rotate);
  tcase_add_test(tc, line_clear);
  tcase_add_test(tc, game_over_state);
  tcase_add_test(tc, score_and_highscore);
  tcase_add_test(tc, pause_functionality);
  tcase_add_test(tc, level_and_speed);
  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  Suite *s = tetris_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_ENV);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (failed == 0) ? 0 : 1;
}

