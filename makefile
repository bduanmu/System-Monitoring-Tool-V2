# Compiler
CC = gcc
# Compiler Flags
CFLAGS = -Wall -Werror -std=c99 -g
# Output executable
TARGET = system-monitoring-tool
# Source files
SRC = memory-info.c cpu-info.c cores-info.c output-utils.c system-monitoring-tool.c
# Object files
OBJ = $(SRC:.c=.o)
# Header files
DEPS = memory-info.h cpu-info.h cores-info.h output-utils.h

# Default rule to build the executable
all: $(TARGET)

# Compile object files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

# Clean rule to remove compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean