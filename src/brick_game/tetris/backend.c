
#include "backend.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
static InternalState g_state;
static GameInfo_t g_info;
// текущее время в миллисекундах
////////////////////////////////
long long GetTimeMs(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
}
// Выделение памяти матриц
int **AllocMatrix(int rows, int cols) {
  int **m = malloc(rows * sizeof(int *));
  if (m == NULL) return NULL;
  for (int i = 0; i < rows; ++i) {
    m[i] = malloc(cols * sizeof(int));
    if (m[i] == NULL) {
      for (int j = 0; j < i; ++j) {
        free(m[j]);
      }
      free(m);
      return NULL;
    }
  }
  return m;
}
// Освобождение памяти матриц
void FreeMatrix(int **m, int rows) {
  if (m == NULL) return;
  for (int i = 0; i < rows; ++i) {
    free(m[i]);
  }
  free(m);
}
// очистка поля
void ClearField(void) {
  for (int r = 0; r < FIELD_HEIGHT; ++r) {
    for (int c = 0; c < FIELD_WIDTH; ++c) {
      g_state.field[r][c] = 0;
    }
  }
}
// проверка возможности размещения фигуры
bool CanPlaceFigure(int row, int col, const int fig[NEXT_H][NEXT_W]) {
  bool ok = true;

  for (int r = 0; r < NEXT_H && ok; ++r) {
    for (int c = 0; c < NEXT_W && ok; ++c) {
      if (fig[r][c] == 0) continue;

      int fr = row + r;
      int fc = col + c;

      if (fr < 0 || fr >= FIELD_HEIGHT || fc < 0 || fc >= FIELD_WIDTH) {
        ok = false;
        break;
      }

      if (g_state.field[fr][fc] != 0) {
        ok = false;
        break;
      }
    }
  }

  return ok;
}
// прикрепление текущей фигуры к полю
void AttachCurrentFigureToField(void) {
  for (int r = 0; r < NEXT_H; ++r) {
    for (int c = 0; c < NEXT_W; ++c) {
      if (g_state.current[r][c] == 0) continue;

      int fr = g_state.cur_row + r;
      int fc = g_state.cur_col + c;

      if (fr >= 0 && fr < FIELD_HEIGHT && fc >= 0 && fc < FIELD_WIDTH) {
        g_state.field[fr][fc] = g_state.current[r][c];
      }
    }
  }
}
// проверка, заполнена ли строка
bool RowIsFull(int row) {
  bool full = true;

  for (int c = 0; c < FIELD_WIDTH; ++c) {
    if (g_state.field[row][c] == 0) {
      full = false;
      break;
    }
  }

  return full;
}
// сдвиг строк вниз
void CollapseRow(int row) {
  for (int r = row; r > 0; --r) {
    for (int c = 0; c < FIELD_WIDTH; ++c) {
      g_state.field[r][c] = g_state.field[r - 1][c];
    }
  }
  for (int c = 0; c < FIELD_WIDTH; ++c) {
    g_state.field[0][c] = 0;
  }
}
// очистка заполненных строк
int ClearFullLines(void) {
  int cleared = 0;
  for (int r = FIELD_HEIGHT - 1; r >= 0; --r) {
    if (RowIsFull(r)) {
      CollapseRow(r);
      ++cleared;
      ++r;
    }
  }
  return cleared;
}
// загрузка рекорда из файла
void LoadHighScore(void) {
  FILE *f = fopen(HIGHSCORE_FILE, "r");
  if (f == NULL) {
    g_state.high_score = 0;
    return;
  }
  if (fscanf(f, "%d", &g_state.high_score) != 1) {
    g_state.high_score = 0;
  }
  fclose(f);
}
// сохранение рекорда в файл
void SaveHighScore(void) {
  FILE *f = fopen(HIGHSCORE_FILE, "w");
  if (f == NULL) {
    return;
  }
  fprintf(f, "%d\n", g_state.high_score);
  fclose(f);
}
// добавление очков за очищенные линии
void AddScoreForLines(int lines) {
  if (lines <= 0) return;
  int add = 0;
  switch (lines) {
    case 1:
      add = SCORE_1_LINE;
      break;
    case 2:
      add = SCORE_2_LINES;
      break;
    case 3:
      add = SCORE_3_LINES;
      break;
    case 4:
      add = SCORE_4_LINES;
      break;
    default:
      break;
  }
  g_state.score += add;
  // обновление уровня
  int target_level = g_state.score / SCORE_PER_LEVEL + 1;
  if (target_level > MAX_LEVEL) {
    target_level = MAX_LEVEL;
  }
  if (target_level > g_state.level) {
    g_state.level = target_level;
  }

  g_state.speed = g_state.level;

  if (g_state.score > g_state.high_score) {
    g_state.high_score = g_state.score;
    SaveHighScore();
  }
}
// спавн новой фигуры
void SpawnFigure(void) {
  if (g_state.state == STATE_INITIAL) {
    clear_figure(g_state.current);
    clear_figure(g_state.next);

    generate_random_figure(g_state.current);
    generate_random_figure(g_state.next);
  } else {
    copy_figure(g_state.current, g_state.next);
    generate_random_figure(g_state.next);
  }

  g_state.cur_row = 0;
  g_state.cur_col = FIELD_WIDTH / 2 - 2;

  if (!CanPlaceFigure(g_state.cur_row, g_state.cur_col, g_state.current)) {
    g_state.state = STATE_GAME_OVER;
  } else {
    g_state.state = STATE_MOVING;
  }
}
// инициализация игры
void initGame(void) {
  memset(&g_state, 0, sizeof(g_state));

  ClearField();
  clear_figure(g_state.current);
  clear_figure(g_state.next);

  g_state.score = 0;
  g_state.level = 1;
  g_state.speed = 1;
  g_state.paused = false;
  g_state.state = STATE_INITIAL;
  g_state.last_tick_ms = GetTimeMs();

  LoadHighScore();

  g_info.field = AllocMatrix(FIELD_HEIGHT, FIELD_WIDTH);
  g_info.next = AllocMatrix(NEXT_H, NEXT_W);
  g_info.score = 0;
  g_info.high_score = g_state.high_score;
  g_info.level = g_state.level;
  g_info.speed = g_state.speed;
  g_info.pause = 0;
}
// перезапуск игры
void resetGame(void) {
  ClearField();
  clear_figure(g_state.current);
  clear_figure(g_state.next);

  g_state.score = 0;
  g_state.level = 1;
  g_state.speed = 1;
  g_state.paused = false;
  g_state.state = STATE_INITIAL;
  g_state.last_tick_ms = GetTimeMs();
}
// освобождение ресурсов игры
void closeGame(void) {
  FreeMatrix(g_info.field, FIELD_HEIGHT);
  FreeMatrix(g_info.next, NEXT_H);
  g_info.field = NULL;
  g_info.next = NULL;
}
// игровой тик
void TickGame(void) {
  long long now = GetTimeMs();
  long long dt = now - g_state.last_tick_ms;

  int delay_ms = 500;
  if (g_state.level > 0) {
    delay_ms = 1000 / g_state.level;
    if (delay_ms < 50) {
      delay_ms = 50;
    }
  }

  if (dt < delay_ms || g_state.paused || g_state.state != STATE_MOVING) {
    return;
  }

  g_state.last_tick_ms = now;

  int new_row = g_state.cur_row + 1;
  int new_col = g_state.cur_col;

  if (CanPlaceFigure(new_row, new_col, g_state.current)) {
    g_state.cur_row = new_row;
  } else {
    AttachCurrentFigureToField();

    int lines = ClearFullLines();
    AddScoreForLines(lines);

    SpawnFigure();
  }
}

void HardDropFigure(void) {
  if (g_state.state != STATE_MOVING || g_state.paused) {
    return;
  }

  // Пока можно — двигаем фигуру вниз
  while (
      CanPlaceFigure(g_state.cur_row + 1, g_state.cur_col, g_state.current)) {
    g_state.cur_row++;
  }

  // Приземлили — прикрепляем
  AttachCurrentFigureToField();

  int lines = ClearFullLines();
  AddScoreForLines(lines);

  // Спавним следующую фигуру
  SpawnFigure();
}

// обработка ввода пользователя
void userInput(UserAction_t action, bool hold) {
  (void)hold;

  switch (action) {
    case Start:
      if (g_state.state == STATE_INITIAL || g_state.state == STATE_GAME_OVER) {
        resetGame();
        SpawnFigure();
      }
      break;

    case Pause:
      if (g_state.state == STATE_MOVING) {
        g_state.paused = !g_state.paused;
      }
      break;

    case Terminate:
      g_state.state = STATE_GAME_OVER;
      break;

    case Left:
      if (g_state.state == STATE_MOVING && !g_state.paused) {
        int new_col = g_state.cur_col - 1;
        if (CanPlaceFigure(g_state.cur_row, new_col, g_state.current)) {
          g_state.cur_col = new_col;
        }
      }
      break;

    case Right:
      if (g_state.state == STATE_MOVING && !g_state.paused) {
        int new_col = g_state.cur_col + 1;
        if (CanPlaceFigure(g_state.cur_row, new_col, g_state.current)) {
          g_state.cur_col = new_col;
        }
      }
      break;

    case Down:
      if (g_state.state == STATE_MOVING && !g_state.paused) {
        int new_row = g_state.cur_row + 1;
        if (CanPlaceFigure(new_row, g_state.cur_col, g_state.current)) {
          g_state.cur_row = new_row;
        } else {
          AttachCurrentFigureToField();
          int lines = ClearFullLines();
          AddScoreForLines(lines);
          SpawnFigure();
        }
      }
      break;

    case Up:
      HardDropFigure();
      break;

    case Action:
      if (g_state.state == STATE_MOVING && !g_state.paused) {
        int tmp[FIG_SIZE][FIG_SIZE];
        copy_figure(tmp, g_state.current);
        rotate_figure_cw(tmp);
        if (CanPlaceFigure(g_state.cur_row, g_state.cur_col, tmp)) {
          copy_figure(g_state.current, tmp);
        }
      }
      break;

    default:
      break;
  }
}
// обновление логики и получение состояния
GameInfo_t updateCurrentState(void) {
  TickGame();

  for (int r = 0; r < FIELD_HEIGHT; ++r) {
    for (int c = 0; c < FIELD_WIDTH; ++c) {
      g_info.field[r][c] = g_state.field[r][c];
    }
  }

  for (int fr = 0; fr < NEXT_H; ++fr) {
    for (int fc = 0; fc < NEXT_W; ++fc) {
      if (g_state.current[fr][fc] == 0) continue;
      int r = g_state.cur_row + fr;
      int c = g_state.cur_col + fc;
      if (r >= 0 && r < FIELD_HEIGHT && c >= 0 && c < FIELD_WIDTH) {
        g_info.field[r][c] = g_state.current[fr][fc];
      }
    }
  }

  for (int r = 0; r < NEXT_H; ++r) {
    for (int c = 0; c < NEXT_W; ++c) {
      g_info.next[r][c] = g_state.next[r][c];
    }
  }

  g_info.score = g_state.score;
  g_info.high_score = g_state.high_score;
  g_info.level = g_state.level;
  g_info.speed = g_state.speed;
  g_info.pause = g_state.paused ? 1 : 0;

  return g_info;
}
// получение статуса игры (для экранов start/pause/game over)
GameStatus_t getGameStatus(void) {
  switch (g_state.state) {
    case STATE_INITIAL:
      return Initial;

    case STATE_MOVING:
      return Moving;

    case STATE_GAME_OVER:
      return GameOver;
  }
  // На случай некорректного состояния — fallback
  return GameOver;
}
