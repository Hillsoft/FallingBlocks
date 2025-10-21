add_library(game.scenes.gameover STATIC "GameOver.hpp" "GameOver.cpp")
target_link_libraries(game.scenes.gameover
	application	
	engine.actor
	engine.scene
	engine.sceneloader
	globalsubsystemstack
	game.background
	game.gameoverui
	game.scenes.level1
	game.resource.defaultfont
	input.inputsubsystem)

add_library(game.scenes.levelconsts INTERFACE "LevelConsts.hpp")

add_library(game.scenes.level1 STATIC "Level1.hpp" "Level1.cpp")
target_link_libraries(game.scenes.level1
	engine.scene
	engine.sceneloader
	game.background
	game.ball
	game.block
	game.blocksscene
	game.paddle
	game.resource.defaultfont
	game.scoreui
	game.scenes.levelconsts
	game.scenes.level2
	globalsubsystemstack
	math.vec)

add_library(game.scenes.level2 STATIC "Level2.hpp" "Level2.cpp")
target_link_libraries(game.scenes.level2
	engine.scene
	engine.sceneloader
	game.background
	game.ball
	game.ballspawnblock
	game.block
	game.blocksscene
	game.paddle
	game.resource.defaultfont
	game.scoreui
	game.scenes.levelconsts
	globalsubsystemstack
	math.vec)

add_library(game.scenes.mainmenu STATIC "MainMenu.hpp" "MainMenu.cpp")
target_link_libraries(game.scenes.mainmenu
	engine.scene
	engine.sceneloader
	game.background
	game.button
	game.mainmenuui
	game.resource.defaultfont
	globalsubsystemstack)
