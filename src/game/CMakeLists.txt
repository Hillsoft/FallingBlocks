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
	audio.audiosubsystem
	engine.actor
	engine.drawableregistry
	engine.resourcemanager
	engine.resourceref
	engine.scene
	engine.textureresource
	engine.tickregistry
	game.block
	game.blocksscene
	game.paddle
	globalsubsystemstack
	math.vec
	physics.physicsscene
	render.renderables.renderabletex2d
	render.rendersubsystem
	util.debug
	util.meta_utils)

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

add_library(game.gameoverui "GameOverUI.hpp" "GameOverUI.cpp")
target_link_libraries(game.gameoverui
	engine.scene
	game.uiactor
	globalsubsystemstack
	game.resource.defaultfont
	render.font
	ui.uiobject
	ui.uitext
	util.debug
	util.notnull)

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
	engine.resourceref
	engine.scene
	engine.textureresource
	engine.tickregistry
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	physics.physicsscene
	render.rendersubsystem
	util.debug
	util.meta_utils)

add_library(game.mainmenuui "MainMenuUI.hpp" "MainMenuUI.cpp")
target_link_libraries(game.mainmenuui
	application
	engine.scene
	game.resource.defaultfont
	game.scenes.level1
	game.scenes.mainmenu
	game.uiactor
	globalsubsystemstack
	math.vec
	render.font
	render.rendersubsystem
	render.renderables.renderablecolor2d
	ui.uibutton
	ui.uiobject
	ui.uitext
	util.debug
	util.notnull)

add_library(game.scoreui "ScoreUI.hpp" "ScoreUI.cpp")
target_link_libraries(game.scoreui
	engine.actor
	engine.scene
	game.blocksscene
	game.resource.defaultfont
	game.uiactor
	globalsubsystemstack
	render.font
	ui.uiobject
	ui.uitext
	util.debug
	util.notnull
	util.string)

add_library(game.statictext "StaticText.hpp" "StaticText.cpp")
target_link_libraries(game.statictext
	engine.actor
	engine.drawableregistry
	engine.scene
	game.resource.defaultfont
	math.vec
	render.font)

add_library(game.testtext "TestText.hpp" "TestText.cpp")
target_link_libraries(game.testtext
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	loader.font.font
	math.vec
	render.font)

add_library(game.uiactor "UIActor.hpp" "UIActor.cpp")
target_link_libraries(game.uiactor
	engine.actor
	engine.drawableregistry
	engine.scene
	globalsubsystemstack
	math.vec
	render.simple2dcamera
	ui.layout
	ui.uiobject)
