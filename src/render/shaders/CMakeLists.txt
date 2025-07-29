add_library(render.shaders.tex2dshader "Tex2DShader.hpp" "Tex2DShader.cpp")
target_link_libraries(render.shaders.tex2dshader
	render.quad
	render.vulkangraphicsdevice
	render.vulkanshader
	render.vulkanshaderprogram
	render.vulkan.descriptorsetlayoutbuilder)
