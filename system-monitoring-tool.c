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

void sigint() {
    ///_|> descry: this function handles the CTRL + C signal
    ///_|> returning: this function does not return anything

    // Buffer for user input
    char input[100];

    // Prompt user to quit the program or not
    printf(MOVE_CURSOR, THIRD_SECTION_START_ROW, 0);
    printf("Would you like to quit [y/n]? ");

    // Get user input
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // Remove newline if present
        input[strcspn(input, "\n")] = 0;

        // If user wants to quit, exit
        if (strcmp(input, "y") == 0) exit(EXIT_SUCCESS);
    }

    // Otherwise, ignore the signal
    printf(MOVE_CURSOR, THIRD_SECTION_START_ROW, 0);
    printf(CLEAR_LINE);
}

void processArguments(int argc, char* argv[], int arguments[5]) {
    ///_|> descry: this function processes command line arguments
    ///_|> argc: the number of command line arguments, type int
    ///_|> argv: the arguments, type array of strings
    ///_|> arguments: will store the processed arguments in a list in the
    ///_|> following order: [samples, tdelay, memory, cpu, cores], type array of ints
    ///_|> returning: this function does not return anything

    // Standard values of samples and tdelay are 20 and 500000 respectively
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

    // If samples or delay are invalid, set to default value
    if (arguments[0] <= 0) arguments[0] = STANDARD_SAMPLES;
    if (arguments[1] < 10000) arguments[1] = STANDARD_TDELAY;

    // If neither memory, CPU, nor cores are specified, show all three
    if (!arguments[2] && !arguments[3] && !arguments[4]) {
        arguments[2] = true;
        arguments[3] = true;
        arguments[4] = true;
    }
}

void delay(unsigned int microseconds) {
    ///_|> descry: this function delays the program
    ///_|> microseconds: indicates how long the delay is in microseconds, type unsigned int
    ///_|> returning: this function does not return anything

    struct timespec start, end;
    long elapsed;

    // Get current time
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Loop until enough time has passed
    do {
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed = (end.tv_sec - start.tv_sec) * (long)(METRIC_CONVERSION * METRIC_CONVERSION)
            + (end.tv_nsec - start.tv_nsec) / (long)METRIC_CONVERSION;
    } while (elapsed < microseconds);
}

void writeMemoryData(int fd, int samples, int tdelay) {
    ///_|> descry: this function writes memory data to a pipe
    ///_|> fd: the pipe to write to, type int
    ///_|> samples: the number of samples of data, type int
    ///_|> tdelay: the delay between each sample, type int
    ///_|> returning: this function does not return anything

    // memory_info stores [total_ram, free_ram]
    long memory_info[2];
    
    // Get the required number of samples
    for (int i = 0; i < samples; i++) {
        // Get the memory data
        retrieveMemoryData(memory_info);
        
        // Write to pipe
        write(fd, memory_info, sizeof(memory_info));
        delay(tdelay);
    }

    // Close the pipe and exit
    close(fd);
    exit(EXIT_SUCCESS);
}

void writeCPUData(int fd, int samples, int tdelay) {
    ///_|> descry: this function writes CPU data to a pipe
    ///_|> fd: the pipe to write to, type int
    ///_|> samples: the number of samples of data, type int
    ///_|> tdelay: the delay between each sample, type int
    ///_|> returning: this function does not return anything

    // previous_cpu_usage and current_cpu_usage stores CPU times in the following order: 
    // [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice]
    long long previous_cpu_usage[10];
    long long current_cpu_usage[10];

    // Get initial data point
    retrieveCPUData(current_cpu_usage);
    delay(tdelay);

    // Get the required number of samples
    for (int i = 0; i < samples; i++) {
        // Update previous CPU data and retrieve the new data
        memcpy(previous_cpu_usage, current_cpu_usage, sizeof(previous_cpu_usage));
        retrieveCPUData(current_cpu_usage);

        // Find the utilization, set as -1 if unable to retrieve data
        double utilization = -1;
        if (current_cpu_usage[0] != -1) {
            utilization = processCPUUtilization(previous_cpu_usage, current_cpu_usage);
        }

        // Write to pipe
        write(fd, &utilization, sizeof(utilization));
        delay(tdelay);
    }

    // Close the pipe and exit
    close(fd);
    exit(EXIT_SUCCESS);
}

void writeCoresData(int fd) {
    ///_|> descry: this function writes cores data to a pipe
    ///_|> fd: the pipe to write to, type int
    ///_|> returning: this function does not return anything

    // cores_info contains the number of cores and the maximum frequency in that order.
    long cores_info[2];
    
    // Get the cores data
    retrieveCoresData(cores_info);

    // Write to pipe, close the pipe and exit
    write(fd, cores_info, sizeof(cores_info));
    close(fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    ///_|> descry: this is the main function
    ///_|> argc: the number of command line arguments, type int
    ///_|> argv: the arguments, type array of strings
    ///_|> returning: this function returns the exit code, type int

    // Disable output buffering
    setbuf(stdout, NULL);

    // Ignore CTRL + Z
    signal(SIGTSTP, SIG_IGN);

    // arguments holds the specifications of the program in a list in the following order: [samples, tdelay, memory, cpu, cores]
    // memory, cpu, and cores are boolean values
    int arguments[5];
    processArguments(argc, argv, arguments);
    int samples = arguments[0];
    int tdelay = arguments[1];
    int show_memory = arguments[2];
    int show_cpu = arguments[3];
    int show_cores = arguments[4];

    // Output number of samples and the delay between each sample
    printf(CLEAR);
    printf(MOVE_CURSOR_TOP_LEFT);
    printf("Number of samples: %d -- every %d microSecs (%.3f secs)", samples, tdelay, tdelay / (float)(METRIC_CONVERSION * METRIC_CONVERSION));

    // Pipes
    int memory_pipe[2];
    int cpu_pipe[2];
    int cores_pipe[2];

    // Children PIDs
    pid_t memory_pid = -1;
    pid_t cpu_pid = -1;
    pid_t cores_pid = -1;
    
    // Create children
    if (show_memory) {
        pipe(memory_pipe);
        if ((memory_pid = fork()) == 0) {
            // Children should ignore CTRL + C
            signal(SIGINT, SIG_IGN);

            close(memory_pipe[0]);

            // Retrieve data
            writeMemoryData(memory_pipe[1], samples, tdelay);
        }
        close(memory_pipe[1]);
    }
    if (show_cpu) {
        pipe(cpu_pipe);
        if ((cpu_pid = fork()) == 0) {
            // Children should ignore CTRL + C
            signal(SIGINT, SIG_IGN);

            close(cpu_pipe[0]);

            // Retrieve data
            writeCPUData(cpu_pipe[1], samples, tdelay);
        }
        close(cpu_pipe[1]);
    }
    if (show_cores) {
        pipe(cores_pipe);
        if ((cores_pid = fork()) == 0) {
            // Children should ignore CTRL + C
            signal(SIGINT, SIG_IGN);

            close(cores_pipe[0]);

            // Retrieve data
            writeCoresData(cores_pipe[1]);
        }
        close(cores_pipe[1]);
    }

    // Handle CTRL + C
    struct sigaction sa;
    sa.sa_handler = sigint;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error with signal handler. ");
        exit(EXIT_FAILURE);
    }

    // Main loop
    if (show_memory || show_cpu) {
        for (int i = 0; i < samples; i++) {
            // Output data
            if (show_memory) {
                // Get data from child
                long memory_info[2];
                read(memory_pipe[0], memory_info, sizeof(memory_info));

                // If unable to retrieve data, exit unsuccessfully
                if (memory_info[0] == -1 && memory_info[1] == -1) {
                    fprintf(stderr, MOVE_CURSOR, FIRST_SECTION_START_ROW, 0);
                    fprintf(stderr, "Error retrieving memory data.\n");
                } else {
                    // Otherwise, print memory utilization
                    double total_memory = 0;
                    double used_memory = 0;
                    processMemoryUtilization(memory_info, &total_memory, &used_memory);
                    outputMemoryUtilization(total_memory, used_memory, i, samples, FIRST_SECTION_START_ROW);
                }
            }
            if (show_cpu) {
                // Get data from child
                double utilization;
                read(cpu_pipe[0], &utilization, sizeof(utilization));

                // If unable to retrieve data, exit unsuccessfully
                if (utilization == -1) {
                    fprintf(stderr, MOVE_CURSOR, show_memory ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW, 0);
                    fprintf(stderr, "Error retrieving CPU data.\n");
                } else {
                    // Otherwise, print CPU utilization
                    outputCPUUtilization(utilization, i, samples, show_memory ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW);
                }
            }
            
            // Move cursor to bottom left
            printf(MOVE_CURSOR, 999, 0);

            // Wait for the desired amount of time
            delay(tdelay);
        }
    }
    
    if (show_cores) {
        // Get data from child
        long cores_info[2];
        read(cores_pipe[0], cores_info, sizeof(cores_info));

        // Setting the start row
        int cores_start_row = show_memory || show_cpu ? SECOND_SECTION_START_ROW : FIRST_SECTION_START_ROW;
        if (show_cpu && show_memory) cores_start_row = THIRD_SECTION_START_ROW;

        // If unable to retrieve data, exit unsuccessfully
        if (cores_info[0] == -1 && cores_info[1] == -1) {
            fprintf(stderr, MOVE_CURSOR, cores_start_row, 0);
            fprintf(stderr, "Error retrieving cores data.\n");
        } else {
            // Otherwise, print the cores info
            outputCores(cores_info[0], cores_info[1], cores_start_row);
        }
    }

    // Close pipes and wait for children
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

    // Move cursor to bottom left
    printf(MOVE_CURSOR, 999, 0);

    // Exit successfully
    exit(EXIT_SUCCESS);
}

