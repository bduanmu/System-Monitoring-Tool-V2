#include "cpu-info.h"

#include <stdio.h>
#include <stdlib.h>

// Retrieves CPU data.
// cpu_usage stores CPU times in the following order: [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice].
void retrieveCPUData(long long cpu_usage[10]) {
    // Retrieving CPU times info.
    FILE* fcpu_usage = fopen("/proc/stat", "r");
    if (!fcpu_usage) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }

    // Getting CPU times from /proc/stat. First line is always "cpu" followed by the respective times.
    fscanf(fcpu_usage, "cpu  %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", 
        &cpu_usage[0], &cpu_usage[1], &cpu_usage[2], &cpu_usage[3], &cpu_usage[4], 
        &cpu_usage[5], &cpu_usage[6], &cpu_usage[7], &cpu_usage[8], &cpu_usage[9]);

    fclose(fcpu_usage);
}

// Processes the CPU usage and returns the percent usage.
double processCPUUtilization(long long previous_cpu_usage[10], long long current_cpu_usage[10]) {
    // Finding the difference in total time (user + nice + system + idle + iowait + irq + softirq).
    long long total_time = 0;
    for (int i = 0; i < 7; i++) total_time += current_cpu_usage[i] - previous_cpu_usage[i];

    // Finding the difference in idle time. (idle)
    long long idle_time = current_cpu_usage[3] - previous_cpu_usage[3];

    // Returning CPU usage.
    return 100.0 - ((100.0 * idle_time) / (1.0 * total_time));
}