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

using namespace spaint;

#define KEY_ESCAPE 9
#define KEY_R      27

// Z-Buffer visualisation using spaint tool.

// bash c.sh "-lX11" example/ImageBuffer_example


class scene : public component {
	
	void create() {
		get_window().set_title("ImageBuffer example");
		
		updated = 1;
		buffer  = get_paint().createImageBuffer(250, 250);
		buffer.set_color(Color::RED);
	};
	
	void destroy() {
		get_paint().destroyImageBuffer(buffer);
	};
	
	bool mouse_down = 0;
	bool resized = 0;
	bool updated = 0;
	
	ImageBuffer buffer;
	
	void resize() {
		resized = 1;
	};
	
	int runner = 0;
	
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
			
			if (mouse_down) {
				int y = runner;
				if (y == 250) {
					buffer.set_color(Color::BLACK);
					buffer.fill();
					buffer.set_color(Color::RED);
					runner = 0;
					return;
				}
				
				for (int x = 0; x < 250; ++x)
					buffer.set_pixel(x, y);
				buffer.put(0, 0);
				
				++runner;
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

