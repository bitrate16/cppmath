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
#include "cppmath.h"
#include "ivec3.h"
#include "vec2.h"
#include "ZBuffer.h"

using namespace spaint;
using namespace cppmath;

#define KEY_ESCAPE 9
#define KEY_R      27

// 3D cube visualisation using spaint tool.

// bash c.sh "-lX11" example/cube


class scene : public component {
	
	void create() {
		get_window().set_title("Cube example");
		
		updated = 1;
		buffer  = get_paint().createImageBuffer(250, 250);
	};
	
	void destroy() {
		get_paint().destroyImageBuffer(buffer);
	};
	
	bool mouse_down = 0;
	bool resized = 0;
	bool updated = 0;
	
	ImageBuffer buffer;
	ZBuffer zbuf = ZBuffer(400, 400);
	
	vec3 cube_verts[8]    = { {1, 1, 1}, {1, 1, -1}, {1, -1, 1}, {1, -1, -1}, {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1} };
	int cube_faces[12][3] = { {1, 5, 3}, {3, 5, 7}, {6, 5, 7}, {6, 5, 8}, {5, 1, 6}, {1, 6, 2}, {6, 2, 8}, {8, 4, 2}, {8, 4, 7}, {4, 7, 3}, {2, 1, 4}, {1, 4, 3} };
	Color faces_color[6] = { Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW, Color::CYAN, Color::MAGENTA };
	int cube_edges[12][2] = { {1, 5}, {5, 6}, {6, 2}, {2, 1}, {1, 3}, {5, 7}, {6, 8}, {2, 4}, {7, 3}, {3, 4}, {4, 8}, {8, 7} };
	
	vec3 cube_center = vec3(125, 125, 200);
	double cube_size = 25;
	
	double angle = 0.0;
	
	void resize() {
		resized = 1;
	};
	
	void draw_cube(vec3 translation, double angle1, double angle2, double angle3) {
		translation *= 2.0;
		for (int i = 0; i < 12; ++i) {
			vec3 A = cube_verts[cube_edges[i][0] - 1] + translation;
			vec3 B = cube_verts[cube_edges[i][1] - 1] + translation;
			A = math::rotateAroundVector(A, vec3::X, angle1);
			B = math::rotateAroundVector(B, vec3::X, angle1);
			A = math::rotateAroundVector(A, vec3::Y, angle2);
			B = math::rotateAroundVector(B, vec3::Y, angle2);
			A = math::rotateAroundVector(A, vec3::Z, angle3);
			B = math::rotateAroundVector(B, vec3::Z, angle3);
			zbuf.line(A * cube_size + cube_center, B * cube_size + cube_center, Color::WHITE);
		}
	}
	
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
			
		if (resized || updated || mouse_down || 1) {
			
			resized = 0;
			updated = 0;
			
			//p.clear();
			
			angle += 0.01;
			
			zbuf.clear();
			
			/*for (int i = 0; i < 12; ++i) {
				vec3 A = cube_verts[cube_faces[i][0] - 1];
				vec3 B = cube_verts[cube_faces[i][1] - 1];
				vec3 C = cube_verts[cube_faces[i][2] - 1];
				A = math::rotateAroundVector(A, vec3::X, angle * 1.0);
				B = math::rotateAroundVector(B, vec3::X, angle * 1.0);
				C = math::rotateAroundVector(C, vec3::X, angle * 1.0);
				A = math::rotateAroundVector(A, vec3::Y, angle * 2.0);
				B = math::rotateAroundVector(B, vec3::Y, angle * 2.0);
				C = math::rotateAroundVector(C, vec3::Y, angle * 2.0);
				A = math::rotateAroundVector(A, vec3::Z, angle * 3.0);
				B = math::rotateAroundVector(B, vec3::Z, angle * 3.0);
				C = math::rotateAroundVector(C, vec3::Z, angle * 3.0);
				Color& c = faces_color[i >> 1];
				zbuf.triangle(A * cube_size + cube_center, B * cube_size + cube_center, C * cube_size + cube_center, c);
			}*/
			
			draw_cube(vec3(0, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(1, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(0, 1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(1, 1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(0, 0, 1), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(1, 0, 1), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(0, 1, 1), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(1, 1, 1), angle, angle * 2.0, angle * 3.0);
			
			/*draw_cube(vec3(1, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(2, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(2, 1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(2, 2, 0), angle, angle * 2.0, angle * 3.0);
			
			draw_cube(vec3(-1, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(-2, 0, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(-2, -1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(-2, -2, 0), angle, angle * 2.0, angle * 3.0);
			
			draw_cube(vec3(0, 1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(0, 2, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(-1, 2, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(-2, 2, 0), angle, angle * 2.0, angle * 3.0);
			
			draw_cube(vec3(0, -1, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(0, -2, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(1, -2, 0), angle, angle * 2.0, angle * 3.0);
			draw_cube(vec3(2, -2, 0), angle, angle * 2.0, angle * 3.0);*/
			
			buffer.set_color(Color::BLACK);
			buffer.fill();
			buffer.set_color(Color::WHITE);
			
			int scale = 1;
			int r = -1, g = -1, b = -1;
			
			for (int x = 0; x < zbuf.get_width(); ++x)
				for (int y = 0; y < zbuf.get_height(); ++y)
					if (zbuf.get(x, y).z >= 0) {
						/*int cr = zbuf.get(x, y).r;
						int cg = zbuf.get(x, y).g;
						int cb = zbuf.get(x, y).b;
						
						if (r == -1 || r != cr || g != cg || b != cb) {
							buffer.set_color(Color(cr, cg, cb));
							r = cr;
							g = cg;
							b = cb;
						}*/
						
						for (int i = 0; i < scale; ++i)
							for (int j = 0; j < scale; ++j)
								buffer.set_pixel(x * scale + i, y * scale + j);
					}
							
			buffer.put(0, 0);
			
			if (mouse_down) {
				window::pointer point = w.get_pointer();
				int x = point.x;
				int y = point.y;
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

