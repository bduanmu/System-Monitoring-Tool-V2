#include "memory-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

void retrieveMemoryData(long memory_info[2]) {
    ///_|> descry: this function retrieves memory data
    ///_|> memory_info: will store the memory data in bytes in the
    ///_|> following order: [total_ram, free_ram], type array of longs
    ///_|> returning: this function does not return anything

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

void processMemoryUtilization(long memory_info[2], double* total_memory, double* used_memory) {
    ///_|> descry: this function processes memory utilization
    ///_|> memory_info: the memory data in bytes, type array of longs
    ///_|> total_memory: the pointer to the total memory of the machine in GiB, type double pointer
    ///_|> used_memory: the pointer to the amount of memory in use in GiB, type double pointer
    ///_|> returning: this function does not return anything

    // Converting to GiB and finding the used memory.
    *total_memory = memory_info[0] / (float)(BYTES_CONVERSION * BYTES_CONVERSION * BYTES_CONVERSION);
    *used_memory = (memory_info[0] - memory_info[1]) / (float)(BYTES_CONVERSION * BYTES_CONVERSION * BYTES_CONVERSION);
}