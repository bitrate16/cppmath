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

namespace cppmath {
	class ivec2 {
		
	public:
	
		int x, y;
		
		ivec2() { 
			this->x = 0;
			this->y = 0;
		};
		
		ivec2(int x, int y) { 
			this->x = x;
			this->y = y;
		};
		
		ivec2(int r) {
			x = y = r;
		};
		
		ivec2(double x, double y) {
			this->x = x;
			this->y = y;
		};
		
		ivec2(double r) {
			x = y = r;
		};
		
		ivec2(const vec2& v) {
			x = v.x;
			y = v.y;
		};
		
		ivec2& mul(int i) { 
			x *= i;
			y *= i;
			return *this;
		};
		
		ivec2& div(int i) { 
			x /= i;
			y /= i;
			return *this;
		};
		
		inline long long dist2(const ivec2& v) const {
			return (v.x - x) * (v.x - x) + (v.y - y) * (v.y - y);
		};
		
		inline long long len2() const {
			return x * x + y * y;
		};
		
		// No sqrt in denominator
		inline long long dist_line(const ivec2& a, const ivec2& b) const {
			return std::abs(((b.y - a.y) * x - (b.x - a.x) * y + b.x * a.y - b.y * a.x)) / std::sqrt(((b.y - a.y) * (b.y - a.y) + (b.x - a.x) * (b.x - a.x)));
		};
		
		operator vec2() const {
			vec2 v;
			v.x = x;
			v.y = y;
			return v;
		};
	};
	
	bool operator==(const ivec2& a, const ivec2& b) { return a.x == b.x && a.y == b.y; };
	bool operator!=(const ivec2& a, const ivec2& b) { return a.x != b.x || a.y != b.y; };
	
	ivec2 operator+(const ivec2& a, const ivec2& b) { return ivec2(a.x + b.x, a.y + b.y); };
	ivec2 operator-(const ivec2& a, const ivec2& b) { return ivec2(a.x - b.x, a.y - b.y); };
	ivec2 operator*(const ivec2& a, const ivec2& b) { return ivec2(a.x * b.x, a.y * b.y); };
	ivec2 operator/(const ivec2& a, const ivec2& b) { return ivec2(a.x / b.x, a.y / b.y); };
	
	ivec2& operator+=(ivec2& a, const ivec2& b) { a.x += b.x; a.y += b.y; return a; };
	ivec2& operator-=(ivec2& a, const ivec2& b) { a.x -= b.x; a.y -= b.y; return a; };
	ivec2& operator*=(ivec2& a, const ivec2& b) { a.x *= b.x; a.y *= b.y; return a; };
	ivec2& operator/=(ivec2& a, const ivec2& b) { a.x /= b.x; a.y /= b.y; return a; };
};
