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

#define WIDTH 100
#define HEIGHT 100
#define SCALE 0.4
#define PIXEL_SCALE 3

// bash c.sh "-lX11" example/raytrace


class scene : public component {
	
	void create() {
		get_paint().init_font();
		get_window().set_title("RayTrace example");
		
		updated = 1;
		rt.camera = Camera(WIDTH, HEIGHT);
		rt.set_background(Color::BLACK);
		//rt.get_scene().soft_shadows = 1;
		//rt.get_scene().diffuse_light = 1;
		rt.get_scene().use_shadows = 1;
		rt.get_scene().average_light_points = 1;
		rt.get_scene().soft_shadows_scale = 1.0;
		rt.get_scene().random_diffuse_ray = 1;
		rt.get_scene().random_diffuse_count = 8;
		rt.get_scene().MAX_RAY_DEPTH = 4;
		
		// Triangles
		Triangle* red_triangle = new Triangle(vec3(-10, 30, 149) * SCALE, vec3(-15, 25, 149) * SCALE, vec3(-15, 35, 149) * SCALE);
		red_triangle->material.color = Color::RED;
		red_triangle->material.diffuse = 1.0;
		rt.get_scene().addObject(red_triangle);
		
		// Surrounding
		Plane* red_plane = new Plane(vec3(0, -50, 0) * SCALE, vec3(0, 1, 0));
		red_plane->material.color = Color::WHITE;
		red_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(red_plane);
		
		Plane* green_plane = new Plane(vec3(-50, 0, 0) * SCALE, vec3(1, 0, 0));
		green_plane->material.color = Color::BLUE;
		green_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(green_plane);
		
		Plane* blue_plane = new Plane(vec3(50, 0, 0) * SCALE, vec3(-1, 0, 0));
		blue_plane->material.color = Color::RED;
		blue_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(blue_plane);
		
		Plane* cyan_plane = new Plane(vec3(0, 0, 150) * SCALE, vec3(0, 0, -1));
		cyan_plane->material.color = Color::WHITE;
		cyan_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(cyan_plane);
		
		Plane* yellow_plane = new Plane(vec3(0, 50, 0) * SCALE, vec3(0, -1, 0));
		yellow_plane->material.color = Color::WHITE;
		yellow_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(yellow_plane);
		
		Plane* magenta_plane = new Plane(vec3(0, 0, -50) * SCALE, vec3(0, 0, 1));
		magenta_plane->material.color = Color::WHITE;
		magenta_plane->material.diffuse = 1.0;
		rt.get_scene().addObject(magenta_plane);
		
		/*
		// Light
		Sphere* light_sphere = new Sphere(vec3(0, 0, 100) * SCALE, 5 * SCALE);
		light_sphere->material.color = Color::WHITE;
		light_sphere->material.luminosity = 1.0;
		light_sphere->material.surface_visible = 0;
		rt.get_scene().addObject(light_sphere);
		
		Sphere* red_sphere = new Sphere(vec3(0, 20, 100) * SCALE, 5 * SCALE);
		red_sphere->material.color = Color::RED;
		rt.get_scene().addObject(red_sphere);
		
		Sphere* green_sphere = new Sphere(vec3(0, 40, 100) * SCALE, 5 * SCALE);
		green_sphere->material.color = Color::GREEN;
		rt.get_scene().addObject(green_sphere);
		*/
	
		
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
		light_sphere->material.luminosity = 2.0;
		light_sphere->material.surface_visible = 1;
		light_sphere->material.luminosity_scaling = 1;
		light_sphere->setLightSectorsCount(8);
		rt.get_scene().addObject(light_sphere);
		
		light_sphere = new Sphere(vec3(-10, 20, 80) * SCALE, 5 * SCALE);
		light_sphere->material.color = Color::RED;
		light_sphere->material.luminosity = 0.1;
		light_sphere->material.surface_visible = 0;
		rt.get_scene().addObject(light_sphere);
		
		light_sphere = new Sphere(vec3(20, -20, 100) * SCALE, 10 * SCALE);
		light_sphere->material.color = Color::BLUE;
		light_sphere->material.luminosity = 0.2;
		light_sphere->material.surface_visible = 0;
		rt.get_scene().addObject(light_sphere);
		
		
		Sphere* white_sphere = new Sphere(vec3(10, 0, 100) * SCALE, 10 * SCALE);
		white_sphere->material.color = Color::WHITE;
		white_sphere->material.reflect = 0.9;
		white_sphere->material.diffuse = 0.1;
		rt.get_scene().addObject(white_sphere);
		
		Sphere* glass_sphere = new Sphere(vec3(-5, -5, 50) * SCALE, 10 * SCALE);
		glass_sphere->material.color = Color::WHITE;
		glass_sphere->material.refract = 0.9;
		glass_sphere->material.refract_val = 3.3;
		glass_sphere->material.reflect = 0.1;
		glass_sphere->material.diffuse = 0.1;
		rt.get_scene().addObject(glass_sphere);
		
		
		/*Triangle* red_triangle = new Triangle(vec3(20, 10, 90), vec3(10, 20, 90), vec3(30, 20, 90));
		red_triangle->material.color = Color::RED;
		red_triangle->material.luminosity = 1.0;
		rt.get_scene().addObject(red_triangle);*/
		
		/*Plane* white_plane = new Plane(vec3(0, -15, 0), vec3(0, 1, 0));
		white_plane->material.color = Color::WHITE;
		white_plane->material.luminosity = 0.0;
		white_plane->material.diffuse = 0.1;
		white_plane->material.reflect = 0.9;
		rt.get_scene().addObject(white_plane);*/
		
		/*UVPlane* plane = new UVPlane(vec3(0, -50, 100) * SCALE, vec3(0, 1, 0));
		plane->material.color = Color::WHITE;
		plane->material.luminosity = 0.0;
		plane->material.diffuse = 1.0;
		plane->uv_map = [](double u, double v) -> Color {
			int iu = u * 10;
			int iv = v * 10;
			if (iu < 0)
				iu = 256 - ((-iu) % 256);
			else
				iu = iu % 256;
			if (iv < 0)
				iv = 256 - ((-iv) % 256);
			else
				iv = iv % 256;
			
			return Color(abs(iu), 0, abs(iv));
		};
		rt.get_scene().addObject(plane);
		
		Sphere* light_sphere = new Sphere(vec3(0, 50, 100) * SCALE, 5 * SCALE);
		light_sphere->material.color = Color::WHITE;
		light_sphere->material.luminosity = 1.0;
		rt.get_scene().addObject(light_sphere);*/
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
			
			std::cout << "RENDERING\n";
			
			for (int x = 0; x < rt.get_width(); ++x)
				for (int y = 0; y < rt.get_height(); ++y) {
					Color frag = rt.hitColorAt(x, y);
					if (frag != Color::BLACK) {
						p.color(frag);
						for (int sx = 0; sx < PIXEL_SCALE; ++sx)
							for (int sy = 0; sy < PIXEL_SCALE; ++sy)
								p.point(x * PIXEL_SCALE + sx, y * PIXEL_SCALE + sy);
					}
				}
				
			std::cout << "DONE\n";
			
			rt.camera.location.z += 1.0;
				
		} else if (mouse_down) {
			window::pointer point = w.get_pointer();
			int x = point.x / PIXEL_SCALE;
			int y = point.y / PIXEL_SCALE;
			
			/*Color frag = rt.hitColorAt(x, y);
			if (frag != Color::BLACK) {
				p.color(frag);				
				for (int sx = 0; sx < PIXEL_SCALE; ++sx)
					for (int sy = 0; sy < PIXEL_SCALE; ++sy)
						p.point(x * PIXEL_SCALE + sx, y * PIXEL_SCALE + sy);
			}*/
			
			rt.camera.width = WIDTH * PIXEL_SCALE;
			rt.camera.height = HEIGHT * PIXEL_SCALE;
			
			std::cout << "RENDERING HIGH\n";
			
			for (int x = 0; x < rt.get_width(); ++x)
				for (int y = 0; y < rt.get_height(); ++y) {
					Color frag = rt.hitColorAt(x, y);
					if (frag != Color::BLACK) {
						p.color(frag);
						p.point(x, y);
					}
				}
				
				std::cout << "DONE\n";
		}
	};
};


int main() {
	scene s;
	window w(&s, WIDTH * PIXEL_SCALE, HEIGHT * PIXEL_SCALE, 0);
	w.start();
	return 0;
};

