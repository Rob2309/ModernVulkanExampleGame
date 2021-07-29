#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "Quaternion.h"
#include "mat4.h"

constexpr float PI = 3.14159265358979323846f;

inline constexpr float ToRadians(float degrees) {
	return degrees / 180.0f * PI;
}

inline constexpr float ToDegrees(float rad) {
	return rad / PI * 180.0f;
}
