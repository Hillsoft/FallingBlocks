add_subdirectory(loader)
add_subdirectory(math)
add_subdirectory(render)
add_subdirectory(util)

add_library(application STATIC "Application.cpp" "Application.hpp")
target_link_libraries(application
	math.vec
	render.rendersubsystem)
