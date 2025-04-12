add_subdirectory(image)
add_subdirectory(test)

add_library(loader.config STATIC "Config.hpp" "Config.cpp")
target_link_libraries(loader.config
	util.file
	util.string)

add_library(loader.image INTERFACE "Image.hpp")

add_library(loader.loadimage STATIC "LoadImage.hpp" "LoadImage.cpp")
target_link_libraries(loader.loadimage
	loader.image
	loader.image.bitmap
	loader.image.png
	util.string)
