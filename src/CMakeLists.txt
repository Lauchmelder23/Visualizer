add_executable(visualizer
	"main.cpp" "Application.cpp"
 "VertexArrayObject.cpp")

target_sources(visualizer PUBLIC 
	${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
)

target_include_directories(visualizer PUBLIC 
	${GLFW3_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(visualizer PUBLIC ${GLFW3_INCLUDE_DIRS})