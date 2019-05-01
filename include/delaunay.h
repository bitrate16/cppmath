#pragma once

#include <vector>
#include <algorithm>

#include "vec2.h"

namespace delaunay {
	
	static constexpr double eps = 10e-4;
	
	struct edge {
		cppmath::vec2 p0, p1;
		
		edge(const cppmath::vec2& _p0, const cppmath::vec2& _p1) : p0(_p0), p1(_p1) {};
		
		bool operator==(const edge& e) {
			return (e.p0 == p0 && e.p1 == p1) || (e.p0 == p1 && e.p1 == p0);
		};
	};
	
	struct circle {
		cppmath::vec2 center;
		double radius;
	};
	
	struct triangle {
		cppmath::vec2 p0, p1, p2;
		edge e0, e1, e2;
		circle bcircle;
		
		triangle(const cppmath::vec2& _p0, const cppmath::vec2& _p1, const cppmath::vec2& _p2)
				: 
				p0(_p0),
				p1(_p1),
				p2(_p2),
				e0(_p0, _p1),
				e1(_p1, _p2),
				e2(_p0, _p2) 
		{
			double ax = p1.x - p0.x;		
			double ay = p1.y - p0.y;
			double bx = p2.x - p0.x;
			double by = p2.y - p0.y;
			
			double m = p1.len2() - p0.len2();
			double u = p2.len2() - p0.len2();
			double s = 1.0 / (2.0 * (ax * by - ay * bx));
			
			bcircle.center.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
			bcircle.center.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;
			
			double dx = p0.x - bcircle.center.x;
			double dy = p0.y - bcircle.center.y;
			bcircle.radius = dx * dx + dy * dy;
		}
	};
	
	struct delaunay {
		std::vector<triangle> triangles;
		std::vector<edge> edges;
	};
	
	delaunay triangulate(const std::vector<cppmath::vec2>& points) {
		if (points.size() < 3)
			return delaunay();
		
		double xmin = points[0].x;
		double xmax = xmin;
		double ymin = points[0].y;
		double ymax = ymin;
		
		for (auto& pt : points) {
			xmin = xmin < pt.x ? xmin : pt.x;
			xmax = xmax > pt.x ? xmax : pt.x;
			ymin = ymin < pt.y ? ymin : pt.y;
			ymax = ymax > pt.y ? ymax : pt.y;
		}
		
		double dx = xmax - xmin;
		double dy = ymax - ymin;
		double dmax = dx > dy ? dx : dy;
		double midx = (xmax + xmin) / 2.0;
		double midy = (ymax + ymin) / 2.0;
		
		delaunay d;
		
		// Append super-triangle
		cppmath::vec2 p0(midx - 20 * dmax, midy - dmax);
		cppmath::vec2 p1(midx, midy + 20 * dmax);
		cppmath::vec2 p2(midx + 20 * dmax, midy - dmax);
		d.triangles.emplace_back(triangle(p0, p1, p2));
		
		for (auto& pt : points) {
			std::vector<edge> edges;
			std::vector<triangle> tmps;
			
			for (auto& tr : d.triangles) {
				double dist = (tr.bcircle.center - pt).len2();
				
				if ((dist - tr.bcircle.radius) <= eps) {
					edges.push_back(tr.e0);
					edges.push_back(tr.e1);
					edges.push_back(tr.e2);
				} else
					tmps.push_back(tr);
			}
			
			// Collect duplicating edges & remove
			std::vector<bool> duplicate(edges.size(), false);
			for (int i = 0; i < edges.size(); ++i)
				for (int j = 0; j < edges.size(); ++j)
					if (i != j && edges[i] == edges[j]) {
						duplicate[i] = 1;
						duplicate[j] = 1;
					}
			
			edges.erase(std::remove_if(edges.begin(), edges.end(), [&](auto const& e) { return duplicate[&e - &edges[0]]; }), edges.end());
			
			// Append new triangles
			for (auto& e : edges) 
				tmps.push_back({ e.p0, e.p1, pt });
			
			d.triangles = tmps;
		}
		
		// Remove super-triangle
		d.triangles.erase(std::remove_if(d.triangles.begin(), d.triangles.end(), 
			[&](auto const& tr) {
				return  tr.p0 == p0 || tr.p1 == p0 || tr.p2 == p0 
						||
						tr.p0 == p1 || tr.p1 == p1 || tr.p2 == p1
						||
						tr.p0 == p2 || tr.p1 == p2 || tr.p2 == p2;
			}), d.triangles.end());
			
		// Append edges
		for (auto& tr : d.triangles) {
			d.edges.push_back(tr.e0);
			d.edges.push_back(tr.e1);
			d.edges.push_back(tr.e2);
		}
		
		return d;
	};
};