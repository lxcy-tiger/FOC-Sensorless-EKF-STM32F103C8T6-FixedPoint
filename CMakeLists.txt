cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#
# Setup compiler settings
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")
# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name`
set(CMAKE_PROJECT_NAME FOC_SENSORLESS_FIXED)
# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/Inc/fixed_point.h
        Core/Inc/other.h
        Core/Src/other.c
        Core/Src/PMSM_Control_Core/Clarke_Park.c
        Core/Inc/PMSM_Control_Core/Clarke_Park.h
        Core/Inc/PMSM_Control_Core/Hardware.h
        Core/Src/PMSM_Control_Core/Hardware.c
        Core/Inc/PMSM_Control_Core/SVPWM.h
        Core/Src/PMSM_Control_Core/SVPWM.c
        Core/Src/PMSM_Control_Core/PI_Controller.c
        Core/Inc/PMSM_Control_Core/PI_Controller.h
        Core/Inc/PMSM_Control_Core/EKF.h
        Core/Src/PMSM_Control_Core/EKF.c
        Core/Inc/USB_JustFloat.h
        Core/Src/USB_JustFloat.c)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
# 开O2优化
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -O2)
