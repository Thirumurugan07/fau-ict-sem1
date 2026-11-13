#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include "draw.h"

// If you want you can use the provided struct and static function

typedef struct COLOR_s {
    unsigned char r; // red channel
    unsigned char g; // green channel
    unsigned char b; // blue channel
} COLOR;

static COLOR color_converter(double value) {
    COLOR c;
    if (value < 0.0) {
        value = 0.0;
    } else if (value > 1.0) {
        value = 1.0;
    }

    if (value <= 0.5) {
        c.r = 0;
    } else if (value < 0.75) {
        c.r = (unsigned char)(255.0 * (value - 0.5) / 0.25);
    } else {
        c.r = 255;
    }

    if (value < 0.25) {
        c.g = (unsigned char)(255.0 * value / 0.25);
    } else if (value <= 0.75) {
        c.g = 255;
    } else {
        c.g = (unsigned char)(255.0 * (1.0 - value) / 0.25);
    }

    if (value <= 0.25) {
        c.b = 255;
    } else if (value < 0.5) {
        c.b = (unsigned char)(255.0 * (0.5 - value) / 0.25);
    } else {
        c.b = 0;
    }

    return c;
}



void draw_grid(const double* grid, int64_t x, int64_t y, const char* filepath) {
    // Prevent ppm creation if grid is too large. 
    // During this course we will work on grids as large as 64 GiBs which should not be drawn.
    // This draw grid function should only be used on small arrays to verify the correctness of the code.
    if (x <= 0 || y <= 0 || x > 1000000 / y) {
        fprintf(stderr, "draw_grid(): grid exceeds size limitation -> skipping .ppm file creation\n");
        return;
    }

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("draw_grid(): fopen");
        return;
    }

    int failed = fprintf(file, "P3 %lld %lld 255\n",
                        (long long)x, (long long)y) < 0;
    for (int64_t row = 0; row < y && !failed; row++) {
        for (int64_t column = 0; column < x; column++) {
            COLOR color = color_converter(grid[row * x + column]);
            if (fprintf(file, "%u %u %u%c",
                        (unsigned int)color.r,
                        (unsigned int)color.g,
                        (unsigned int)color.b,
                        column == x - 1 ? '\n' : ' ') < 0) {
                failed = 1;
                break;
            }
        }
    }

    if (fclose(file) != 0) {
        failed = 1;
    }
    if (failed) {
        fprintf(stderr, "draw_grid(): failed to write %s\n", filepath);
    }
}
