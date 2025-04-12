add_gtest(loader.test.config "config.cpp")
target_link_libraries(loader.test.config PUBLIC
	loader.config)
