#include "cores-info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void retrieveCoresData(long info[2]) {
    ///_|> descry: this function retrieves the cores data
    ///_|> info: will store the number of cores and the maximum
    ///_|> frequency in that order, type array of longs
    ///_|> returning: this function does not return anything

    // Getting core max frequency data.
    FILE* fcore_freq = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
    // If unable to open file, return.
    if (!fcore_freq) {
        info[0] = -1;
        info[1] = -1;
        return;
    }

    // Getting max core frequency from the file. The file only contains one line with the max frequency in kHz. 
    char core_freq[64];
    fgets(core_freq, 64, fcore_freq);
    info[1] = atoi(core_freq);

    fclose(fcore_freq);


    // Getting number of cores
    FILE* fcpuinfo = fopen("/proc/cpuinfo", "r");
    // If unable to open file, return.
    if (!fcpuinfo) {
        info[0] = -1;
        info[1] = -1;
        return;
    }

    // Searching through the document for a line that starts with "siblings".
    // Once found getting the corresponding number and setting num_cores.
    char line[320];
    while (fgets(line, 320, fcpuinfo)) {
        if (strncmp(line, "siblings", 8) == 0) {
            char* num_cores = line + 11; // Number appears at the 12th character.
            num_cores[strlen(num_cores) - 1] = '\0'; // Removing the \n.

            info[0] = atoi(num_cores);
            break;
        }
    }

    fclose(fcpuinfo);
}