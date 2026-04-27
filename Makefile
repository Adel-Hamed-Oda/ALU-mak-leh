# 1. Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g -I.

# 2. Automatically find main.c and all .c files in subfolders
# (This assumes you renamed your folders to have no spaces!)
SRCS = main.c $(wildcard */*.c)

# 3. Name of the final output program
TARGET = OS_Project.exe

# 4. The default rule (what happens when you just type 'make')
all: $(TARGET)

# 5. How to build the target
# WARNING: The space before $(CC) MUST be a single TAB character, NOT spaces!
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# 6. A rule to clean up compiled files
# WARNING: The space before rm MUST be a single TAB character!
clean:
	rm -f $(TARGET)