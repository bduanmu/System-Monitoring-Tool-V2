#ifndef CPU_INFO_H
#define CPU_INFO_H

void retrieveCPUData(long long cpu_usage[10]);
///_|> descry: this function retrieves the CPU data
///_|> cpu_usage: will store the CPU times in the following order:
///_|> [user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice], type array of long longs
///_|> returning: this function does not return anything

double processCPUUtilization(long long previous_cpu_usage[10], long long current_cpu_usage[10]);
///_|> descry: this function processes the CPU usage
///_|> previous_cpu_usage: the previous CPU times, type array of long longs
///_|> previous_cpu_usage: the current CPU times, type array of long longs
///_|> returning: this function returns the percent CPU usage

#endif