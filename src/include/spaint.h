/*
    primitive drawing framework using xlib
	
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

#pragma once

#include <X11/Xlib.h>
#include <stdlib.h>
#include <functional>

// Require $(pkg-config --cflags --libs x11) linking

// Simple paint
namespace spaint {
	
	// Scene wrapper
	class window;
	class component {
		friend window;
		
	
	public:
		window* win;
	
		inline window& get_window() {
			return *win;
		};
	
		virtual void start() {};
		virtual void stop() {};
		
		virtual void create() {};
		virtual void destroy() {};
		
		virtual void loop() {};
	};
	
	class spaint {
		friend class window;
		
		Display *dsp;
		Window   win;
		GC        gc;
		XGCValues values;
		
		Colormap cmap;
		XColor current;
		
	public:
	
		inline bool color(int r, int g, int b) {
			current.flags = DoRed | DoGreen | DoBlue;
			current.red = r << 8;
			current.green = g << 8;
			current.blue = b << 8;
			Status rc = XAllocColor(dsp, cmap, &current);
			if (rc == 0)
				return 0;
			XSetForeground(dsp, gc, current.pixel);
			return 1;
		};
		
		inline void clear() {
			XClearWindow(dsp, win);
		};
		
		inline void flush() {
			XFlush(dsp);
		};
		
		inline void point(int x, int y) {
			XDrawPoint(dsp, win, gc, x, y);
		};
		
		inline void line(int x1, int y1, int x2, int y2) {
			XDrawLine(dsp, win, gc, x1, y1, x2, y2);
		};
		
		inline void arc(int x, int y, int width, int height, int angle1 = 0, int angle2 = 360 * 64) {
			XDrawArc(dsp, win, gc, x, y, width, height, angle1, angle2);
		};
		
		inline void fill_rect(int x, int y, int width, int height) {
			XFillRectangle(dsp, win, gc, x, y, width, height);
		};
		
		inline void line_style(int line_width, int line_style = LineSolid, int cap_style = CapButt, int join_style = JoinBevel) {
			XSetLineAttributes(dsp, gc, line_width, line_style, cap_style, join_style);
		};
		
		inline void fill_style(int style = FillSolid) {
			XSetFillStyle(dsp, gc, style);
		};
	};
	
	class window {		
		XEvent evt;
		bool has_event = 0;
		
		// Extract next event
		void pump_event() {			
			has_event = 0;
			while (XPending(paint.dsp)) {
				XNextEvent(paint.dsp, &evt);
				if (evt.type == ClientMessage) // Pump quit
					if (evt.xclient.data.l[0] == wmDelete) {
						state = 0;
						return;
					}
				if (evt.type == ConfigureNotify) // Pump resize
					if (evt.xconfigure.width != width || evt.xconfigure.height != height) {
						width = evt.xconfigure.width;
						height = evt.xconfigure.height;
						continue;
					}
				has_event = 1;
			}
		};
		
		unsigned int white;
		unsigned int black;
		
		// Reference to component
		component* comp = nullptr;
		
		// Screen number
		int screen;
		
		// Default background
		bool background;
		
		// Delete event
		Atom wmDelete;
		
		// Is running
		bool state = 1;
		
	public:
		
		int width;
		int height;
		
		spaint paint;
		
		window() {};
		
		window(component* _comp, int _width, int _height, bool _background = 1) : comp(_comp), 
																					width(_width), 
																					height(_height), 
																					background(_background) {
			if (comp == nullptr)
				throw "component is null";
							
			comp->win = this;
																						
			paint.dsp = XOpenDisplay(nullptr);
			if(!paint.dsp) 
				throw "failed open display"; 

			screen = DefaultScreen(paint.dsp);
			white = WhitePixel(paint.dsp, screen);
			black = BlackPixel(paint.dsp, screen);


			// Create window
			paint.win = XCreateSimpleWindow(paint.dsp, DefaultRootWindow(paint.dsp),
										0, 0, 
										width, height, // size
										0, black,      // border width/clr
										_background ? white : black); // backgrd clr


			// Window close event
			wmDelete = XInternAtom(paint.dsp, "WM_DELETE_WINDOW", true);
			XSetWMProtocols(paint.dsp, paint.win, &wmDelete, 1);
			
			
			// Create GC
			unsigned long valuemask;
			
			paint.gc = XCreateGC(paint.dsp, paint.win, valuemask, &paint.values);
			if (paint.gc < 0)
				throw "failed create GC";
			
			// Allocate foreground and background colors for this GC
			if (!_background) {
				XSetForeground(paint.dsp, paint.gc, WhitePixel(paint.dsp, screen));
				XSetBackground(paint.dsp, paint.gc, BlackPixel(paint.dsp, screen));
			} else {
				XSetForeground(paint.dsp, paint.gc, BlackPixel(paint.dsp, screen));
				XSetBackground(paint.dsp, paint.gc, WhitePixel(paint.dsp, screen));
			}
			
			XSync(paint.dsp, false);
			
			
			// Create colormap
			paint.cmap = DefaultColormap(paint.dsp, screen);
			

			// Input
			long eventMask = StructureNotifyMask;
			eventMask |= ButtonPressMask | ButtonReleaseMask // Mouse
			           | KeyPressMask      | KeyReleaseMask;
			XSelectInput(paint.dsp, paint.win, eventMask);
			
			
			// Show window
			XMapWindow(paint.dsp, paint.win);
			XFlush(paint.dsp);
			
			comp->create();
		};
		
		window& operator=(const window& w) {
			this->evt = w.evt;
			this->has_event = w.has_event;
			this->white = w.white;
			this->black = w.black;
			this->comp = w.comp;
			this->screen = w.screen;
			this->background = w.background;
			this->wmDelete = w.wmDelete;
			this->state = w.state;
			this->width = w.width;
			this->height = w.height;
			this->paint = w.paint;
			
			// Pass pointer to new window on assignment
			if (this->comp)
				this->comp->win = this;
		};
		
		inline spaint& get_paint() {
			return paint;
		};
		
		// Check for new events, skip quit & resize
		inline bool check_event() {
			if (has_event)
				return 1;
			pump_event();
			return has_event;
		};
		
		inline bool next_event() {
			pump_event();
			return has_event;
		};
		
		// Get event
		inline XEvent& get_event() {
			has_event = 0;
			return evt;
		};
		int n = 0;
		void start() {
			comp->start();
			
			while (state) {
				check_event();
				
				comp->loop();
			}
			
			comp->stop();
			comp->destroy();
		};
	
		void stop() {
			state = 0;
			++n;
		};
	
		int get_width() {
			return width;
		};
		
		int get_height() {
			return height;
		};
		
		inline void clear_events() {
			while (check_event()) get_event();
		};
		
		// Event handling
		int get_scroll() { 
			if (evt.type == ButtonPress || evt.type == ButtonRelease)
				if (evt.xbutton.button == Button4)
					return 1;
				else if (evt.xbutton.button == Button5)
					return -1;
			return 0;
		};
	
		int get_scroll_x() {
			if (evt.type == ButtonRelease)
				return evt.xbutton.x;
		};
	
		int get_scroll_y() {
			if (evt.type == ButtonRelease)
				return evt.xbutton.y;
		};
	
		bool has_scroll_event(bool ignore_other = 1) { // pop all events till expected reached
			while (check_event()) {
				if ((evt.type == ButtonPress || evt.type == ButtonRelease) && (evt.xbutton.button == Button4 || evt.xbutton.button == Button5)) {
					get_event();
					return 1;
				} else if (!ignore_other) 
					return 0;
				get_event();
			}
			return 0;
		};
	
		bool has_mouse_event(bool ignore_other = 1) {
			while (check_event()) {
				if ((evt.type == ButtonPress || evt.type == ButtonRelease) && !(evt.xbutton.button == Button4 || evt.xbutton.button == Button5)) {
					get_event();
					return 1;
				} else if (!ignore_other)
					return 0;
				get_event();
			}
			return 0;
		};
		
		int get_mouse_x() {
			if (evt.type == ButtonPress || evt.type == ButtonRelease)
				return evt.xbutton.x;
			return -1;
		};
		
		int get_mouse_y() {
			if (evt.type == ButtonPress || evt.type == ButtonRelease)
				return evt.xbutton.y;
			return -1;
		};
	
		// Returns number of pressed button
		int get_button_down() {
			if (evt.type == ButtonPress)
				return evt.xbutton.button;
		};
		
		// Returns number of released button
		int get_button_up() {
			if (evt.type == ButtonRelease)
				return evt.xbutton.button;
		};
	
		bool has_key_event(bool ignore_other = 1) {
			while (check_event()) {
				if (evt.type == KeyPress || evt.type == KeyRelease) {
					get_event();
					return 1;
				} else if (!ignore_other)
					return 0;
				get_event();
			}
			return 0;
		};
		
		// Returns number of pressed key
		int get_key_down() {
			if (evt.type == KeyPress)
				return evt.xkey.keycode;
		};
		
		// Returns number of released key
		int get_key_up() {
			if (evt.type == KeyRelease)
				return evt.xkey.keycode;
		};
	
		// Result of get_pointer
		struct pointer {
			// pointer relative location
			int x, y,
				// window location
				win_x, win_y;
			
			pointer(int x, int y, int win_x, int win_y) {
				this->x = x;
				this->y = y;
				this->win_x = win_x;
				this->win_y = win_y;
			};
		};
		
		pointer get_pointer() {
			int win_x, win_y, root_x, root_y = 0;
			unsigned int mask = 0;
			Window child_win, root_win;
			XQueryPointer(paint.dsp, paint.win,
							&child_win, &root_win,
							&root_x, &root_y, &win_x, &win_y, &mask);
			return pointer(win_x, win_y, root_x, root_y);
		};
	};
};
