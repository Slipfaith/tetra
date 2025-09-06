.RECIPEPREFIX := ;
CC=gcc
CFLAGS=-Wall -Wextra -std=c11
LDFLAGS=-lncurses

SRC_GAME=$(wildcard src/brick_game/tetris/*.c)
SRC_CLI=$(wildcard src/gui/cli/*.c)
OBJ_GAME=$(SRC_GAME:.c=.o)
OBJ_CLI=$(SRC_CLI:.c=.o)

all: tetris

%.o: %.c
;$(CC) $(CFLAGS) -c $< -o $@

tetris: $(OBJ_GAME) $(OBJ_CLI)
;$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install:
;@echo "Install is not implemented"

uninstall:
;@echo "Uninstall is not implemented"

clean:
;rm -f $(OBJ_GAME) $(OBJ_CLI) tetris

dvi:
;@echo "DVI target is not implemented"

dist:
;@echo "Dist target is not implemented"

test:
;@echo "No tests provided"

gcov_report:
;@echo "Coverage not implemented"

.PHONY: all install uninstall clean dvi dist test gcov_report
