#ifndef TETRIS_H
#define TETRIS_H

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

typedef struct {
  int field[FIELD_HEIGHT][FIELD_WIDTH];
} GameInfo;

void init_game(GameInfo *game);
void step_game(GameInfo *game);

#endif  // TETRIS_H
