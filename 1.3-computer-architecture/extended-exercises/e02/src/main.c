#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <immintrin.h>      // _mm_malloc, _mm_free
#include "jacobi.h"
#include "draw.h"
#include "get_time.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: ./jacobi <edge length>                    (benchmark)\n"
                        "       ./jacobi <edge length> <steps> [out.ppm]   (debug)\n");
        return 1;
    }

    int benchmark = (argc == 2);
    int64_t edge  = atoi(argv[1]);
    int64_t steps = benchmark ? 0 : atoi(argv[2]);
    if (edge < 3 || steps < 0) {
        fprintf(stderr, "Edge length must be at least 3, time steps must not be negative.\n");
        return 1;
    }

    // two linearized grids, aligned to 64 bytes
    size_t cells = (size_t)edge * (size_t)edge;
    double *grid_src  = _mm_malloc(cells * sizeof(double), 64);
    double *grid_trgt = _mm_malloc(cells * sizeof(double), 64);
    if (grid_src == NULL || grid_trgt == NULL) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }

    // left column and top row = 1.0, all other cells = 0.0 (in BOTH grids)
    for (int64_t y = 0; y < edge; y++) {
        for (int64_t x = 0; x < edge; x++) {
            double v = (x == 0 || y == 0) ? 1.0 : 0.0;
            grid_src [y * edge + x] = v;
            grid_trgt[y * edge + x] = v;
        }
    }

    if (benchmark) {
        const uint64_t minimal_runtime_ms = 1000;
        uint64_t actual_runtime_us = 0;
        uint64_t runs;

        // double the number of time steps until one round takes >= 1 s
        for (runs = 1; actual_runtime_us < minimal_runtime_ms * 1000; runs = runs << 1) {
            uint64_t start = get_time_micros();
            for (uint64_t i = 0; i < runs; i++) {
                jacobi(grid_src, grid_trgt, edge, edge);
                double *tmp = grid_src;
                grid_src  = grid_trgt;
                grid_trgt = tmp;
            }
            uint64_t stop = get_time_micros();
            actual_runtime_us = stop - start;
        }
        runs /= 2;   // the for loop doubles runs once more after the last measured round

        // one update = one inner cell, so (edge-2)^2 updates per time step
        double updates = (double)runs * (double)(edge - 2) * (double)(edge - 2);
        double mups = updates / (double)actual_runtime_us;   // updates per microsecond = MUp/s
        uint64_t array_size_bytes = 2 * (uint64_t)cells * sizeof(double);   // both grids

        // ArraySize, MUp/s, ActualRuntime [ms], MinimalRuntime [ms], EdgeLength
        printf("%" PRIu64 ",%lf,%" PRIu64 ",%" PRIu64 ",%" PRId64 "\n",
               array_size_bytes, mups, actual_runtime_us / 1000,
               minimal_runtime_ms, edge);
    } else {
        // debug mode: fixed number of steps
        for (int64_t step = 0; step < steps; step++) {
            jacobi(grid_src, grid_trgt, edge, edge);
            double *tmp = grid_src;
            grid_src  = grid_trgt;
            grid_trgt = tmp;
        }
        if (edge <= 16) {
            for (int64_t y = 0; y < edge; y++) {
                for (int64_t x = 0; x < edge; x++) {
                    printf("%.6f ", grid_src[y * edge + x]);
                }
                printf("\n");
            }
        }
        draw_grid(grid_src, edge, edge, (argc == 4) ? argv[3] : "jacobi.ppm");
    }

    _mm_free(grid_src);
    _mm_free(grid_trgt);
    return 0;
}
