cmake_minimum_required(VERSION 3.10)

# =============================================================================
# Project
# =============================================================================

project(LILIANTESTS LANGUAGES CXX)


# =============================================================================
# C++ standard
# =============================================================================

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# =============================================================================
# Build type
# =============================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()


# =============================================================================
# Shared libraries
# =============================================================================

set(BUILD_SHARED_LIBS ON)


# =============================================================================
# Optional user-specific CMake modules
# =============================================================================

if(DEFINED ENV{CMAKE_MODULE_PATH})
    set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH})
endif()


# =============================================================================
# OpenCV
# =============================================================================

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")


# =============================================================================
# Optional BIAS support
# =============================================================================

option(USE_BIAS "Enable BIAS support" OFF)

if(USE_BIAS)

    include_directories(
        ${BIAS_INCLUDE_DIR}
        /usr/local/include/BIAS
        /usr/include/ImageMagick
        ${WXWIDGETS_INCLUDE_DIR}
    )

    link_directories(
        ${BIAS_LINK_DIRECTORIES}
    )

endif()


# =============================================================================
# Compiler optimization
# =============================================================================

add_compile_options(
    -O3
)


# =============================================================================
# LineMatchingLib source files
# =============================================================================

set(LineMatchingLib_SRCS

    # PairwiseLineMatching.cpp

    LineDescriptor.cpp
    EDLineDetector.cpp
)


# =============================================================================
# LineMatchingLib header files
# =============================================================================

set(LineMatchingLib_HEADERS

    # PairwiseLineMatching.hh

    LineDescriptor.hh
    EDLineDetector.hh
    LineStructure.hh
)


# =============================================================================
# Create LineMatchingLib
# =============================================================================

add_library(
    LineMatchingLib

    ${LineMatchingLib_SRCS}
    ${LineMatchingLib_HEADERS}
)


# =============================================================================
# Include directories for LineMatchingLib
# =============================================================================

target_include_directories(
    LineMatchingLib

    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)


# =============================================================================
# Link OpenCV to LineMatchingLib
# =============================================================================

target_link_libraries(
    LineMatchingLib

    PUBLIC

    ${OpenCV_LIBS}
)


# =============================================================================
# Test executable
# =============================================================================

add_executable(
    TestLineMatchingAlgorithm

    TestLineMatchingAlgorithm.cpp
)


# =============================================================================
# Link test executable
# =============================================================================

target_link_libraries(
    TestLineMatchingAlgorithm

    PRIVATE

    LineMatchingLib
    ${OpenCV_LIBS}
)
