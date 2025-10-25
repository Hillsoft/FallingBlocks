add_gtest(serialization.yaml.test.yamltokenizer "YAMLTokenizerTest.cpp")
target_link_libraries(serialization.yaml.test.yamltokenizer PUBLIC
	serialization.yaml.yamltokenizer)
