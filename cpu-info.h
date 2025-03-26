#ifndef CPU_INFO_H
#define CPU_INFO_H

// Retrieves CPU data.
// cpu_usage stores CPU times in the following order: [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice].
void retrieveCPUData(long long cpu_usage[10]);

// Processes the CPU usage and returns the percent usage.
double processCPUUtilization(long long previous_cpu_usage[10], long long current_cpu_usage[10]);

#endif