add_library(render.renderables.renderablecolor2d "RenderableColor2D.hpp" "RenderableColor2D.cpp")
target_link_libraries(render.renderables.renderablecolor2d
	render.renderableobject
	render.quad
	render.resource.shaderprogrammanager
	render.resource.texturemanager
	render.shaders.col2dshader
	render.vulkanbuffer
	render.vulkandescriptorpool
	render.vulkangraphicsdevice
	render.vulkanshaderprogram)

add_library(render.renderables.renderabletex2d "RenderableTex2D.hpp" "RenderableTex2D.cpp")
target_link_libraries(render.renderables.renderabletex2d
	math.vec
	render.renderableobject
	render.quad
	render.resource.shaderprogrammanager
	render.resource.texturemanager
	render.shaders.tex2dshader
	render.vulkanbuffer
	render.vulkandescriptorpool
	render.vulkangraphicsdevice
	render.vulkanshaderprogram
	render.vulkantexture)
