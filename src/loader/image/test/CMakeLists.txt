add_gtest(loader.image.test.bitmap "bitmap.cpp")
target_link_libraries(loader.image.test.bitmap
	PUBLIC
	loader.image.bitmap)
