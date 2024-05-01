cmake_minimum_required(VERSION 3.0)
project(my_project)

# Compiler
set(CMAKE_C_COMPILER gcc)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99")

# Include directories
include_directories(include)

# Source files
set(SERVER_SRCS
    server_test.c
    server.c
    hashmap.c
    request.c
    response.c
    queue.c
    common.c
)

# Object files for server
set(SERVER_OBJS "")
foreach(src ${SERVER_SRCS})
    get_filename_component(obj_name obj/${src} NAME_WE)
    list(APPEND SERVER_OBJS ${obj_name}.o)
endforeach()

# Executable names
set(SERVER_TARGET bin/server)

# Compile the source files into object files for server
foreach(src ${SERVER_SRCS})
    get_filename_component(obj_name obj/${src} NAME_WE)
    add_executable(${obj_name} ${src})
    set_target_properties(${obj_name} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -I./include")
    target_link_libraries(${obj_name} PUBLIC m)  # Link any necessary libraries
endforeach()

# Link the object files into the executables
add_custom_target(server)
foreach(obj ${SERVER_OBJS})
    add_dependencies(server ${obj})
endforeach()

# Create directories if they don't exist
file(MAKE_DIRECTORY obj bin)

# Clean up intermediate files and the executables
add_custom_target(clean_all)
add_custom_command(TARGET clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory obj bin
)
