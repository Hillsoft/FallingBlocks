add_library(physics.physicsscene "PhysicsScene.hpp" "PhysicsScene.cpp" "RectCollider.hpp" "RectCollider.cpp")
target_link_libraries(physics.physicsscene
	math.vec
	util.debug
	util.registry)
