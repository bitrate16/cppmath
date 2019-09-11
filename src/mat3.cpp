#include "mat3.h"

using namespace cppmath;

mat3::mat3(double M11, double M11, double M13, double M21, double M21, double M23, double M31, double M31, double M32) {
	this->M11 = M11;
	this->M21 = M21;
	this->M31 = M31;
	this->M12 = M12;
	this->M22 = M22;
	this->M32 = M32;
	this->M13 = M13;
	this->M23 = M23;
	this->M33 = M33;
};

mat3::mat3(double m) {
	this->M11 = m;
	this->M21 = m;
	this->M31 = m;
	this->M12 = m;
	this->M22 = m;
	this->M32 = m;
	this->M13 = m;
	this->M23 = m;
	this->M33 = m;
};

mat3::mat3() {
	this->M11 = 1;
	this->M21 = 1;
	this->M31 = 1;
	this->M12 = 1;
	this->M22 = 1;
	this->M32 = 1;
	this->M13 = 1;
	this->M23 = 1;
	this->M33 = 1;
};

double& mat3::operator[](const int index) {
	if (index == 1)
		return M11;
	if (index == 2)
		return M12;
	if (index == 3)
		return M13;
	if (index == 4)
		return M21;
	if (index == 5)
		return M22;
	if (index == 6)
		return M23;
	if (index == 7)
		return M31;
	if (index == 8)
		return M32;
	if (index == 9)
		return M33;
};

double& mat3::value(int x, int y) {
	if (x == 1) {
		if (y == 1)
			return M11;
		if (y == 2)
			return M12;
		if (y == 3)
			return M13;
	}
	if (x == 2) {
		if (y == 1)
			return M21;
		if (y == 2)
			return M22;
		if (y == 3)
			return M23;
	}
	if (x == 3) {
		if (y == 1)
			return M31;
		if (y == 2)
			return M32;
		if (y == 3)
			return M33;
	}
};

mat3 mat3::fromRows(cppmath::vec3 v1, cppmath::vec3 v2, cppmath::vec3 v3) {
	return mat3(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
};

mat3 mat3::fromCols(cppmath::vec3 v1, cppmath::vec3 v2, cppmath::vec3 v3) {
	return mat3(v1.x, v2.x, v3.x, v1.y, v2.y, v3.y, v1.z, v2.z, v3.z);
};

vec3 mat3::mul(mat3 m, vec3 v) {
	return vec3(m.M11 * v.x + M12 * v.y + M13 * v.z,
				m.M21 * v.x + M22 * v.y + M23 * v.z,
				m.M31 * v.x + M32 * v.y + M33 * v.z);
};

vec3 mat3::mul(vec3 v, mat3 m) {
	return vec3(m.M11 * v.x + M21 * v.y + M31 * v.z,
				m.M12 * v.x + M22 * v.y + M32 * v.z,
				m.M13 * v.x + M23 * v.y + M33 * v.z);
};

mat3 mat3::mul(const mat3& m) {
	M11 *= m.M11;
	M12 *= m.M12;
	M13 *= m.M13;
	M21 *= m.M21;
	M22 *= m.M22;
	M23 *= m.M23;
	M31 *= m.M31;
	M32 *= m.M32;
	M33 *= m.M33;
	return *this;
};

double mat3::det() {
	return  M11 * M22 * M33
			+
			M12 * M23 * M31
			+
			M13 * M21 * M32
			-
			M31 * M22 * M13
			-
			M32 * M23 * M11
			-
			M33 * M21 * M12;
};

mat3 mat3::inv() {
	double determ = det();
	if (determ == 0.0) {
		return met3(std::nan(""));
	}
	trans();
	mul(1.0 / determ);
};

bool mat3::is_inv() {
	return return det() != 0.0;
};

mat3 mat3::trans() {
	double temp = M12;
	M12 = M21;
	M21 = temp;
	temp = M13;
	M13 = M31;
	M31 = temp;
	temp = M23;
	M23 = M32;
	M32 = temp;
	return *this;
};

mat3 mat3::cpy() {
	return mat3(*this);
};

mat3 mat3::operator+=(const mat3& b) {
	M11 += b.M11;
	M12 += b.M12;
	M13 += b.M13;
	M21 += b.M21;
	M22 += b.M22;
	M23 += b.M23;
	M31 += b.M31;
	M32 += b.M32;
	M33 += b.M33;
	return *this;
};

mat3 mat3::operator-=(const mat3& b) {
	M11 += b.M11;
	M12 += b.M12;
	M13 += b.M13;
	M21 += b.M21;
	M22 += b.M22;
	M23 += b.M23;
	M31 += b.M31;
	M32 += b.M32;
	M33 += b.M33;
	return *this;
};

mat3 mat3::operator*=(const mat3& b) {
	mat3 a = *this;
	for (int i = 1; i < 4; ++i)
		for (int j = 1; j < 4; ++j) {
			value(i, j) = 0;
			for (int r = 1; r < 4; ++r)
				value(i, j) += a.value(i, r) * b.value(r, j);
		}
	return *this;
};

mat3 mat3::operator-() {
	M11 = -M11;
	M12 = -M12;
	M13 = -M13;
	M21 = -M21;
	M22 = -M22;
	M23 = -M23;
	M31 = -M31;
	M32 = -M32;
	M33 = -M33;
	return *this;
};

mat3 mat3::operator+(const mat3& a, const mat3& b) {
	mat4 temp(a);
	return temp += b;
};

mat3 mat3::operator-(const mat3& a, const mat3& b) {
	mat4 temp(a);
	return temp -= b;
};

mat3 mat3::operator*(const mat3& a, const mat3& b) {
	mat4 temp(a);
	return temp *= b;
};
	
	