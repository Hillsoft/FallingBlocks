add_library(render.resource.shaderprogrammanager STATIC "ShaderProgramManager.cpp" "ShaderProgramManager.hpp")
target_link_libraries(render.resource.shaderprogrammanager
	render.vulkangraphicsdevice	
	render.vulkanshaderprogram
	util.debug)
