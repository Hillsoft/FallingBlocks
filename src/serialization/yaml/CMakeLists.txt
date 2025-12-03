add_subdirectory(test)

add_library(serialization.yaml.yamlparser "YAMLParser.hpp" "YAMLParser.cpp")
target_link_libraries(serialization.yaml.yamlparser
	serialization.yaml.yamltokenizer
	util.debug)

add_library(serialization.yaml.yamlserializationprovider "YAMLSerializationProvider.hpp" "YAMLSerializationProvider.cpp")
target_link_libraries(serialization.yaml.yamlserializationprovider
	serialization.yaml.yamlparser
	serialization.yaml.yamltokenizer
	util.debug)

add_library(serialization.yaml.yamltokenizer "YAMLTokenizer.hpp" "YAMLTokenizer.cpp")
target_link_libraries(serialization.yaml.yamltokenizer
	util.array
	util.string)
