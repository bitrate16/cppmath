/*

	spaint paint example
	
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

// Example directory
#include "include/spaint.h"

#include <unistd.h>
#include <iostream>

// bash c.sh "$(pkg-config --cflags --libs x11)" sandbox

// ESC - exit
// W - clear
// Scroll - resize brush

using namespace spaint;
using namespace std;

spaint::window win;

class scene : public spaint::component {
	public:
	
	scene() {};
	
	void start() {
		get_window().get_paint().color(255, 255, 0);
	};
	
	int prev_x = -1, prev_y = -1;
	int stroke_size = 1;
	void loop() {
		spaint::window& w = get_window();
		spaint::painter& p = w.get_paint();
		
		// Check for key
		if (w.has_key_event(0)) 
			if (w.get_key_down() == 0x09) 
				w.stop();
			else if (w.get_key_down() == 25) 
				p.clear();
			
		// Check for scroll
		if (w.has_scroll_event(0)) {
			if (w.get_scroll() == 1)
				p.line_style(++stroke_size);
			if (w.get_scroll() == -1)
				p.line_style(--stroke_size);
		}
		
		// Clear event queue
		w.clear_events();
		
		// Get current mouse location & paint
		spaint::window::pointer pt = w.get_pointer();
		if (prev_x != -1)
			p.line(pt.x, pt.y, prev_x, prev_y);
		
		
		prev_x = pt.x;
		prev_y = pt.y;
	};
};

int main() {
	// Create scene and window
	scene s;
	window win(&s, 400, 400, 0);
	win.start();
	
	return 0;
};
