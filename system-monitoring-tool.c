#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "memory-info.h"
#include "cpu-info.h"
#include "cores-info.h"
#include "output-utils.h"

// Standard values
#define STANDARD_SAMPLES 20
#define STANDARD_TDELAY 500000

// Processes command line arguments.
// arguments holds the specifications of the program in a list in the following order: [samples, tdelay, memory, cpu, cores].
// memory, cpu, and cores are boolean values.
void processArguments(int argc, char* argv[], int arguments[5]) {
    // Standard values of samples and tdelay are 20 and 500000 respectively.
    arguments[0] = STANDARD_SAMPLES;
    arguments[1] = STANDARD_TDELAY;
    arguments[2] = false;
    arguments[3] = false;
    arguments[4] = false;

    // Checking for flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--memory") == 0) arguments[2] = true;
        else if (strcmp(argv[i], "--cpu") == 0) arguments[3] = true;
        else if (strcmp(argv[i], "--cores") == 0) arguments[4] = true;
        else if (strncmp(argv[i], "--samples=", 10) == 0) arguments[0] = atoi(argv[i] + 10);
        else if (strncmp(argv[i], "--tdelay=", 9) == 0) arguments[1] = atoi(argv[i] + 9);
        else if (i == 1) arguments[0] = atoi(argv[i]);
        else if (i == 2) arguments[1] = atoi(argv[i]);
    }

    // If samples or delay are invalid, set to default value.
    if (arguments[0] <= 0) arguments[0] = STANDARD_SAMPLES;
    if (arguments[1] <= 0) arguments[1] = STANDARD_TDELAY;

    // If neither memory, CPU, nor cores are specified, show all three. 
    if (!arguments[2] && !arguments[3] && !arguments[4]) {
        arguments[2] = true;
        arguments[3] = true;
        arguments[4] = true;
    }
}

// Delays program by microseconds microseconds.
void delay(unsigned int microseconds) {
    struct timespec start, end;
    long elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    do {
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed = (end.tv_sec - start.tv_sec) * (long)(METRIC_CONVERSION * METRIC_CONVERSION)
            + (end.tv_nsec - start.tv_nsec) / (long)METRIC_CONVERSION;
    } while (elapsed < microseconds);
}

int main(int argc, char* argv[]) {
    // Disable output buffering.
    setbuf(stdout, NULL);

    // arguments holds the specifications of the program in a list in the following order: [samples, tdelay, memory, cpu, cores].
    // memory, cpu, and cores are boolean values.
    int arguments[5];
    processArguments(argc, argv, arguments);
    int samples = arguments[0];
    int tdelay = arguments[1];
    int show_memory = arguments[2];
    int show_cpu = arguments[3];
    int show_cores = arguments[4];

    // Output number of samples and the delay between each sample.
    printf(CLEAR);
    printf(MOVE_CURSOR_TOP_LEFT);
    printf("Nbr of samples: %d -- every %d microSecs (%.3f secs)", samples, tdelay, tdelay / (float)(METRIC_CONVERSION * METRIC_CONVERSION));


    // memory_info stores [total_ram, free_ram, shared_ram, buffer_ram].
    long memory_info[2];

    // previous_cpu_usage and current_cpu_usage stores CPU times in the following order: 
    // [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice].
    long long previous_cpu_usage[10];
    long long current_cpu_usage[10];


    if (show_cpu) {
        // Retrieve initial CPU data.
        retrieveCPUData(current_cpu_usage);

        // Wait tdelay microseconds.
        delay(tdelay);
    }

    // Main loop
    if (show_cpu || show_memory) {
        for (int i = 0; i < samples; i++) {
            // Retrieve data.
            if (show_memory) retrieveMemoryData(memory_info);
            if (show_cpu) {
                // Update previous CPU data and retrieve the new data.
                memcpy(previous_cpu_usage, current_cpu_usage, 10 * sizeof(current_cpu_usage[0]));
                retrieveCPUData(current_cpu_usage);
            }


            // Output data.
            if (show_memory) {
                // Print memory utilization.
                double total_memory = 0;
                double used_memory = 0;
                processMemoryUtilization(memory_info, &total_memory, &used_memory);
                outputMemoryUtilization(total_memory, used_memory, i, samples, FIRST_SECTION_START_ROW);
            }

            if (show_cpu) {
                // Print CPU utilization.
                outputCPUUtilization(processCPUUtilization(previous_cpu_usage, current_cpu_usage), i, samples, show_memory ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW);
            }

            // Waits tdelay microseconds.
            delay(tdelay);
        }
    }

    
    if (show_cores) {
        // cores_info contains the number of cores and the maximum frequency in that order.
        long cores_info[2];
        retrieveCoresData(cores_info);

        // Output cores and frequency.
        int cores_start_row = show_memory || show_cpu ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW;
        if (show_cpu && show_memory) cores_start_row = THIRD_SECTION_START_ROW;
        outputCores(cores_info[0], cores_info[1], cores_start_row);
    }
    

    // Move cursor to bottom left.
    printf(MOVE_CURSOR, 999, 0);
    exit(EXIT_SUCCESS);
}

