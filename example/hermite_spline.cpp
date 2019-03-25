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

#include <cmath>
#include <vector>

#include "spaint.h"
#include "cppmath.h"
#include "vec2.h"
#include "ivec2.h"

using namespace std;
using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE  9
#define KEY_R      27

// Hermite spline by points visualisation using spaint tool.

// bash c.sh "-lX11" example/hermite_spline

// curve function

inline double t_a() { return 0.0; }

inline double t_b() { return 1.0; }

inline double t_d() { return 0.00001; }

vec2 curve(double t) {
	t *= 6.28318530718;
	return vec2(50.0) + vec2(100.0 * (0.5 + std::sin(t) * 0.5) * std::cos(t * 0.5), 100.0 * (0.5 + std::cos(t) * 0.5)).mul(1.0);
};

// rendering

class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Bezier draw example");
	};
	
	vector<vec2> points;
	
	bool mouse_down = 0;
	ivec2 last_pointer;
	bool resized = 1;
	bool updated = 0;
	
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
				points.clear();
				updated = 1;
			}
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (mouse_down) {
			// XXX: Move points.
			// XXX: edit derivatives
                        // XXX: ceate function for rendering in namespace

			window::pointer point = w.get_pointer();
			points.push_back(vec2(point.x, point.y));
			
			mouse_down = 0;
			updated = 1;
		}
			
		if (resized || updated) {
			window::pointer point = w.get_pointer();			
			p.clear();
			
			last_pointer.x = point.x;
			last_pointer.y = point.y;
			
			p.color(0, 255, 0);
			p.arc(point.x-10, point.y-10, 20, 20);
			
			if (points.size() > 1) {
				
				p.color(255, 0, 0);
				
				const int CURVE_STEPS = 1000;
				double d = 1.0 / (double) CURVE_STEPS;
				
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
				
				vector<vec2> derivatives;
				for (int i = 0; i < points.size(); ++i) {
					vec2 ptan;
					
					if (i == 0) 
						ptan = points[1] - points[0];
					else if (i == points.size() - 1)
						ptan = points[i] - points[i - 1];
					else
						ptan = (points[i + 1] - points[i - 1]) / 2.0;
						
					derivatives.push_back(ptan);
				}
				
				for (int i = 0; i < points.size() - 1; ++i) {
					for (double t = 0.0; t <= 1.0; t += d) {
						double x = P(t, points[i].x, points[i + 1].x, derivatives[i].x, derivatives[i + 1].x);
						double y = P(t, points[i].y, points[i + 1].y, derivatives[i].y, derivatives[i + 1].y);
						
						p.point(x, y);
					}
				}
			
				p.color(0, 0, 255);
				
				for (int i = 0; i < points.size() - 1; ++i) 
					p.line(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);	
			}
			
			p.color(0, 255, 255);
			
			for (int i = 0; i < points.size(); ++i) {
				p.color(0, 255, 255);
				p.point(points[i].x, points[i].y);
				p.color(255, 255, 0);
				string ind = to_string(i);
				p.text(points[i].x, points[i].y, ind.c_str());
			}
			
			resized = 0;
			updated = 0;
		}
	};
};


int main() {
	scene s;
	window w(&s, 200, 200, 0);
	w.start();
	return 0;
};
