add_library(game.resource.defaultfont "DefaultFont.hpp" "DefaultFont.cpp")
target_link_libraries(game.resource.defaultfont
	globalsubsystemstack
	loader.font.font
	render.font
	util.debug)
