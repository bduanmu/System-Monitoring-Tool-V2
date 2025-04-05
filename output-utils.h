#ifndef OUTPUT_UTILS_H
#define OUTPUT_UTILS_H

// ESCape codes
#define CLEAR "\033[2J"
#define CLEAR_LINE "\x1b[2K"
#define MOVE_CURSOR_TOP_LEFT "\033[H"
#define MOVE_CURSOR "\x1b[%d;%df"

// Formatting
#define THIRD_SECTION_START_ROW SECOND_SECTION_START_ROW + CPU_GRAPH_HEIGHT + SPACER
#define SECOND_SECTION_START_ROW FIRST_SECTION_START_ROW + MEM_GRAPH_HEIGHT + SPACER
#define FIRST_SECTION_START_ROW SPACER
#define MEM_GRAPH_HEIGHT 12
#define CPU_GRAPH_HEIGHT 10
#define SPACER 3

// Units
#define METRIC_CONVERSION 1000

// Prints the formatted memory utilization graph.
void outputMemoryUtilization(double total_memory, double used_memory, int current_sample, int total_samples, int start_row);

// Prints the formatted CPU utilization graph.
void outputCPUUtilization(double precent_usage, int current_sample, int total_samples, int start_row);

// Prints the formatted core info.
void outputCores(long num_cores, long max_freq, int start_row);

#endif