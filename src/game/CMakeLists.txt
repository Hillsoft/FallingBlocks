add_library(game.paddle "Paddle.hpp" "Paddle.cpp")
target_link_libraries(game.paddle
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	render.rendersubsystem)
