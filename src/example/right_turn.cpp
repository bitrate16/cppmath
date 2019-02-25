/*
    Example shows use of spaint and cppmath::math.right_turn
	
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

#include "include/spaint.h"
#include "include/math.h"

using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE 9

// Right turn visualisation using spaint tool.

// bash c.sh "-lX11" right_turn

class scene : public component {
	
	void create() {
		get_paint().init_font();
	};
	
	bool mouse_down = 0;
	int last_turn = 2;
	int last_x = -1, last_y = -1;
	bool resized = 1;
	
	void resize() {
		resized = 1;
	};
	
	void loop() {
		window& w = get_window();
		painter& p = w.get_paint();
		
		// Block untill event is reached
		if (!mouse_down) w.wait_event();
		
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
			
			p.color(255, 255, 0);
			p.line(50, 50, w.get_width()-50, w.get_height()-50);
			p.text(50, 50, "B");
			p.text(w.get_width()-50, w.get_height()-50, "A");
			
			p.color(64, 0, 64);
			p.line(45, 55, w.get_width()-55, w.get_height()-45);
			p.line(55, 45, w.get_width()-45, w.get_height()-55);
			
			
			if (last_x != -1) {
				p.color(0, 0, 0);
				p.arc(last_x-10, last_y-10, 20, 20);
			}
			last_x = point.x;
			last_y = point.y;
			p.color(0, 255, 0);
			p.arc(point.x-10, point.y-10, 20, 20);
			
			p.color(0, 0, 255);
			int turn;
			int turn1 = math::right_turn(vec2(point.x, point.y), vec2(45, 55), vec2(w.get_width()-55, w.get_height()-45));
			int turn2 = math::right_turn(vec2(point.x, point.y), vec2(55, 45), vec2(w.get_width()-45, w.get_height()-55));
			
			// Fat matching line
			if (turn1 == -1 && turn2 == 1 || turn1 == 0 || turn2 == 0)
				turn = 0;
			
			// Left-axis
			if (turn1 == 1)
				turn = -1;
			if (turn2 == -1)
				turn = 1;
			
			if (last_turn != turn || resized) {
				p.clear_rect(0, 0, w.get_width(), 16);
				p.text(10, 10, turn == 0 ? "inline" : (turn == 1 ? "right" : "left"));
				last_turn = turn;
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

