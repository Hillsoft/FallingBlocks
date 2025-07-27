add_library(render.resource.shaderprogrammanager STATIC "ShaderProgramManager.cpp" "ShaderProgramManager.hpp")
target_link_libraries(render.resource.shaderprogrammanager
	render.vulkangraphicsdevice	
	render.vulkanshaderprogram
	util.debug)

add_library(render.resource.texturemanager STATIC "TextureManager.hpp" "TextureManager.cpp")
target_link_libraries(render.resource.texturemanager
	render.vulkancommandpool
	render.vulkangraphicsdevice
	render.vulkantexture)
