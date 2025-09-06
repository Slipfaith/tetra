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
  mvprintw(8, FIELD_WIDTH * 2 + 2, "Level: %d", g->level);
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
  int pause_pressed = 0;
  while (1) {
    int ch = getch();
    if (ch != ERR) {
      switch (ch) {
        case KEY_LEFT:
          userInput(ACT_LEFT);
          pause_pressed = 0;
          break;
        case KEY_RIGHT:
          userInput(ACT_RIGHT);
          pause_pressed = 0;
          break;
        case KEY_DOWN:
          userInput(ACT_DOWN);
          pause_pressed = 0;
          break;
        case ' ':
          userInput(ACT_ROTATE);
          pause_pressed = 0;
          break;
        case '\n':
          userInput(ACT_DROP);
          pause_pressed = 0;
          break;
        case 'q':
          userInput(ACT_QUIT);
          quit_requested = 1;
          pause_pressed = 0;
          break;
        case 'p':
        case 'P':
          if (!pause_pressed) {
            userInput(ACT_PAUSE);
            pause_pressed = 1;
          }
          break;
        default:
          pause_pressed = 0;
          break;
      }
    } else {
      pause_pressed = 0;
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
