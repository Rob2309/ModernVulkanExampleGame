#pragma once

#include <cmath>

struct vec3 {
	float x, y, z;

	vec3()
		: x{}, y{}, z{}
	{ }

	vec3(float x, float y, float z)
		: x{ x }, y{ y }, z{ z }
	{ }

	float SqrMagnitude() const {
		return x * x + y * y + z * z;
	}

	float Magnitude() const {
		return std::sqrt(SqrMagnitude());
	}

	/// <summary>
	/// Dot product between this and o
	/// </summary>
	float Dot(const vec3& o) const {
		return x * o.x + y * o.y + z * o.z;
	}

	/// <summary>
	/// Cross product between this and o.
	/// </summary>
	vec3 Cross(const vec3& o) const {
		return vec3{
			y * o.z - z * o.y,
			z * o.x - x * o.z,
			x * o.y - y * o.x,
		};
	}

	/// <summary>
	/// Normalizes this in place.
	/// </summary>
	/// <returns>Reference to this</returns>
	vec3& Normalize() {
		auto m = Magnitude();
		x /= m;
		y /= m;
		z /= m;
		return *this;
	}
	/// <summary>
	/// Returns a normalized copy ofloat this.
	/// </summary>
	vec3 Normalized() const {
		return vec3{ *this }.Normalize();
	}

	vec3 operator+(const vec3& r) const {
		return vec3{ *this } += r;
	}
	vec3 operator-(const vec3& r) const {
		return vec3{ *this } -= r;
	}
	vec3 operator*(const vec3& r) const {
		return vec3{ *this } *= r;
	}
	vec3 operator/(const vec3& r) const {
		return vec3{ *this } /= r;
	}

	vec3 operator*(float r) const {
		return vec3{ *this } *= r;
	}
	vec3 operator/(float r) const {
		return vec3{ *this } /= r;
	}

	vec3& operator+=(const vec3& r) {
		x += r.x;
		y += r.y;
		z += r.z;
		return *this;
	}
	vec3& operator-=(const vec3& r) {
		x -= r.x;
		y -= r.y;
		z -= r.z;
		return *this;
	}
	vec3& operator*=(const vec3& r) {
		x *= r.x;
		y *= r.y;
		z *= r.z;
		return *this;
	}
	vec3& operator/=(const vec3& r) {
		x /= r.x;
		y /= r.y;
		z /= r.z;
		return *this;
	}

	vec3& operator*=(float r) {
		x *= r;
		y *= r;
		z *= r;
		return *this;
	}
	vec3& operator/=(float r) {
		x /= r;
		y /= r;
		z /= r;
		return *this;
	}

	vec3& Negate() {
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	vec3 Negated() const {
		return vec3{ *this }.Negate();
	}
	vec3 operator-() const {
		return Negated();
	}

	bool operator==(const vec3& b) const = default;

};
