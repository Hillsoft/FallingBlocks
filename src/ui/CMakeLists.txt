add_library(ui.layout STATIC "Layout.hpp" "Layout.cpp")
target_link_libraries(ui.layout
	math.vec
	render.simple2dcamera
	ui.uiobject
	util.debug)

add_library(ui.uibutton STATIC "UIButton.hpp" "UIButton.cpp")
target_link_libraries(ui.uibutton
	globalsubsystemstack
	input.inputsubsystem
	math.vec
	render.rendersubsystem
	render.renderables.renderablecolor2d
	render.simple2dcamera
	ui.uiobject)

add_library(ui.uiobject STATIC "UIObject.hpp" "UIObject.cpp")
target_link_libraries(ui.uiobject
	math.vec
	render.simple2dcamera
	util.notnull)

add_library(ui.uirect STATIC "UIRect.hpp" "UIRect.cpp")
target_link_libraries(ui.uirect
	globalsubsystemstack
	math.vec
	render.rendersubsystem
	render.renderables.renderablecolor2d
	render.simple2dcamera
	ui.uiobject)

add_library(ui.uitext STATIC "UIText.hpp" "UIText.cpp")
target_link_libraries(ui.uitext
	math.vec
	render.font
	render.simple2dcamera
	ui.uiobject
	util.notnull)
