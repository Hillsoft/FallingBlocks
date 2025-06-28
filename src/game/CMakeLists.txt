add_library(game.ball "Ball.hpp" "Ball.cpp")
target_link_libraries(game.ball
	engine.drawableregistry
	engine.tickregistry
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.rendersubsystem)

add_library(game.paddle "Paddle.hpp" "Paddle.cpp")
target_link_libraries(game.paddle
	engine.drawableregistry
	engine.tickregistry
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	physics.physicsscene
	render.rendersubsystem)
