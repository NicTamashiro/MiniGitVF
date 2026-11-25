CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:src/%.c=build/%.o)
BIN     = minigit

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

run: $(BIN)
	./$(BIN)

clean:
	rm -rf build $(BIN) $(BIN).exe

-include $(OBJ:.o=.d)

.PHONY: run clean
