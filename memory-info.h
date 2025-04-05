#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#define BYTES_CONVERSION 1024

void retrieveMemoryData(long memory_info[2]);
///_|> descry: this function retrieves memory data
///_|> memory_info: will store the memory data in bytes in the
///_|> following order: [total_ram, free_ram], type array of longs
///_|> returning: this function does not return anything

void processMemoryUtilization(long memory_info[2], double* total_memory, double* used_memory);
///_|> descry: this function processes memory utilization
///_|> memory_info: the memory data in bytes, type array of longs
///_|> total_memory: the pointer to the total memory of the machine in GiB, type double pointer
///_|> used_memory: the pointer to the amount of memory in use in GiB, type double pointer
///_|> returning: this function does not return anything

#endif