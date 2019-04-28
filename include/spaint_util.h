#pragma once 

#include <vector>
#include <iostream>

#include "spaint.h"
#include "vec2.h"

// Utility functions for painter
namespace spaint {
	
	// Render hermite spline for given points
	void hermite_spline(spaint::painter& p, const std::vector<cppmath::vec2>& points, int steps = 1000) {
		if (points.size() < 2)
			return;
		
		if (steps <= 0)
			return;
		
		double d = 1.0 / (double) steps;
		
		if (d == 0.0)
			return;
		
		auto P = [](double t, double p0, double p1, double m0, double m1) -> double { 
			double t2 = t * t;
			double t3 = t2 * t;
			return  (2.0 * t3 - 3.0 * t2 + 1.0) * p0
					+
					(t3 - 2.0 * t2 + t) * m0
					+
					(-2.0 * t3 + 3.0 * t2) * p1
					+
					(t3 - t2) * m1;
		};
		
		std::vector<cppmath::vec2> derivatives;
		for (int i = 0; i < points.size(); ++i) {
			cppmath::vec2 ptan;
			
			if (i == 0) 
				ptan = points[1] - points[0];
			else if (i == points.size() - 1)
				ptan = points[i] - points[i - 1];
			else
				ptan = (points[i + 1] - points[i - 1]) / 2.0;
				
			derivatives.push_back(ptan);
		}
		
		for (int i = 0; i < points.size() - 1; ++i) {
			double p_x = P(0.0, points[i].x, points[i + 1].x, derivatives[i].x, derivatives[i + 1].x);
			double p_y = P(0.0, points[i].y, points[i + 1].y, derivatives[i].y, derivatives[i + 1].y);
			
			for (double t = d; t <= 1.0; t += d) {
				double x = P(t, points[i].x, points[i + 1].x, derivatives[i].x, derivatives[i + 1].x);
				double y = P(t, points[i].y, points[i + 1].y, derivatives[i].y, derivatives[i + 1].y);
				
				p.line(p_x, p_y, x, y);
				
				p_x = x;
				p_y = y;
			}
		}
	};
	
	// Render hermite spline for given points and given derivatives
	void hermite_spline(spaint::painter& p, const std::vector<cppmath::vec2>& points, const std::vector<cppmath::vec2>& derivatives, int steps = 1000) {
		if (points.size() < 2)
			return;
		
		if (points.size() > derivatives.size())
			return;
		
		if (steps <= 0)
			return;
		
		double d = 1.0 / (double) steps;
		
		if (d == 0.0)
			return;
		
		auto P = [](double t, double p0, double p1, double m0, double m1) -> double { 
			double t2 = t * t;
			double t3 = t2 * t;
			return  (2.0 * t3 - 3.0 * t2 + 1.0) * p0
					+
					(t3 - 2.0 * t2 + t) * m0
					+
					(-2.0 * t3 + 3.0 * t2) * p1
					+
					(t3 - t2) * m1;
		};
		
		for (int i = 0; i < points.size() - 1; ++i) {
			double p_x = P(0.0, points[i].x, points[i + 1].x, derivatives[i].x, derivatives[i + 1].x);
			double p_y = P(0.0, points[i].y, points[i + 1].y, derivatives[i].y, derivatives[i + 1].y);
			
			for (double t = d; t <= 1.0; t += d) {
				double x = P(t, points[i].x, points[i + 1].x, derivatives[i].x, derivatives[i + 1].x);
				double y = P(t, points[i].y, points[i + 1].y, derivatives[i].y, derivatives[i + 1].y);
				
				p.line(p_x, p_y, x, y);
				
				p_x = x;
				p_y = y;
			}
		}
	};

	void bezier_curve(spaint::painter& p, const std::vector<cppmath::vec2>& points, int steps = 1000) {
		if (points.size() > 1) {
			
			if (steps <= 0)
				return;
		
			double d = 1.0 / (double) steps;
			
			if (d == 0)
				return;
			
			std::function<double(int)> factorial;
			factorial = [&factorial](int n) -> double {
				if (n <= 1)
					return 1;
				return factorial(n - 1) * n;
			};
			
			std::vector<double> C;
			for (int i = 0; i < points.size(); ++i) 
				C.push_back(factorial(points.size() - 1) / (factorial(i) * factorial(points.size() - 1 - i)));
			
			auto B = [&C](int i, int n, double t) -> double { 
				return C[i] * std::pow(t, i) * std::pow(1 - t, n - i);
			};
			
			// Bezier:
			// B[i, n](t) = C[n, i] * t ^ i * (1 - t) ^ (n - i)
			// r(t) = SUM[ i = 0; i <= N; B[i, n](t) * P[i]
			
			cppmath::vec2 point_old = points[0];
			
			for (double t = 0.0; t <= 1.0; t += d) {
				cppmath::vec2 point;
				
				for (int i = 0; i < points.size(); ++i) {
					double part_mult = B(i, points.size() - 1, t);
					point += part_mult * points[i];
				}
				
				if (t != 0.0) {
					p.line(point_old.x, point_old.y, point.x, point.y);
					point_old = point;
				}
			}				
		}
	};
};