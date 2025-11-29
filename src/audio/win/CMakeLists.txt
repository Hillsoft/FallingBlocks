add_library(audio.win.audioclient "AudioClient.hpp" "AudioClient.cpp")
target_link_libraries(audio.win.audioclient
	util.debug)
