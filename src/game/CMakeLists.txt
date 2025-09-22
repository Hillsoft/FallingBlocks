add_subdirectory(resource)
add_subdirectory(scenes)

add_library(game.background "Background.hpp" "Background.cpp")
target_link_libraries(game.background
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	math.vec
	render.renderables.renderabletex2d
	render.rendersubsystem
	util.debug)

add_library(game.ball "Ball.hpp" "Ball.cpp")
target_link_libraries(game.ball
	engine.actor
	engine.drawableregistry
	engine.scene
	engine.tickregistry
	game.block
	game.blocksscene
	game.paddle
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.renderables.renderabletex2d
	render.rendersubsystem
	util.debug)

add_library(game.ballspawnblock "BallSpawnBlock.hpp" "BallSpawnBlock.cpp")
target_link_libraries(game.ballspawnblock
	engine.scene
	game.ball
	game.block
	globalsubsystemstack
	math.vec
	render.renderables.renderabletex2d
	render.rendersubsystem
	util.debug)

add_library(game.block "Block.hpp" "Block.cpp")
target_link_libraries(game.block
	engine.actor
	engine.drawableregistry
	engine.scene
	game.blocksscene
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.renderables.renderabletex2d
	render.rendersubsystem
	util.debug)

add_library(game.blocksscene "BlocksScene.hpp" "BlocksScene.cpp")
target_link_libraries(game.blocksscene
	application
	engine.scene
	engine.sceneloader
	game.ball
	game.block
	game.scenes.gameover)

add_library(game.button "Button.hpp" "Button.cpp")
target_link_libraries(game.button
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	render.font
	render.renderables.renderablecolor2d
	render.rendersubsystem
	game.resource.defaultfont
	util.debug)

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
	render.rendersubsystem
	util.debug)

add_library(game.scoreui "ScoreUI.hpp" "ScoreUI.cpp")
target_link_libraries(game.scoreui
	engine.actor
	engine.drawableregistry
	engine.scene
	game.blocksscene
	game.resource.defaultfont
	util.debug
	util.string)

add_library(game.statictext "StaticText.hpp" "StaticText.cpp")
target_link_libraries(game.statictext
	engine.actor
	engine.drawableregistry
	engine.scene
	game.resource.defaultfont
	math.vec)

add_library(game.testtext "TestText.hpp" "TestText.cpp")
target_link_libraries(game.testtext
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	loader.font.font
	math.vec
	render.font)
