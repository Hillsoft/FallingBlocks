add_subdirectory(test)

add_library(loader.image.bitmap "Bitmap.hpp" "Bitmap.cpp")
target_link_libraries(loader.image.bitmap
	loader.image
	util.file)

add_library(loader.image.png "Png.hpp" "Png.cpp")
target_link_libraries(loader.image.png
	loader.image
	math.vec
	util.debug
	util.file
	util.zlib)
