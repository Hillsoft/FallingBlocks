add_subdirectory(test)

add_library(loader.image.bitmap "Bitmap.hpp" "Bitmap.cpp")
target_link_libraries(loader.image.bitmap
	loader.image
	util.file)
