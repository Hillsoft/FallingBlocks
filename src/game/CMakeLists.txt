add_library(game.ball "Ball.hpp" "Ball.cpp")
target_link_libraries(game.ball
	globalsubsystemstack
	math.vec
	render.rendersubsystem)

add_library(game.paddle "Paddle.hpp" "Paddle.cpp")
target_link_libraries(game.paddle
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	render.rendersubsystem)
