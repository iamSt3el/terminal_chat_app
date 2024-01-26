# CMakeLists.txt

# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.5)

# Set the project name
project(client)

# Add the -Wno-format-security option
add_compile_options(-Wno-format-security)

# Add your source files here
set(SOURCES
    main.cpp
    ui.cpp
    # Add other source files as needed
)

# Find the Boost libraries
find_package(Boost REQUIRED COMPONENTS system filesystem)
# Find the ncurses library
find_package(Curses REQUIRED)

# Check if Boost was found
if(Boost_FOUND)
    message(STATUS "Boost found")
    message(STATUS "Boost version: ${Boost_VERSION}")
    message(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

# Add the executable
add_executable(client ${SOURCES})

# Include the directory containing ui.h
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Include the Boost headers
target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})

# Link against Boost libraries
target_link_libraries(client PRIVATE ${Boost_LIBRARIES})


# Link against the ncurses library
target_link_libraries(client PRIVATE ${CURSES_LIBRARIES})
target_include_directories(client PRIVATE ${CURSES_INCLUDE_DIRS})


