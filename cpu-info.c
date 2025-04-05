#include "cpu-info.h"

#include <stdio.h>
#include <stdlib.h>

void retrieveCPUData(long long cpu_usage[10]) {
    ///_|> descry: this function retrieves the CPU data
    ///_|> cpu_usage: will store the CPU times in the following order:
    ///_|> [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice], type array of long longs
    ///_|> returning: this function does not return anything

    // Retrieving CPU times info.
    FILE* fcpu_usage = fopen("/proc/stat", "r");
    // If unable to open file, return.
    if (!fcpu_usage) {
        for (int i = 0; i < 10; i++) cpu_usage[i] = -1;
        return;
    }

    // Getting CPU times from /proc/stat. First line is always "cpu" followed by the respective times.
    fscanf(fcpu_usage, "cpu  %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", 
        &cpu_usage[0], &cpu_usage[1], &cpu_usage[2], &cpu_usage[3], &cpu_usage[4], 
        &cpu_usage[5], &cpu_usage[6], &cpu_usage[7], &cpu_usage[8], &cpu_usage[9]);

    fclose(fcpu_usage);
}

double processCPUUtilization(long long previous_cpu_usage[10], long long current_cpu_usage[10]) {
    ///_|> descry: this function processes the CPU usage
    ///_|> previous_cpu_usage: the previous CPU times, type array of long longs
    ///_|> previous_cpu_usage: the current CPU times, type array of long longs
    ///_|> returning: this function returns the percent CPU usage

    // Finding the difference in total time (user + nice + system + idle + iowait + irq + softirq).
    long long total_time = 0;
    for (int i = 0; i < 7; i++) total_time += current_cpu_usage[i] - previous_cpu_usage[i];

    // Finding the difference in idle time. (idle)
    long long idle_time = current_cpu_usage[3] - previous_cpu_usage[3];

    // Returning CPU usage.
    return 100.0 - ((100.0 * idle_time) / (1.0 * total_time));
}