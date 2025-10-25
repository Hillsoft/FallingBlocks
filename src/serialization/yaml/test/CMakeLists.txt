add_gtest(serialization.yaml.test.yamlparser "YAMLParserTest.cpp")
target_link_libraries(serialization.yaml.test.yamlparser PUBLIC
	serialization.yaml.yamlparser
	serialization.yaml.yamltokenizer)

add_gtest(serialization.yaml.test.yamltokenizer "YAMLTokenizerTest.cpp")
target_link_libraries(serialization.yaml.test.yamltokenizer PUBLIC
	serialization.yaml.yamltokenizer)
