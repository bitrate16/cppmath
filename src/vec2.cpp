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

#include "vec2.h"
#include "vec3.h"

using namespace cppmath;

const vec2 vec2::X = vec2(1, 0);
const vec2 vec2::Y = vec2(0, 1);	
	

vec2::vec2(const vec2 &o) {
	x = o.x;
	y = o.y;
}

vec2::vec2(double x, double y) {
	this->x = x;
	this->y = y;
}

vec2::vec2(double r) {
	x = r;
	y = r;
}

vec2::vec2() : x(0), y(0) {
	
};


// B A S I C _ O P E R A T O R S

vec2 vec2::operator=(const vec2 &v) {
	x = v.x;
	y = v.y;
	
	return *this;
}

/* Negative vector */
vec2 vec2::operator-() {
	vec2 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	return v;
}

/* Nothing */
vec2 vec2::operator+() {
	vec2 v(*this);
	v.x = -v.x;
	v.y = -v.y;
	return v;
}


/* Regular vector sum */
vec2 cppmath::operator+(const vec2 &v1, const vec2 &v2) {
	vec2 v(v1);
	v.x += v2.x;
	v.y += v2.y;
	return v;
}

/* Regular vector dif */
vec2 cppmath::operator-(const vec2 &v1, const vec2 &v2) {
	vec2 v(v1);
	v.x -= v2.x;
	v.y -= v2.y;
	return v;
}

/* Compare equals */
bool cppmath::operator==(const vec2 &v1, const vec2 &v2) {
	return v1.x == v2.x && v1.y == v2.y;
}

/* Compare not equals */
bool cppmath::operator!=(const vec2 &v1, const vec2 &v2) {
	return v1.x != v2.x || v1.y != v2.y;
}


// B A S I C _ F U N C T I O N S


// [[ M U L T I P L I C A T I O N S ]]

/* Multiply components */
vec2 &vec2::mul(const vec2 &v) {
	x *= v.x;
	y *= v.y;
	return *this;
}

/* Scalar product */
double vec2::smul(const vec2 &v) {
	return x * v.x + y * v.y;
}

/* Vector product */
vec3 vec2::vmul(const vec2 &v) {
	return vec3(0, 0, v.x * v.y);
}
		
		
// [[ M A T H ]]

double vec2::len() {
	return sqrt(x * x + y * y);
}

double vec2::len2() {
	return y * y + y * y;
}

double vec2::inv() {
	return 1.0 / (x * x + y * y);
}


// C U S T O M _ O P E R A T O R S

/* Normalize the vector */
vec2 vec2::operator~() {
	if (*this == 0)
		return 0;
	
	vec2 v(*this);
	double lent = len();
	v.x /= lent;
	v.y /= lent;
	
	return v;
}


/* Vector component multiplication product */
vec2 cppmath::operator*(const vec2 &v1, const vec2 &v2) {
	vec2 v(v1);
	v.x *= v2.x;
	v.y *= v2.y;
	return v;
};

/* Vector component division product */
vec2 cppmath::operator/(const vec2 &v1, const vec2 &v2) {
	vec2 v(v1);
	v.x /= v2.x == 0 ? NAN : v2.x;
	v.y /= v2.y == 0 ? NAN : v2.y;
	return v;
};
	
vec2& cppmath::operator+=(vec2& a, const vec2& b) {
	a.x += b.x;
	a.y += b.y;
	return a;
};

vec2& cppmath::operator-=(vec2& a, const vec2& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
};

vec2& cppmath::operator*=(vec2& a, const vec2& b) {
	a.x *= b.x;
	a.y *= b.y;
	return a;
};

vec2& cppmath::operator/=(vec2& a, const vec2& b) {
	a.x /= b.x == 0 ? NAN : b.x;
	a.y /= b.y == 0 ? NAN : b.y;
	return a;
};

std::ostream& cppmath::operator<<(std::ostream& os, const vec2& v) {
	os << '(' << v.x << ", " << v.y << ')';
	return os;
}

