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
#include <unistd.h>

#include "spaint.h"
#include "ivec2.h"
#include "vec2.h"

#include "math_func.h"
#include "math_func_util.h"

using namespace spaint;
using namespace cppmath;
using namespace math_func;

#define KEY_ESCAPE 9
#define KEY_R      27

// Evolute visualisation using spaint tool.

// bash c.sh "-lX11" example/evolute


class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Evolute example");
		
		std::string str_x = "width / 2 + (width / 8) * cos(t) ^ 3";
		std::string str_y = "height / 2 + (height / 8) * sin(t) ^ 3";
		
		f_x = math_func::parse(str_x);
		f_y = math_func::parse(str_y);
		
		math_func::func* df_x = math_func::derivate(f_x, "t");
		math_func::func* df_y = math_func::derivate(f_y, "t");
		
		math_func::func* df2_x = math_func::derivate(df_x, "t");
		math_func::func* df2_y = math_func::derivate(df_y, "t");
		
		func* mult = 	div(sum(pow(df_x->copy(), 2),
								pow(df_y->copy(), 2)),
							sub(mul(df_x->copy(),
									df2_y),
								mul(df2_x,
									df_y->copy())));
									
		func* f = optimize(mult);
		delete mult;
		mult = f;
		
		c_x = sub(f_x->copy(), mul(df_y, mult->copy()));
		f = optimize(c_x);
		delete c_x;
		c_x = f;
		
		c_y = sum(f_y->copy(), mul(df_x, mult));
		f = optimize(c_y);
		delete c_y;
		c_y = f;
		
		values["width"]  = get_window().get_width();
		values["height"] = get_window().get_height();
		values["pi"] = 3.141628;
		
		functions["sin"] = [](const std::vector<double>& args) -> double {
			return std::sin(args[0]);
		};
		functions["cos"] = [](const std::vector<double>& args) -> double {
			return std::cos(args[0]);
		};
		
		updated = 1;
	};
	
	void destroy() {
		delete c_x;
		delete c_y;
		delete f_x;
		delete f_y;
	};
	
	// Vector-function
	math_func::func* f_x;
	math_func::func* f_y;
	
	// Vector-evolute-function
	math_func::func* c_x;
	math_func::func* c_y;
	
	// Map with constants
	math_func::func_constants values;
	math_func::func_functions functions;
	
	static constexpr double start = 0.0;
	static constexpr double end   = 6.4;
	static constexpr double step  = 0.01;
	
	// Evolute state
	vec2 evol_prev;
	double evol_t;
	bool evol_color = 1;
	
	bool mouse_down = 0;
	bool resized = 0;
	bool updated = 0;
	
	void resize() {
		values["width"]  = get_window().get_width();
		values["height"] = get_window().get_height();
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
			if (w.get_key_down() == KEY_R) 
				updated = 1;
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (resized || updated) {
			
			resized = 0;
			updated = 0;
			
			p.clear();
			
			p.color(255, 0, 0);
			
			values["t"] = start;
			vec2 prev_eval = vec2(f_x->evaluate(values, functions), f_y->evaluate(values, functions));
			
			for (double t = start + step; t <= end; t += step) {
				values["t"] = t;
				vec2 point = vec2(f_x->evaluate(values, functions), f_y->evaluate(values, functions));
				
				p.line(prev_eval.x, prev_eval.y, point.x, point.y);
				prev_eval = point;
			}
			
			evol_t = step;
			evol_color = 1;
			
			values["t"] = evol_t;
			evol_prev = vec2(f_x->evaluate(values, functions), f_y->evaluate(values, functions));
			
		} else {
			if (evol_color)
				p.color(255, 255, 255);
			else
				p.color(0, 0, 0);
			
			evol_t += step;
			
			if (evol_t >= end) {
				evol_t = step;
				evol_color = !evol_color;
			}
			
			values["t"] = evol_t;
			vec2 point = vec2(c_x->evaluate(values, functions), c_y->evaluate(values, functions));
			
			p.line(evol_prev.x, evol_prev.y, point.x, point.y);
			evol_prev = point;
			
			usleep(10000);
		}
	};
};


int main() {
	scene s;
	window w(&s, 200, 200, 0);
	w.start();
	return 0;
};

