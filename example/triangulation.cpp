/*
    Example shows use of spaint and cppmath::math::right_turn
	
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

#include "cstdlib"
#include "iostream"

#include "spaint.h"
#include "cppmath.h"
#include "ivec2.h"

using namespace std;
using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE  9
#define KEY_R      27

// Right turn visualisation using spaint tool.

// bash c.sh "-lX11" triangulation

class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Triangulation example");
	};
	
	static const int POINTS_AMOUNT = 16;
	
	int total_points = 0;
	ivec2 points[POINTS_AMOUNT];
	
	int total_nodes = 0;
	// Array of nodes with indices of vertices
	int nodes[POINTS_AMOUNT * POINTS_AMOUNT][2]; 
	
	bool mouse_down = 0;
	ivec2 last_pointer;
	
	bool resized = 1;
	
	// Describes the triangle.
	// Contains nodes_ids of the triangle.
	struct triangle {
		int vert_ids[3];
	};
	
	int total_triangles = 0;
	// Array of triangles
	triangle triangles[POINTS_AMOUNT * POINTS_AMOUNT];
	
	// Returns triangle cointaining passed point.
	triangle get_triangle(const ivec2& v) {
		if (total_nodes < 3)
			return {{ -1, -1, -1 }};
		
		for (int i = 0; i < total_triangles; ++i) 
			if (math::in_triangle(v, points[triangles[i].vert_ids[0]], points[triangles[i].vert_ids[1]], points[triangles[i].vert_ids[2]])) 
				return triangles[i];
					
		return {{ -1, -1, -1 }};
	};
	
	// Returns 1 if point is inside of some triangle
	bool in_triangle(const ivec2& v) {
		if (total_nodes < 3)
			return 0;
		
		for (int i = 0; i < total_nodes; ++i)
			for (int j = 0; j < total_nodes; ++j)
				for (int k = 0; k < total_nodes; ++k) 
					if (i != j && j != k && k != i) {	
						bool nd1 = 	nodes[i][0] == nodes[j][0] || nodes[i][0] == nodes[j][1]
									||
									nodes[i][0] == nodes[k][0] || nodes[i][0] == nodes[k][1];
						bool nd2 = 	nodes[i][1] == nodes[j][0] || nodes[i][1] == nodes[j][1]
									||
									nodes[i][1] == nodes[k][0] || nodes[i][1] == nodes[k][1];
									
						bool nd3 = 	nodes[j][0] == nodes[i][0] || nodes[j][0] == nodes[i][1]
									||
									nodes[j][0] == nodes[k][0] || nodes[j][0] == nodes[k][1];
						bool nd4 = 	nodes[j][1] == nodes[i][0] || nodes[j][1] == nodes[i][1]
									||
									nodes[j][1] == nodes[k][0] || nodes[j][1] == nodes[k][1];
									
						bool nd5 = 	nodes[k][0] == nodes[i][0] || nodes[k][0] == nodes[i][1]
									||
									nodes[k][0] == nodes[j][0] || nodes[k][0] == nodes[j][1];
						bool nd6 = 	nodes[k][1] == nodes[i][0] || nodes[k][1] == nodes[i][1]
									||
									nodes[k][1] == nodes[j][0] || nodes[k][1] == nodes[j][1];
									
						if (!nd1 || !nd2 || !nd3 || !nd4 || !nd5 || !nd6)
							continue;
				
				
						int vid[3];
						vid[0] = nodes[i][0];
						vid[1] = nodes[i][1];
						
						if (nodes[j][0] != points[vid[0]] && nodes[j][0] != points[vid[1]]) 
							vid[2] = nodes[j][0];
						else if (nodes[j][1] != points[vid[0]] && nodes[j][1] != points[vid[1]])
							vid[2] = nodes[j][1];
						
						else if (nodes[k][0] != points[vid[0]] && nodes[k][0] != points[vid[1]]) 
							vid[2] = nodes[k][0];
						else if (nodes[k][1] != points[vid[0]] && nodes[k][1] != points[vid[1]]) 
							vid[2] = nodes[k][1];
						
						if (math::in_triangle(v, points[vid[0]], points[vid[1]], points[vid[2]]))
							return 1;
					}
					
		return 0;
	};
	
	bool contains_point(const ivec2& v) {
		for (int i = 0; i < total_points; ++i)
			if (points[i] == v)
				return 1;
			
		return 0;
	};
	
	bool self_intersects(const ivec2& a, const ivec2& b) {
		for (int i = 0; i < total_nodes; ++i)
			if (a != points[nodes[i][0]] && b != points[nodes[i][0]] && a != points[nodes[i][1]] && b != points[nodes[i][1]]) 
				if (math::intersects(a, b, points[nodes[i][0]], points[nodes[i][1]]))
					return 1;
			
		return 0;
	};
	
	static const int MIN_DISTANCE = 4;
	
	bool closest_distance_check(const ivec2& v) {
		for (int i = 0; i < total_nodes; ++i)
			if (v.dist_line(points[nodes[i][0]], points[nodes[i][1]]) < MIN_DISTANCE)
				return 0;
		
		return 1;
	};
	
	void collect_triangles() {
		total_triangles = 0;
		
		// Collect triangles by splitting nodes into them
		for (int i = 0; i < total_nodes; ++i)
			for (int j = 0; j < total_nodes; ++j)
				for (int k = 0; k < total_nodes; ++k) 
					if (i != j && j != k && k != i) {
						bool nd1 = 	nodes[i][0] == nodes[j][0] || nodes[i][0] == nodes[j][1]
									||
									nodes[i][0] == nodes[k][0] || nodes[i][0] == nodes[k][1];
						bool nd2 = 	nodes[i][1] == nodes[j][0] || nodes[i][1] == nodes[j][1]
									||
									nodes[i][1] == nodes[k][0] || nodes[i][1] == nodes[k][1];
									
						bool nd3 = 	nodes[j][0] == nodes[i][0] || nodes[j][0] == nodes[i][1]
									||
									nodes[j][0] == nodes[k][0] || nodes[j][0] == nodes[k][1];
						bool nd4 = 	nodes[j][1] == nodes[i][0] || nodes[j][1] == nodes[i][1]
									||
									nodes[j][1] == nodes[k][0] || nodes[j][1] == nodes[k][1];
									
						bool nd5 = 	nodes[k][0] == nodes[i][0] || nodes[k][0] == nodes[i][1]
									||
									nodes[k][0] == nodes[j][0] || nodes[k][0] == nodes[j][1];
						bool nd6 = 	nodes[k][1] == nodes[i][0] || nodes[k][1] == nodes[i][1]
									||
									nodes[k][1] == nodes[j][0] || nodes[k][1] == nodes[j][1];
									
						if (!nd1 || !nd2 || !nd3 || !nd4 || !nd5 || !nd6)
							continue;
				
				
						int vid[3];
						vid[0] = nodes[i][0];
						vid[1] = nodes[i][1];
						
						if (nodes[j][0] != points[vid[0]] && nodes[j][0] != points[vid[1]]) 
							vid[2] = nodes[j][0];
						else if (nodes[j][1] != points[vid[0]] && nodes[j][1] != points[vid[1]])
							vid[2] = nodes[j][1];
						
						else if (nodes[k][0] != points[vid[0]] && nodes[k][0] != points[vid[1]]) 
							vid[2] = nodes[k][0];
						else if (nodes[k][1] != points[vid[0]] && nodes[k][1] != points[vid[1]]) 
							vid[2] = nodes[k][1];
						
						bool is_triangle = 1;
						// Check for all points being outside of the triangle for this being triangle
						for (int p = 0; p < total_points; ++p) 
							if (p != vid[0] && p != vid[1] && p != vid[2])
								if (math::in_triangle(points[p], points[vid[0]], points[vid[1]], points[vid[2]])) {
									is_triangle = 0;
									break;
								}
								
						if (is_triangle) 
							triangles[total_triangles++] = {{vid[0], vid[1], vid[2]}};
					}
	};
	
	void triangulate() {
		total_nodes = 0;
		total_points = 0;
		
		for (int i = 0; i < POINTS_AMOUNT; ++i)
			for (int j = 0; j < POINTS_AMOUNT; ++j)
				
		for (int i = 0; i < POINTS_AMOUNT; ++i)	
			while (total_points < POINTS_AMOUNT) {
				// Create point and check triangulation conditions
				ivec2 point = random_point();
				
				if (contains_point(point))
					continue;
				
				// Three first are absolute random
				if (total_points < 2) {			
					points[total_points++] = point;
					
					continue;
				} else if (total_points == 2) {
					points[total_points++] = point;
					
					nodes[total_nodes]  [0] = 0;
					nodes[total_nodes++][1] = 1;
					
					nodes[total_nodes]  [0] = 1;
					nodes[total_nodes++][1] = 2;
					
					nodes[total_nodes]  [0] = 2;
					nodes[total_nodes++][1] = 0;
					continue;
				}
				
				// this shit should run forever :D
				// if (in_triangle(point))
				// 	continue;
				
				// Distance to closes node >= 16px
				if (!closest_distance_check(point))
					continue;
				
				points[total_points++] = point;
				
				// Connect this point with all other points that will not insersect
				for (int i = 0; i < total_points - 1; ++i) {
					// Check for self-insersection of the nodes
					if (!self_intersects(point, points[i])) {
						nodes[total_nodes][0] = i;
						nodes[total_nodes++][1] = total_points - 1;
					}
				}
			}
		
		collect_triangles();
	};
	
	ivec2 random_point() {
		return ivec2(rand() % static_cast<int>(get_window().get_width() + 1), rand() % static_cast<int>(get_window().get_height() + 1));
	};
	
	void start() {
		triangulate();
	};
	
	void resize() {
		resized = 1;
	};
	
	void loop() {
		window& w = get_window();
		painter& p = w.get_paint();
		
		// Block untill event is reached
		if (!mouse_down) w.wait_event(1);
		
		if (w.has_key_event(0))
			if (w.get_key_down() == KEY_ESCAPE)
				w.stop();
			else if (w.get_key_down() == KEY_R) {
				triangulate();
				resized = 1;
			}
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (mouse_down || resized) { // REPAINTING
			window::pointer point = w.get_pointer();
			last_pointer.x = point.x;
			last_pointer.y = point.y;
			p.clear();
			
			if (mouse_down) {
				triangle matched_triangle = get_triangle(last_pointer);
				if (matched_triangle.vert_ids[0] != -1) {
					
					p.color(255, 0, 255);
					XPoint verts[4] = {
										Point(points[matched_triangle.vert_ids[0]].x, points[matched_triangle.vert_ids[0]].y), 
										Point(points[matched_triangle.vert_ids[1]].x, points[matched_triangle.vert_ids[1]].y), 
										Point(points[matched_triangle.vert_ids[2]].x, points[matched_triangle.vert_ids[2]].y),
										Point(points[matched_triangle.vert_ids[0]].x, points[matched_triangle.vert_ids[0]].y)
									};
					
					p.fill_poly(verts, 4);
					
					string ind = to_string(matched_triangle.vert_ids[0]) + ", " + to_string(matched_triangle.vert_ids[1]) + ", " + to_string(matched_triangle.vert_ids[2]);
					p.text(16, 16, ind.c_str());
				}
			}
			
			if (last_pointer.x != -1) {
				p.color(0, 0, 0);
				p.arc(last_pointer.x-10, last_pointer.y-10, 20, 20);
			}
			p.color(0, 255, 0);
			p.arc(point.x-10, point.y-10, 20, 20);
			
			p.color(0, 0, 255);
			
			for (int i = 0; i < total_nodes; ++i) 
				p.line(points[nodes[i][0]].x, points[nodes[i][0]].y, points[nodes[i][1]].x, points[nodes[i][1]].y);
			
			p.color(0, 255, 255);
			
			for (int i = 0; i < total_points; ++i) {
				p.color(0, 255, 255);
				p.point(points[i].x, points[i].y);
				p.color(255, 255, 0);
				string ind = to_string(i);
				p.text(points[i].x, points[i].y, ind.c_str());
			}

			resized = 0;
		}
	};
};


int main() {	
	scene s;
	window w(&s, 200, 200, 0);
	w.start();
	return 0;
};

