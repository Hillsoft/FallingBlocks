add_library(engine.drawableregistry STATIC "DrawableRegistry.hpp" "DrawableRegistry.cpp")
target_link_libraries(engine.drawableregistry
	util.registry)

add_library(engine.tickregistry STATIC "TickRegistry.hpp" "TickRegistry.cpp")
target_link_libraries(engine.tickregistry
	util.registry)
