#include "output-utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Prints the formatted memory utilization graph.
void outputMemoryUtilization(double total_memory, double used_memory, int current_sample, int total_samples, int start_row) {
    // Move cursor to start of CPU section.
    printf(MOVE_CURSOR, start_row, 0);
    printf("v Memory  %.2f GB         ", used_memory);

    // Printing the axes.
    printf(MOVE_CURSOR, start_row + 1, 2);
    printf("%.0f GB", total_memory);
    printf(MOVE_CURSOR, start_row + MEM_GRAPH_HEIGHT + 1, (int)floor(log10(total_memory)) + 2);
    printf("0 GB ");
    for (int i = 0; i < total_samples + 2; i++) printf("-");
    for (int i = 0; i < MEM_GRAPH_HEIGHT; i++) {
        printf(MOVE_CURSOR, start_row + 1 + i, 7 + (int)floor(log10(total_memory)));
        printf("|");
    }

    // Printing the graph.
    printf(MOVE_CURSOR, start_row + (MEM_GRAPH_HEIGHT + 1) - (int)ceil(MEM_GRAPH_HEIGHT * used_memory / total_memory), 
        7 + (int)floor(log10(total_memory)) + 1 + current_sample);
    printf("#");
}

// Prints the formatted CPU utilization graph.
void outputCPUUtilization(double precent_usage, int current_sample, int total_samples, int start_row) {
    // Move cursor to start of CPU section.
    printf(MOVE_CURSOR, start_row, 0);
    printf("v CPU  %.2f %%              ", precent_usage);

    // Printing the axes.
    printf(MOVE_CURSOR, start_row + 1, 0);
    printf("  100%%");
    printf(MOVE_CURSOR, start_row + CPU_GRAPH_HEIGHT + 1, 0);
    printf("    0%% ");
    for (int i = 0; i < total_samples + 2; i++) printf("-");
    for (int i = 0; i < CPU_GRAPH_HEIGHT; i++) {
        printf(MOVE_CURSOR, start_row + 1 + i, 8);
        printf("|");
    }

    // Printing the graph.
    printf(MOVE_CURSOR, start_row + (CPU_GRAPH_HEIGHT + 1) - (int)ceil(CPU_GRAPH_HEIGHT * precent_usage / 100.0), 9 + current_sample);
    printf(":");
}

// Prints the formatted core info.
void outputCores(long num_cores, long max_freq, int start_row) {
    // Move cursor to start of cores section.
    printf(MOVE_CURSOR, start_row, 0); 
    printf("v Number of Cores: %ld @ %.2f GHz", num_cores, max_freq / (float)(METRIC_CONVERSION * METRIC_CONVERSION));

    // Print a square for each core.
    for (int i = 0; i < num_cores; i++) {
        printf(MOVE_CURSOR, start_row + 1 + 3 * (i / 4), 7 * (i % 4) + 1);
        printf("  +---+");
        printf(MOVE_CURSOR, start_row + 1 + 3 * (i / 4) + 1, 7 * (i % 4) + 1);
        printf("  |   |");
        printf(MOVE_CURSOR, start_row + 1 + 3 * (i / 4) + 2, 7 * (i % 4) + 1);
        printf("  +---+");
    }

    // Print new line.
    printf("\n");
}