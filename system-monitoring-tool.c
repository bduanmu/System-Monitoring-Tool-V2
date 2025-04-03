#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

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
    if (arguments[1] < 10000) arguments[1] = STANDARD_TDELAY;

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

void writeMemoryData(int fd, int samples, int tdelay) {
    long memory_info[2];
    for (int i = 0; i < samples; i++) {
        retrieveMemoryData(memory_info);
        write(fd, memory_info, sizeof(memory_info));
        delay(tdelay);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}

void writeCPUData(int fd, int samples, int tdelay) {
    long long previous_cpu_usage[10];
    long long current_cpu_usage[10];
    retrieveCPUData(current_cpu_usage);
    delay(tdelay);
    for (int i = 0; i < samples; i++) {
        memcpy(previous_cpu_usage, current_cpu_usage, sizeof(previous_cpu_usage));
        retrieveCPUData(current_cpu_usage);
        double utilization = -1;
        if (current_cpu_usage[0] != -1) {
            utilization = processCPUUtilization(previous_cpu_usage, current_cpu_usage);
        }
        write(fd, &utilization, sizeof(utilization));
        delay(tdelay);
    }
    close(fd);
    exit(EXIT_SUCCESS);
}

void writeCoresData(int fd) {
    long cores_info[2];
    retrieveCoresData(cores_info);
    write(fd, cores_info, sizeof(cores_info));
    close(fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    // Disable output buffering.
    setbuf(stdout, NULL);

    // Ignore CTRL + Z
    signal(SIGTSTP, SIG_IGN);

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
    printf("Number of samples: %d -- every %d microSecs (%.3f secs)", samples, tdelay, tdelay / (float)(METRIC_CONVERSION * METRIC_CONVERSION));

    int memory_pipe[2], cpu_pipe[2], cores_pipe[2];
    pid_t memory_pid = -1, cpu_pid = -1, cores_pid = -1;
    
    if (show_memory) {
        pipe(memory_pipe);
        if ((memory_pid = fork()) == 0) {
            close(memory_pipe[0]);
            writeMemoryData(memory_pipe[1], samples, tdelay);
        }
        close(memory_pipe[1]);
    }
    if (show_cpu) {
        pipe(cpu_pipe);
        if ((cpu_pid = fork()) == 0) {
            close(cpu_pipe[0]);
            writeCPUData(cpu_pipe[1], samples, tdelay);
        }
        close(cpu_pipe[1]);
    }
    if (show_cores) {
        pipe(cores_pipe);
        if ((cores_pid = fork()) == 0) {
            close(cores_pipe[0]);
            writeCoresData(cores_pipe[1]);
        }
        close(cores_pipe[1]);
    }

    if (show_memory || show_cpu) {
        for (int i = 0; i < samples; i++) {
            if (show_memory) {
                long memory_info[2];
                read(memory_pipe[0], memory_info, sizeof(memory_info));

                // If unable to retrieve data, exit unsuccessfully.
                if (memory_info[0] == -1 && memory_info[1] == -1) {
                    fprintf(stderr, MOVE_CURSOR, FIRST_SECTION_START_ROW, 0);
                    fprintf(stderr, "Error retrieving memory data.\n");
                } else {
                    double total_memory = 0;
                    double used_memory = 0;
                    processMemoryUtilization(memory_info, &total_memory, &used_memory);
                    outputMemoryUtilization(total_memory, used_memory, i, samples, FIRST_SECTION_START_ROW);
                }
            }
            if (show_cpu) {
                double utilization;
                read(cpu_pipe[0], &utilization, sizeof(utilization));

                // If unable to retrieve data, exit unsuccessfully.
                if (utilization == -1) {
                    fprintf(stderr, MOVE_CURSOR, show_memory ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW, 0);
                    fprintf(stderr, "Error retrieving CPU data.\n");
                } else {
                    outputCPUUtilization(utilization, i, samples, show_memory ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW);
                }
            }
            
            printf(MOVE_CURSOR, 999, 0);
            delay(tdelay);
        }
    }
    
    if (show_cores) {
        long cores_info[2];
        read(cores_pipe[0], cores_info, sizeof(cores_info));
        int cores_start_row = show_memory || show_cpu ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW;
        if (show_cpu && show_memory) cores_start_row = THIRD_SECTION_START_ROW;

        // If unable to retrieve data, exit unsuccessfully.
        if (cores_info[0] == -1 && cores_info[1] == -1) {
            fprintf(stderr, MOVE_CURSOR, cores_start_row, 0);
            fprintf(stderr, "Error retrieving cores data.\n");
        } else {
            outputCores(cores_info[0], cores_info[1], cores_start_row);
        }
    }

    if (show_memory) {
        close(memory_pipe[0]);
        waitpid(memory_pid, NULL, 0);
    }
    if (show_cpu) {
        close(cpu_pipe[0]);
        waitpid(cpu_pid, NULL, 0);
    }
    if (show_cores) { 
        close(cores_pipe[0]);
        waitpid(cores_pid, NULL, 0);
    }

    printf(MOVE_CURSOR, 999, 0);
    exit(EXIT_SUCCESS);
}

