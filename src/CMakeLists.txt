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
	game.ball
	game.paddle
	globalsubsystemstack
	input.inputsubsystem
	util.raii_helpers)

add_library(globalsubsystemstack STATIC "GlobalSubSystemStack.cpp" "GlobalSubSystemStack.hpp")
target_link_libraries(globalsubsystemstack
	engine.drawableregistry
	engine.tickregistry
	input.inputsubsystem
	physics.physicsscene
	render.rendersubsystem
	util.debug
	util.raii_helpers)
