#pragma once

#include "vec3.h"

struct Quaternion {
	float x, y, z, w;

	Quaternion()
		: x{}, y{}, z{}, w{1.0f}
	{ }
	Quaternion(float x, float y, float z, float w)
		: x{x}, y{y}, z{z}, w{w}
	{ }
	Quaternion(const vec3& axis, float rad)
		: w{std::cos(rad * 0.5f)}
	{
		auto s = std::sin(rad * 0.5f);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}

	float SqrMagnitude() const {
		return x * x + y * y + z * z + w * w;
	}

	float Magnitude() const {
		return std::sqrt(SqrMagnitude());
	}

	/// <summary>
	/// Dot product between this and o
	/// </summary>
	float Dot(const Quaternion& o) const {
		return x * o.x + y * o.y + z * o.z + w * o.w;
	}

	/// <summary>
	/// Normalizes this in place.
	/// </summary>
	/// <returns>Reference to this</returns>
	Quaternion& Normalize() {
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
	Quaternion Normalized() const {
		return Quaternion{ *this }.Normalize();
	}

	Quaternion operator*(const Quaternion& r) const {
		float nx = w * r.x + x * r.w + y * r.z - z * r.y;
		float ny = w * r.y + y * r.w + z * r.x - x * r.w;
		float nz = w * r.z + z * r.w + x * r.y - y * r.x;
		float nw = w * r.w - x * r.x - y * r.y - z * r.z;
		return Quaternion{ nx, ny, nz, nw };
	}
	Quaternion& operator*=(const Quaternion& r) {
		*this = *this * r;
		return *this;
	}

	vec3 Rotate(const vec3& r) const {
		// TODO: use optimized formula
		auto res = *this * Quaternion{ r.x, r.y, r.z, 0 } *-*this;
		return vec3{ res.x, res.y, res.z };
	}
	vec3 operator*(const vec3& r) const {
		return Rotate(r);
	}

	Quaternion& Negate() {
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	Quaternion Negated() const {
		return Quaternion{ *this }.Negate();
	}
	Quaternion operator-() const {
		return Negated();
	}

	vec3 Right() const {
		return *this * vec3{ 1.0f, 0.0f, 0.0f };
	}
	vec3 Up() const {
		return *this * vec3{ 0.0f, 1.0f, 0.0f };
	}
	vec3 Forward() const {
		return *this * vec3{ 0.0f, 0.0f, 1.0f };
	}

	bool operator==(const Quaternion& r) const = default;
	
};
