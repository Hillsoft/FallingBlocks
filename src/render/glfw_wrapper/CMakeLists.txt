add_library(render.glfw_wrapper.window STATIC "Window.cpp" "Window.hpp")
target_link_libraries(render.glfw_wrapper.window
	util.string
	util.raii_helpers)
