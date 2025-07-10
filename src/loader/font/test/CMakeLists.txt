add_gtest(loader.font.test.font "font.cpp")
target_link_libraries(loader.font.test.font
	PUBLIC
	loader.font.font)
