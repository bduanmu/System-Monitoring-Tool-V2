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

void outputMemoryUtilization(double total_memory, double used_memory, int current_sample, int total_samples, int start_row);
///_|> descry: this function outputs the memory utilization graph
///_|> total_memory: the total memory of the machine in GiB, type double
///_|> used_memory: the amount of memory in use in GiB, type double
///_|> current_sample: the current sample number, type int
///_|> total_samples: the total number of samples, type int
///_|> start_row: the row to start printing the graph, type int
///_|> returning: this function does not return anything

void outputCPUUtilization(double precent_usage, int current_sample, int total_samples, int start_row);
///_|> descry: this function outputs the CPU utilization graph
///_|> percent_usage: the percent of the CPU in use, type double
///_|> current_sample: the current sample number, type int
///_|> total_samples: the total number of samples, type int
///_|> start_row: the row to start printing the graph, type int
///_|> returning: this function does not return anything

void outputCores(long num_cores, long max_freq, int start_row);
///_|> descry: this function outputs the cores info
///_|> num_cores: the number of cores, type long
///_|> max_freq: the maximum frequency of each core in kHz, type long
///_|> start_row: the row to start printing the info, type int
///_|> returning: this function does not return anything

#endif