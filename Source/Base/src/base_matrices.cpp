#include "base_matrices.h"

#include <iomanip>
#include <cmath>

// mat3

Temple::Base::mat3::mat3() {
	data[0][0] = 0.0f; 
	data[0][1] = 0.0f; 
	data[0][2] = 0.0f;

	data[1][0] = 0.0f; 
	data[1][1] = 0.0f; 
	data[1][2] = 0.0f;

	data[2][0] = 0.0f; 
	data[2][1] = 0.0f; 
	data[2][2] = 0.0f;
}

Temple::Base::mat3::mat3(float m00, float m01, float m02,
	 					 float m10, float m11, float m12,
	 					 float m20, float m21, float m22) {
	data[0][0] = m00; 
	data[0][1] = m01; 
	data[0][2] = m02;

	data[1][0] = m10; 
	data[1][1] = m11; 
	data[1][2] = m12;

	data[2][0] = m20; 
	data[2][1] = m21; 
	data[2][2] = m22;
}

Temple::Base::mat3::mat3(const vec3& a,
						 const vec3& b,
						 const vec3& c) {
	r0.x = a.x; 
	r0.y = a.y; 
	r0.z = a.z;

	r1.x = b.x; 
	r1.y = b.y; 
	r1.z = b.z;

	r2.x = c.x; 
	r2.y = c.y; 
	r2.z = c.z;
}

Temple::Base::mat3::mat3(const mat3& M) {
	data[0][0] = M.data[0][0]; 
	data[0][1] = M.data[0][1]; 
	data[0][2] = M.data[0][2];

	data[1][0] = M.data[1][0]; 
	data[1][1] = M.data[1][1]; 
	data[1][2] = M.data[1][2];

	data[2][0] = M.data[2][0]; 
	data[2][1] = M.data[2][1]; 
	data[2][2] = M.data[2][2];
}

Temple::Base::mat3::mat3(mat3&& M) noexcept {
	data[0][0] = M.data[0][0]; 
	data[0][1] = M.data[0][1]; 
	data[0][2] = M.data[0][2];

	data[1][0] = M.data[1][0]; 
	data[1][1] = M.data[1][1]; 
	data[1][2] = M.data[1][2];

	data[2][0] = M.data[2][0]; 
	data[2][1] = M.data[2][1]; 
	data[2][2] = M.data[2][2];
}

Temple::Base::mat3& Temple::Base::mat3::operator =(const mat3& M) {
	data[0][0] = M.data[0][0];
    data[0][1] = M.data[1][0];
    data[0][2] = M.data[2][0];

	data[1][0] = M.data[0][1];
    data[1][1] = M.data[1][1];
    data[1][2] = M.data[2][1];

	data[2][0] = M.data[0][2];
    data[2][1] = M.data[1][2];
    data[2][2] = M.data[2][2];
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator =(mat3&& M) noexcept {
	data[0][0] = M.data[0][0];
	data[0][1] = M.data[0][1];
	data[0][2] = M.data[0][2];

	data[1][0] = M.data[1][0];
    data[1][1] = M.data[1][1];
    data[1][2] = M.data[1][2];

	data[2][0] = M.data[2][0];
    data[2][1] = M.data[2][1];
    data[2][2] = M.data[2][2];
	return *this;
}

Temple::Base::vec3& Temple::Base::mat3::operator[](int i) {
	return (&r0)[i];
}

const Temple::Base::vec3& Temple::Base::mat3::operator[](int i) const {
	return (&r0)[i];
}

float& Temple::Base::mat3::operator()(int i, int j) {
	return data[i][j];
}

const float& Temple::Base::mat3::operator()(int i, int j) const {
	return data[i][j];
}

const bool Temple::Base::mat3::operator==(const mat3& M) const {
	return r0 == M[0] && r1 == M[1] && r2 == M[2];
}

const bool Temple::Base::mat3::operator!=(const mat3& M) const {
	return !((*this) == M);
}

Temple::Base::mat3& Temple::Base::mat3::operator *=(float s) {
	data[0][0] *= s;
	data[0][1] *= s;
	data[0][2] *= s;

	data[1][0] *= s;
	data[1][1] *= s;
	data[1][2] *= s;

	data[2][0] *= s;
	data[2][1] *= s;
	data[2][2] *= s;
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator /=(float s) {
	s = 1.0f / s;
	(*this) *= s;
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator +=(const mat3& M) {
	data[0][0] += M.data[0][0];
    data[0][1] += M.data[0][1];
    data[0][2] += M.data[0][2];

	data[1][0] += M.data[1][0];
    data[1][1] += M.data[1][1];
    data[1][2] += M.data[1][2];

	data[2][0] += M.data[2][0];
    data[2][1] += M.data[2][1];
    data[2][2] += M.data[2][2];
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator -=(const mat3& M) {
	data[0][0] -= M.data[0][0];
    data[0][1] -= M.data[0][1];
    data[0][2] -= M.data[0][2];

	data[1][0] -= M.data[1][0];
    data[1][1] -= M.data[1][1];
    data[1][2] -= M.data[1][2];

	data[2][0] -= M.data[2][0];
    data[2][1] -= M.data[2][1];
    data[2][2] -= M.data[2][2];
	return *this;
}

Temple::Base::mat3 Temple::Base::mat3::operator +(const mat3& M) const {
	return mat3(data[0][0] + M.data[0][0], 
				data[0][1] + M.data[0][1], 
				data[0][2] + M.data[0][2],
				data[1][0] + M.data[1][0], 
				data[1][1] + M.data[1][1], 
				data[1][2] + M.data[1][2],
				data[2][0] + M.data[2][0], 
				data[2][1] + M.data[2][1], 
				data[2][2] + M.data[2][2]);
}

Temple::Base::mat3 Temple::Base::mat3::operator -() const {
	return mat3(-data[0][0], 
				-data[0][1], 
				-data[0][2],
				-data[1][0], 
				-data[1][1], 
				-data[1][2],
				-data[2][0], 
				-data[2][1], 
				-data[2][2]);
}

Temple::Base::mat3 Temple::Base::mat3::operator -(const mat3& M) const {
	return mat3(data[0][0] - M.data[0][0], 
				data[0][1] - M.data[0][1], 
				data[0][2] - M.data[0][2],
				data[1][0] - M.data[1][0], 
				data[1][1] - M.data[1][1], 
				data[1][2] - M.data[1][2],
				data[2][0] - M.data[2][0], 
				data[2][1] - M.data[2][1], 
				data[2][2] - M.data[2][2]);
}

Temple::Base::mat3 Temple::Base::mat3::operator *(const mat3& M) const {
	return mat3(data[0][0] * M.data[0][0] + data[0][1] * M.data[1][0] + data[0][2] * M.data[2][0], 
				data[0][0] * M.data[0][1] + data[0][1] * M.data[1][1] + data[0][2] * M.data[2][1], 
				data[0][0] * M.data[0][2] + data[0][1] * M.data[1][2] + data[0][2] * M.data[2][2],
				data[1][0] * M.data[0][0] + data[1][1] * M.data[1][0] + data[1][2] * M.data[2][0], 
				data[1][0] * M.data[0][1] + data[1][1] * M.data[1][1] + data[1][2] * M.data[2][1], 
				data[1][0] * M.data[0][2] + data[1][1] * M.data[1][2] + data[1][2] * M.data[2][2],
				data[2][0] * M.data[0][0] + data[2][1] * M.data[1][0] + data[2][2] * M.data[2][0], 
				data[2][0] * M.data[0][1] + data[2][1] * M.data[1][1] + data[2][2] * M.data[2][1],
				data[2][0] * M.data[0][2] + data[2][1] * M.data[1][2] + data[2][2] * M.data[2][2]);
}

Temple::Base::vec3 Temple::Base::mat3::operator *(const vec3& v) const {
	return vec3(data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z,
				data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z,
				data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z);
}

std::ostream& Temple::Base::operator<<(std::ostream& os, const mat3& M) {
	os << std::fixed << std::setprecision(4) << "[ " << M(0, 0) << ' ' << M(0, 1) << ' ' << M(0, 2) << '\n' <<
												"  " << M(1, 0) << ' ' << M(1, 1) << ' ' << M(1, 2) << '\n' <<
												"  " << M(2, 0) << ' ' << M(2, 1) << ' ' << M(2, 2) << " ]";
	return os;
}

Temple::Base::mat3 Temple::Base::operator *(float s, const Temple::Base::mat3& M) {
	return mat3(M(0, 0) * s, M(0, 1) * s, M(0, 2) * s,
				M(1, 0) * s, M(1, 1) * s, M(1, 2) * s,
				M(2, 0) * s, M(2, 1) * s, M(2, 2) * s);
}

Temple::Base::mat3 Temple::Base::operator /(const Temple::Base::mat3& M, float s) {
	s = 1.0f / s;
	return s * M;
}

Temple::Base::mat3 Temple::Base::mat3::rotx(float angle) {
	return mat3(1.0f,		0.0f,		 0.0f, 
				0.0f, cos(angle), -sin(angle), 
				0.0f, sin(angle),  cos(angle));
}

Temple::Base::mat3 Temple::Base::mat3::roty(float angle) {
	return mat3( cos(angle), 0.0f, sin(angle), 
				 0.0f,		 1.0f,		 0.0f, 
			    -sin(angle), 0.0f, cos(angle));
}

Temple::Base::mat3 Temple::Base::mat3::rotz(float angle) {
	return mat3(cos(angle), -sin(angle), 0.0f, 
				sin(angle),  cos(angle), 0.0f, 
					  0.0f,		   0.0f, 1.0f);
}

Temple::Base::mat3 Temple::Base::mat3::rot(const Temple::Base::vec3& n, float angle) {
	float R00 = cos(angle) + n.x * n.x * (1.0f - cos(angle));
	float R01 = n.x * n.y * (1.0f - cos(angle)) - n.z * sin(angle);
	float R02 = n.x * n.z * (1.0f - cos(angle)) + n.y * sin(angle);

	float R10 = n.y * n.x * (1.0f - cos(angle)) + n.z * sin(angle);
	float R11 = cos(angle) + n.y * n.y * (1.0f - cos(angle));
	float R12 = n.y * n.z * (1 - cos(angle)) - n.x * sin(angle);

	float R20 = n.z * n.x * (1.0f - cos(angle)) - n.y * sin(angle);
	float R21 = n.z * n.y * (1.0f - cos(angle)) + n.x * sin(angle);
	float R22 = cos(angle) + n.z * n.z * (1 - cos(angle));

	return mat3(R00, R01, R02, 
				R10, R11, R12, 
				R20, R21, R22);
}

Temple::Base::mat3 Temple::Base::mat3::identity() {
    mat3 m;
    m.r0.x = 1.0f;
    m.r1.y = 1.0f;
    m.r2.z = 1.0f;
    return m;
}

// mat4 

Temple::Base::mat4::mat4() {
	data[0][0] = 0.0f;
	data[0][1] = 0.0f;
	data[0][2] = 0.0f;
	data[0][3] = 0.0f;

	data[1][0] = 0.0f;
	data[1][1] = 0.0f;
	data[1][2] = 0.0f;
	data[1][3] = 0.0f;

	data[2][0] = 0.0f;
	data[2][1] = 0.0f;
	data[2][2] = 0.0f;
	data[2][3] = 0.0f;

	data[3][0] = 0.0f;
	data[3][1] = 0.0f;
	data[3][2] = 0.0f;
	data[3][3] = 0.0f;
}

Temple::Base::mat4::mat4(float m00, float m01, float m02, float m03,
						 float m10, float m11, float m12, float m13,
						 float m20, float m21, float m22, float m23, 
						 float m30, float m31, float m32, float m33) {
	data[0][0] = m00;
	data[0][1] = m01;
    data[0][2] = m02;
    data[0][3] = m03;

	data[1][0] = m10;
    data[1][1] = m11;
    data[1][2] = m12;
    data[1][3] = m13;

	data[2][0] = m20;
    data[2][1] = m21;
    data[2][2] = m22;
    data[2][3] = m23;

	data[3][0] = m30;
    data[3][1] = m31;
    data[3][2] = m32;
    data[3][3] = m33;
}

Temple::Base::mat4::mat4(const vec4& a,
						 const vec4& b,
						 const vec4& c, 
						 const vec4& d) {
	r0.x = a.x;
	r0.y = a.y;
	r0.z = a.z;
	r0.w = a.w;

	r1.x = b.x;
	r1.y = b.y;
	r1.z = b.z;
	r1.w = b.w;

	r2.x = c.x;
	r2.y = c.y;
	r2.z = c.z;
	r2.w = c.w;

	r3.x = d.x;
	r3.y = d.y;
	r3.z = d.z;
	r3.w = d.w;
}

Temple::Base::mat4::mat4(const mat4& M) {
	data[0][0] = M.data[0][0];
	data[0][1] = M.data[0][1];
	data[0][2] = M.data[0][2];
	data[0][3] = M.data[0][3];

	data[1][0] = M.data[1][0];
	data[1][1] = M.data[1][1];
	data[1][2] = M.data[1][2];
	data[1][3] = M.data[1][3];

	data[2][0] = M.data[2][0];
	data[2][1] = M.data[2][1];
	data[2][2] = M.data[2][2];
	data[2][3] = M.data[2][3];

	data[3][0] = M.data[3][0];
	data[3][1] = M.data[3][1];
	data[3][2] = M.data[3][2];
	data[3][3] = M.data[3][3];
}

Temple::Base::mat4::mat4(mat4&& M) noexcept {
	data[0][0] = M.data[0][0];
	data[0][1] = M.data[0][1];
	data[0][2] = M.data[0][2];
	data[0][3] = M.data[0][3];

	data[1][0] = M.data[1][0];
	data[1][1] = M.data[1][1];
	data[1][2] = M.data[1][2];
	data[1][3] = M.data[1][3];

	data[2][0] = M.data[2][0];
	data[2][1] = M.data[2][1];
	data[2][2] = M.data[2][2];
	data[2][3] = M.data[2][3];

	data[3][0] = M.data[3][0];
	data[3][1] = M.data[3][1];
	data[3][2] = M.data[3][2];
	data[3][3] = M.data[3][3];
}

Temple::Base::mat4& Temple::Base::mat4::operator =(const mat4& M) {
	data[0][0] = M.data[0][0];
    data[0][1] = M.data[0][1];
    data[0][2] = M.data[0][2];
    data[0][3] = M.data[0][3];

	data[1][0] = M.data[1][0];
    data[1][1] = M.data[1][1];
    data[1][2] = M.data[1][2];
    data[1][3] = M.data[1][3];

	data[2][0] = M.data[2][0];
    data[2][1] = M.data[2][1];
    data[2][2] = M.data[2][2];
    data[2][3] = M.data[2][3];

	data[3][0] = M.data[3][0];
    data[3][1] = M.data[3][1];
    data[3][2] = M.data[3][2];
    data[3][3] = M.data[3][3];
	return *this;
}

Temple::Base::mat4& Temple::Base::mat4::operator =(mat4&& M) noexcept {
    data[0][0] = M.data[0][0];
    data[0][1] = M.data[0][1];
    data[0][2] = M.data[0][2];
    data[0][3] = M.data[0][3];

    data[1][0] = M.data[1][0];
    data[1][1] = M.data[1][1];
    data[1][2] = M.data[1][2];
    data[1][3] = M.data[1][3];

    data[2][0] = M.data[2][0];
    data[2][1] = M.data[2][1];
    data[2][2] = M.data[2][2];
    data[2][3] = M.data[2][3];

    data[3][0] = M.data[3][0];
    data[3][1] = M.data[3][1];
    data[3][2] = M.data[3][2];
    data[3][3] = M.data[3][3];
	return *this;
}

Temple::Base::vec4& Temple::Base::mat4::operator[](int i) {
	return (&r0)[i];
}

const Temple::Base::vec4& Temple::Base::mat4::operator[](int i) const {
	return (&r0)[i];
}

float& Temple::Base::mat4::operator()(int i, int j) {
	return data[i][j];
}

const float& Temple::Base::mat4::operator()(int i, int j) const {
	return data[i][j];
}

const bool Temple::Base::mat4::operator==(const mat4& M) const {
	return r0 == M[0] && r1 == M[1] && r2 == M[2] && r3 == M[3];
}

const bool Temple::Base::mat4::operator!=(const mat4& M) const {
	return !((*this) == M);
}

Temple::Base::mat4& Temple::Base::mat4::operator *=(float s) {
	data[0][0] *= s;
	data[0][1] *= s;
	data[0][2] *= s;
	data[0][3] *= s;

	data[1][0] *= s;
	data[1][1] *= s;
	data[1][2] *= s;
	data[1][3] *= s;

	data[2][0] *= s;
	data[2][1] *= s;
	data[2][2] *= s;
	data[2][3] *= s;

	data[3][0] *= s;
	data[3][1] *= s;
	data[3][2] *= s;
	data[3][3] *= s;
	return *this;
}

Temple::Base::mat4& Temple::Base::mat4::operator /=(float s) {
	s = 1.0f / s;
	(*this) *= s;
	return *this;
}

Temple::Base::mat4& Temple::Base::mat4::operator +=(const mat4& M) {
	data[0][0] += M.data[0][0];
    data[0][1] += M.data[0][1];
    data[0][2] += M.data[0][2];
    data[0][3] += M.data[0][3];

	data[1][0] += M.data[1][0];
    data[1][1] += M.data[1][1];
    data[1][2] += M.data[1][2];
    data[1][3] += M.data[1][3];

	data[2][0] += M.data[2][0];
    data[2][1] += M.data[2][1];
    data[2][2] += M.data[2][2];
    data[2][3] += M.data[2][3];

	data[3][0] += M.data[3][0];
    data[3][1] += M.data[3][1];
    data[3][2] += M.data[3][2];
    data[3][3] += M.data[3][3];
	return *this;
}

Temple::Base::mat4& Temple::Base::mat4::operator -=(const mat4& M) {
	data[0][0] -= M.data[0][0];
    data[0][1] -= M.data[0][1];
    data[0][2] -= M.data[0][2];
    data[0][3] -= M.data[0][3];

	data[1][0] -= M.data[1][0];
    data[1][1] -= M.data[1][1];
    data[1][2] -= M.data[1][2];
    data[1][3] -= M.data[1][3];

	data[2][0] -= M.data[2][0];
    data[2][1] -= M.data[2][1];
    data[2][2] -= M.data[2][2];
    data[2][3] -= M.data[2][3];

	data[3][0] -= M.data[3][0];
    data[3][1] -= M.data[3][1];
    data[3][2] -= M.data[3][2];
    data[3][3] -= M.data[3][3];
	return *this;
}

Temple::Base::mat4 Temple::Base::mat4::operator +(const mat4& M) const {
	return mat4(data[0][0] + M.data[0][0],
				data[0][1] + M.data[0][1],
				data[0][2] + M.data[0][2],
				data[0][3] + M.data[0][3],
				data[1][0] + M.data[1][0],
				data[1][1] + M.data[1][1],
				data[1][2] + M.data[1][2],
				data[1][3] + M.data[1][3],
				data[2][0] + M.data[2][0],
				data[2][1] + M.data[2][1],
				data[2][2] + M.data[2][2],
				data[2][3] + M.data[2][3],
				data[3][0] + M.data[3][0],
				data[3][1] + M.data[3][1],
				data[3][2] + M.data[3][2],
				data[3][3] + M.data[3][3]);
}

Temple::Base::mat4 Temple::Base::mat4::operator -() const { // [dim] TODO
	return mat4(-data[0][0],
				-data[0][1],
				-data[0][2],
				-data[0][3],
				-data[1][0],
				-data[1][1],
				-data[1][2],
				-data[1][3],
				-data[2][0],
				-data[2][1],
				-data[2][2],
				-data[2][3],
				-data[3][0],
				-data[3][1],
				-data[3][2],
				-data[3][3]);
}

Temple::Base::mat4 Temple::Base::mat4::operator -(const mat4& M) const {
	return mat4(data[0][0] - M.data[0][0],
				data[0][1] - M.data[0][1],
				data[0][2] - M.data[0][2],
				data[0][3] - M.data[0][3],
				data[1][0] - M.data[1][0],
				data[1][1] - M.data[1][1],
				data[1][2] - M.data[1][2],
				data[1][3] - M.data[1][3],
				data[2][0] - M.data[2][0],
				data[2][1] - M.data[2][1],
				data[2][2] - M.data[2][2],
				data[2][3] - M.data[2][3],
				data[3][0] - M.data[3][0],
				data[3][1] - M.data[3][1],
				data[3][2] - M.data[3][2],
				data[3][3] - M.data[3][3]);
}

Temple::Base::mat4 Temple::Base::mat4::operator *(const mat4& M) const {
	return mat4(data[0][0] * M.data[0][0] + data[0][1] * M.data[1][0] + data[0][2] * M.data[2][0] + data[0][3] * M.data[3][0],
				data[0][0] * M.data[0][1] + data[0][1] * M.data[1][1] + data[0][2] * M.data[2][1] + data[0][3] * M.data[3][1],
				data[0][0] * M.data[0][2] + data[0][1] * M.data[1][2] + data[0][2] * M.data[2][2] + data[0][3] * M.data[3][2],
				data[0][0] * M.data[0][3] + data[0][1] * M.data[1][3] + data[0][2] * M.data[2][3] + data[0][3] * M.data[3][3],

				data[1][0] * M.data[0][0] + data[1][1] * M.data[1][0] + data[1][2] * M.data[2][0] + data[1][3] * M.data[3][0],
				data[1][0] * M.data[0][1] + data[1][1] * M.data[1][1] + data[1][2] * M.data[2][1] + data[1][3] * M.data[3][1],
				data[1][0] * M.data[0][2] + data[1][1] * M.data[1][2] + data[1][2] * M.data[2][2] + data[1][3] * M.data[3][2],
				data[1][0] * M.data[0][3] + data[1][1] * M.data[1][3] + data[1][2] * M.data[2][3] + data[1][3] * M.data[3][3],

				data[2][0] * M.data[0][0] + data[2][1] * M.data[1][0] + data[2][2] * M.data[2][0] + data[2][3] * M.data[3][0],
				data[2][0] * M.data[0][1] + data[2][1] * M.data[1][1] + data[2][2] * M.data[2][1] + data[2][3] * M.data[3][1],
				data[2][0] * M.data[0][2] + data[2][1] * M.data[1][2] + data[2][2] * M.data[2][2] + data[2][3] * M.data[3][2],
				data[2][0] * M.data[0][3] + data[2][1] * M.data[1][3] + data[2][2] * M.data[2][3] + data[2][3] * M.data[3][3],

				data[3][0] * M.data[0][0] + data[3][1] * M.data[1][0] + data[3][2] * M.data[2][0] + data[3][3] * M.data[3][0],
				data[3][0] * M.data[0][1] + data[3][1] * M.data[1][1] + data[3][2] * M.data[2][1] + data[3][3] * M.data[3][1],
				data[3][0] * M.data[0][2] + data[3][1] * M.data[1][2] + data[3][2] * M.data[2][2] + data[3][3] * M.data[3][2],
				data[3][0] * M.data[0][3] + data[3][1] * M.data[1][3] + data[3][2] * M.data[2][3] + data[3][3] * M.data[3][3]);
}

Temple::Base::vec4 Temple::Base::mat4::operator *(const vec4& v) const {
	return vec4(data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z + data[0][3] * v.w,
				data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z + data[1][3] * v.w,
				data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z + data[2][3] * v.w,
				data[3][0] * v.x + data[3][1] * v.y + data[3][2] * v.z + data[3][3] * v.w);
}

std::ostream& Temple::Base::operator<<(std::ostream& os, const mat4& M) {
	os << std::fixed << std::setprecision(4) << "[ " << M(0, 0) << ' ' << M(0, 1) << ' ' << M(0, 2) << ' ' << M(0, 3) << '\n' <<
		"  " << M(1, 0) << ' ' << M(1, 1) << ' ' << M(1, 2) << ' ' << M(1, 3) << '\n' <<
		"  " << M(2, 0) << ' ' << M(2, 1) << ' ' << M(2, 2) << ' ' << M(2, 3) << '\n' <<
		"  " << M(3, 0) << ' ' << M(3, 1) << ' ' << M(3, 2) << ' ' << M(3, 3) << " ]";
	return os;
}

Temple::Base::mat4 Temple::Base::operator *(float s, const Temple::Base::mat4& M) {
	return mat4(M.data[0][0] * s, M.data[0][1] * s, M.data[0][2] * s, M.data[0][3] * s,
				M.data[1][0] * s, M.data[1][1] * s, M.data[1][2] * s, M.data[1][3] * s,
				M.data[2][0] * s, M.data[2][1] * s, M.data[2][2] * s, M.data[2][3] * s,
				M.data[3][0] * s, M.data[3][1] * s, M.data[3][2] * s, M.data[3][3] * s);
}

Temple::Base::mat4 Temple::Base::operator /(const Temple::Base::mat4& M, float s) {
	s = 1.0f / s;
	return s * M;
}

Temple::Base::mat4 Temple::Base::mat4::rotx(float angle) {
	return mat4(1.0f,		0.0f,		 0.0f, 0.0f,
				0.0f, cos(angle), -sin(angle), 0.0f,
				0.0f, sin(angle),  cos(angle), 0.0f,
				0.0f,		0.0f,		 0.0f, 1.0f);
}

Temple::Base::mat4 Temple::Base::mat4::roty(float angle) {
	return mat4( cos(angle), 0.0f, sin(angle), 0.0f,
					   0.0f, 1.0f,		 0.0f, 0.0f,
			    -sin(angle), 0.0f, cos(angle), 0.0f,
					  0.0f,  0.0f,		 0.0f, 1.0f);
}

Temple::Base::mat4 Temple::Base::mat4::rotz(float angle) {
	return mat4(cos(angle), -sin(angle), 0.0f, 0.0f,
				sin(angle),  cos(angle), 0.0f, 0.0f,
					  0.0f,		   0.0f, 1.0f, 0.0f,
					  0.0f,		   0.0f, 0.0f, 1.0f);
}

Temple::Base::mat4 Temple::Base::mat4::rot(const Temple::Base::vec4& n, float angle) {
	float R00 = cos(angle) + n.x * n.x * (1.0f - cos(angle));
	float R01 = n.x * n.y * (1.0f - cos(angle)) - n.z * sin(angle);
	float R02 = n.x * n.z * (1.0f - cos(angle)) + n.y * sin(angle);

	float R10 = n.y * n.x * (1.0f - cos(angle)) + n.z * sin(angle);
	float R11 = cos(angle) + n.y * n.y * (1.0f - cos(angle));
	float R12 = n.y * n.z * (1 - cos(angle)) - n.x * sin(angle);

	float R20 = n.z * n.x * (1.0f - cos(angle)) - n.y * sin(angle);
	float R21 = n.z * n.y * (1.0f - cos(angle)) + n.x * sin(angle);
	float R22 = cos(angle) + n.z * n.z * (1 - cos(angle));

	return mat4(R00,  R01,  R02,  0.0f,
				R10,  R11,  R12,  0.0f,
				R20,  R21,  R22,  0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
}

Temple::Base::mat4 Temple::Base::mat4::identity() {
    mat4 m;
    m.r0.x = 1.0f;
    m.r1.y = 1.0f;
    m.r2.z = 1.0f;
    m.r3.w = 1.0f;
    return m;
}

Temple::Base::mat4 Temple::Base::mat4::projection(float fov, float aspect, float znear, float zfar) {
    mat4 m = identity();
    // take aspect into account + fov
    // only fov in range of somewhat 45<fov<180 is described correctly - right limit is crucial, ctg of 90 is zero, nothing is seen.
    // fov / 2 in radians
    fov = fov * 0.5f;
    m.r1.y = cosf(fov) / sinf(fov); // ctg(fov), use vertical field of view. In case of 90 degrees it is just 1 as it was before fov addition
    m.r0.x = m.r1.y / aspect;
    // compress x and y according to perspective positions during canonical transformation (dividing by w) 
    // for that purpose z should be placed into w - far from znear - closer to center
    m.r3.z = 1.0f;
    m.r3.w = 0.0f;
    // change z-coordinate from scene to NDC
    // there are two possible options:
    //      1. Default:
    // m.r2.z = zfar / (zfar - znear);
    // m.r2.w = (-znear * zfar) / (zfar - znear);
    // left-handed coordinate system to map z/w to range from 0 at near plane to 1 at far plane
    // this is classic transformation, z-value now contains normalized depth of the point 
    // but it is not distributed uniformly - around znear precision is fine, closer to zfar - precision is low
    //      2. Reverse-Z:
    m.r2.z = znear / (znear - zfar);
    m.r2.w = (-znear * zfar) / (znear - zfar);
    // this transformation maps z to 1 at znear and to 0 at zfar
    // such trick allows to fix low precision and non-linear nature of previous transformation
    // z-itself after classical perspective transformation is not linear, but 1/z is!
    // after multiplying w contains divider that shoud not be modified and used for division to obtain final transformations
    return m;
}
