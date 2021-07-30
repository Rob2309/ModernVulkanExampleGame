#pragma once

#include "vec3.h"

/// <summary>
/// 4D number describing a 3D rotation.
/// </summary>
struct Quaternion {
	float x, y, z, w;

	/*
	 * Quaternions are basically just the term xi + yj + zk + w.
	 * However, there are special rules:
	 *		i * i = -1
	 *		j * j = -1
	 *		k * k = -1
	 *
	 *		when multiplying "forward", i.e. i * j, j * k, k * i, the next letter comes out, e.g. i * j = k
	 *		when multiplying "backwards", i.e. k * j, j * i, k * i, the negative previous letter comes out, e.g. j * i = -k
	 *		think of the three letters as forming a clock, going foward gives the next letter, going backwards gives minus the previous letter.
	 *
	 *	With these rules every quaternion operation can just be calculated by multiplying two terms together.
	 */

	/// <summary>
	/// Creates the identity Quaternion
	/// </summary>
	Quaternion()
		: x{}, y{}, z{}, w{1.0f}
	{ }
	/// <summary>
	/// Creates a Quaternion using the given components, w being the real component
	/// </summary>
	Quaternion(float x, float y, float z, float w)
		: x{x}, y{y}, z{z}, w{w}
	{ }
	/// <summary>
	/// Creates a Quaternion representing a rotation of rad radians around axis.
	/// </summary>
	Quaternion(vec3 axis, float rad)
		: w{std::cos(rad * 0.5f)}
	{
		/*
		 * To represent a rotation as a quaternion, we need to return
		 * (xi + yj + zk) * sin(angle/2) + cos(angle/2).
		 * Don't ask me why, it just works :D
		 */

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
		/*
		 * The below calculations are just the term
		 * (x1i + y1j + z1k + w1) * (x2i + y2j + z2k + w2).
		 */

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

	/// <summary>
	/// Rotates a vector with this quaternion.
	/// </summary>
	vec3 Rotate(const vec3& r) const {
		/*
		 * To rotate a vector by a quaternion, we need to calculate q * r * -q with -q being the conjugate of the quaternion.
		 * After this calculation, the w component will be zero and the i, j, k components are the rotated vector.
		 * Don't ask me why this works, I have no idea.
		 *
		 * The calculations below are just the term
		 * (x1i + y1j + z1k + w1) * (x2i + y2j + z2k) * (-x1i - y1j - z1j + w1)
		 */

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
		/*
		 * To negate a quaternion (meaning creating a quaternion that cancels a rotation)
		 * we just need to invert the components representing the axis, w stays the same.
		 */

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

	/// <summary>
	/// Returns the vector representing the right direction of this quaternion.
	///	this * (1, 0, 0)
	/// </summary>
	vec3 Right() const {
		/*
		 * Just a simplified version of the vector (1, 0, 0) rotated by the quaternion.
		 * Basically the term (xi + yj + zk + w) * i * (-xi - yj - zk + w).
		 */
		return vec3{
			x * x - y * y - z * z + w * w,
			2.0f * (z * w + x * y),
			2.0f * (x * z - y * w)
		};
	}
	/// <summary>
	/// Returns the vector representing the up direction of this quaternion.
	///	this * (0, 1, 0)
	/// </summary>
	vec3 Up() const {
		/*
		 * Just a simplified version of the vector (0, 1, 0) rotated by the quaternion.
		 * Basically the term (xi + yj + zk + w) * j * (-xi - yj - zk + w).
		 */
		return vec3{
			2.0f * (x * y - z * w),
			-x * x + y * y - z * z + w * w,
			2.0f * (x * w + y * z),
		};
	}
	/// <summary>
	/// Returns the vector representing the forward direction of this quaternion.
	///	this * (0, 0, 1)
	/// </summary>
	vec3 Forward() const {
		/*
		 * Just a simplified version of the vector (0, 0, 1) rotated by the quaternion.
		 * Basically the term (xi + yj + zk + w) * k * (-xi - yj - zk + w).
		 */
		return vec3{
			2.0f * (x * z + y * w),
			2.0f * (y * z - x * w),
			-x * x - y * y + z * z + w * w,
		};
	}

	bool operator==(const Quaternion& r) const = default;
	
};
