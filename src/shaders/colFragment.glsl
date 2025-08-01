#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;

layout(location = 1) flat in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = inColor;
}
