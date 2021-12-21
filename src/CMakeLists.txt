add_executable(visualizer
	"main.cpp" "Application.cpp"
 "backend/VertexArrayObject.cpp" "backend/Shader.cpp"  "Cuboid.cpp" "backend/Drawable.cpp" "backend/Transformable.cpp")

target_sources(visualizer PUBLIC 
	${VENDOR_DIR}/glad/src/glad.c

	${VENDOR_DIR}/imgui/backends/imgui_impl_opengl3.cpp
	${VENDOR_DIR}/imgui/backends/imgui_impl_glfw.cpp
	${VENDOR_DIR}/imgui/imgui.cpp
	${VENDOR_DIR}/imgui/imgui_draw.cpp
	${VENDOR_DIR}/imgui/imgui_tables.cpp
	${VENDOR_DIR}/imgui/imgui_widgets.cpp
	${VENDOR_DIR}/imgui/imgui_demo.cpp
)

target_include_directories(visualizer PRIVATE 
	${GLFW3_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${VENDOR_DIR}/glad/include
	${VENDOR_DIR}/imgui
)

target_link_libraries(visualizer PRIVATE 
	${GLFW3_LIBRARIES} 
	${GLM_LIBRARIES}
)