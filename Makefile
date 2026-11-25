# Mini-Git: duas interfaces (CLI e TUI) sobre o mesmo core.
#
# Cada camada compila SO com os cabecalhos que ela pode usar: se o core
# incluir algo da CLI ou da TUI (ex.: "paineis.h"), a compilacao falha.
# E so a TUI liga com ncurses: se o core chamar uma funcao do curses, o
# link da CLI (minigit) falha. A separacao e conferida pela ferramenta.
#
#   estruturas  ->  so ela mesma
#   core        ->  estruturas
#   cli / tui   ->  core + estruturas (e a TUI, ncurses)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -MMD -MP

INC_ESTRUTURAS = -Iinclude/estruturas
INC_CORE       = $(INC_ESTRUTURAS) -Iinclude/core
INC_CLI        = $(INC_CORE) -Iinclude/cli
INC_TUI        = $(INC_CORE) -Iinclude/tui

# ncurses no Linux/macOS. No Windows (MinGW + PDCurses): make tui CURSES_LIBS=-lpdcurses
CURSES_CFLAGS ?= $(shell pkg-config --cflags ncursesw 2>/dev/null)
CURSES_LIBS   ?= $(shell pkg-config --libs ncursesw 2>/dev/null || echo -lncursesw)

camada = $(patsubst src/%.c,build/%.o,$(wildcard src/$(1)/*.c))
OBJ_ESTRUTURAS = $(call camada,estruturas)
OBJ_CORE       = $(call camada,core)
OBJ_CLI        = $(call camada,cli)
OBJ_TUI        = $(call camada,tui)

all: minigit minigit-tui

cli: minigit
tui: minigit-tui

minigit: $(OBJ_ESTRUTURAS) $(OBJ_CORE) $(OBJ_CLI)
	$(CC) $(CFLAGS) -o $@ $^

minigit-tui: $(OBJ_ESTRUTURAS) $(OBJ_CORE) $(OBJ_TUI)
	$(CC) $(CFLAGS) -o $@ $^ $(CURSES_LIBS)

build/estruturas/%.o: src/estruturas/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_ESTRUTURAS) -c $< -o $@

build/core/%.o: src/core/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_CORE) -c $< -o $@

build/cli/%.o: src/cli/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_CLI) -c $< -o $@

build/tui/%.o: src/tui/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_TUI) $(CURSES_CFLAGS) -c $< -o $@

run: minigit
	./minigit

run-tui: minigit-tui
	./minigit-tui

clean:
	rm -rf build minigit minigit-tui minigit.exe minigit-tui.exe

-include $(shell find build -name '*.d' 2>/dev/null)

.PHONY: all cli tui run run-tui clean
