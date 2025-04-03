#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(binding = 1) uniform PositionInfo {
  vec2 pos0;
  vec2 pos1;
} posInfo;

layout(location = 0) out vec2 outUV;

void main() {
  gl_Position = vec4(inPosition * posInfo.pos1 + (1.0 - inPosition) * posInfo.pos0, 0.0, 1.0);
  outUV = inUV;
}
