# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Visualizer")

set(CMAKE_CXX_STANDARD 17)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

find_package(GLFW3)
if(NOT GLFW3_FOUND)
	message(STATUS "Could not find GLFW binaries on system, building from source instead")
	add_subdirectory("vendor/glfw")

	set(GLFW3_INCLUDE_DIRS glfw)
	set(GLFW3_LIBRARIES glfw)
endif()

find_package(SDL2)
if(NOT SDL2_FOUND)
	message(STATUS "Could not find SDL binaries on system, building from source instead")
	add_subdirectory("vendor/sdl")

	set(SDL2_INCLUDE_DIRS sdl_static)
	set(SDL2_LIBRARIES sdl_static)
endif()

# Include sub-projects.
add_subdirectory ("vendor/lol")
add_subdirectory ("src")
