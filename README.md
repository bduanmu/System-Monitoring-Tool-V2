# CSCB09 Assignment 1 - System Monitoring Tool
This is a program that present system information in real-time in graphical form by displaying at each instant of time how the quantities are changing. This program presents memory usage and CPU usage in the form of a graph, provides a visual representation of the number of cores as well as the maximum frequency of the cores. 

## How to Run/Compile
When compiling, make sure you compile with the `-lm`, as the math library is used. This program compiles with `-Wall` and `-Werror` with no issues, and also compiles with `-std-c99`.

This program can be run with up to five command line arguments: 
* `--memory` to indicate the memory usage graph should be generated.
* `--cpu` to indicate the CPU usage graph should be generated.
* `--cores` to indicate the cores information should be generated.
* `--samples=N` to indicate there will be N repititions meaning data will be sampled N times. If not indicated the default value is 20.
* `--tdelay=T` to indicate between each sample there will be a T microseconds delay. If not indicated the default value is 500000.

The number of samples and delay can also be passed as positional arguments. `samples` can be passed as the first argument and `tdelay` can be passed as the second. 

In the case both the positional arguments and `--samples=N` or `--tdelay-T` or multiple verions of `--samples=N` or `--tdelay-T` are passed in, the last argument will be used. If multiple of the other flags are passed, it will be treated as if there were only one.

The number of samples and the delay should both be positive integers. Should invalid inputs (i.e. 0, -12, "akljf") be passed in, the default values will be used. For the sake of accuracy and formatting, **it is assumed that the number of samples is less than the width of the terminal and the delay is greater than 10000 microseconds.** The program still runs with such inputs, but unexpected results may occur. 

**It is also assumed the user does not resize their terminal while the program is running, as otherwise the graphs will be outputted incorrectly.**

Examples:
* `./system-monitoring-tool`
    * 20 samples with 500000 microseconds delay, showing memory usage, CPU usage, and cores info.
* `./system-monitoring-tool 10 1000000 --cpu` 
    * 10 samples with 1000000 microseconds delay, showing only CPU usage.
* `./system-monitoring-tool --memory --cores --samples=60`
    * 60 samples with 500000 microseconds delay, showing memory usage and cores info.
* `./system-monitoring-tool 10 500001 --tdelay=800000 --samples=99 --samples=16 --cores` 
    * 16 samples with 800000 microseconds delay, showing only cores info.
* `./system-monitoring-tool -tdelay=250000 600000`
    * 20 samples with 600000 microseconds delay, showing memory usage, CPU usage, and cores info.
* `./system-monitoring-tool khsahfakjh 0 od fja ndfjkasn jk aJFK JEN A K a`
    * 20 samples with 500000 microseconds delay, showing memory usage, CPU usage, and cores info.

## Implementation Details
### Processing the Arguments
The `processArguments` function first sets the arguments to the default values. Then, it loops through all command line arguments. If a given argument starts with one of the flags, it updates the corresponding values. If a given argument doesn't match any of the flags but it is the first or second argument, the number of samples and the delay is updated to the argument respectively. After the loop, if samples and/or delay is invalid, it resets it to the default value. If none of memory, CPU, or cores is selected, all are generated. 

### Retrieving Data
Memory data is retrieved by `retrieveMemoryData`. This function uses the `sys/sysinfo.h` library to retrieve the total and free RAM converted to bytes. Should the system fail to retrieve this data, the program exits and prints an error message.

CPU data is retrieved by `retrieveCPUData`. This function uses the `/proc/stat` file. The first line of this file contains the aggregate CPU information from all the CPUs. This line consists of the string `cpu` followed by 10 integers. Each integer represents total time the CPU spent performing different types of work. The 4th and 5th numbers represent idle and iowait work respectively, which in total make up the time the CPU is not in use. Should the system fail to retrieve this data, the program exits and prints an error message.

The cores info is retrieved by `retrieveCoresData`. The number of cores is retrieved from the file `/proc/cpuinfo` by parsing through each line until one is found which starts with the word `siblings`, and the corresponding number is found which represents the number of cores. The maximum frequency is retrieved from the file `/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq`. The first and only line in this file contains a singular integer which represents the maximum frequency of the CPU in kHz. Should the system fail to retrieve this data, the program exits and prints an error message.

### Processing the Data
Memory usage is processed by `processMemoryUtilization`. This function processes the used memory by subtracting free from total memory, and converts it and total memory into GiBs.

CPU usage is processed by `processCPUUtilization`. This function takes in two sets of CPU time data and first calculated the total time between the two sets of data by adding the total time from one and subtracting that from the total time of the other. It calculates the difference in non-use time in the same way but only adding/subtracting the idle and iowait times. It then returns the use time, which is just total time minus non-use time, as a percentage of total time.

### Outputting the Data
The memory and CPU graphs are outputted using `outputMemoryUtilization` and `outputCPUUtilization` respectively. The way they are printed are very similar. The axes are outputted first, starting at the left side of the terminal and at the given starting row. The y-axis is drawn relative to the length of the maximum value, meaning the maximum RAM or 100%. The x-axis' length corresponds to the number of samples. Then, one individual character is outputted representing the memory/CPU usage at the given sample. 

The cores info is outputted using `outputCores`. This prints the maximum frequency and then in rows of four the number of cores represented by squares. This starts at the given start row. 

### Delay
Since `usleep` does not conform to the C99 standard, I opted to write my own delay function using the `time.h` library. This function works by getting the current time on start, and looping continuously while calculating the elapsed time by subtracting the start time from the current time and converting to microseconds. Once the elapsed time is greater than or equal to the time we want to delay the program in microseconds, the loop and the function ends. 

### Main Function
The `main` function first gets and processes the arguments. Then, it clears the terminal and moves the cursor to the top left of the screen and outputs the number of samples and the delay. It then checks if CPU info is being requested. If so, it retrieves an initial sample of the CPU info and delays because to process the CPU info two sets of information is needed. Then, it moves on to the main loop. The loop only runs if either CPU or memory info is being requested, as we only need to retrieve cores info once. If so, it loops for the number of samples and retrieves the necessary data first. Then, it processes it and then finally outputs it and delays. At the end, if cores info is being requested, it will retrieve and output it and finally exits the program. 