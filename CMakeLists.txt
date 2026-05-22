# cmake_minimum_required(VERSION 3.10)
# project(main)

# # Set the CMake generator to Ninja
# set(CMAKE_GENERATOR "Ninja")

# # Set the C compiler to the one provided by MSVC Build Tools
# set(CMAKE_C_COMPILER "cl.exe")

# # Set the C++ compiler to the one provided by MSVC Build Tools
# set(CMAKE_CXX_COMPILER "cl.exe")

# # Set the C++ standard to C++20
# # set(CMAKE_CXX_STANDARD 17)

# # Set the include directory for Raylib
# set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/include")

# # Set the library directory for Raylib
# set(RAYLIB_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib")

# # Add the Raylib libraries to the link directories
# link_directories(${RAYLIB_LIBRARY_DIR})

# # Add the Raylib include directory to the include path
# include_directories(${RAYLIB_INCLUDE_DIR})

# # Add the executable
# add_executable(${PROJECT_NAME} src/main.cpp)

# # Link the Raylib libraries to the executable
# target_link_libraries(${PROJECT_NAME} raylib.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib)




cmake_minimum_required(VERSION 3.16)
project(main VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the include directory for Raylib
set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/include")

# Set the library directory for Raylib
set(RAYLIB_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib")

# Add the Raylib libraries to the link directories
link_directories(${RAYLIB_LIBRARY_DIR})

# Add the Raylib include directory to the include path
include_directories(${RAYLIB_INCLUDE_DIR})

# Collect sources (auto-updates when files added; requires CMake >= 3.12)
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

# Include your bundled raylib headers
# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/raylib/include
# )

# Option A: Link a prebuilt raylib lib (Windows .lib)
# Provide full path or use generator expressions if you have multiple configurations.
# Example (adjust filename):
target_link_libraries(${PROJECT_NAME} PRIVATE raylib.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib)

# Option B: Build/link raylib from source (preferred if you have the raylib CMake project)
# add_subdirectory(raylib)
# target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Recommended: use modern target properties for compile options
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)