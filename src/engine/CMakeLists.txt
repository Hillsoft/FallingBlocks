add_library(engine.actor STATIC "Actor.hpp" "Actor.cpp")
target_link_libraries(engine.actor
	util.portability
	util.raii_helpers)

add_library(engine.drawableregistry STATIC "DrawableRegistry.hpp" "DrawableRegistry.cpp")
target_link_libraries(engine.drawableregistry
	util.registry)

add_library(engine.scene STATIC "Scene.hpp" "Scene.cpp")
target_link_libraries(engine.scene
	engine.actor
	engine.drawableregistry
	engine.tickregistry
	physics.physicsscene
	util.debug)

add_library(engine.tickregistry STATIC "TickRegistry.hpp" "TickRegistry.cpp")
target_link_libraries(engine.tickregistry
	util.registry)
