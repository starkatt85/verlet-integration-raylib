cmake_minimum_required(VERSION 3.10)
project(main)

# Set the CMake generator to Ninja
set(CMAKE_GENERATOR "Ninja")

# Set the C compiler to the one provided by MSVC Build Tools
set(CMAKE_C_COMPILER "cl.exe")

# Set the C++ compiler to the one provided by MSVC Build Tools
set(CMAKE_CXX_COMPILER "cl.exe")

# Set the C++ standard to C++20
# set(CMAKE_CXX_STANDARD 17)

# Set the include directory for Raylib
set(RAYLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/include")

# Set the library directory for Raylib
set(RAYLIB_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib/lib")

# Add the Raylib libraries to the link directories
link_directories(${RAYLIB_LIBRARY_DIR})

# Add the Raylib include directory to the include path
include_directories(${RAYLIB_INCLUDE_DIR})

# Add the executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Link the Raylib libraries to the executable
target_link_libraries(${PROJECT_NAME} raylib.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winmm.lib)