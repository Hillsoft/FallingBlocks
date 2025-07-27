add_subdirectory(test)

add_library(loader.font.font STATIC "Font.hpp" "Font.cpp" "test/font.cpp")
target_link_libraries(loader.font.font
	util.debug
	util.file)
