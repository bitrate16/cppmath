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

#include <iostream>

#include "vec3.h"

namespace cppmath {
	class vec3;
	class vec2 {
		public:
		
		static const vec2 X;
		static const vec2 Y;
		
		double x;
		double y;
		
		vec2(const vec2 &o);
		
		vec2(double x, double y);
		
		vec2(double r);
		
		vec2();
		
		
		// B A S I C _ O P E R A T O R S
		
		
		vec2 operator=(const vec2 &v);

		/* Negative vector */
		vec2 operator-();
		
		/* Nothing */
		vec2 operator+();
		
		
		// B A S I C _ F U N C T I O N S


		// [[ M U L T I P L I C A T I O N S ]]
		
		/* Multiply components */
		vec2 &mul(const vec2 &v);
		
		/* Scalar product */
		double smul(const vec2 &v);
		
		/* Vector product */
		vec3 vmul(const vec2 &v);
		
		
		// [[ M A T H ]]
		
		double len();
		
		double len2();
		
		double inv();
		
		double tan();
		
		/* Normalize the vector */
		vec2 operator~();
	
		friend std::ostream& operator<<(std::ostream& os, const vec2& v);
	};
	
	
	// B A S I C _ O P E R A T O R S
		
	/* Regular vector sum */
	vec2 operator+(const vec2 &v1, const vec2 &v2);
	
	/* Regular vector dif */
	vec2 operator-(const vec2 &v1, const vec2 &v2);
	
	/* Compare equals */
	bool operator==(const vec2 &v1, const vec2 &v2);
	
	/* Compare not equals */
	bool operator!=(const vec2 &v1, const vec2 &v2);
	
	
	// C U S T O M _ O P E R A T O R S
		
	/* Vector component multiplication product */
	vec2 operator*(const vec2 &v1, const vec2 &v2);
	
	/* Vector component division product */
	vec2 operator/(const vec2 &v1, const vec2 &v2);
	
	vec2& operator+=(vec2& a, const vec2& b);
	vec2& operator-=(vec2& a, const vec2& b);
	vec2& operator*=(vec2& a, const vec2& b);
	vec2& operator/=(vec2& a, const vec2& b);
	
	std::ostream& operator<<(std::ostream& os, const vec2& v);
};
