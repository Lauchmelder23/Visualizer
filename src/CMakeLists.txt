add_executable(visualizer
	"main.cpp" "Application.cpp"
 "backend/VertexArrayObject.cpp" "backend/Shader.cpp"  "Cuboid.cpp" "backend/Drawable.cpp" "backend/Transformable.cpp")

target_sources(visualizer PUBLIC 
	${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
)

target_include_directories(visualizer PRIVATE 
	${GLFW3_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(visualizer PRIVATE 
	${GLFW3_LIBRARIES} 
	${GLM_LIBRARIES}
)