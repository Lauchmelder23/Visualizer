add_library(backend STATIC 
	"src/Drawable.cpp"
	"src/Transformable.cpp"
	"src/Shader.cpp"
	"src/VertexArrayObject.cpp"
)

target_sources(backend PUBLIC 
	${VENDOR_DIR}/glad/src/glad.c
)

target_include_directories(backend PUBLIC 
	${GLM_INCLUDE_DIRS}
	${VENDOR_DIR}/glad/include
	include 
)

target_link_libraries(backend PUBLIC
	${GLM_LIBRARIES}
)