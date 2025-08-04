add_library(render.shaders.col2dshader "Col2DShader.hpp" "Col2DShader.cpp")
target_link_libraries(render.shaders.col2dshader
	math.vec
	render.shaders.uvvertex
	render.vulkangraphicsdevice
	render.vulkanshader
	render.vulkanshaderprogram
	render.vulkan.descriptorsetlayoutbuilder)

add_library(render.shaders.fontshader "FontShader.hpp" "FontShader.cpp")
target_link_libraries(render.shaders.fontshader
	math.vec
	render.shaders.uvvertex
	render.vulkangraphicsdevice
	render.vulkanshader
	render.vulkanshaderprogram
	render.vulkan.descriptorsetlayoutbuilder)

add_library(render.shaders.tex2dshader "Tex2DShader.hpp" "Tex2DShader.cpp")
target_link_libraries(render.shaders.tex2dshader
	render.quad
	render.vulkangraphicsdevice
	render.vulkanshader
	render.vulkanshaderprogram
	render.vulkan.descriptorsetlayoutbuilder)

add_library(render.shaders.uvvertex "UVVertex.hpp" "UVVertex.cpp")
target_link_libraries(render.shaders.uvvertex
	math.vec)
