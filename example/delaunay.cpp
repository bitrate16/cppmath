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
#include "delaunay.h"

using namespace std;
using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE  9
#define KEY_R      27
#define KEY_C      54
#define KEY_I      31

// Right turn visualisation using spaint tool.

// bash c.sh "-lX11" example/delaunay

class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Delaunay example");
	};
	
	static const int POINTS_AMOUNT = 32;
	
	// Keep all randomized points
	std::vector<vec2> points;
	
	delaunay::delaunay dln;
	
	bool mouse_down = 0;
	ivec2 last_pointer;
	
	bool resized = 1;
	
	bool render_circles = 0; // C
	bool render_indices = 0; // I
	
	// Returns triangle cointaining passed point.
	int get_triangle(const ivec2& v) {
		if (points.size() < 3)
			return -1;
		
		for (int i = 0; i < dln.triangles.size(); ++i) 
			if (math::in_triangle(v, dln.triangles[i].p0, dln.triangles[i].p1, dln.triangles[i].p2)) 
				return i;
					
		return -1;
	};
	
	void triangulate() {
		points.clear();
		
		for (int i = 0; i < POINTS_AMOUNT; ++i)
			points.push_back(random_point());
		
		dln = delaunay::triangulate(points);
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
			} else if (w.get_key_down() == KEY_C) {
				render_circles = !render_circles;
				resized = 1;
			} else if (w.get_key_down() == KEY_I) {
				render_indices = !render_indices;
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
				int matched_triangle = get_triangle(last_pointer);
				if (matched_triangle != -1) {
					
					p.color(255, 0, 255);
					delaunay::triangle& tr = dln.triangles[matched_triangle];
					
					XPoint verts[4] = {
										Point(tr.p0.x, tr.p0.y), 
										Point(tr.p1.x, tr.p1.y), 
										Point(tr.p2.x, tr.p2.y),
										Point(tr.p0.x, tr.p0.y)
									};
					
					p.fill_poly(verts, 4);
					
					string ind = to_string(matched_triangle);
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
			
			// Edges
			for (int i = 0; i < dln.edges.size(); ++i) 
				p.line(dln.edges[i].p0.x, dln.edges[i].p0.y, dln.edges[i].p1.x, dln.edges[i].p1.y);
			
			p.color(0, 255, 255);
			
			// Circles
			if (render_circles)
				for (int i = 0; i < dln.triangles.size(); ++i) {
					double x1 = dln.triangles[i].p0.x;
					double y1 = dln.triangles[i].p0.y;
					double x2 = dln.triangles[i].p1.x;
					double y2 = dln.triangles[i].p1.y;
					double x3 = dln.triangles[i].p2.x;
					double y3 = dln.triangles[i].p2.y;
					
					int x12 = x1 - x2; 
					int x13 = x1 - x3; 
				  
					int y12 = y1 - y2; 
					int y13 = y1 - y3; 
				  
					int y31 = y3 - y1; 
					int y21 = y2 - y1; 
				  
					int x31 = x3 - x1; 
					int x21 = x2 - x1; 
				  
					// x1^2 - x3^2 
					int sx13 = pow(x1, 2) - pow(x3, 2); 
				  
					// y1^2 - y3^2 
					int sy13 = pow(y1, 2) - pow(y3, 2); 
				  
					int sx21 = pow(x2, 2) - pow(x1, 2); 
					int sy21 = pow(y2, 2) - pow(y1, 2); 
				  
					int f = ((sx13) * (x12) 
							 + (sy13) * (x12) 
							 + (sx21) * (x13) 
							 + (sy21) * (x13)) 
							/ (2 * ((y31) * (x12) - (y21) * (x13))); 
					int g = ((sx13) * (y12) 
							 + (sy13) * (y12) 
							 + (sx21) * (y13) 
							 + (sy21) * (y13)) 
							/ (2 * ((x31) * (y12) - (x21) * (y13))); 
				  
					int c = -pow(x1, 2) - pow(y1, 2) - 2 * g * x1 - 2 * f * y1; 
				  
					// eqn of circle be x^2 + y^2 + 2*g*x + 2*f*y + c = 0 
					// where centre is (h = -g, k = -f) and radius r 
					// as r^2 = h^2 + k^2 - c 
					int h = -g; 
					int k = -f; 
					int sqr_of_r = h * h + k * k - c; 
				  
					// r is the radius 
					float r = sqrt(sqr_of_r); 
				  
					p.arc(h - r, k - r, 2.0 * r, 2.0 * r);
				}
							
			p.color(0, 255, 255);
			
			if (render_indices)
				for (int i = 0; i < points.size(); ++i) {
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

