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

int intersectsStraight(ivec2 a, ivec2 b) {
  if ((a.y < uv.y && b.y > uv.y) || (a.y > uv.y && b.y < uv.y)) {
	float dx = (b.x - a.x) / (b.y - a.y);
	float xIntercept = a.x + dx * (uv.y - a.y);
	if (xIntercept > uv.x) {
	  return b.y > a.y ? 1 : -1;
	}
  }

  return 0;
}

int intersectsQuadratic(ivec2 a, ivec2 handle, ivec2 b) {
  return intersectsStraight(a, b);
}

void main() {
  GlyphPoint first = fontData.glyphPoints[glyphStart];
  GlyphPoint second = fontData.glyphPoints[glyphStart + 1];

  int windingNumber = 0;

  int contourStart = glyphStart;
  for (int glyphIndex = glyphStart; glyphIndex < glyphEnd - 1; glyphIndex++) {
	int nextIndex = glyphIndex + 1;
	if (fontData.glyphPoints[glyphIndex].contourEnd) {
	  nextIndex = contourStart;
	  contourStart = glyphIndex + 1;
	}

	if (fontData.glyphPoints[nextIndex].onCurve) {
	  windingNumber += intersectsStraight(fontData.glyphPoints[glyphIndex].point, fontData.glyphPoints[nextIndex].point);
	}
	else {
	  int nextNextIndex = nextIndex + 1;
	  if (fontData.glyphPoints[nextIndex].contourEnd) {
	    nextNextIndex = contourStart;
		contourStart = glyphIndex + 1;
	  }
	  windingNumber += intersectsQuadratic(fontData.glyphPoints[glyphIndex].point, fontData.glyphPoints[nextIndex].point, fontData.glyphPoints[nextNextIndex].point);
	  glyphIndex++;
	}
  }

  if (windingNumber == 0) {
    outColor = vec4(0.0, 0.0, 0.0, 0.0);
  }
  else {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
}
