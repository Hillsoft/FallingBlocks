#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;

layout(location = 2) in vec2 pos0;
layout(location = 3) in vec2 pos1;
layout(location = 4) in int glyphStart;
layout(location = 5) in int glyphEnd;
layout(location = 6) in mat3 uvToGlyphSpace;

layout(location = 0) out vec2 outUV;
layout(location = 1) out flat int outGlyphStart;
layout(location = 2) out flat int outGlyphEnd;

void main() {
  gl_Position = vec4(inPosition * pos1 + (1.0 - inPosition) * pos0, 0.0, 1.0);
  vec2 adjInUV = inUV;
  adjInUV.y = 1.0 - adjInUV.y;
  outUV = (uvToGlyphSpace * vec3(adjInUV, 1.0)).xy;
  outGlyphStart = glyphStart;
  outGlyphEnd = glyphEnd;
}
