#include "base_matrices.h"

#include <iomanip>

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
	data[0][1] = m10; 
	data[0][2] = m20;

	data[1][0] = m01; 
	data[1][1] = m11; 
	data[1][2] = m21;

	data[2][0] = m02; 
	data[2][1] = m12; 
	data[2][2] = m22;
}

Temple::Base::mat3::mat3(const vec3& a,
								  const vec3& b,
								  const vec3& c) {
	c0.x = a.x; 
	c0.y = a.y; 
	c0.z = a.z;

	c1.x = b.x; 
	c1.y = b.y; 
	c1.z = b.z;

	c2.x = c.x; 
	c2.y = c.y; 
	c2.z = c.z;
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
	data[0][0] = M(0, 0);
	data[0][1] = M(1, 0);
	data[0][2] = M(2, 0);

	data[1][0] = M(0, 1);
	data[1][1] = M(1, 1);
	data[1][2] = M(2, 1);

	data[2][0] = M(0, 2);
	data[2][1] = M(1, 2);
	data[2][2] = M(2, 2);
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator =(mat3&& M) noexcept {
	data[0][0] = M(0, 0);
	data[0][1] = M(1, 0);
	data[0][2] = M(2, 0);

	data[1][0] = M(0, 1);
	data[1][1] = M(1, 1);
	data[1][2] = M(2, 1);

	data[2][0] = M(0, 2);
	data[2][1] = M(1, 2);
	data[2][2] = M(2, 2);
	return *this;
}

Temple::Base::vec3& Temple::Base::mat3::operator[](int i) {
	return (&c0)[i];
}

const Temple::Base::vec3& Temple::Base::mat3::operator[](int i) const {
	return (&c0)[i];
}

float& Temple::Base::mat3::operator()(int i, int j) {
	return data[j][i];
}

const float& Temple::Base::mat3::operator()(int i, int j) const {
	return data[j][i];
}

const bool Temple::Base::mat3::operator==(const mat3& M) const {
	return c0 == M[0] && c1 == M[1] && c2 == M[2];
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
	data[0][0] += M(0, 0);
	data[0][1] += M(1, 0);
	data[0][2] += M(2, 0);

	data[1][0] += M(0, 1);
	data[1][1] += M(1, 1);
	data[1][2] += M(2, 1);

	data[2][0] += M(0, 2);
	data[2][1] += M(1, 2);
	data[2][2] += M(2, 2);
	return *this;
}

Temple::Base::mat3& Temple::Base::mat3::operator -=(const mat3& M) {
	data[0][0] -= M(0, 0);
	data[0][1] -= M(1, 0);
	data[0][2] -= M(2, 0);

	data[1][0] -= M(0, 1);
	data[1][1] -= M(1, 1);
	data[1][2] -= M(2, 1);

	data[2][0] -= M(0, 2);
	data[2][1] -= M(1, 2);
	data[2][2] -= M(2, 2);
	return *this;
}

Temple::Base::mat3 Temple::Base::mat3::operator +(const mat3& M) const {
	return mat3(data[0][0] + M(0, 0), 
				data[1][0] + M(0, 1), 
				data[2][0] + M(0, 2),
				data[0][1] + M(1, 0), 
				data[1][1] + M(1, 1), 
				data[2][1] + M(1, 2),
				data[0][2] + M(2, 0), 
				data[1][2] + M(2, 1), 
				data[2][2] + M(2, 2));
}

Temple::Base::mat3 Temple::Base::mat3::operator -() const {
	return mat3(-data[0][0], 
				-data[1][0], 
				-data[2][0],
				-data[0][1], 
				-data[1][1], 
				-data[2][1],
				-data[0][2], 
				-data[1][2], 
				-data[2][2]);
}

Temple::Base::mat3 Temple::Base::mat3::operator -(const mat3& M) const {
	return mat3(data[0][0] - M(0, 0), 
				data[1][0] - M(0, 1), 
				data[2][0] - M(0, 2),
				data[0][1] - M(1, 0), 
				data[1][1] - M(1, 1), 
				data[2][1] - M(1, 2),
				data[0][2] - M(2, 0), 
				data[1][2] - M(2, 1), 
				data[2][2] - M(2, 2));
}

Temple::Base::mat3 Temple::Base::mat3::operator *(const mat3& M) const {
	return mat3(data[0][0] * M(0, 0) + data[1][0] * M(1, 0) + data[2][0] * M(2, 0), 
				data[0][0] * M(0, 1) + data[1][0] * M(1, 1) + data[2][0] * M(2, 1), 
				data[0][0] * M(0, 2) + data[1][0] * M(1, 2) + data[2][0] * M(2, 2),
				data[0][1] * M(0, 0) + data[1][1] * M(1, 0) + data[2][1] * M(2, 0), 
				data[0][1] * M(0, 1) + data[1][1] * M(1, 1) + data[2][1] * M(2, 1), 
				data[0][1] * M(0, 2) + data[1][1] * M(1, 2) + data[2][1] * M(2, 2),
				data[0][2] * M(0, 0) + data[1][2] * M(1, 0) + data[2][2] * M(2, 0), 
				data[0][2] * M(0, 1) + data[1][2] * M(1, 1) + data[2][2] * M(2, 1),
				data[0][2] * M(0, 2) + data[1][2] * M(1, 2) + data[2][2] * M(2, 2));
}

Temple::Base::vec3 Temple::Base::mat3::operator *(const vec3& v) const {
	return vec3(data[0][0] * v.x + data[1][0] * v.y + data[2][0] * v.z,
				data[0][1] * v.x + data[1][1] * v.y + data[2][1] * v.z,
				data[0][2] * v.x + data[1][2] * v.y + data[2][2] * v.z);
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
