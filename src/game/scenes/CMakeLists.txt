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
	game.scoreui
	game.scenes.levelconsts
	globalsubsystemstack
	math.vec)
