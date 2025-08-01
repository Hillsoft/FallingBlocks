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
	render.renderables.renderabletex2d
	render.rendersubsystem)

add_library(game.block "Block.hpp" "Block.cpp")
target_link_libraries(game.block
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.renderables.renderabletex2d
	render.rendersubsystem)

add_library(game.loadingscreen "LoadingScreen.hpp" "LoadingScreen.cpp")
target_link_libraries(game.loadingscreen
	engine.actor
	engine.drawableregistry
	engine.scene
	engine.tickregistry
	globalsubsystemstack
	math.vec
	render.renderables.renderabletex2d
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
	render.renderables.renderabletex2d
	render.rendersubsystem)

add_library(game.testtext "TestText.hpp" "TestText.cpp")
target_link_libraries(game.testtext
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	loader.font.font
	math.vec
	render.font)
