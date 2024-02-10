#pragma once

#include "base_constants.h"
#include "base_utils.h"

#include <cmath>
#include <iostream>

namespace Temple {
    namespace Base {
        struct vec2 {
            union {
                struct {
                    float x, y;
                };
                struct {
                    float u, v;
                };
                float data[2];
            };
            vec2();
            vec2(float a, float b);
            vec2(const vec2& a);
            vec2(vec2&& a);
            vec2& operator=(const vec2& a);
            vec2& operator=(vec2&& a) noexcept;
            inline const float& operator [](int i) const {
                return (&x)[i]; // can use offset from first property
            }
            inline float& operator [](int i) {
                return (&x)[i];
            }
            inline vec2& operator *=(float s) {
                x *= s;
                y *= s;
                return *this;
            }
            inline vec2 operator *(float s) {
                return vec2(x * s, y * s);
            }
            inline vec2& operator /=(float s) {
                s = 1.0f / s;
                x *= s;
                y *= s;
                return *this;
            }
            inline vec2 operator /(float s) {
                s = 1.0f / s;
                return vec2(x * s, y * s);
            }
            inline vec2& operator +=(const vec2& v) {
                x += v.x;
                y += v.y;
                return *this;
            }
            inline vec2& operator -=(const vec2& v) {
                x -= v.x;
                y -= v.y;
                return *this;
            }
            inline const bool operator ==(const vec2& v) const {
                float l = (x - v.x) * (x - v.x) +
                          (y - v.y) * (y - v.y);
                return l < EPS;
            }
            inline const bool operator !=(const vec2& v) const {
                return !(v == (*this));
            }
            inline const float magnitude() const {
                return sqrt(x * x + y * y);
            }
            inline vec2 norm() {
                float l = magnitude();
                l = 1.0f / l;
                return vec2(x * l, y * l);
            }
        };
        inline vec2 operator -(const vec2& v) {
            return vec2(-v.x, -v.y);
        }
        inline vec2 operator *(const vec2& v, float s) {
            return vec2(v.x * s,
                        v.y * s);
        }
        inline vec2 operator *(const vec2& v, const vec2& u) {
            return vec2(v.x * u.x,
                        v.y * u.y);
        }
        inline vec2 operator /(const vec2& v, float s) {
            s = 1.0f / s;
            return vec2(v.x * s,
                        v.y * s);
        }
        inline vec2 operator +(const vec2& v, const vec2& u) {
            return vec2(v.x + u.x,
                        v.y + u.y);
        }
        inline vec2 operator +(const vec2& v, float s) {
            return vec2(v.x + s,
                        v.y + s);
        }
        inline vec2 operator -(const vec2& v, const vec2& u) {
            return vec2(v.x - u.x,
                        v.y - u.y);
        }
        inline vec2 operator -(const vec2& v, float s) {
            return vec2(v.x - s,
                        v.y - s);
        }
        std::ostream& operator <<(std::ostream& os, const vec2& v);

        struct vec3 {
            union {
                struct {
                    float x, y, z;
                };
                struct {
                    float r, g, b;
                };
                float data[3];
            };
            vec3();
            vec3(float a, float b, float c);
            vec3(const vec3& a);
            vec3(vec3&& a);
            vec3& operator=(const vec3& a);
            vec3& operator=(vec3&& a) noexcept;
            inline const float& operator [](int i) const {
                return (&x)[i]; // can use offset from first property
            }
            inline float& operator [](int i) {
                return (&x)[i];
            }
            inline vec3& operator *=(float s) {
                x *= s; 
                y *= s; 
                z *= s;
                return *this;
            }
            inline vec3 operator *(float s) {
                return vec3(x * s, y * s, z * s);
            }
            inline vec3& operator /=(float s) {
                s = 1.0f / s;
                x *= s; 
                y *= s; 
                z *= s;
                return *this;
            }
            inline vec3 operator /(float s) {
                s = 1.0f / s;
                return vec3(x * s, y * s, z * s);
            }
            inline vec3& operator +=(const vec3& v) {
                x += v.x; 
                y += v.y; 
                z += v.z;
                return *this;
            }
            inline vec3& operator -=(const vec3& v) {
                x -= v.x; 
                y -= v.y; 
                z -= v.z;
                return *this;
            }
            inline const bool operator ==(const vec3& v) const {
                float l = sqrt((x - v.x) * (x - v.x) + 
                               (y - v.y) * (y - v.y) + 
                               (z - v.z) * (z - v.z));
                return l < EPS;
            }
            inline const bool operator !=(const vec3& v) const {
                return !(v == (*this));
            }
            inline const float magnitude() const {
                return sqrt(x * x + y * y + z * z);
            }
            inline vec3 norm() {
                float l = magnitude();
                l = 1.0f / l;
                return vec3(x * l, y * l, z * l);
            }
            inline float dot(const vec3& v) const {
                return x * v.x + y * v.y + z * v.z;
            }
            inline vec3 cross(const vec3& v) const {
                return vec3(y * v.z - z * v.y, 
                            z * v.x - x * v.z,
                            x * v.y - y * v.x);
            }
        };
        inline vec3 operator -(const vec3& v) {
            return vec3(-v.x, -v.y, -v.z);
        }
        inline vec3 operator *(const vec3& v, float s) {
            return vec3(v.x * s,
                        v.y * s, 
                        v.z * s);
        }
        inline vec3 operator *(const vec3& v, const vec3& u) {
            return vec3(v.x * u.x, 
                        v.y * u.y, 
                        v.z * u.z);
        }
        inline vec3 operator /(const vec3& v, float s) {
            s = 1.0f / s;
            return vec3(v.x * s, 
                        v.y * s, 
                        v.z * s);
        }
        inline vec3 operator +(const vec3& v, const vec3& u) {
            return vec3(v.x + u.x, 
                        v.y + u.y, 
                        v.z + u.z);
        }
        inline vec3 operator +(const vec3& v, float s) {
            return vec3(v.x + s,
                        v.y + s, 
                        v.z + s);
        }
        inline vec3 operator -(const vec3& v, const vec3& u) {
            return vec3(v.x - u.x, 
                        v.y - u.y, 
                        v.z - u.z);
        }
        inline vec3 operator -(const vec3& v, float s) {
            return vec3(v.x - s,
                        v.y - s, 
                        v.z - s);
        }
        std::ostream& operator <<(std::ostream& os, const vec3& v);

        struct vec4 {
            union {
                struct {
                    float x, y, z, w;
                };
                struct {
                    float r, g, b, a;
                };
                struct {
                    vec3 xyz;
                    float w;
                };
                float data[4];
            };
            vec4();
            vec4(float a, float b, float c, float d);
            vec4(float a, float b, float c);
            vec4(const vec3& v, float w);
            vec4(const vec4& a);
            vec4(vec4&& a);
            vec4& operator=(const vec4& a);
            vec4& operator=(vec4&& a) noexcept;
            inline const float& operator [](int i) const {
                return (&x)[i]; // can use offset from first property
            }
            inline float& operator [](int i) {
                return (&x)[i];
            }
            inline vec4& operator *=(float s) {
                x *= s; 
                y *= s; 
                z *= s; 
                w *= s;
                return *this;
            }
            inline vec4 operator *(float s) {
                return vec4(x * s, y * s, z * s, w * s);
            }
            inline vec4& operator /=(float s) {
                s = 1.0f / s;
                x *= s; 
                y *= s; 
                z *= s; 
                w *= s;
                return *this;
            }
            inline vec4 operator /(float s) {
                s = 1.0f / s;
                return vec4(x * s, y * s, z * s, w * s);
            }
            inline vec4& operator +=(const vec4& v) {
                x += v.x; 
                y += v.y; 
                z += v.z; 
                w += v.w;
                return *this;
            }
            inline vec4& operator -=(const vec4& v) {
                x -= v.x; 
                y -= v.y; 
                z -= v.z; 
                w -= v.w;
                return *this;
            }
            inline const bool operator ==(const vec4& v) const {
                float l = sqrt((x - v.x) * (x - v.x) + 
                               (y - v.y) * (y - v.y) + 
                               (z - v.z) * (z - v.z) + 
                               (w - v.w) * (w - v.w));
                return l < EPS;
            }
            inline const bool operator !=(const vec4& v) const {
                return !(v == (*this));
            }
            inline const float magnitude() const {
                return sqrt(x * x + y * y + z * z + w * w);
            }
            inline vec4 norm() {
                float l = magnitude();
                l = 1.0f / l;
                return vec4(x * l, y * l, z * l, w * l);
            }
            inline vec4 clamp(float start, float end) {
                vec4 r;
                for (int i = 0; i < 4; i++) {
                    r.data[i] = data[i];
                    if (r.data[i] < start) {
                        r.data[i] = start;
                    }
                    if (r.data[i] > end) {
                        r.data[i] = end;
                    }
                }
                return r;
            }
            inline vec4 cross(const vec4& v) {
                return vec4(y * v.z - z * v.y,
                            z * v.x - x * v.z,
                            x * v.y - y * v.x,
                            0.0f);
            }
        };
        inline vec4 operator -(const vec4& v) {
            return vec4(-v.x, -v.y, -v.z, -v.w);
        }
        inline vec4 operator *(const vec4& v, float s) {
            return vec4(v.x * s, 
                        v.y * s, 
                        v.z * s, 
                        v.w * s);
        }
        inline vec4 operator *(const vec4& v, const vec4& u) {
            return vec4(v.x * u.x, 
                        v.y * u.y, 
                        v.z * u.z, 
                        v.w * u.w);
        }
        inline vec4 operator /(const vec4& v, float s) {
            s = 1.0f / s;
            return vec4(v.x * s, 
                        v.y * s, 
                        v.z * s, 
                        v.w * s);
        }
        inline vec4 operator +(const vec4& v, const vec4& u) {
            return vec4(v.x + u.x, 
                        v.y + u.y, 
                        v.z + u.z, 
                        v.w + u.w);
        }
        inline vec4 operator +(const vec4& v, float s) {
            return vec4(v.x + s, 
                        v.y + s, 
                        v.z + s, 
                        v.w + s);
        }
        inline vec4 operator -(const vec4& v, const vec4& u) {
            return vec4(v.x - u.x, 
                        v.y - u.y, 
                        v.z - u.z, 
                        v.w - u.w);
        }
        inline vec4 operator -(const vec4& v, float s) {
            return vec4(v.x - s, 
                        v.y - s, 
                        v.z - s, 
                        v.w - s);
        }
        std::ostream& operator <<(std::ostream& os, const vec4& v);
    }
}


