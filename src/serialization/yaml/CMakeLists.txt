add_subdirectory(test)

add_library(serialization.yaml.yamltokenizer "YAMLTokenizer.hpp" "YAMLTokenizer.cpp")
target_link_libraries(serialization.yaml.yamltokenizer
	util.array
	util.string)
