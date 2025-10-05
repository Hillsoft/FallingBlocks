add_subdirectory(win)

add_library(audio.audiosubsystem "AudioSubSystem.hpp" "AudioSubSystem.cpp")
target_link_libraries(audio.audiosubsystem
	audio.win.audioclient)
