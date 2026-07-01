
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../brick_game/tetris/backend.h"
#include "frontend.h"

int main(void) {
  initGame();
  init_ncurses();
  init_colors();

  bool running = true;

  while (running) {
    int ch = getch();
    if (ch != ERR) {
      handle_input(ch, &running);
    }

    GameInfo_t gi = updateCurrentState();
    GameStatus_t status = getGameStatus();

    render(status, gi);

    napms(20);
  }

  cleanup_ncurses();
  closeGame();
  return 0;
}
