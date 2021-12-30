add_executable(visualizer
	"main.cpp" 
	"Application.cpp" 
    "OrbitingCamera.cpp" 
 	"Topology.cpp"
	"Colormaps.cpp"
	"ScrollingPlot.cpp"
	"AudioFile.cpp"
	"Spectrogram.cpp"
)

target_sources(visualizer PUBLIC 
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
	${SDL2_INCLUDE_DIRS}
	${VENDOR_DIR}/imgui
	lol
)

target_link_libraries(visualizer PRIVATE 
	${GLFW3_LIBRARIES} 
	${SDL2_LIBRARIES}
	lol
)

add_custom_command(TARGET visualizer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:visualizer>/res
)