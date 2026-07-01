
#ifndef BRICK_GAME_TETRIS_FIGURES_H_
#define BRICK_GAME_TETRIS_FIGURES_H_

#define FIG_SIZE 4

void clear_figure(int fig[FIG_SIZE][FIG_SIZE]);
void copy_figure(int dst[FIG_SIZE][FIG_SIZE],
                 const int src[FIG_SIZE][FIG_SIZE]);
void rotate_figure_cw(int fig[FIG_SIZE][FIG_SIZE]);
void generate_random_figure(int out[FIG_SIZE][FIG_SIZE]);

#endif  // BRICK_GAME_TETRIS_FIGURES_H_
