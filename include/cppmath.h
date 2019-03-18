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

#include <limits>

#include "vec2.h"
#include "vec2.h"
#include "interval.h"
#include <iostream>
using namespace std;
namespace cppmath {
	namespace math {
		/* 
		 * Mutual arragement of two vec2 lines
		 * 
		 * 0: A-----------B
		 *          C
		 *         /
		 *        /
		 *       D
		 * 
		 * 1: A    D
		 *     \  /
		 *      \/
		 *      /\
		 *     /  \
		 *    C    B
		 * 
		 * 2: A----C------B
		 *          \
		 *           \
		 *            \
		 *             D
		 * 
		 * 3:  (D,A)
		 *      / \
		 *     /   \
		 *    C     \
		 *           B
		 * 
		 * 4: A--------------B
		 *       C--------D
		 * 
		 * 5: A----C-------D--B
		 * 
		 * 6: A---C----------B----D
		 *  
		 * 7: A-------(B,C)--------D
		 * 
		 * 8: A------B    C--------------D
		 *
		 * 9: A == B || C == D
		 * 
		 */
		enum LINE2_ARRANGEMENT {
			NOINTERSECT_UNPARALLEL = 0, 
			INTERSECT_NORMAL = 1,
			INTERSECT_NODE = 2,
			MATCH_NODES = 3,
			NOINTERSECT_PARALLEL = 4,
			OVERLAP_STRAIGHT_CONTAIN = 5,
			OVERLAP_STRAIGHT = 6,
			STRAIGHT_NODE = 7,
			NOINTERSECT_STRAIGHT = 8, 
			UNDEFINED = 9
		};
		
		inline double abs(double x) {
			return x < 0 ? -x : x;
		}
		
		/* Checks for 0 < |x| < |b| */
		inline bool in_abs_interval(double x, double b) {
			return 0 < (x = abs(x)) && x < abs(b);
		};
		
		/* Checks for 0 < x < b */
		inline bool in_interval(double x, double b) {
			return 0 < x && x < b || 0 < (x = -x) && x < -b;
		};
		
		LINE2_ARRANGEMENT arrangement(vec2 A, vec2 B, vec2 C, vec2 D) {
			if (A == B || C == D)
				return UNDEFINED;
			
			// Linear equation determiniant
			double s11 = B.x - A.x; // x2 - x1
			double s12 = C.x - D.x; // x3 - x4
			double s13 = C.x - A.x; // x3 - x1
			double s21 = B.y - A.y;
			double s22 = C.y - D.y; 
			double s23 = C.y - A.y;
			
			double Dt = s11 * s22 - s12 * s21;
			
			double Dx = s13 * s22 - s12 * s23;
			double Dy = s11 * s23 - s13 * s21;
				
			// cout << "Dt = " << Dt << ", Dx = " << Dx << ", Dy = " << Dy << endl;
			
			if (Dt == 0) { // Parallel { 4, 5, 6, 7, 8 }
				if (Dx != 0 || Dy != 0) // Parallel no intersection { 4 }
					return NOINTERSECT_PARALLEL;
				
				double s14 = D.x - A.x; // x4 - x1
				double s15 = C.x - B.x; // x3 - x2
				double s16 = D.x - C.x; // x4 - x3
				
				// C and D on AB
				double Pc = s13;
				double Pd = s14;
				
				// cout << "Pc = " << Pc << ", Pd = " << Pd << endl;
				
				// A and B on CD
				// double Pa = s15 / s16;
				// double Pb = s13 / s16;
				
				if (in_interval(Pc, s11)) // A < C < B
					if (in_interval(Pd, s11)) // A < C <> D < B
						return OVERLAP_STRAIGHT_CONTAIN;
					else
						return OVERLAP_STRAIGHT;
				else if (in_interval(Pd, s11)) // A < C < B
					if (in_interval(Pc, s11)) // A < D <> C < B
						return OVERLAP_STRAIGHT_CONTAIN;
					else
						return OVERLAP_STRAIGHT;
				else if (Pc == 0 || Pc == s11 || Pd == 0 || Pd == s11)
					return STRAIGHT_NODE;
				else
					return NOINTERSECT_STRAIGHT;
			}
			
			// cout << "in_abs_interval(Dx, Dt) = " << in_abs_interval(Dx, Dt) << endl;
			if (in_interval(Dx, Dt))
				if (Dy == 0 || Dy == Dt) // Overlap single node { 2 }
					return INTERSECT_NODE;
				else if (in_interval(Dy, Dt)) // Intersection { 1 }
					return INTERSECT_NORMAL;
				else // No intersection { 0 }
					return NOINTERSECT_UNPARALLEL;
			else if (in_interval(Dy, Dt))
				if (Dx == 0 || Dx == Dt) // Overlap single node { 2 }
					return INTERSECT_NODE;
				else if (in_interval(Dx, Dt)) // Intersection { 1 }
					return INTERSECT_NORMAL;
				else // No intersection { 0 }
					return NOINTERSECT_UNPARALLEL;
			else if ((Dx == 0 || Dx == Dt) && (Dy == 0 || Dy == Dt)) // Match nodes
				return MATCH_NODES;
			else // 
				return NOINTERSECT_UNPARALLEL;
			
			return UNDEFINED;
		};
	
		/*
		 *
		 *                B
		 *                |
		 *      -1        |        +1
		 *                |
		 *          C*    |     
		 *                |         *D
		 *                |
		 *                *E = 0
		 *                |
		 *                A
		 *
		 *
		 */
		enum RIGHT_TURN2 {
			RT_LEFT      = -1,
			RT_COLLINEAR =  0,
			RT_RIGHT     =  1
		};
		
		RIGHT_TURN2 right_turn(vec2 x, vec2 A, vec2 B) {
			double p = (B.x - A.x) * (x.y - A.y);
			double q = (x.x - A.x) * (B.y - A.y);
			
			double eps = (std::abs(p) + std::abs(q)) * std::numeric_limits<double>::epsilon() * 4;
			
			double det = p - q;
			
			if (det > eps)
				return RT_RIGHT;
			if (det < -eps)
				return RT_LEFT;
			
			interval iax(A.x), iay(A.y), ibx(B.x), iby(B.y), ixx(x.x), ixy(x.y);
			
			interval ip = (ibx - iax) * (ixy - iay);
			interval iq = (ixx - iax) * (iby - iay);
			
			interval idet = ip - iq;
			
			if (!idet.has(0.0))
				if (idet > 0)
					return RT_RIGHT;
				else
					return RT_LEFT;
			
			// Assume no more precision
			return RT_COLLINEAR;
		};
	};
};
