/*
	cpp math utilities
    Copyright (C) 2019-3041  bitrate16

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "vec3.h"
#include "vec3.h"

using namespace cppmath;

const vec3 vec3::X = vec3(1, 0, 0);
const vec3 vec3::Y = vec3(0, 1, 0);
const vec3 vec3::Z = vec3(0, 0, 1);	
const vec3 vec3::Zero = vec3(0, 0, 0);	
	

vec3::vec3(const vec3 &o) {
	x = o.x;
	y = o.y;
	z = o.z;
}

vec3::vec3(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3::vec3(double r) {
	x = r;
	y = r;
	z = r;
}

vec3::vec3() : x(0), y(0), z(0) {
	
};


// B A S I C _ O P E R A T O R S

vec3 vec3::operator=(const vec3 &v) {
	x = v.x;
	y = v.y;
	z = v.z;
	
	return *this;
}

/* Negative vector */
vec3 vec3::operator-() {
	vec3 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

/* Nothing */
vec3 vec3::operator+() {
	vec3 v(*this);
	v.x = v.x;
	v.y = v.y;
	v.z = v.z;
	return v;
}


/* Regular vector sum */
vec3 cppmath::operator+(const vec3 &v1, const vec3 &v2) {
	vec3 v(v1);
	v.x += v2.x;
	v.y += v2.y;
	v.z += v2.z;
	return v;
}

/* Regular vector dif */
vec3 cppmath::operator-(const vec3 &v1, const vec3 &v2) {
	vec3 v(v1);
	v.x -= v2.x;
	v.y -= v2.y;
	v.z -= v2.z;
	return v;
}

/* Compare equals */
bool cppmath::operator==(const vec3 &v1, const vec3 &v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

/* Compare not equals */
bool cppmath::operator!=(const vec3 &v1, const vec3 &v2) {
	return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
}


// B A S I C _ F U N C T I O N S


// [[ M U L T I P L I C A T I O N S ]]

/* Multiply components */
vec3 &vec3::mul(const vec3 &v) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

/* Scalar product */
double vec3::smul(const vec3 &v) {
	return x * v.x + y * v.y + z * v.z;
}

/* Vector product */
vec3 vec3::vmul(const vec3 &v) {
	return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

		
// [[ M A T H ]]

double vec3::len() const {
	return sqrt(x * x + y * y + z * z);
}

double vec3::len2() const {
	return x * x + y * y + z * z;
}

double vec3::inv() const {
	return 1.0 / (x * x + y * y + z * z);
}

vec3 vec3::norm() {
	vec3 v(*this);
	double lent = len();
	v.x /= lent;
	v.y /= lent;
	v.z /= lent;
	
	return v;
}

double vec3::dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
};

vec3 vec3::cross(const vec3& a, const vec3& b) {
	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
};

vec3 vec3::reflect(const vec3& v, const vec3& normal) {
	return v - 2.0 * vec3::dot(v, normal) * normal;
};

vec3 vec3::refract(const vec3& v, const vec3& normal, double eta) {
	eta = 2.0f - eta;
    double cosv = dot(normal, v);
    return (v * eta - normal * (-cosv + eta * cosv));
    /*
	double ndv = vec3::dot(normal, v);
	double k = 1.0 - eta * eta * (1.0 - ndv * ndv);
	if (k < 0.0)
		return vec3::Zero;
	else
		return eta * v - (eta * ndv + std::sqrt(k)) * normal;
	*/
};

vec3 vec3::from_direction_cosines(double ax, double ay, double az) {
	return vec3(std::cos(ax), std::cos(ay), std::cos(az)).norm();
};

double vec3::cos_between(const vec3& a, const vec3& b) {
	double len = a.len() * b.len();
	if (len == 0)
		return 0;
	return dot(a, b) / len;
};

// C U S T O M _ O P E R A T O R S

/* Normalize the vector */
vec3 vec3::operator~() {
	if (*this == 0)
		return 0;
	
	vec3 v(*this);
	double lent = len();
	v.x /= lent;
	v.y /= lent;
	v.z /= lent;
	
	return v;
}


/* Vector component multiplication product */
vec3 cppmath::operator*(const vec3 &v1, const vec3 &v2) {
	vec3 v(v1);
	v.x *= v2.x;
	v.y *= v2.y;
	v.z *= v2.z;
	return v;
};

/* Vector component division product */
vec3 cppmath::operator/(const vec3 &v1, const vec3 &v2) {
	vec3 v(v1);
	v.x /= v2.x == 0 ? NAN : v2.x;
	v.y /= v2.y == 0 ? NAN : v2.y;
	v.z /= v2.z == 0 ? NAN : v2.z;
	return v;
};
	
vec3& cppmath::operator+=(vec3& a, const vec3& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
};

vec3& cppmath::operator-=(vec3& a, const vec3& b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
};

vec3& cppmath::operator*=(vec3& a, const vec3& b) {
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	return a;
};

vec3& cppmath::operator/=(vec3& a, const vec3& b) {
	a.x /= b.x == 0 ? NAN : b.x;
	a.y /= b.y == 0 ? NAN : b.y;
	a.y /= b.z == 0 ? NAN : b.z;
	return a;
};

std::ostream& cppmath::operator<<(std::ostream& os, const vec3& v) {
	os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
	return os;
}

