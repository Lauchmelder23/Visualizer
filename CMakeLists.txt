# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Visualizer")

find_package(GLFW3)
if(NOT GLFW3_FOUND)
	message(STATUS "Could not find GLFW binaries on system, building from source instead")
	add_subdirectory("vendor/glfw")

	set(GLFW3_INCLUDE_DIRS glfw)
	set(GLFW3_LIBRARIES glfw)
endif()

find_package(GLM)
if(NOT GLM_FOUND)
	message(STATUS "Could not find GLM on system, including from source instead")
	add_subdirectory("vendor/glm")

	set(GLM_INCLUDE_DIRS glm)
	set(GLM_LIBRARIES glm)
endif()

# Include sub-projects.
add_subdirectory ("src")
