#pragma once

#include "base_vectors.h"

#include <iostream>

namespace Temple {
	namespace Base {
		struct mat3 {
			// column-wise - each array contains column instead of row
			union {
				float data[3][3];
				struct {
					vec3 c0, c1, c2;
				};
			};
			mat3();
			mat3(float m00, float m01, float m02,
					 float m10, float m11, float m12,
					 float m20, float m21, float m22);
			mat3(const vec3& a, 
					 const vec3& b, 
					 const vec3& c);
			mat3(const mat3& m);
			mat3(mat3&& m) noexcept;
			mat3& operator=(const mat3& M);
			mat3& operator=(mat3&& M) noexcept;
			vec3& operator[](int i);
			const vec3& operator[](int i) const;
			float& operator()(int i, int j);
			const float& operator()(int i, int j) const;
			const bool operator==(const mat3& M) const;
			const bool operator!=(const mat3& M) const;
			mat3& operator *=(float s);
			mat3& operator /=(float s);
			mat3& operator +=(const mat3& M);
			mat3& operator -=(const mat3& M);
			mat3 operator +(const mat3& M) const;
			mat3 operator -() const;
			mat3 operator -(const mat3& M) const;
			mat3 operator *(const mat3& M) const;
			vec3 operator *(const vec3& v) const;
		};

		std::ostream& operator<<(std::ostream& os, const mat3& M);

		mat3 operator* (float s, const mat3& M);
		mat3 operator/ (const mat3& M, float s);
	}
}
