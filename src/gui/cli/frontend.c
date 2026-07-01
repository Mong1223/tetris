#include "frontend.h"

#include <string.h>

#define GAME_FIELD_N 22
#define GAME_FIELD_M 32

#define GAME_INFO_M 25
#define CONTROLS_M 24

#define TOP_MARGIN 0
#define LEFT_MARGIN 0
#define START_MENU_N 8
#define START_MENU_M 24
#define PAUSE_MENU_N 5
#define PAUSE_MENU_M 24
#define FINISH_GAME_N 10
#define FINISH_GAME_M 24

void init_ncurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);
  curs_set(0);
  mouseinterval(1);
}

void cleanup_ncurses() { endwin(); }

void init_colors() {
  start_color();

  init_pair(1, COLOR_CYAN, COLOR_CYAN);
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  init_pair(3, COLOR_RED, COLOR_RED);
  init_pair(4, COLOR_YELLOW, COLOR_YELLOW);

  init_pair(5, COLOR_WHITE, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_RED, COLOR_BLACK);
  init_pair(8, COLOR_GREEN, COLOR_BLACK);
}

void render(GameStatus_t status, GameInfo_t gi) {
  refresh();

  bool show_controls = true;
  WINDOW *controls_window;
  if (show_controls) {
    controls_window = print_controls();
    wrefresh(controls_window);
  }

  WINDOW *game_window = NULL;
  if (!gi.pause && status != Initial && status != GameOver) {
    game_window = print_game_field(gi);
    wrefresh(game_window);
  }

  if (status == Initial) {
    print_start_menu();
  } else if (gi.pause) {
    print_pause_menu();
  } else if (status == GameOver) {
    print_game_over(gi);
  }

  WINDOW *info_window = print_game_info(gi);
  wrefresh(info_window);

  if (game_window != NULL) delwin(game_window);
  delwin(info_window);
  if (show_controls) delwin(controls_window);
}

void print_start_menu() {
  WINDOW *w =
      newwin(START_MENU_N, START_MENU_M, (GAME_FIELD_N - START_MENU_N) / 2,
             GAME_FIELD_M - (GAME_FIELD_M - START_MENU_M) / 2 + 1);

  box(w, 0, 0);
  wattron(w, COLOR_PAIR(5));
  mvwprintw(w, 2, 4, "Welcome to Tetris");
  wattroff(w, COLOR_PAIR(5));

  wattron(w, COLOR_PAIR(6));
  mvwprintw(w, 4, 3, "Press 's' to start");
  mvwprintw(w, 5, 3, "Press 'q' to quit");
  wattroff(w, COLOR_PAIR(6));

  wrefresh(w);
  delwin(w);
}

void print_pause_menu() {
  WINDOW *w =
      newwin(PAUSE_MENU_N, PAUSE_MENU_M, (GAME_FIELD_N - PAUSE_MENU_N) / 2,
             CONTROLS_M + (GAME_FIELD_M - PAUSE_MENU_M) / 2 + 1);

  box(w, 0, 0);
  wbkgd(w, COLOR_PAIR(6));

  mvwprintw(w, 2, 8, "Paused");
  mvwprintw(w, 3, 2, "press 'p' to unpause");

  wrefresh(w);
  delwin(w);
}

void print_game_over(GameInfo_t gi) {
  WINDOW *w =
      newwin(FINISH_GAME_N, FINISH_GAME_M, (GAME_FIELD_N - FINISH_GAME_N) / 2,
             CONTROLS_M + (GAME_FIELD_M - FINISH_GAME_M) / 2 + 1);

  box(w, 0, 0);
  wbkgd(w, COLOR_PAIR(7));

  mvwprintw(w, 3, 6, "Game over :(");
  mvwprintw(w, 5, 6, "Score %d", gi.score);
  mvwprintw(w, 6, 6, "Level %d", gi.level);

  wrefresh(w);
  delwin(w);
}

WINDOW *print_controls() {
  WINDOW *w = newwin(GAME_FIELD_N, CONTROLS_M, TOP_MARGIN, LEFT_MARGIN);
  box(w, 0, 0);

  mvwprintw(w, 0, 8, "Controls");
  mvwprintw(w, 2, 2, "Start:   S");
  mvwprintw(w, 4, 2, "Pause:   P");
  mvwprintw(w, 6, 2, "Rotate:  SPACE");
  mvwprintw(w, 8, 2, "Left:    LEFT");
  mvwprintw(w, 10, 2, "Right:   RIGHT");
  mvwprintw(w, 12, 2, "Down:    DOWN");
  mvwprintw(w, 14, 2, "UP:      DROP");
  mvwprintw(w, 16, 2, "Quit:    Q");

  return w;
}

WINDOW *print_game_field(GameInfo_t gi) {
  WINDOW *w =
      newwin(GAME_FIELD_N, GAME_FIELD_M, TOP_MARGIN, LEFT_MARGIN + GAME_INFO_M);

  box(w, 0, 0);
  mvwprintw(w, 0, (GAME_FIELD_M - 6) / 2, "Tetris");

  for (int i = 0; i < FIELD_HEIGHT; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (gi.field[i][j] != 0) {
        wattron(w, COLOR_PAIR(4));
        mvwprintw(w, i + 1, 3 * j + 1, "   ");
        wattroff(w, COLOR_PAIR(4));
      } else {
        wattron(w, COLOR_PAIR(5));
        mvwprintw(w, i + 1, 3 * j + 1, " . ");
        wattroff(w, COLOR_PAIR(5));
      }
    }
  }

  return w;
}

WINDOW *print_game_info(GameInfo_t gi) {
  WINDOW *w = newwin(GAME_FIELD_N, GAME_INFO_M, TOP_MARGIN,
                     LEFT_MARGIN + GAME_FIELD_M + GAME_INFO_M);
  box(w, 0, 0);

  mvwprintw(w, 0, (GAME_INFO_M - 9) / 2, "Game Info");

  print_next_figure(w, gi);

  mvwprintw(w, 8, 2, "High score %d", gi.high_score);
  mvwprintw(w, 11, 2, "Score      %d", gi.score);
  mvwprintw(w, 14, 2, "Level      %d", gi.level);
  mvwprintw(w, 17, 2, "Speed      %d", gi.speed);

  return w;
}

void print_next_figure(WINDOW *w, GameInfo_t gi) {
  mvwprintw(w, 2, 2, "Next:");

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gi.next[i][j] != 0) {
        wattron(w, COLOR_PAIR(2));
        mvwprintw(w, i + 4, j * 3 + 4, "   ");
        wattroff(w, COLOR_PAIR(2));
      }
    }
  }
}

void handle_input(int ch, bool *running) {
  switch (ch) {
    case 'q':
    case 'Q':
      *running = false;
      userInput(Terminate, false);
      break;
    case 's':
    case 'S':
    case 'r':
    case 'R':
      userInput(Start, false);
      break;
    case 'p':
    case 'P':
      userInput(Pause, false);
      break;
    case KEY_LEFT:
      userInput(Left, false);
      break;
    case KEY_RIGHT:
      userInput(Right, false);
      break;
    case KEY_DOWN:
      userInput(Down, false);
      break;
    case KEY_UP:
      userInput(Up, false);
      break;
    case ' ':
      userInput(Action, false);
      break;
    default:
      break;
  }
}
