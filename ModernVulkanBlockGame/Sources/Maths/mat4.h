#pragma once

#include "vec3.h"
#include "vec4.h"
#include "Quaternion.h"

#define RC(r, c) ((r) + (c) * 4) 

struct mat4 {
	float values[4 * 4];

	mat4()
		: mat4{1.0f}
	{ }

	explicit mat4(float diagonal)
		: values{}
	{
		values[RC(0, 0)] = diagonal;
		values[RC(1, 1)] = diagonal;
		values[RC(2, 2)] = diagonal;
		values[RC(3, 3)] = diagonal;
	}

	mat4 operator*(const mat4& o) const {
		mat4 res;
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
				res.values[RC(r, c)] =
					values[RC(r, 0)] * o.values[RC(0, c)] +
					values[RC(r, 1)] * o.values[RC(1, c)] +
					values[RC(r, 2)] * o.values[RC(2, c)] +
					values[RC(r, 3)] * o.values[RC(3, c)];
		return res;
	}
	mat4& operator*=(const mat4& o) {
		*this = *this * o;
		return *this;
	}

	vec3 operator*(const vec3& r) const {
		auto x = values[RC(0, 0)] * r.x + values[RC(0, 1)] * r.y + values[RC(0, 2)] * r.z;
		auto y = values[RC(1, 0)] * r.x + values[RC(1, 1)] * r.y + values[RC(1, 2)] * r.z;
		auto z = values[RC(2, 0)] * r.x + values[RC(2, 1)] * r.y + values[RC(2, 2)] * r.z;
		return vec3{ x, y, z };
	}

	vec4 operator*(const vec4& r) const {
		auto x = values[RC(0, 0)] * r.x + values[RC(0, 1)] * r.y + values[RC(0, 2)] * r.z + values[RC(0, 3)] * r.w;
		auto y = values[RC(1, 0)] * r.x + values[RC(1, 1)] * r.y + values[RC(1, 2)] * r.z + values[RC(1, 3)] * r.w;
		auto z = values[RC(2, 0)] * r.x + values[RC(2, 1)] * r.y + values[RC(2, 2)] * r.z + values[RC(2, 3)] * r.w;
		auto w = values[RC(3, 0)] * r.x + values[RC(3, 1)] * r.y + values[RC(3, 2)] * r.z + values[RC(3, 3)] * r.w;
		return vec4{ x, y, z, w };
	}

	static mat4 Translate(const vec3& t) {
		mat4 res;
		res.values[RC(0, 3)] = t.x;
		res.values[RC(1, 3)] = t.y;
		res.values[RC(1, 3)] = t.z;
		return res;
	}
	static mat4 Rotate(const Quaternion& t) {
		auto right = t.Right();
		auto up = t.Up();
		auto forward = t.Forward();
		
		mat4 res;
		res.values[RC(0, 0)] = right.x;
		res.values[RC(1, 0)] = right.y;
		res.values[RC(2, 0)] = right.z;

		res.values[RC(0, 1)] = up.x;
		res.values[RC(1, 1)] = up.y;
		res.values[RC(2, 1)] = up.z;

		res.values[RC(0, 2)] = forward.x;
		res.values[RC(1, 2)] = forward.y;
		res.values[RC(2, 2)] = forward.z;
		return res;
	}
	static mat4 Scale(const vec3& s) {
		mat4 res;
		res.values[RC(0, 0)] = s.x;
		res.values[RC(1, 1)] = s.y;
		res.values[RC(2, 2)] = s.z;
		return res;
	}

	static mat4 LocalToWorld(const vec3& pos, const Quaternion& rot, const vec3& scale) {
		return Translate(pos) * Rotate(rot) * Scale(scale);
	}
	static mat4 WorldToLocal(const vec3& pos, const Quaternion& rot, const vec3& scale) {
		auto invScale = vec3{ 1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z };
		return Scale(invScale) * Rotate(-rot) * Translate(-pos);
	}

	static mat4 Perspective(float fov, float near, float far, float aspect) {
		auto thfov = std::tan(fov * 0.5f);
		
		mat4 res;
		res.values[RC(0, 0)] = 1.0f / (thfov * aspect);
		res.values[RC(1, 1)] = 1.0f / thfov;
		res.values[RC(2, 2)] = far / (far - near);
		res.values[RC(2, 3)] = (-far * near) / (far - near);
		res.values[RC(3, 2)] = 1.0f;
		res.values[RC(3, 3)] = 0.0f;
		return res;
	}
	
};

#undef RC
