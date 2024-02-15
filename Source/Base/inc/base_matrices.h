#pragma once

#include "base_vectors.h"

#include <iostream>

namespace Temple
{
namespace Base
{
  struct mat3 {
    // column-wise - each array contains column instead of row
    union {
      float data[3][3];
      struct {
        vec3_t r0, r1, r2;
      };
    };
    vec3_t&
    operator[](int i);
    const vec3_t&
    operator[](int i) const;
    float&
    operator()(int i, int j);
    const float&
    operator()(int i, int j) const;
    const bool
    operator==(const mat3& M) const;
    const bool
    operator!=(const mat3& M) const;
    mat3&
    operator*=(float s);
    mat3&
    operator/=(float s);
    mat3&
    operator+=(const mat3& M);
    mat3&
    operator-=(const mat3& M);
    mat3
    operator+(const mat3& M) const;
    mat3
    operator-() const;
    mat3
    operator-(const mat3& M) const;
    mat3
    operator*(const mat3& M) const;
    vec3_t
    operator*(const vec3_t& v) const;
    //
    static mat3
    rotx(float angle);
    static mat3
    roty(float angle);
    static mat3
    rotz(float angle);
    static mat3
    rot(const vec3_t& n, float angle);
    static mat3
    identity();
  };

  std::ostream&
  operator<<(std::ostream& os, const mat3& M);

  mat3
  operator*(float s, const mat3& M);
  mat3
  operator/(const mat3& M, float s);

  struct mat4 {
    union {
      float data[4][4];
      struct {
        vec4_t r0, r1, r2, r3;
      };
    };
    vec4_t&
    operator[](int i);
    const vec4_t&
    operator[](int i) const;
    float&
    operator()(int i, int j);
    const float&
    operator()(int i, int j) const;
    const bool
    operator==(const mat4& M) const;
    const bool
    operator!=(const mat4& M) const;
    mat4&
    operator*=(float s);
    mat4&
    operator/=(float s);
    mat4&
    operator+=(const mat4& M);
    mat4&
    operator-=(const mat4& M);
    mat4
    operator+(const mat4& M) const;
    mat4
    operator-() const;
    mat4
    operator-(const mat4& M) const;
    mat4
    operator*(const mat4& M) const;
    vec4_t
    operator*(const vec4_t& v) const;
    //
    static mat4
    rotx(float angle);
    static mat4
    roty(float angle);
    static mat4
    rotz(float angle);
    static mat4
    rot(const vec4_t& n, float angle);
    static mat4
    identity();
    static mat4
    projection(float fov, float aspect, float znear, float zfar);
  };

  std::ostream&
  operator<<(std::ostream& os, const mat4& M);

  mat4
  operator*(float s, const mat4& M);
  mat4
  operator/(const mat4& M, float s);
} // namespace Base
} // namespace Temple
