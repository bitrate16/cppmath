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
#include <vector>

#include "vec2.h"
#include "vec2.h"
#include "interval.h"

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
		
		LINE2_ARRANGEMENT arrangement(const vec2& A, const vec2& B, const vec2& C, const vec2& D) {
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
	
		inline bool intersects(const vec2& A, const vec2& B, const vec2& C, const vec2& D) {
			LINE2_ARRANGEMENT a = arrangement(A, B, C, D);
			
			return a == INTERSECT_NORMAL || a == INTERSECT_NODE || a == MATCH_NODES || a == OVERLAP_STRAIGHT_CONTAIN || a == OVERLAP_STRAIGHT || a == STRAIGHT_NODE;
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
		
		RIGHT_TURN2 right_turn(const vec2& x, const vec2& A, const vec2& B) {
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
		
		// Checks for two segments intersection
		bool right_intersects(const vec2& A, const vec2& B, const vec2& C, const vec2& D) {
			return right_turn(A, C, D) != right_turn(B, C, D) && right_turn(C, A, B) != right_turn(D, A, B);
		};
		
		// Check if point belongs to line segment
		bool point_on_segment(const vec2& X, const vec2& A, const vec2& B) {
			if (A.x == B.x) {
				if (X.x != A.x)
					return 0;
				else 
					return A.y >= X.y && X.y >= B.y || A.y <= X.y && X.y <= B.y;
			} else if (A.y == B.y) {
				if (X.y != A.y)
					return 0;
				else 
					return A.x >= X.x && X.x >= B.x || A.x <= X.x && X.x <= B.x;
			} else {
				double dx1 = A.x - X.x;
				double dy1 = A.y - X.y;
				double dx2 = X.x - B.x;
				double dy2 = X.y - B.y;
				return dx1 / dy1 == dx2 / dy2;
			}
		};
	
		// Check if point is inside the triangle using right turn predicate
		inline bool in_triangle(const vec2& x, const vec2& a, const vec2& b, const vec2& c) {
			//                 B #                                                                       
			//                  /\                                                                       
			//                 /  \                                                                      
			//                /    \          * P          but align(P) != align(A)                      
			//               /      \                                                                    
			//              /        \                                                                   
			//             /     * T  \              for i=1..3 :: align(T) == align(Vert[i]) => inside       
			//          A #-_          \                                                                 
			//               --_        \                                                                
			//                  --_      \                                                               
			//                     --_    \                                                              
			//                        --_  \                                                             
			//                           --_\                                                            
			//                              # C                                                          
			//                                                                                           
			
			return 
					right_turn(x, a, b) == right_turn(c, a, b)
					&&
					right_turn(x, c, a) == right_turn(b, c, a)
					&&
					right_turn(x, b, c) == right_turn(a, b, c);
		};
	
		// Calculates length of a 2D curve with given amount of steps from starting point to ending.
		double curve2_length(int steps, double start, double end, double (*function_x) (double), double (*function_y) (double)) {
			if (steps <= 0)
				return -1;
			
			if (end <= start)
				return 0;
			
			double d = (end - start) / (double) steps;
			if (d == 0)
				return std::numeric_limits<double>::quiet_NaN();
			
			double total_len = 0;
			
			// Previous coords
			double p_x = function_x(start);
			double p_y = function_y(start);
			
			for (double t = start + d; t <= end; t += d) {
				double x = function_x(t);
				double y = function_y(t);
				
				double dx = x - p_x;
				double dy = y - p_y;
				
				p_x = x;
				p_y = y;
				
				total_len += std::sqrt(dx * dx + dy * dy);
			}
			
			return total_len;
		};
		
		// Calculates length of a vector 2D curve with given amount of steps from starting point to ending.
		double curve2_length(int steps, double start, double end, vec2 (*function) (double)) {
			if (steps <= 0)
				return -1;
			
			if (end <= start)
				return 0;
			
			double d = (end - start) / (double) steps;
			if (d == 0)
				return std::numeric_limits<double>::quiet_NaN();
			
			double total_len = 0;
			
			// Previous coords
			vec2 p = function(start);
			
			for (double t = start + d; t <= end; t += d) {
				vec2 v = function(t);
				
				double dx = v.x - p.x;
				double dy = v.y - p.y;
				
				p = v;
				
				total_len += std::sqrt(dx * dx + dy * dy);
			}
			
			return total_len;
		};
		
		inline long long factorial(int n) {
			return (n < 0) ? 0 : ((n == 1 || n == 0) ? 1 : factorial(n - 1) * n);
		};
		
		// Calculate length of bezier curve with given amount of steps
		double bezier_length(int steps, const std::vector<vec2>& points) {
			 
			// Bezier:
			// B(t) = SUM [ i = 1; i <= N; P[i] * C[N, i] * t^i * (1 - t)^i ]
			
			// Derivative:
			// B'(t) = SUM [ i = 1; i <= N; P[i] * C[N, i] * (i * (1 - t)^i * t^(i-1) - i * (1 - t)^(i-1) * t^i) ]
			
			// Length:
			// SUM [ t = 0; t <= 1; t += 1 / steps; sqrt(B'(t).x^2 + B'(t).y^2)) ]
			
			
			if (points.size() < 2)
				return 0;
			
			if (steps <= 0)
				return -1;
			
			double d = 1.0 / (double) steps;
			if (d == 0)
				return std::numeric_limits<double>::quiet_NaN();
			
			std::vector<vec2> const_multipliers; // [::] = P[i] * C[N, i]
			for (int i = 0; i < points.size(); ++i) {
				double C = (double) factorial(points.size()) / ((double) factorial(i + 1) * (double) factorial(points.size() - i - 1));
				const_multipliers.push_back(points[i] * C);
			}
			
			double curve_len = 0;
			vec2 prev_derivative;
			
			for (double t = 0.0; t <= 1.0; t += d) {
				vec2 derivative;
				
				for (double i = 1; i < points.size() + 1; i += 1.0) {
					double point_derivative = i * std::pow(1 - t, i) * std::pow(t, i - 1) - i * std::pow(1 - t, i - 1) * std::pow(t, i);
					derivative += const_multipliers[(int) i - 1] * point_derivative;
				}
				
				if (t == 0.0) {
					prev_derivative = derivative;
					continue;
				}
				
				curve_len = (derivative - prev_derivative).len();
				prev_derivative = derivative;
			}
			
			return curve_len;
		};
	
		bool inside_poly(vec2 x, const std::vector<vec2>& poly_points) {
			if (poly_points.size() <= 2)
				return 0;
			
			// 0 -> 0, 1
			// 1 -> 1, 2
			// 2 -> 2, 3
			// 3 -> 3, 4
			// .........
			
			// right turn arrangements of edge points
			typedef struct { int a, b; } edge_t;
			
			// Maximal right vertex coord * 2
			double right = poly_points[0].x;
			for (int i = 1; i < poly_points.size(); ++i)
				right = right < poly_points[i].x ? poly_points[i].x * 2.0 : right;
			
			// Ray coords
			vec2 ray_a = x;
			vec2 ray_b(right, x.y);
			int trace = 0;
			
			std::vector<int> arrange(poly_points.size());
			for (int i = 0; i < poly_points.size(); ++i)
				arrange[i] = right_turn(poly_points[i], ray_a, ray_b);
			
			auto edge = [&arrange](int i) -> edge_t {
				i = i % arrange.size();
				if (i == arrange.size() - 1) // n-1 -> n-1, 0
					return { arrange.back(), arrange.front() };
				return { arrange[i], arrange[i + 1] };
			};
			                                                                                                                  
			// \                 \                /    \                 \                      /                                                                            
			//  \                 \              /      \                 \                    /                                                                              
			//   \                 \            /        \                 \                  /                                                                               
			//    \                 \          /          \                 \     5          /               7                                                                 
			//     \                 \   3    /            \                 \              /             _______                                                                    
			//      \                 \      /              \                 \            /                                      \                                             
			//       \                 \    /                \                 \          /                                        \    8                                        
			//        \                 \  /                  \                 \        /                                          \                                          
			//     1   \                 \/                    \______           \______/            ______                      ____\____                                                    
			//         /         /\                                   \                             /      \                          \                                                 
			//        /         /  \                                   \                           /        \                          \                                        
			//       /         /    \                               4   \                         /          \                          \                                       
			//      /         /      \                                   \                       /            \                          \                                      
			//     /         /        \                                   \                     /   6          \                                                               
			//    /         /          \                                   \                   /                \                                                              
			//   /         /      2     \                                   \                 /                  \                                                             
			//  /         /              \                                   \               /                    \                                                            
			// /         /                \                                   \             /                      \                                                                                                                             
			
			int inter = 0;
			
			for (int i = 0; i < arrange.size(); ++i) {
				if (edge(i).a == 0 && edge(i).b == 0) // 7
					continue;
				if (edge(i).a == -1 && edge(i).b == 1 || edge(i).a == 1 && edge(i).b == -1) // 8
					if (right_intersects(poly_points[i], poly_points[i == poly_points.size() - 1 ? 0 : i + 1], ray_a, ray_b))
						++inter;
				if (edge(i).a != 0 && edge(i).b == 0) { // 1, 2, 3, 4, 5, 6
					int start = edge(i).a;
					int start_i = (i + 1) % poly_points.size();
					
					++i;
					while (edge(i).a == 0 && edge(i).b == 0) ++i;
					
					int end = edge(i).b;
					int end_i = i % poly_points.size();
					
					if (start == -1 && end == -1 || start == 1 && end == 1) // 2, 3, 5, 6
						continue;
					
					if (point_on_segment(poly_points[start_i], ray_a, ray_b)
						&&
						point_on_segment(poly_points[end_i], ray_a, ray_b))
						++inter; // 1
				}
			};
			
			return inter % 2;
		};
	
		double dot2(const vec2& a, const vec2& b) {
			return a.x * b.x + a.y * b.y;
		};
	
		double dot3(const vec3& a, const vec3& b) {
			return a.x * b.x + a.y * b.y + a.z * b.z;
		};
		
		double cross2(const vec2& a, const vec2& b) {
			return a.x * b.y - a.y * b.x;
		};
	
		vec3 cross3(const vec3& a, const vec3& b) {
			return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		};
		
		vec3 rotateAroundVector(const vec3& v, const vec3& r, double angle) {
			/*vec3 out;
			double cosa = std::cos(angle);
			double sina = std::sin(angle);
			double cose = 1.0 - cosa;
			
			out.x = r.x * (r.x * v.x + r.y * v.y + r.z * v.z) * cose + v.x * cosa + (-r.z * v.y + r.y * v.z) * sina;
			out.y = r.y * (r.x * v.x + r.y * v.y + r.z * v.z) * cose + v.y * cosa + (r.z * v.x - r.x * v.z) * sina;
			out.z = r.z * (r.x * v.x + r.y * v.y + r.z * v.z) * cose + v.z * cosa + (-r.y * v.x + r.x * v.y) * sina;
			
			return out;*/
			
			/*double c = std::cos(angle);
			double s = std::sin(angle);
			double C = 1.0 - c;

			double Q[3][3];
			Q[0][0] = axe.x * axe.x * C + c;
			Q[0][1] = axe.y * axe.x * C + axe.z * s;
			Q[0][2] = axe.z * axe.x * C - axe.y * s;

			Q[1][0] = axe.y * axe.x * C - axe.z * s;
			Q[1][1] = axe.y * axe.y * C + c;
			Q[1][2] = axe.z * axe.y * C + axe.x * s;

			Q[2][0] = axe.x * axe.z * C + axe.y * s;
			Q[2][1] = axe.z * axe.y * C - axe.x * s;
			Q[2][2] = axe.z * axe.z * C + c;
			
			vec3 out;
			
			out.x = v.x * Q[0][0] + v.x * Q[0][1] + v.x * Q[0][2];
			out.y = v.y * Q[1][0] + v.y * Q[1][1] + v.y * Q[1][2];
			out.z = v.z * Q[2][0] + v.z * Q[2][1] + v.z * Q[2][2];
			
			return out;*/
			
			double cos_a = std::cos(angle);
			double sin_a = std::sin(angle);
			
			vec3 rot = (v * cos_a) + (cross3(r, v) * sin_a) + (r * dot3(r, v)) * (1 - cos_a);
			
			return rot;
		};
	};
};

