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

#include "spaint.h"
#include "vec2.h"

using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE 9

// Right turn visualisation using spaint tool.

// bash c.sh "-lX11" right_turn

// integer coord cast

struct ivec2 {
	int x, y;
	
	ivec2(int x, int y) { 
		this->x =x;
		this->y = y;
	}
	
	ivec2(int r) {
		x = y = r;
	}
	
	ivec2(double x, double y) {
		this->x = x;
		this->y = y;
	}
	
	ivec2(double r) {
		x = y = r;
	};
	
	ivec2(const vec2& v) {
		x = v.x;
		y = v.y;
	};
};

// curve function

inline double t_a() { return 0.0; }

inline double t_b() { return 1.0; }

inline double t_d() { return 0.00001; }

vec2 curve(double t) {
	t *= 100.0;
	return vec2(t * 4.0, 100.0 * (0.5+cos(t)*0.5));
};

// rendering

class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Right turn example");
	};
	
	bool mouse_down = 0;
	int last_turn = 2;
	int last_x = -1, last_y = -1;
	bool resized = 1;
	double t_local = 0;
	
	void resize() {
		resized = 1;
	};
	
	void loop() {
		window& w = get_window();
		painter& p = w.get_paint();
		
		// Block untill event is reached
		// if (!mouse_down) w.wait_event(1);
		
		if (w.has_key_event(0))
			if (w.get_key_down() == KEY_ESCAPE)
				w.stop();
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (mouse_down || resized) {
			window::pointer point = w.get_pointer();			
			p.clear();
			
			if (last_x != -1) {
				p.color(0, 0, 0);
				p.arc(last_x-10, last_y-10, 20, 20);
			}
			last_x = point.x;
			last_y = point.y;
			p.color(0, 255, 0);
			p.arc(point.x-10, point.y-10, 20, 20);
			
			p.color(155, 0, 0);
			double t = t_a();
			t_local = 0;
			
			for (; t < t_b(); t += t_d()) {
				ivec2 v(curve(t));
				p.point(v.x, v.y);
			}
			
			resized = 0;
		} else {
			t_local += t_d();
			ivec2 v(curve(t_local));
			
			p.color(0, 0, 255);
			p.arc(v.x-2, v.y-2, 4, 4);
			
			if (t_local > t_b())
				t_local = 0;
		}
	};
};


int main() {
	scene s;
	window w(&s, 200, 200, 0);
	w.start();
	return 0;
};

