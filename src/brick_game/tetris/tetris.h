#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20

// User actions that can be sent from interface to the library
// The "hold" flag in userInput indicates whether the button is pressed
// continuously. The interface provides eight physical buttons.
typedef enum {
  Start,     // begin a new game
  Pause,     // toggle pause state
  Terminate, // terminate the current game
  Left,      // move the active piece to the left
  Right,     // move the active piece to the right
  Up,        // not used in tetris
  Down,      // drop the piece down
  Action     // rotate the piece
} UserAction_t;

// Internal state of the game
typedef enum {
  STATE_START,
  STATE_SPAWN,
  STATE_MOVE,
  STATE_PAUSE,
  STATE_GAMEOVER
} GameState;

// Structure describing information that can be rendered by the interface.
// Pointers are used for the play field and preview of the next piece. The
// memory pointed to is managed by the library and remains valid until the next
// call to updateCurrentState().
typedef struct {
  int **field;      // play field including moving block
  int **next;       // preview of next block (4x4)
  int score;       // current score
  int high_score;  // maximum stored score
  int level;       // current level (1-10)
  int speed;       // current falling speed (ticks between moves)
  int pause;       // 1 when game is paused
} GameInfo_t;

// Internal game state structure. It is exposed primarily for tests to allow
// inspection of additional information that is not required by the interface.
typedef struct {
  int field[FIELD_HEIGHT][FIELD_WIDTH];
  int next[4][4];
  int current_x;
  int current_y;
  int current_shape;
  int rotation;
  int game_over;  // 1 when game finished
  int score;      // current score
  int high_score; // maximum stored score
  int paused;     // 1 when game is paused
  int level;      // current level (1-10)
} GameInfo;

// Receive user input. The action will be processed on next updateCurrentState()
void userInput(UserAction_t action, bool hold);

// Perform one step of the finite state machine and return current game state
GameInfo_t updateCurrentState(void);

// Direct access to internal game state (useful for tests)
GameInfo *getGame(void);

// Set number of update ticks between automatic downward moves
void setFallSpeed(int delay);

#endif  // TETRIS_H
