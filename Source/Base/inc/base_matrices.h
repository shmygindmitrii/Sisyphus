#pragma once

#include "base_vectors.h"

#include <iostream>

namespace Temple
{
namespace Base
{
  struct mat3_t {
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
    operator==(const mat3_t& M) const;
    const bool
    operator!=(const mat3_t& M) const;
    mat3_t&
    operator*=(float s);
    mat3_t&
    operator/=(float s);
    mat3_t&
    operator+=(const mat3_t& M);
    mat3_t&
    operator-=(const mat3_t& M);
    mat3_t
    operator+(const mat3_t& M) const;
    mat3_t
    operator-() const;
    mat3_t
    operator-(const mat3_t& M) const;
    mat3_t
    operator*(const mat3_t& M) const;
    vec3_t
    operator*(const vec3_t& v) const;
    //
    static mat3_t
    rotx(float angle);
    static mat3_t
    roty(float angle);
    static mat3_t
    rotz(float angle);
    static mat3_t
    rot(const vec3_t& n, float angle);
    static mat3_t
    identity();
  };

  std::ostream&
  operator<<(std::ostream& os, const mat3_t& M);

  mat3_t
  operator*(float s, const mat3_t& M);
  mat3_t
  operator/(const mat3_t& M, float s);

  struct mat4_t {
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
    operator==(const mat4_t& M) const;
    const bool
    operator!=(const mat4_t& M) const;
    mat4_t&
    operator*=(float s);
    mat4_t&
    operator/=(float s);
    mat4_t&
    operator+=(const mat4_t& M);
    mat4_t&
    operator-=(const mat4_t& M);
    mat4_t
    operator+(const mat4_t& M) const;
    mat4_t
    operator-() const;
    mat4_t
    operator-(const mat4_t& M) const;
    mat4_t
    operator*(const mat4_t& M) const;
    vec4_t
    operator*(const vec4_t& v) const;
    //
    static mat4_t
    rotx(float angle);
    static mat4_t
    roty(float angle);
    static mat4_t
    rotz(float angle);
    static mat4_t
    rot(const vec4_t& n, float angle);
    static mat4_t
    identity();
    static mat4_t
    projection(float fov, float aspect, float znear, float zfar);
  };

  std::ostream&
  operator<<(std::ostream& os, const mat4_t& M);

  mat4_t
  operator*(float s, const mat4_t& M);
  mat4_t
  operator/(const mat4_t& M, float s);
} // namespace Base
} // namespace Temple
