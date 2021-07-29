#pragma once

#include "vec3.h"

struct vec4 {
	float x, y, z, w;

	vec4()
		: x{}, y{}, z{}, w{}
	{ }

	vec4(float x, float y, float z, float w)
		: x{ x }, y{ y }, z{ z }, w{w}
	{ }

	vec4(const vec3& a, float w)
		: x{a.x}, y{a.y}, z{a.z}, w{w}
	{}

	float SqrMagnitude() const {
		return x * x + y * y + z * z + w * w;
	}

	float Magnitude() const {
		return std::sqrt(SqrMagnitude());
	}

	/// <summary>
	/// Dot product between this and o
	/// </summary>
	float Dot(const vec4& o) const {
		return x * o.x + y * o.y + z * o.z + w * o.w;
	}

	/// <summary>
	/// Normalizes this in place.
	/// </summary>
	/// <returns>Reference to this</returns>
	vec4& Normalize() {
		auto m = Magnitude();
		x /= m;
		y /= m;
		z /= m;
		w /= m;
		return *this;
	}
	/// <summary>
	/// Returns a normalized copy ofloat this.
	/// </summary>
	vec4 Normalized() const {
		return vec4{ *this }.Normalize();
	}

	vec4 operator+(const vec4& r) const {
		return vec4{ *this } += r;
	}
	vec4 operator-(const vec4& r) const {
		return vec4{ *this } -= r;
	}
	vec4 operator*(const vec4& r) const {
		return vec4{ *this } *= r;
	}
	vec4 operator/(const vec4& r) const {
		return vec4{ *this } /= r;
	}

	vec4 operator*(float r) const {
		return vec4{ *this } *= r;
	}
	vec4 operator/(float r) const {
		return vec4{ *this } /= r;
	}

	vec4& operator+=(const vec4& r) {
		x += r.x;
		y += r.y;
		z += r.z;
		w += r.w;
		return *this;
	}
	vec4& operator-=(const vec4& r) {
		x -= r.x;
		y -= r.y;
		z -= r.z;
		w -= r.w;
		return *this;
	}
	vec4& operator*=(const vec4& r) {
		x *= r.x;
		y *= r.y;
		z *= r.z;
		w *= r.w;
		return *this;
	}
	vec4& operator/=(const vec4& r) {
		x /= r.x;
		y /= r.y;
		z /= r.z;
		w /= r.w;
		return *this;
	}

	vec4& operator*=(float r) {
		x *= r;
		y *= r;
		z *= r;
		w *= r;
		return *this;
	}
	vec4& operator/=(float r) {
		x /= r;
		y /= r;
		z /= r;
		w /= r;
		return *this;
	}

	vec4& Negate() {
		x = -x;
		y = -y;
		z = -z;
		w = -w;
		return *this;
	}
	vec4 Negated() const {
		return vec4{ *this }.Negate();
	}
	vec4 operator-() const {
		return Negated();
	}

	bool operator==(const vec4& b) const = default;

};
