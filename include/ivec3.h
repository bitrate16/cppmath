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

#pragma once

#include <cmath>

#include "vec2.h"
#include "vec3.h"

namespace cppmath {
	class ivec3 {
		
	public:
	
		long x, y, z;
		
		ivec3() { 
			this->x = 0;
			this->y = 0;
			this->z = 0;
		};
		
		ivec3(long x, long y, long z) { 
			this->x = x;
			this->y = y;
			this->z = z;
		};
		
		ivec3(long r) {
			x = y = z = r;
		};
		
		/*
		ivec3(double x, double y, double z) {
			this->x = x;
			this->y = y;
			this->z = z;
		};
		
		ivec3(double r) {
			x = y = z = r;
		};
		*/
		
		ivec3(const vec3& v) {
			x = v.x;
			y = v.y;
			z = v.z;
		};
		
		ivec3(const vec2& v) {
			x = v.x;
			y = v.y;
		};
		
		ivec3& mul(long i) { 
			x *= i;
			y *= i;
			z *= i;
			return *this;
		};
		
		ivec3& div(long i) { 
			x /= i;
			y /= i;
			z /= i;
			return *this;
		};
		
		inline long long dist2(const ivec3& v) const {
			return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z);
		};
		
		inline long long len2() const {
			return x * x + y * y + z * z;
		};
		
		operator vec2() const {
			vec2 v;
			v.x = x;
			v.y = y;
			return v;
		};
		
		operator vec3() const {
			vec3 v;
			v.x = x;
			v.y = y;
			v.z = z;
			return v;
		};
	};
	
	bool operator==(const ivec3& a, const ivec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; };
	bool operator!=(const ivec3& a, const ivec3& b) { return a.x != b.x || a.y != b.y || a.z != b.z; };
	
	ivec3 operator+(const ivec3& a, const ivec3& b) { return ivec3(a.x + b.x, a.y + b.y, a.z + b.z); };
	ivec3 operator-(const ivec3& a, const ivec3& b) { return ivec3(a.x - b.x, a.y - b.y, a.z - b.z); };
	ivec3 operator*(const ivec3& a, const ivec3& b) { return ivec3(a.x * b.x, a.y * b.y, a.z * b.z); };
	ivec3 operator/(const ivec3& a, const ivec3& b) { return ivec3(a.x / b.x, a.y / b.y, a.z / b.z); };
	
	ivec3& operator+=(ivec3& a, const ivec3& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; };
	ivec3& operator-=(ivec3& a, const ivec3& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; };
	ivec3& operator*=(ivec3& a, const ivec3& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; };
	ivec3& operator/=(ivec3& a, const ivec3& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; };
};
