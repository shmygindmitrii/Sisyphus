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
					vec3 r0, r1, r2;
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
			//
			static mat3 rotx(float angle);
			static mat3 roty(float angle);
			static mat3 rotz(float angle);
			static mat3 rot(const vec3& n, float angle);
            static mat3 identity();
		};

		std::ostream& operator<<(std::ostream& os, const mat3& M);

		mat3 operator* (float s, const mat3& M);
		mat3 operator/ (const mat3& M, float s);

		struct mat4 {
			union {
				float data[4][4];
				struct {
					vec4 r0, r1, r2, r3;
				};
			};
			mat4();
			mat4(float m00, float m01, float m02, float m03,
				 float m10, float m11, float m12, float m13,
				 float m20, float m21, float m22, float m23, 
				 float m30, float m31, float m32, float m33);
			mat4(const vec4& a,
				 const vec4& b,
				 const vec4& c,
				 const vec4& d);
			mat4(const mat4& m);
			mat4(mat4&& m) noexcept;
			mat4& operator=(const mat4& M);
			mat4& operator=(mat4&& M) noexcept;
			vec4& operator[](int i);
			const vec4& operator[](int i) const;
			float& operator()(int i, int j);
			const float& operator()(int i, int j) const;
			const bool operator==(const mat4& M) const;
			const bool operator!=(const mat4& M) const;
			mat4& operator *=(float s);
			mat4& operator /=(float s);
			mat4& operator +=(const mat4& M);
			mat4& operator -=(const mat4& M);
			mat4 operator +(const mat4& M) const;
			mat4 operator -() const;
			mat4 operator -(const mat4& M) const;
			mat4 operator *(const mat4& M) const;
			vec4 operator *(const vec4& v) const;
			//
			static mat4 rotx(float angle);
			static mat4 roty(float angle);
			static mat4 rotz(float angle);
			static mat4 rot(const vec4& n, float angle);
            static mat4 identity();
            static mat4 projection(float znear, float zfar);
		};

		std::ostream& operator<<(std::ostream& os, const mat4& M);

		mat4 operator* (float s, const mat4& M);
		mat4 operator/ (const mat4& M, float s);
	}
}
