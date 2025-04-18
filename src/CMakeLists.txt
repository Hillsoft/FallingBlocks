add_subdirectory(input)
add_subdirectory(loader)
add_subdirectory(math)
add_subdirectory(render)
add_subdirectory(util)

add_library(application STATIC "Application.cpp" "Application.hpp"  "input/InputSubSystem.cpp")
target_link_libraries(application
	input.inputsubsystem
	math.vec
	render.glfw_wrapper.window
	render.rendersubsystem)
