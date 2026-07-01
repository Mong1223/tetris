
#ifndef BRICK_GAME_TETRIS_BACKEND_H_
#define BRICK_GAME_TETRIS_BACKEND_H_

#include <stdbool.h>

#include "figures.h"

#define FIELD_HEIGHT 20
#define FIELD_WIDTH 10

#define NEXT_H FIG_SIZE
#define NEXT_W FIG_SIZE

// PART2 POINTS VALUE
#define SCORE_1_LINE 100
#define SCORE_2_LINES 300
#define SCORE_3_LINES 700
#define SCORE_4_LINES 1500

#define SCORE_PER_LEVEL 600
#define MAX_LEVEL 10

#define HIGHSCORE_FILE "highscore.dat"

typedef enum { STATE_INITIAL = 0, STATE_MOVING, STATE_GAME_OVER } GameState_t;

typedef struct {
  int field[FIELD_HEIGHT][FIELD_WIDTH];
  int current[NEXT_H][NEXT_W];
  int next[NEXT_H][NEXT_W];

  int cur_row;
  int cur_col;

  int score;
  int high_score;
  int level;
  int speed;
  bool paused;

  GameState_t state;

  long long last_tick_ms;
} InternalState;

// Пользовательские действия консоли
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

// Публичные статусы игры для интерфейса
typedef enum { Initial, Moving, GameOver } GameStatus_t;

// Структура с информацией для отрисовки
typedef struct {
  int **field;  // поле 20x10
  int **next;   // матрица 4x4 для следующей фигуры
  int score;
  int high_score;
  int level;
  int speed;
  int pause;  // 0/1
} GameInfo_t;

// Инициализация/перезапуск/освобождение игры
void initGame(void);
void resetGame(void);
void closeGame(void);

// Обработка ввода
void userInput(UserAction_t action, bool hold);

// Обновление логики и получение состояния
GameInfo_t updateCurrentState(void);

// Получение статуса игры (для экранов start/pause/game over)
GameStatus_t getGameStatus(void);
long long GetTimeMs(void);
int **AllocMatrix(int rows, int cols);
void FreeMatrix(int **m, int rows);
void ClearField(void);
bool CanPlaceFigure(int row, int col, const int fig[NEXT_H][NEXT_W]);
void AttachCurrentFigureToField(void);
bool RowIsFull(int row);
void CollapseRow(int row);
int ClearFullLines(void);
void LoadHighScore(void);
void SaveHighScore(void);
void AddScoreForLines(int lines);
void SpawnFigure(void);
void HardDropFigure(void);
void TickGame(void);

#endif  // BRICK_GAME_TETRIS_BACKEND_H_
