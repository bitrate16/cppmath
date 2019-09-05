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
#include "ivec3.h"
#include "vec2.h"
#include "ZBuffer.h"

using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE 9
#define KEY_R      27

// Z-Buffer visualisation using spaint tool.

// bash c.sh "-lX11" example/z_buffer


class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("Z-Buffer example");
		
		updated = 1;
	};
	
	void destroy() {
		
	};
	
	bool mouse_down = 0;
	bool resized = 0;
	bool updated = 0;
	
	ZBuffer zbuf = ZBuffer(400, 400);
	vec3 A = vec3(10, 10, 1), B = vec3(30, 20, 1), C = vec3(50, 15, 1);
	
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
			if (w.get_key_down() == KEY_R) 
				updated = 1;
		
		if (w.has_mouse_event(0)) 
			if (w.get_button_down() == Button1) 
				mouse_down = 1;
			else if (w.get_button_up() == Button1) 
				mouse_down = 0;
			
		w.clear_events();
			
		if (resized || updated || mouse_down) {
			
			resized = 0;
			updated = 0;
			
			p.clear();
			
			zbuf.clear();
			
			// zbuf.line(A, B, Color::RED);
			// zbuf.line(B, C, Color::RED);
			// zbuf.line(C, A, Color::RED);
			
			Color c1(255, 0, 0);
			Color c2(0, 255, 0);
			Color c3(0, 0, 255);
			zbuf.triangle(A, B, C, c1, c2, c3);
			
			int scale = 4;
			
			for (int x = 0; x < zbuf.get_width(); ++x)
				for (int y = 0; y < zbuf.get_height(); ++y)
					if (zbuf.get(x, y).z >= 0) {
						p.color(zbuf.get(x, y).r, zbuf.get(x, y).g, zbuf.get(x, y).b);
						for (int i = 0; i < scale; ++i)
							for (int j = 0; j < scale; ++j)
								p.point(x * scale + i, y * scale + j);
					}
			
			if (mouse_down) {
				window::pointer point = w.get_pointer();
				int x = point.x / scale;
				int y = point.y / scale;
				
				B.x = x;
				B.y = y;
			
				p.color(255, 0, 255);
				
				for (int i = 0; i < scale; ++i)
					for (int j = 0; j < scale; ++j)
						p.point(x * scale + i, y * scale + j);
					
				//std::cout << "Z = " << zbuf.get(x, y).z << std::endl;
			}
			
		} else {
			// ...
		}
	};
};


int main() {
	scene s;
	window w(&s, 250, 250, 0);
	w.start();
	return 0;
};

