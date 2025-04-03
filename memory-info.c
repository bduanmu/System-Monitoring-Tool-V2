#include "memory-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

// Retrieves memory data.
// memory_info stores [total_ram, free_ram].
void retrieveMemoryData(long memory_info[2]) {
    // Retrieving memory usage info.
    struct sysinfo system_info;
    // If unable to open file, return.
    if (sysinfo(&system_info) != 0) {
        memory_info[0] = -1;
        memory_info[1] = -1;
        return;
    }

    // Setting the total RAM and free RAM.
    memory_info[0] = system_info.totalram * system_info.mem_unit;
    memory_info[1] = system_info.freeram * system_info.mem_unit;
}

// Processing functions
// Processes the memory usage and sets the values of total_memory and used_memory.
void processMemoryUtilization(long memory_info[2], double* total_memory, double* used_memory) {
    // Converting to GiB and finding the used memory.
    *total_memory = memory_info[0] / (float)(BYTES_CONVERSION * BYTES_CONVERSION * BYTES_CONVERSION);
    *used_memory = (memory_info[0] - memory_info[1]) / (float)(BYTES_CONVERSION * BYTES_CONVERSION * BYTES_CONVERSION);
}