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

#include "spaint.h"
#include "RayTrace.h"

using namespace spaint;
using namespace cppmath;
using namespace raytrace;

#define KEY_ESCAPE 9
#define KEY_R      27

#define WIDTH 250
#define HEIGHT 250
#define SCALE 1

// bash c.sh "-lX11" example/raytrace


class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("RayTrace example");
		
		updated = 1;
		rt.camera = Camera(WIDTH, HEIGHT);
		rt.set_background(Color::BLACK);
		
		// Surrounding
		Plane* red_plane = new Plane(vec3(0, -50, 0) * SCALE, vec3(0, 1, 0));
		red_plane->material.color = Color::WHITE;
		red_plane->material.luminosity = 0.0;
		red_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(red_plane);
		
		Plane* green_plane = new Plane(vec3(-50, 0, 0) * SCALE, vec3(1, 0, 0));
		green_plane->material.color = Color::BLUE;
		green_plane->material.luminosity = 0.0;
		green_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(green_plane);
		
		Plane* blue_plane = new Plane(vec3(50, 0, 0) * SCALE, vec3(-1, 0, 0));
		blue_plane->material.color = Color::RED;
		blue_plane->material.luminosity = 0.0;
		blue_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(blue_plane);
		
		Plane* cyan_plane = new Plane(vec3(0, 0, 150) * SCALE, vec3(0, 0, -1));
		cyan_plane->material.color = Color::WHITE;
		cyan_plane->material.luminosity = 0.0;
		cyan_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(cyan_plane);
		
		Plane* yellow_plane = new Plane(vec3(0, 50, 0) * SCALE, vec3(0, -1, 0));
		yellow_plane->material.color = Color::WHITE;
		yellow_plane->material.luminosity = 0.0;
		yellow_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(yellow_plane);
		
		Plane* magenta_plane = new Plane(vec3(0, 0, -50) * SCALE, vec3(0, 0, 1));
		magenta_plane->material.color = Color::WHITE;
		magenta_plane->material.luminosity = 0.0;
		magenta_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(magenta_plane);
		
		// Spheres
		Sphere* red_sphere = new Sphere(vec3(20, 20, 120) * SCALE, 5 * SCALE);
		red_sphere->material.color = Color::RED;
		red_sphere->material.luminosity = 0.0;
		rt.get_scene().addObject(red_sphere);
		
		Sphere* green_sphere = new Sphere(vec3(15, -15, 100) * SCALE, 5 * SCALE);
		green_sphere->material.color = Color::GREEN;
		green_sphere->material.luminosity = 0.0;
		rt.get_scene().addObject(green_sphere);
		
		Sphere* light_sphere = new Sphere(vec3(0, 20, 80) * SCALE, 5 * SCALE);
		light_sphere->material.color = Color::WHITE;
		light_sphere->material.luminosity = 1.0;
		rt.get_scene().addObject(light_sphere);
		
		Sphere* white_sphere = new Sphere(vec3(10, 0, 100) * SCALE, 10 * SCALE);
		white_sphere->material.color = Color::WHITE;
		white_sphere->material.reflect = 1.0;
		rt.get_scene().addObject(white_sphere);
		
		Sphere* glass_sphere = new Sphere(vec3(-5, -5, 50) * SCALE, 10 * SCALE);
		glass_sphere->material.color = Color::WHITE;
		glass_sphere->material.refract = 0.9;
		glass_sphere->material.refract_val = 0.5;
		rt.get_scene().addObject(glass_sphere);
		
		/*Triangle* red_triangle = new Triangle(vec3(60, -14, 60), vec3(60, -14, 50), vec3(50, -14, 50));
		red_triangle->material.color = Color::RED;
		red_triangle->material.luminosity = 1.0;
		rt.get_scene().addObject(red_triangle);*/
		
		/*Plane* white_plane = new Plane(vec3(0, -15, 0), vec3(0, 1, 0));
		white_plane->material.color = Color::WHITE;
		white_plane->material.luminosity = 0.0;
		white_plane->material.diffuse = 0.1;
		white_plane->material.reflect = 0.9;
		rt.get_scene().addObject(white_plane);*/
	};
	
	void destroy() {
		
	};
	
	bool mouse_down = 0;
	bool resized = 0;
	bool updated = 0;
	
	RayTrace rt;
	
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
			
		if (resized || updated) {
			
			resized = 0;
			updated = 0;
			
			
			for (int x = 0; x < rt.get_width(); ++x)
				for (int y = 0; y < rt.get_height(); ++y) {
					Color frag = rt.hitColorAt(x, y);
					if (frag != Color::BLACK) {
						p.color(frag);
						p.point(x, y);
					}
				}
				std::cout << "DONE\n";
				
				
		} else if (mouse_down) {
			window::pointer point = w.get_pointer();
			int x = point.x ;
			int y = point.y ;
			Color frag = rt.hitColorAt(x, y);
			if (frag != Color::BLACK) {
				p.color(frag);
				p.point(x, y);
			}
		}
	};
};


int main() {
	scene s;
	window w(&s, WIDTH, HEIGHT, 0);
	w.start();
	return 0;
};
