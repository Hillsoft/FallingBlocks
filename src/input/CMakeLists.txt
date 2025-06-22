add_library(input.inputsubsystem STATIC "InputSubSystem.hpp" "InputSubSystem.cpp" "InputHandler.hpp" "InputHandler.cpp")
target_link_libraries(input.inputsubsystem
	render.glfw_wrapper.window)
