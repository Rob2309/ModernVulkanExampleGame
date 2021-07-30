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
	Quaternion(vec3 axis, float rad)
		: w{std::cos(rad * 0.5f)}
	{
		axis.Normalize();
		
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
		float ny = w * r.y + y * r.w + z * r.x - x * r.z;
		float nz = w * r.z + z * r.w + x * r.y - y * r.x;
		float nw = w * r.w - x * r.x - y * r.y - z * r.z;
		return Quaternion{ nx, ny, nz, nw };
	}
	Quaternion& operator*=(const Quaternion& r) {
		*this = *this * r;
		return *this;
	}

	vec3 Rotate(const vec3& r) const {
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;
		float w2 = w * w;

		float xx = x * r.x;
		float yy = y * r.y;
		float zz = z * r.z;

		float nx = r.x * (x2 - y2 - z2 + w2) + 2.0f * (x * yy + x * zz + w * y * r.z - w * z * r.y);
		float ny = r.y * (-x2 + y2 - z2 + w2) + 2.0f * (y * xx + y * zz + w * z * r.x - w * x * r.z);
		float nz = r.z * (-x2 - y2 + z2 + w2) + 2.0f * (z * xx + z * yy + w * x * r.y + w * y * r.x);
		return vec3{ nx, ny, nz };
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
		return vec3{
			x * x - y * y - z * z + w * w,
			2.0f * (z * w + x * y),
			2.0f * (x * z - y * w)
		};
	}
	vec3 Up() const {
		return vec3{
			2.0f * (x * y - z * w),
			-x * x + y * y - z * z + w * w,
			2.0f * (x * w + y * z),
		};
	}
	vec3 Forward() const {
		return vec3{
			2.0f * (x * z + y * w),
			2.0f * (y * z - x * w),
			-x * x - y * y + z * z + w * w,
		};
	}

	bool operator==(const Quaternion& r) const = default;
	
};
