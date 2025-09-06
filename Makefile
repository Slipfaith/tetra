.RECIPEPREFIX := ;
CC=gcc
CFLAGS=-Wall -Wextra -std=c11
LDFLAGS=-lncurses
CHECK_CFLAGS=$(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS=$(shell pkg-config --libs check 2>/dev/null)

SRC_GAME=src/brick_game/tetris/tetris.c
SRC_CLI=src/gui/cli/main.c
OBJ_GAME=$(SRC_GAME:.c=.o)
OBJ_CLI=$(SRC_CLI:.c=.o)
LIB=libtetris.a
TEST_SRC=tests/test_tetris.c
TEST_OBJ=$(TEST_SRC:.c=.o)
TEST_BIN=tests/test.out

all: tetris

$(OBJ_GAME): $(SRC_GAME)
;$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_CLI): $(SRC_CLI)
;$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJ_GAME)
;ar rcs $@ $^

tetris: $(OBJ_CLI) $(LIB)
;$(CC) $(CFLAGS) -o $@ $(OBJ_CLI) $(LIB) $(LDFLAGS)

install:
;@echo "Install is not implemented"

uninstall:
;@echo "Uninstall is not implemented"

clean:
;rm -f $(OBJ_GAME) $(OBJ_CLI) $(TEST_OBJ) $(TEST_BIN) $(LIB) tetris *.gcno *.gcda *.info
;rm -rf report

dvi:
;@echo "DVI target is not implemented"

dist:
;@echo "Dist target is not implemented"

test: $(LIB) $(TEST_OBJ)
;$(CC) $(CFLAGS) $(CHECK_CFLAGS) $(TEST_OBJ) $(LIB) $(CHECK_LIBS) -o $(TEST_BIN)
;./$(TEST_BIN)

$(TEST_OBJ): $(TEST_SRC)
;$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@

gcov_report: CFLAGS += -fprofile-arcs -ftest-coverage $(CHECK_CFLAGS)
gcov_report: clean $(LIB) $(TEST_OBJ)
;$(CC) $(CFLAGS) $(TEST_OBJ) $(LIB) $(CHECK_LIBS) -o $(TEST_BIN)
;./$(TEST_BIN)
;gcov $(SRC_GAME)

.PHONY: all install uninstall clean dvi dist test gcov_report
