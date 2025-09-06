#define _XOPEN_SOURCE 500
#include <ncurses.h>
#include <unistd.h>
#include "../../brick_game/tetris/tetris.h"

static void draw_game(const GameInfo *g) {
  clear();
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      mvprintw(i, j * 2, g->field[i][j] ? "[]" : " .");
    }
  }
  mvprintw(0, FIELD_WIDTH * 2 + 2, "Next:");
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      mvprintw(1 + i, FIELD_WIDTH * 2 + 2 + j * 2,
               g->next[i][j] ? "[]" : "  ");
    }
  }
  mvprintw(6, FIELD_WIDTH * 2 + 2, "Score: %d", g->score);
  mvprintw(7, FIELD_WIDTH * 2 + 2, "High: %d", g->high_score);
  if (g->game_over)
    mvprintw(FIELD_HEIGHT / 2, FIELD_WIDTH, "GAME OVER");
  else if (g->paused)
    mvprintw(FIELD_HEIGHT / 2, FIELD_WIDTH, "PAUSED");
  refresh();
}

int main(void) {
  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  timeout(0);

  setFallSpeed(20);
  userInput(ACT_START);
  int quit_requested = 0;
  while (1) {
    int ch = getch();
    if (ch != ERR) {
      switch (ch) {
        case KEY_LEFT:
          userInput(ACT_LEFT);
          break;
        case KEY_RIGHT:
          userInput(ACT_RIGHT);
          break;
        case KEY_DOWN:
          userInput(ACT_DOWN);
          break;
        case ' ':
          userInput(ACT_ROTATE);
          break;
        case '\n':
          userInput(ACT_DROP);
          break;
        case 'q':
          userInput(ACT_QUIT);
          quit_requested = 1;
          break;
        case 'p':
        case 'P':
          userInput(ACT_PAUSE);
          break;
      }
    }
    GameInfo g = updateCurrentState();
    draw_game(&g);
    if (g.game_over) break;
    usleep(50000);
  }
  if (!quit_requested) {
    nodelay(stdscr, FALSE);
    mvprintw(FIELD_HEIGHT + 1, 0, "Press any key to exit");
    refresh();
    getch();
  }
  endwin();
  return 0;
}
