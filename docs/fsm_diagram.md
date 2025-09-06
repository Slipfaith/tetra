# Tetris Finite State Machine

```
+---------+     start      +-------+
|  START  |  ------------> | SPAWN |
+---------+                +-------+
     ^                          |
     | restart                  v
     |                     +---------+
     |                     |  MOVE   |
     |                     +---------+
     |      game over           |
     |<-------------------------+
     |                          v
     |                     +-----------+
     +---------------------| GAMEOVER |
                           +-----------+
```

* **START** – awaiting the start command from user.
* **SPAWN** – create a new falling piece and prepare preview of the next one. If
  the piece cannot be placed, transition to **GAMEOVER**.
* **MOVE** – main gameplay state. Handles user input (left, right, rotate,
  down, drop). Each tick the piece moves one cell down. When the piece can no
  longer move downward it is merged into the field and filled lines are cleared,
  then transition back to **SPAWN**.
* **GAMEOVER** – shown when a new piece cannot be spawned or user quits the
  game. From here the game can be restarted by sending the start command.
```
