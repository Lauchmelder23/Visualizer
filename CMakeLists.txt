# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Visualizer")

find_package(glfw3)
if(NOT GLFW3_FOUND)
	message(STATUS "Could not find GLFW binaries on system, building from source instead")
	add_subdirectory("vendor/glfw")

	set(GLFW3_INCLUDE_DIRS glfw)
	set(GLFW3_LIBRARIES glfw)
endif()

# Include sub-projects.
add_subdirectory ("src")
