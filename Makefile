# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Include directories
INCLUDES = -I./include

# Source files for client and server
CLIENT_SRCS = client.c common.c
SERVER_SRCS = server.c queue.c common.c

# Object files for client and server
CLIENT_OBJS = $(CLIENT_SRCS:%.c=obj/%.o)
SERVER_OBJS = $(SERVER_SRCS:%.c=obj/%.o)

# Executable names
CLIENT_TARGET = bin/client
SERVER_TARGET = bin/server

# Phony targets
.PHONY: all client server clean

# Default target
all: client server

# Compile the source files into object files for client
client: $(CLIENT_TARGET)

# Compile the source files into object files for server
server: $(SERVER_TARGET)

# Compile rule for client and server source files
obj/%.o: %.c | obj
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link the object files into the executables
$(CLIENT_TARGET): $(CLIENT_OBJS) | bin
	$(CC) $(CLIENT_OBJS) -o $(CLIENT_TARGET)

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
