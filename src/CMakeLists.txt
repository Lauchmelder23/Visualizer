add_executable(visualizer
	"main.cpp" "Application.cpp"
)

target_include_directories(visualizer PUBLIC ${GLFW3_INCLUDE_DIRS})

target_link_libraries(visualizer PUBLIC ${GLFW3_INCLUDE_DIRS})