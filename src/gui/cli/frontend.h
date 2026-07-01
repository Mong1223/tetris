
#ifndef FRONTEND_H
#define FRONTEND_H

#include <ncurses.h>
#include <stdbool.h>

#include "../../brick_game/tetris/backend.h"

void init_ncurses();
void init_colors();
void cleanup_ncurses();

// void render(GameStatus_t status, bool win, GameInfo_t gi);
void render(GameStatus_t status, GameInfo_t gi);

WINDOW* print_controls();
WINDOW* print_game_field(GameInfo_t gi);
WINDOW* print_game_info(GameInfo_t gi);
void print_start_menu();
void print_pause_menu();
// void print_win(GameInfo_t gi);
void print_game_over(GameInfo_t gi);
void print_next_figure(WINDOW* w, GameInfo_t gi);

void handle_input(int ch, bool* running);

#endif
