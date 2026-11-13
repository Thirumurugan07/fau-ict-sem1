#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>

/* Draws a grid of dimension x and y with double values between [0.0 ; 1.0] as a .ppm file.
 * The PPM file standard is described at https://en.wikipedia.org/wiki/Netpbm#File_formats
 * Ensure to limit the maximum drawable grid size.
 * Very large ppm files should not be created to ensure not filling up the disk space by accident.
 */
void draw_grid(const double* grid, int64_t x, int64_t y, const char* filepath);

#endif
