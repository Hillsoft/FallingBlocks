#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 2) in mat3 modelTransform;
layout(location = 5) in int glyphStart;
layout(location = 6) in int glyphEnd;
layout(location = 7) in mat3 uvToGlyphSpace;

layout(push_constant) uniform pc {
  mat3 viewMatrix;
};

layout(location = 0) out vec2 outUV;
layout(location = 1) out flat int outGlyphStart;
layout(location = 2) out flat int outGlyphEnd;

void main() {
  vec2 screenSpace = (viewMatrix * modelTransform * vec3(inPosition, 1.0)).xy;
  gl_Position = vec4(screenSpace, 0.0, 1.0);
  vec2 adjInUV = inUV;
  adjInUV.y = 1.0 - adjInUV.y;
  outUV = (uvToGlyphSpace * vec3(adjInUV, 1.0)).xy;
  outGlyphStart = glyphStart;
  outGlyphEnd = glyphEnd;
}
