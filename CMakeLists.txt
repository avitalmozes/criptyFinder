cmake_minimum_required(VERSION 3.0)
project(CriptyFinder)

# Define options
option(DEBUG "Compile with debug flags (-g)" OFF) # Default is OFF
option(RELEASE "Compile in release mode" OFF)    # Default is OFF

# Set default build type to Release if neither DEBUG nor RELEASE option is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

# Debug mode
if(DEBUG)
    message(STATUS "Compiling with debug flags (-g)")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

# Release mode
if(RELEASE)
    message(STATUS "Compiling in release mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif()

# Print build type message
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: Release")
else()
    message(WARNING "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

# Apply flags
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

# Debug Flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall -Wextra")

# Release Flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")


file(COPY ${CMAKE_SOURCE_DIR}/etc/cripty.sig DESTINATION ${CMAKE_BINARY_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES 
    src/main.cpp 
    src/cripty.cpp
    src/thread_pool.cpp
)

# Header files
set(HEADERS
    include/cripty.hpp
    include/thread_pool.hpp
)

# Add executable
add_executable(find_sig ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(find_sig PRIVATE include)

# Link against the pthread library
target_link_libraries(find_sig PRIVATE pthread)
