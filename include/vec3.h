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

#include "vec2.h"

namespace cppmath {
	class vec2;
	class vec3 {
		public:
		
		static const vec3 X;
		static const vec3 Y;
		static const vec3 Z;
		static const vec3 Zero;
		
		double x;
		double y;
		double z;
		
		vec3(const vec3 &o);
		
		vec3(double x, double y, double z);
		
		vec3(double r);
		
		vec3();
		
		
		// B A S I C _ O P E R A T O R S
		
		
		vec3 operator=(const vec3 &v);

		/* Negative vector */
		vec3 operator-();
		
		/* Nothing */
		vec3 operator+();
		
		
		// B A S I C _ F U N C T I O N S


		// [[ M U L T I P L I C A T I O N S ]]
		
		/* Multiply components */
		vec3 &mul(const vec3 &v);
		
		/* Scalar product */
		double smul(const vec3 &v);
		
		/* Vector product */
		vec3 vmul(const vec3 &v);
		
		
		// [[ M A T H ]]
		
		double len();
		
		double len2();
		
		double inv();

		vec3 norm();
		
		static double dot(const vec3& a, const vec3& b);
		
		static vec3 cross(const vec3& a, const vec3& b);
		
		// Calculate refrection of a vector to a normal vector
		// \    |    *  
		//  \v  |n  /out
		//   \  |  /    
		//    \ | /     
		//     *|/      
		static vec3 reflect(const vec3& v, const vec3& normal);
		
		// Calculate refraction of a vector to a normal vector
		// \    |        
		//  \v  |n       
		//   \  |        
		//    \ |        
		//     *|        
		//      |\       
		//      |  \     
		//      |    \out
		//      |      \ 
		//      |       *
		static vec3 refract(const vec3& v, const vec3& normal, double eta);
		
		/* Normalize the vector */
		vec3 operator~();
	
		friend std::ostream& operator<<(std::ostream& os, const vec3& v);
	};
	
	
	// B A S I C _ O P E R A T O R S
		
	/* Regular vector sum */
	vec3 operator+(const vec3 &v1, const vec3 &v2);
	
	/* Regular vector dif */
	vec3 operator-(const vec3 &v1, const vec3 &v2);
	
	/* Compare equals */
	bool operator==(const vec3 &v1, const vec3 &v2);
	
	/* Compare not equals */
	bool operator!=(const vec3 &v1, const vec3 &v2);
	
	
	// C U S T O M _ O P E R A T O R S
		
	/* Vector component multiplication product */
	vec3 operator*(const vec3 &v1, const vec3 &v2);
	
	/* Vector component division product */
	vec3 operator/(const vec3 &v1, const vec3 &v2);
	
	vec3& operator+=(vec3& a, const vec3& b);
	vec3& operator-=(vec3& a, const vec3& b);
	vec3& operator*=(vec3& a, const vec3& b);
	vec3& operator/=(vec3& a, const vec3& b);
	
	std::ostream& operator<<(std::ostream& os, const vec3& v);
};
