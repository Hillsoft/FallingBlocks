add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(input)
add_subdirectory(loader)
add_subdirectory(math)
add_subdirectory(physics)
add_subdirectory(render)
add_subdirectory(util)

add_library(application STATIC "Application.cpp" "Application.hpp")
target_link_libraries(application
	engine.scene
	engine.sceneloader
	game.ball
	game.loadingscreen
	game.scenes.level1
	globalsubsystemstack
	input.inputsubsystem)

add_library(globalsubsystemstack STATIC "GlobalSubSystemStack.cpp" "GlobalSubSystemStack.hpp")
target_link_libraries(globalsubsystemstack
	engine.resourcesentinelmanager
	input.inputsubsystem
	render.rendersubsystem
	util.debug
	util.raii_helpers)
