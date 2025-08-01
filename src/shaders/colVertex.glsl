#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 2) in vec2 pos0;
layout(location = 3) in vec2 pos1;
layout(location = 4) in vec4 color;

layout(location = 0) out vec2 outUV;
layout(location = 1) out flat vec4 outColor;

void main() {
  gl_Position = vec4(inPosition * pos1 + (1.0 - inPosition) * pos0, 0.0, 1.0);
  outUV = inUV;
  outColor = color;
}
