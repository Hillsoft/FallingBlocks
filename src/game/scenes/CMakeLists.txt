add_library(game.scenes.gameover STATIC "GameOver.hpp" "GameOver.cpp")
target_link_libraries(game.scenes.gameover
	application	
	engine.actor
	engine.scene
	engine.sceneloader
	globalsubsystemstack
	game.background
	game.gameoverui
	game.resource.defaultfont
	input.inputsubsystem)

add_library(game.scenes.levelconsts INTERFACE "LevelConsts.hpp")
