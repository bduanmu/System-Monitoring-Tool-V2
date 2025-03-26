#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#define BYTES_CONVERSION 1024

// Retrieves memory data.
// memory_info stores [total_ram, free_ram].
void retrieveMemoryData(long memory_info[2]);

// Processing functions
// Processes the memory usage and sets the values of total_memory and used_memory.
void processMemoryUtilization(long memory_info[2], double* total_memory, double* used_memory);

#endif