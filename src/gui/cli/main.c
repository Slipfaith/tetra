#include <ncurses.h>
#include "../../brick_game/tetris/tetris.h"

int main(void) {
  GameInfo game;
  init_game(&game);

  initscr();
  noecho();
  curs_set(FALSE);

  mvprintw(0, 0, "Tetris stub. Field %dx%d", FIELD_WIDTH, FIELD_HEIGHT);
  refresh();
  getch();

  endwin();
  return 0;
}
