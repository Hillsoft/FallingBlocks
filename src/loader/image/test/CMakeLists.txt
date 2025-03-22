add_gtest(loader.image.test.bitmap "bitmap.cpp")
target_link_libraries(loader.image.test.bitmap
	PUBLIC
	loader.image.bitmap)

add_gtest(loader.image.test.png "png.cpp")
target_link_libraries(loader.image.test.png
	PUBLIC
	loader.image.png)
