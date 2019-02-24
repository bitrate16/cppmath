#pragma once

#include <X11/Xlib.h>
#include <stdlib.h>

// Require $(pkg-config --cflags --libs x11) linking

// Simple paint
namespace spaint {
	
	// Scene wrapper
	class window;
	class component {
		friend window;
		
		window* win = nullptr;
	
	public:
	
		inline window& get_window() {
			return *win;
		};
	
		virtual void start() {};
		virtual void exit() {};
		
		virtual void create() {};
		virtual void destroy() {};
		
		virtual void loop() {};
	};
	
	class spaint {
		friend class window;
		
		Display *dsp;
		Window   win;
		GC        gc;
		
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
		
		
		inline void line_style(int line_width, int line_style = LineSolid, int cap_style = CapButt, int join_style = JoinBevel) {
			XSetLineAttributes(dsp, gc, line_width, line_style, cap_style, join_style);
		};
		
		inline void fill_style(int style = FillSolid) {
			XSetFillStyle(dsp, gc, style);
		};
	};
	
	struct event {
		XEvent evt;
		
	};
	
	class window {		
		event evt;
		
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
		
		void pump_event() {
			while (XPending(paint.dsp)) {
				
			}
		};
		
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
			
			XGCValues values;
			paint.gc = XCreateGC(paint.dsp, paint.win, valuemask, &values);
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
			eventMask |= ButtonPressMask | ButtonReleaseMask | MotionNotify // Mouse
			           | MotionNotify    | KeyPressMask      | KeyReleaseMask;
			XSelectInput(paint.dsp, paint.win, eventMask);
			
			
			// Show window
			XMapWindow(paint.dsp, paint.win);
			XFlush(paint.dsp);
			
			comp->create();
		};
		
		inline spaint& get_paint() {
			return paint;
		};
				
		inline bool check_event() {
			if (has_event)
				return 1;
			
			if (XPending(paint.dsp)) {
				
			}
		};
		
		inline event get_event() {
			return 
		};
		
		void start() {
			comp->start();
			
			while (state) {
				pump_event();
				
				
				//XCheckWindowEvent();
				comp->loop();
			}
			
			comp->exit();
			comp->destroy();
		};
		
		void exit() {
			state = 0;
		};
	};
};