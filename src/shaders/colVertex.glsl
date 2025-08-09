#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 2) in mat3 modelTransform;
layout(location = 5) in vec4 color;

layout(push_constant) uniform pc {
  mat3 viewMatrix;
};

layout(location = 0) out vec2 outUV;
layout(location = 1) out flat vec4 outColor;

void main() {
  vec2 screenSpace = (viewMatrix * modelTransform * vec3(inPosition, 1.0)).xy;
  gl_Position = vec4(screenSpace, 0.0, 1.0);
  outUV = inUV;
  outColor = color;
}
