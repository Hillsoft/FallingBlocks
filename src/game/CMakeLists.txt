add_library(game.ball "Ball.hpp" "Ball.cpp")
target_link_libraries(game.ball
	engine.actor
	engine.drawableregistry
	engine.scene
	engine.tickregistry
	game.block
	game.paddle
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.rendersubsystem)

add_library(game.block "Block.hpp" "Block.cpp")
target_link_libraries(game.block
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.rendersubsystem)

add_library(game.loadingscreen "LoadingScreen.hpp" "LoadingScreen.cpp")
target_link_libraries(game.loadingscreen
	engine.actor
	engine.drawableregistry
	engine.scene
	engine.tickregistry
	globalsubsystemstack
	math.vec
	render.rendersubsystem)

add_library(game.paddle "Paddle.hpp" "Paddle.cpp")
target_link_libraries(game.paddle
	engine.actor
	engine.drawableregistry
	engine.scene
	engine.tickregistry
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	physics.physicsscene
	render.rendersubsystem)
