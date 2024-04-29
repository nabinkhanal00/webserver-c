# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Include directories
INCLUDES = -I./include

SERVER_SRCS = server_test.c server.c hashmap.c request.c response.c queue.c common.c

# Object files for client and server
SERVER_OBJS = $(SERVER_SRCS:%.c=obj/%.o)

# Executable names
SERVER_TARGET = bin/server

# Phony targets
.PHONY: all server clean

# Default target
all: server

# Compile the source files into object files for server
server: $(SERVER_TARGET)

# Compile rule for client and server source files
obj/%.o: %.c | obj
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link the object files into the executables
$(SERVER_TARGET): $(SERVER_OBJS) | bin
	$(CC) $(SERVER_OBJS) -o $(SERVER_TARGET)

# Create obj directory if it doesn't exist
obj:
	mkdir -p obj

# Create bin directory if it doesn't exist
bin:
	mkdir -p bin

# Clean up intermediate files and the executables
clean:
	rm -rf obj bin
