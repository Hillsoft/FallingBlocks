#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec2 outUV;

void main() {
  gl_Position = vec4(inPosition, 0.0, 1.0);
  outUV = inUV;
}
