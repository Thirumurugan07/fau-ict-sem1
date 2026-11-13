#define _POSIX_C_SOURCE 199309L
#include "jacobi.h"

void jacobi(const double * restrict grid_source, double * restrict grid_target, int64_t x, int64_t y) {
	for (int64_t row = 1; row < y - 1; row++) {
		for (int64_t column = 1; column < x - 1; column++) {
			grid_target[row * x + column] =
				(grid_source[(row - 1) * x + column] +
				 grid_source[(row + 1) * x + column] +
				 grid_source[row * x + column - 1] +
				 grid_source[row * x + column + 1]) / 4.0;
		}
	}
}
