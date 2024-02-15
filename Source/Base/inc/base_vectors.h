#pragma once

#include "base_constants.h"
#include "base_utils.h"

#include <cmath>
#include <iostream>

namespace Temple
{
namespace Base
{
  struct vec2_t {
    union {
      struct {
        float x, y;
      };
      struct {
        float u, v;
      };
      float data[2];
    };
    inline const float&
    operator[](int i) const
    {
      return (&x)[i]; // can use offset from first property
    }
    inline float&
    operator[](int i)
    {
      return (&x)[i];
    }
    inline vec2_t&
    operator*=(float s)
    {
      x *= s;
      y *= s;
      return *this;
    }
    inline vec2_t
    operator*(float s)
    {
      return vec2_t {x * s, y * s};
    }
    inline vec2_t&
    operator/=(float s)
    {
      s = 1.0f / s;
      x *= s;
      y *= s;
      return *this;
    }
    inline vec2_t
    operator/(float s)
    {
      s = 1.0f / s;
      return vec2_t {x * s, y * s};
    }
    inline vec2_t&
    operator+=(const vec2_t& v)
    {
      x += v.x;
      y += v.y;
      return *this;
    }
    inline vec2_t&
    operator-=(const vec2_t& v)
    {
      x -= v.x;
      y -= v.y;
      return *this;
    }
    inline const bool
    operator==(const vec2_t& v) const
    {
      float l = (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y);
      return l < Base::eps;
    }
    inline const bool
    operator!=(const vec2_t& v) const
    {
      return !(v == (*this));
    }
    inline const float
    calculate_magnitude() const
    {
      return sqrt(x * x + y * y);
    }
    inline vec2_t
    calculate_normalized()
    {
      float l = this->calculate_magnitude();
      l = 1.0f / l;
      return vec2_t {x * l, y * l};
    }
  };
  inline vec2_t
  operator-(const vec2_t& v)
  {
    return vec2_t {-v.x, -v.y};
  }
  inline vec2_t
  operator*(const vec2_t& v, float s)
  {
    return vec2_t {v.x * s, v.y * s};
  }
  inline vec2_t
  operator*(const vec2_t& v, const vec2_t& u)
  {
    return vec2_t {v.x * u.x, v.y * u.y};
  }
  inline vec2_t
  operator/(const vec2_t& v, float s)
  {
    s = 1.0f / s;
    return vec2_t {v.x * s, v.y * s};
  }
  inline vec2_t
  operator+(const vec2_t& v, const vec2_t& u)
  {
    return vec2_t {v.x + u.x, v.y + u.y};
  }
  inline vec2_t
  operator+(const vec2_t& v, float s)
  {
    return vec2_t {v.x + s, v.y + s};
  }
  inline vec2_t
  operator-(const vec2_t& v, const vec2_t& u)
  {
    return vec2_t {v.x - u.x, v.y - u.y};
  }
  inline vec2_t
  operator-(const vec2_t& v, float s)
  {
    return vec2_t {v.x - s, v.y - s};
  }
  std::ostream&
  operator<<(std::ostream& os, const vec2_t& v);

  struct vec3_t {
    union {
      struct {
        float x, y, z;
      };
      struct {
        float r, g, b;
      };
      float data[3];
    };
    inline const float&
    operator[](int i) const
    {
      return (&x)[i]; // can use offset from first property
    }
    inline float&
    operator[](int i)
    {
      return (&x)[i];
    }
    inline vec3_t&
    operator*=(float s)
    {
      x *= s;
      y *= s;
      z *= s;
      return *this;
    }
    inline vec3_t
    operator*(float s)
    {
      return vec3_t {x * s, y * s, z * s};
    }
    inline vec3_t&
    operator/=(float s)
    {
      s = 1.0f / s;
      x *= s;
      y *= s;
      z *= s;
      return *this;
    }
    inline vec3_t
    operator/(float s)
    {
      s = 1.0f / s;
      return vec3_t {x * s, y * s, z * s};
    }
    inline vec3_t&
    operator+=(const vec3_t& v)
    {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }
    inline vec3_t&
    operator-=(const vec3_t& v)
    {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }
    inline const bool
    operator==(const vec3_t& v) const
    {
      float l = sqrt(
        (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
      return l < Base::eps;
    }
    inline const bool
    operator!=(const vec3_t& v) const
    {
      return !(v == (*this));
    }
    inline const float
    calculate_magnitude() const
    {
      return sqrt(x * x + y * y + z * z);
    }
    inline vec3_t
    calculate_normalized()
    {
      float l = this->calculate_magnitude();
      l = 1.0f / l;
      return vec3_t {x * l, y * l, z * l};
    }
    inline float
    calculate_dot_product(const vec3_t& v) const
    {
      return x * v.x + y * v.y + z * v.z;
    }
    inline vec3_t
    calculate_cross_product(const vec3_t& v) const
    {
      return vec3_t {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
    }
  };
  inline vec3_t
  operator-(const vec3_t& v)
  {
    return vec3_t {-v.x, -v.y, -v.z};
  }
  inline vec3_t
  operator*(const vec3_t& v, float s)
  {
    return vec3_t {v.x * s, v.y * s, v.z * s};
  }
  inline vec3_t
  operator*(const vec3_t& v, const vec3_t& u)
  {
    return vec3_t {v.x * u.x, v.y * u.y, v.z * u.z};
  }
  inline vec3_t
  operator/(const vec3_t& v, float s)
  {
    s = 1.0f / s;
    return vec3_t {v.x * s, v.y * s, v.z * s};
  }
  inline vec3_t
  operator+(const vec3_t& v, const vec3_t& u)
  {
    return vec3_t {v.x + u.x, v.y + u.y, v.z + u.z};
  }
  inline vec3_t
  operator+(const vec3_t& v, float s)
  {
    return vec3_t {v.x + s, v.y + s, v.z + s};
  }
  inline vec3_t
  operator-(const vec3_t& v, const vec3_t& u)
  {
    return vec3_t {v.x - u.x, v.y - u.y, v.z - u.z};
  }
  inline vec3_t
  operator-(const vec3_t& v, float s)
  {
    return vec3_t {v.x - s, v.y - s, v.z - s};
  }
  std::ostream&
  operator<<(std::ostream& os, const vec3_t& v);

  struct vec4_t {
    union {
      struct {
        float x, y, z, w;
      };
      struct {
        float r, g, b, a;
      };
      struct {
        vec3_t xyz;
        float  w;
      };
      float data[4];
    };
    inline const float&
    operator[](int i) const
    {
      return (&x)[i]; // can use offset from first property
    }
    inline float&
    operator[](int i)
    {
      return (&x)[i];
    }
    inline vec4_t&
    operator*=(float s)
    {
      x *= s;
      y *= s;
      z *= s;
      w *= s;
      return *this;
    }
    inline vec4_t
    operator*(float s)
    {
      return vec4_t {x * s, y * s, z * s, w * s};
    }
    inline vec4_t&
    operator/=(float s)
    {
      s = 1.0f / s;
      x *= s;
      y *= s;
      z *= s;
      w *= s;
      return *this;
    }
    inline vec4_t
    operator/(float s)
    {
      s = 1.0f / s;
      return vec4_t {x * s, y * s, z * s, w * s};
    }
    inline vec4_t&
    operator+=(const vec4_t& v)
    {
      x += v.x;
      y += v.y;
      z += v.z;
      w += v.w;
      return *this;
    }
    inline vec4_t&
    operator-=(const vec4_t& v)
    {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      w -= v.w;
      return *this;
    }
    inline const bool
    operator==(const vec4_t& v) const
    {
      float l = sqrt(
        (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z) +
        (w - v.w) * (w - v.w));
      return l < Base::eps;
    }
    inline const bool
    operator!=(const vec4_t& v) const
    {
      return !(v == (*this));
    }
    inline const float
    calculate_magnitude() const
    {
      return sqrt(x * x + y * y + z * z + w * w);
    }
    inline vec4_t
    calculate_normalized()
    {
      float l = this->calculate_magnitude();
      l = 1.0f / l;
      return vec4_t {x * l, y * l, z * l, w * l};
    }
    inline vec4_t
    clamp(float start, float end)
    {
      vec4_t r;
      for (int i = 0; i < 4; i++)
      {
        r.data[i] = data[i];
        if (r.data[i] < start)
        {
          r.data[i] = start;
        }
        if (r.data[i] > end)
        {
          r.data[i] = end;
        }
      }
      return r;
    }
    inline vec4_t
    calculate_cross_product(const vec4_t& v)
    {
      return vec4_t {
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x,
        0.0f};
    }
  };
  inline vec4_t
  operator-(const vec4_t& v)
  {
    return vec4_t {-v.x, -v.y, -v.z, -v.w};
  }
  inline vec4_t
  operator*(const vec4_t& v, float s)
  {
    return vec4_t {v.x * s, v.y * s, v.z * s, v.w * s};
  }
  inline vec4_t
  operator*(const vec4_t& v, const vec4_t& u)
  {
    return vec4_t {v.x * u.x, v.y * u.y, v.z * u.z, v.w * u.w};
  }
  inline vec4_t
  operator/(const vec4_t& v, float s)
  {
    s = 1.0f / s;
    return vec4_t {v.x * s, v.y * s, v.z * s, v.w * s};
  }
  inline vec4_t
  operator+(const vec4_t& v, const vec4_t& u)
  {
    return vec4_t {v.x + u.x, v.y + u.y, v.z + u.z, v.w + u.w};
  }
  inline vec4_t
  operator+(const vec4_t& v, float s)
  {
    return vec4_t {v.x + s, v.y + s, v.z + s, v.w + s};
  }
  inline vec4_t
  operator-(const vec4_t& v, const vec4_t& u)
  {
    return vec4_t {v.x - u.x, v.y - u.y, v.z - u.z, v.w - u.w};
  }
  inline vec4_t
  operator-(const vec4_t& v, float s)
  {
    return vec4_t {v.x - s, v.y - s, v.z - s, v.w - s};
  }
  std::ostream&
  operator<<(std::ostream& os, const vec4_t& v);
} // namespace Base
} // namespace Temple
