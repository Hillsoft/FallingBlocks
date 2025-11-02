add_subdirectory(audio)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(input)
add_subdirectory(loader)
add_subdirectory(math)
add_subdirectory(physics)
add_subdirectory(render)
add_subdirectory(serialization)
add_subdirectory(ui)
add_subdirectory(util)

add_library(application STATIC "Application.cpp" "Application.hpp")
target_link_libraries(application
	engine.scene
	engine.sceneloader
	game.loadingscreen
	game.scenes.mainmenu
	globalsubsystemstack
	input.inputsubsystem
	util.debug)

add_library(globalsubsystemstack STATIC "GlobalSubSystemStack.cpp" "GlobalSubSystemStack.hpp")
target_link_libraries(globalsubsystemstack
	audio.audiosubsystem
	engine.localisation
	engine.resourcemanager
	engine.resourcesentinelmanager
	input.inputsubsystem
	render.rendersubsystem
	util.debug
	util.raii_helpers)

add_library(resourcetypes INTERFACE "ResourceTypes.hpp")
target_link_libraries(resourcetypes INTERFACE
	game.ball
	util.taggedvariant)
