# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# 1. Dynamically find ALL .c files in root and inside src/ subfolders
SRCS = $(wildcard *.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)

# 2. Dynamically find ALL .h files so we know where your headers are
HDRS = $(wildcard *.h) $(wildcard src/*/*.h) $(wildcard src/*/*/*.h)

# 3. Extract the folder paths from the files we found, and remove duplicates
INCDIRS = $(sort $(dir $(SRCS) $(HDRS)))

# 4. Automatically generate the -I flags for the compiler (e.g., -I./ -Isrc/memory/)
INCFLAGS = $(addprefix -I, $(INCDIRS))

# Output executable name
TARGET = CA_backend.exe

# Default target
all: $(TARGET)

# Compile all source files together into the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCFLAGS) $(SRCS) -o $(TARGET)

# Clean command for Windows
clean:
	del /Q $(TARGET)

.PHONY: all clean