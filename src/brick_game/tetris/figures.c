
#include "figures.h"

#include <stdlib.h>
#include <time.h>

static int g_rand_inited = 0;

static void ensure_rand_init(void) {
  if (!g_rand_inited) {
    srand((unsigned int)time(NULL));
    g_rand_inited = 1;
  }
}

void clear_figure(int fig[FIG_SIZE][FIG_SIZE]) {
  for (int r = 0; r < FIG_SIZE; ++r) {
    for (int c = 0; c < FIG_SIZE; ++c) {
      fig[r][c] = 0;
    }
  }
}

void copy_figure(int dst[FIG_SIZE][FIG_SIZE],
                 const int src[FIG_SIZE][FIG_SIZE]) {
  for (int r = 0; r < FIG_SIZE; ++r) {
    for (int c = 0; c < FIG_SIZE; ++c) {
      dst[r][c] = src[r][c];
    }
  }
}

void rotate_figure_cw(int fig[FIG_SIZE][FIG_SIZE]) {
  int tmp[FIG_SIZE][FIG_SIZE];

  for (int r = 0; r < FIG_SIZE; ++r) {
    for (int c = 0; c < FIG_SIZE; ++c) {
      tmp[r][c] = fig[FIG_SIZE - 1 - c][r];
    }
  }

  for (int r = 0; r < FIG_SIZE; ++r) {
    for (int c = 0; c < FIG_SIZE; ++c) {
      fig[r][c] = tmp[r][c];
    }
  }
}

static const int FIGURE_I[FIG_SIZE][FIG_SIZE] = {
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_J[FIG_SIZE][FIG_SIZE] = {
    {2, 0, 0, 0},
    {2, 2, 2, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_L[FIG_SIZE][FIG_SIZE] = {
    {0, 0, 3, 0},
    {3, 3, 3, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_O[FIG_SIZE][FIG_SIZE] = {
    {4, 4, 0, 0},
    {4, 4, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_S[FIG_SIZE][FIG_SIZE] = {
    {0, 5, 5, 0},
    {5, 5, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_T[FIG_SIZE][FIG_SIZE] = {
    {0, 6, 0, 0},
    {6, 6, 6, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static const int FIGURE_Z[FIG_SIZE][FIG_SIZE] = {
    {7, 7, 0, 0},
    {0, 7, 7, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

static void get_figure_by_index(int index, int out[FIG_SIZE][FIG_SIZE]) {
  switch (index) {
    case 0:
      copy_figure(out, FIGURE_I);
      break;
    case 1:
      copy_figure(out, FIGURE_J);
      break;
    case 2:
      copy_figure(out, FIGURE_L);
      break;
    case 3:
      copy_figure(out, FIGURE_O);
      break;
    case 4:
      copy_figure(out, FIGURE_S);
      break;
    case 5:
      copy_figure(out, FIGURE_T);
      break;
    case 6:
    default:
      copy_figure(out, FIGURE_Z);
      break;
  }
}

void generate_random_figure(int out[FIG_SIZE][FIG_SIZE]) {
  ensure_rand_init();
  int idx = rand() % 7;
  get_figure_by_index(idx, out);
}
