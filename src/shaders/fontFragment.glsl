#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec2 uv;

layout(location = 1) flat in int glyphStart;
layout(location = 2) flat in int glyphEnd;

struct GlyphPoint {
  ivec2 point;
  bool onCurve;
  bool contourEnd;
};

layout(std140, binding = 0) readonly buffer FontData{
	GlyphPoint glyphPoints[];
} fontData;

layout(location = 0) out vec4 outColor;

bool intersects(GlyphPoint a, GlyphPoint b) {
  if ((a.point.y < uv.y && b.point.y > uv.y) || (a.point.y > uv.y && b.point.y < uv.y)) {
	float dx = (b.point.x - a.point.x) / (b.point.y - a.point.y);
	float xIntercept = a.point.x + dx * (uv.y - a.point.y);
	if (xIntercept > uv.x) {
	  return true;
	}
  }

  return false;
}

void main() {
  GlyphPoint first = fontData.glyphPoints[glyphStart];
  GlyphPoint second = fontData.glyphPoints[glyphStart + 1];

  int windingNumber = 0;

  for (int glyphIndex = glyphStart; glyphIndex < glyphEnd - 1; glyphIndex++) {
    if (intersects(fontData.glyphPoints[glyphIndex], fontData.glyphPoints[glyphIndex + 1])) {
	  windingNumber++;
	}
  }

  if (intersects(fontData.glyphPoints[glyphEnd - 1], fontData.glyphPoints[glyphStart])) {
    windingNumber++;
  }

  if (windingNumber % 2 == 0) {
    outColor = vec4(0.5, 0.5, 0.5, 0.5);
  }
  else {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
}
