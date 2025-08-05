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
	float xIntercept = a.x + (uv.y - a.y) * (b.x - a.x) / (b.y - a.y);
	if (xIntercept > uv.x) {
	  return b.y > a.y ? 1 : -1;
	}
  }

  return 0;
}

int intersectsQuadratic(ivec2 a, ivec2 handle, ivec2 b) {
  float qa = a.y - 2 * handle.y + b.y;
  float qb = 2 * handle.y - 2 * a.y;
  float qc = a.y - uv.y;

  float discriminant = qb*qb - 4 * qa * qc;

  if (discriminant < 0) {
    return 0;
  }

  int windingNumber = 0;

  float t0 = (-qb + sqrt(discriminant)) / (2 * qa);
  if (0 < t0 && t0 < 1) {
    float x0 = (1.0 - t0) * (1.0 - t0) * a.x + 2 * t0 * (1.0 - t0) * handle.x + t0 * t0 * b.x;
	if (x0 > uv.x) {
	  float dy = 2 * b.y * t0 - 2 * handle.y * t0 + 2 * handle.y * (1.0 - t0) - 2 * a.y * (1.0 - t0);
	  windingNumber += dy > 0 ? 1 : -1;
	}
  }

  float t1 = (-qb - sqrt(discriminant)) / (2 * qa);
  if (0 < t1 && t1 < 1) {
    float x1 = (1.0 - t1) * (1.0 - t1) * a.x + 2 * t1 * (1.0 - t1) * handle.x + t1 * t1 * b.x;
	if (x1 > uv.x) {
	  float dy = 2 * b.y * t1 - 2 * handle.y * t1 + 2 * handle.y * (1.0 - t1) - 2 * a.y * (1.0 - t1);
	  windingNumber += dy > 0 ? 1 : -1;
	}
  }

  return windingNumber;
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
