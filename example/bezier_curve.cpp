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

#define KEY_ESCAPE 9

// Bezier curve by points visualisation using spaint tool.

// bash c.sh "-lX11" example/bezier_qurve

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
		
		if (w.has_key_event(0)) {
			cout << w.get_key_down() << endl;
			if (w.get_key_down() == KEY_ESCAPE)
				w.stop();
		}
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (mouse_down) {
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
				
				double curve_len = math::bezier_length(CURVE_STEPS, points);
				cout << " curve length = " << curve_len << endl;
				
				// Bezier:
				// B(t) = SUM [ i = 1; i <= N; P[i] * C[N, i] * t^i * (1 - t)^i ]
			
				double d = 1.0 / (double) CURVE_STEPS;
				
				std::vector<vec2> const_multipliers; // [::] = P[i] * C[N, i]
				for (int i = 0; i < points.size(); ++i) {
					double C = (double) math::factorial(points.size()) / ((double) math::factorial(i + 1) * (double) math::factorial(points.size() - i - 1));
					const_multipliers.push_back(points[i] * C);
				}
				
				for (double t = 0.0; t <= 1.0; t += d) {
					vec2 point;
					
					for (double i = 1; i < points.size() + 1; i += 1.0) {
						double part_mult = std::pow(t, i) * std::pow(1 - t, i);
						point += const_multipliers[(int) i - 1] * part_mult;
					}
					
					p.point(point.x, point.y);
				}				
			}
			
			p.color(0, 255, 255);
			for (int i = 0; i < points.size(); ++i)
				p.point(points[i].x, points[i].y);
			
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

