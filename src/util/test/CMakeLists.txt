add_gtest(util.test.generator "generator.cpp")
target_link_libraries(util.test.generator INTERFACE
	util.generator)

add_gtest(util.test.string "string.cpp")
target_link_libraries(util.test.string INTERFACE util.string)
