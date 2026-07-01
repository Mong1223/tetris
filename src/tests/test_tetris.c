#include <check.h>
#include <stdbool.h>
#include <stdio.h>

#include "../brick_game/tetris/backend.h"

START_TEST(init_1) {
  initGame();
  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.score, 0);

  closeGame();
}
END_TEST

START_TEST(init_2) {
  initGame();
  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.level, 1);

  closeGame();
}
END_TEST

START_TEST(init_3) {
  initGame();
  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.pause, 0);

  closeGame();
}
END_TEST

START_TEST(init_4) {
  initGame();
  userInput(Terminate, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, GameOver);

  closeGame();
}
END_TEST

START_TEST(init_5) {
  initGame();

  // R — старт игры (в новом API Start)
  userInput(Start, false);
  updateCurrentState();

  // Q — завершение игры (Terminate)
  userInput(Terminate, false);
  updateCurrentState();

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, GameOver);

  closeGame();
}
END_TEST

START_TEST(start_1) {
  initGame();
  userInput(Start, false);

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(spawn_1) {
  initGame();
  userInput(Start, false);
  userInput(Terminate, false);

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, GameOver);

  closeGame();
}
END_TEST

START_TEST(spawn_2) {
  initGame();
  userInput(Start, false);  // STATE_INITIAL → SPAWN → MOVING
  updateCurrentState();

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_left) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Left, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_right) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Right, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_down) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Down, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_action) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Action, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_up) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Up, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Moving);

  closeGame();
}
END_TEST

START_TEST(moving_pause) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Pause, false);
  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.pause, 1);

  closeGame();
}
END_TEST

START_TEST(moving_terminate) {
  initGame();
  userInput(Start, false);
  updateCurrentState();

  userInput(Terminate, false);
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, GameOver);

  closeGame();
}
END_TEST

START_TEST(attaching_1) {
  initGame();
  userInput(Start, false);

  // опускаем фигуру пока нельзя
  while (getGameStatus() == Moving) {
    userInput(Down, false);
    updateCurrentState();
  }

  GameStatus_t st = getGameStatus();
  ck_assert(st == Moving || st == GameOver);
  closeGame();
}
END_TEST

START_TEST(gameover_1) {
  initGame();
  userInput(Start, false);

  // пока игра не окончена
  while (getGameStatus() != GameOver) {
    userInput(Down, false);
    updateCurrentState();
  }

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, GameOver);

  closeGame();
}
END_TEST

START_TEST(score_0_lines) {
  initGame();
  int old_score = updateCurrentState().score;

  AddScoreForLines(0);

  GameInfo_t gi = updateCurrentState();
  ck_assert_int_eq(gi.score, old_score);

  closeGame();
}
END_TEST

START_TEST(score_1_line) {
  initGame();
  AddScoreForLines(1);

  GameInfo_t gi = updateCurrentState();
  ck_assert_int_eq(gi.score, SCORE_1_LINE);
  ck_assert_int_eq(gi.level, 1);

  closeGame();
}
END_TEST

START_TEST(score_2_lines) {
  initGame();
  AddScoreForLines(2);

  GameInfo_t gi = updateCurrentState();
  ck_assert_int_eq(gi.score, SCORE_2_LINES);

  closeGame();
}
END_TEST

START_TEST(score_3_lines) {
  initGame();
  AddScoreForLines(3);

  GameInfo_t gi = updateCurrentState();
  ck_assert_int_eq(gi.score, SCORE_3_LINES);

  closeGame();
}
END_TEST

START_TEST(score_4_lines) {
  initGame();
  AddScoreForLines(4);

  GameInfo_t gi = updateCurrentState();
  ck_assert_int_eq(gi.score, SCORE_4_LINES);

  closeGame();
}
END_TEST

START_TEST(level_up) {
  initGame();

  // Пусть SCORE_PER_LEVEL = 600 (например)
  // Дадим столько очков, чтобы перейти на следующую ступень
  int need = SCORE_PER_LEVEL;
  int lines = need / SCORE_1_LINE;

  for (int i = 0; i < lines; i++) {
    AddScoreForLines(1);
  }

  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.level, 2);
  ck_assert_int_eq(gi.speed, 2);

  closeGame();
}
END_TEST

START_TEST(level_cap_max) {
  initGame();

  // дадим огромный счёт, чтобы превысить MAX_LEVEL
  for (int i = 0; i < 1000; i++) {
    AddScoreForLines(4);
  }

  GameInfo_t gi = updateCurrentState();

  ck_assert_int_eq(gi.level, MAX_LEVEL);
  ck_assert_int_eq(gi.speed, MAX_LEVEL);

  closeGame();
}
END_TEST

START_TEST(high_score_update) {
  remove(HIGHSCORE_FILE);
  initGame();

  int prev = updateCurrentState().high_score;

  AddScoreForLines(4);  // точно повысит high_score

  GameInfo_t gi = updateCurrentState();
  ck_assert(gi.high_score >= prev);
  ck_assert_int_eq(gi.high_score, gi.score);

  closeGame();
}
END_TEST
START_TEST(status_initial) {
  initGame();

  GameStatus_t st = getGameStatus();
  ck_assert_int_eq(st, Initial);

  closeGame();
}
END_TEST
START_TEST(userinput_default) {
  initGame();

  // Передадим некорректное действие (которого нет в enum)
  userInput((UserAction_t)999, false);

  // Проверим, что состояние игры не изменилось
  GameStatus_t st = getGameStatus();

  ck_assert_int_eq(st, Initial);

  closeGame();
}
END_TEST

Suite *tetris_suite() {
  Suite *s = suite_create("tetris_suite");
  TCase *tc = tcase_create("tetris_tc");

  tcase_add_test(tc, init_1);
  tcase_add_test(tc, init_2);
  tcase_add_test(tc, init_3);
  tcase_add_test(tc, init_4);
  tcase_add_test(tc, init_5);
  tcase_add_test(tc, start_1);
  tcase_add_test(tc, spawn_1);
  tcase_add_test(tc, spawn_2);
  tcase_add_test(tc, moving_left);
  tcase_add_test(tc, moving_right);
  tcase_add_test(tc, moving_down);
  tcase_add_test(tc, moving_action);
  tcase_add_test(tc, moving_up);
  tcase_add_test(tc, moving_pause);
  tcase_add_test(tc, moving_terminate);
  tcase_add_test(tc, attaching_1);
  tcase_add_test(tc, gameover_1);
  tcase_add_test(tc, score_0_lines);
  tcase_add_test(tc, score_1_line);
  tcase_add_test(tc, score_2_lines);
  tcase_add_test(tc, score_3_lines);
  tcase_add_test(tc, score_4_lines);
  tcase_add_test(tc, level_up);
  tcase_add_test(tc, level_cap_max);
  tcase_add_test(tc, high_score_update);
  tcase_add_test(tc, status_initial);
  tcase_add_test(tc, userinput_default);

  suite_add_tcase(s, tc);
  return s;
}

int main() {
  Suite *s = tetris_suite();
  SRunner *sr = srunner_create(s);

  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_VERBOSE);

  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return failed == 0 ? 0 : 1;
}
