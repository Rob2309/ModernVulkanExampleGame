#pragma once

#include <cmath>

struct vec2 {
	float x, y;

	vec2()
		: x{}, y{}
	{ }

	vec2(float x, float y)
		: x{x}, y{y}
	{ }

	float SqrMagnitude() const {
		return x * x + y * y;
	}

	float Magnitude() const {
		return std::sqrt(SqrMagnitude());
	}

	/// <summary>
	/// Dot product between this and o
	/// </summary>
	float Dot(const vec2& o) const {
		return x * o.x + y * o.y;
	}

	/// <summary>
	/// Normalizes this in place.
	/// </summary>
	/// <returns>Reference to this</returns>
	vec2& Normalize() {
		auto m = Magnitude();
		x /= m;
		y /= m;
		return *this;
	}
	/// <summary>
	/// Returns a normalized copy ofloat this.
	/// </summary>
	vec2 Normalized() const {
		return vec2{ *this }.Normalize();
	}

	vec2 operator+(const vec2& r) const {
		return vec2{ *this } += r;
	}
	vec2 operator-(const vec2& r) const {
		return vec2{ *this } -= r;
	}
	vec2 operator*(const vec2& r) const {
		return vec2{ *this } *= r;
	}
	vec2 operator/(const vec2& r) const {
		return vec2{ *this } /= r;
	}

	vec2 operator*(float r) const {
		return vec2{ *this } *= r;
	}
	vec2 operator/(float r) const {
		return vec2{ *this } /= r;
	}

	vec2& operator+=(const vec2& r) {
		x += r.x;
		y += r.y;
		return *this;
	}
	vec2& operator-=(const vec2& r) {
		x -= r.x;
		y -= r.y;
		return *this;
	}
	vec2& operator*=(const vec2& r) {
		x *= r.x;
		y *= r.y;
		return *this;
	}
	vec2& operator/=(const vec2& r) {
		x /= r.x;
		y /= r.y;
		return *this;
	}

	vec2& operator*=(float r) {
		x *= r;
		y *= r;
		return *this;
	}
	vec2& operator/=(float r) {
		x /= r;
		y /= r;
		return *this;
	}

	vec2& Negate() {
		x = -x;
		y = -y;
		return *this;
	}
	vec2 Negated() const {
		return vec2{ *this }.Negate();
	}
	vec2 operator-() const {
		return Negated();
	}

	bool operator==(const vec2& b) const = default;
	
};
