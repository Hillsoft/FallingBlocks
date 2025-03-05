add_subdirectory(render)
add_subdirectory(util)

add_executable(FallingBlocks "main.cpp")
target_link_libraries(FallingBlocks
	render.glfwapplication)
