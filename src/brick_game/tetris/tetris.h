#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

// User actions that can be sent from interface to the library
typedef enum {
  ACT_NONE,
  ACT_LEFT,
  ACT_RIGHT,
  ACT_DOWN,
  ACT_DROP,
  ACT_ROTATE,
  ACT_START,
  ACT_PAUSE,
  ACT_QUIT
} UserAction;

// Internal state of the game
typedef enum {
  STATE_START,
  STATE_SPAWN,
  STATE_MOVE,
  STATE_GAMEOVER
} GameState;

// Structure describing the current game field and additional information
typedef struct {
  int field[FIELD_HEIGHT][FIELD_WIDTH];  // play field including moving block
  int next[4][4];                        // preview of next block
  int current_x;
  int current_y;
  int current_shape;
  int rotation;
  int game_over;  // 1 when game finished
  int score;      // current score
  int high_score; // maximum stored score
} GameInfo;

// Receive user input. The action will be processed on next updateCurrentState()
void userInput(UserAction action);

// Perform one step of the finite state machine and return current game state
GameInfo updateCurrentState(void);

// Direct access to internal game state (useful for tests)
GameInfo *getGame(void);

#endif  // TETRIS_H
