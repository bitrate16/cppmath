
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "RayTrace.h"
#include "Color.h"
#include "lodepng.h"

#define OUTPUT_FOLDER "output"
#define WIDTH 1000
#define HEIGHT 1000
#define SCALE 4

using namespace spaint;
using namespace cppmath;
using namespace raytrace;

// This example renders N steps of simple ray tracing 
//  scene and outputs to images in "output" directory

// bash c.sh "-lX11" example/raytrace_images
// ffmpeg -r 60 -f image2 -s 250x250 -start_number 0 -i frame_%01d.png -vframes 1000 -vcodec libx264 -crf 0  -pix_fmt yuv420p test.mp4

RayTrace rt;

double angle = 0;

void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
	/*Encode the image*/
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	/*if there's an error, display it*/
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

int main() {
	rt.camera = Camera(WIDTH, HEIGHT);
	rt.set_background(Color::BLACK);
	rt.get_scene().soft_shadows = 1;
	rt.get_scene().soft_shadows_scale = 0.5;
		
	/* Scene 1
	Sphere* red_sphere = new Sphere(vec3(20, 20, 90) * SCALE, 5 * SCALE);
	red_sphere->material.color = Color::RED;
	red_sphere->material.luminosity = 0.0;
	rt.get_scene().addObject(red_sphere);
	
	Sphere* green_sphere = new Sphere(vec3(20, -20, 100) * SCALE, 5 * SCALE);
	green_sphere->material.color = Color::GREEN;
	green_sphere->material.luminosity = 0.0;
	rt.get_scene().addObject(green_sphere);
	
	Sphere* light_sphere = new Sphere(vec3(0, 20, 80) * SCALE, 5 * SCALE);
	light_sphere->material.color = Color::WHITE;
	light_sphere->material.luminosity = 1.0;
	light_sphere->material.surface_visible = 0;
	rt.get_scene().addObject(light_sphere);
	
	Sphere* white_sphere = new Sphere(vec3(0, 0, 100) * SCALE, 10 * SCALE);
	white_sphere->material.color = Color::WHITE;
	white_sphere->material.reflect = 1.0;
	rt.get_scene().addObject(white_sphere);
	*/
		
	/* Scene 2
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
	
	// Scene 3
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
	light_sphere->material.surface_visible = 0;
	rt.get_scene().addObject(light_sphere);
	
	Sphere* white_sphere = new Sphere(vec3(10, 0, 100) * SCALE, 10 * SCALE);
	white_sphere->material.color = Color::WHITE;
	white_sphere->material.reflect = 1.0;
	rt.get_scene().addObject(white_sphere);
	
	Sphere* glass_sphere = new Sphere(vec3(-5, -5, 50) * SCALE, 10 * SCALE);
	glass_sphere->material.color = Color::WHITE;
	glass_sphere->material.refract = 0.9;
	glass_sphere->material.refract_val = -0.5;
	glass_sphere->material.reflect = 0.1;
	rt.get_scene().addObject(glass_sphere);

	UVSphere* uv_sphere = new UVSphere(vec3(-10, 30, 120) * SCALE, 10 * SCALE);
	uv_sphere->material.color = Color::WHITE;
	uv_sphere->material.luminosity = 0.0;
	uv_sphere->material.diffuse = 1.0;
	uv_sphere->uv_map = [](double u, double v) -> Color {
		u += angle;
		
		double su = sin(u * 2);
		double sv = sin(v * 2);
		
		if (su < 0 && sv > 0 || su > 0 && sv < 0)
			return Color(255, 0, 255);
		return Color();
		
		/*
		if (!(iu % 2 == 0) && (iv % 2 == 0) || (iu % 2 == 0) && !(iv % 2 == 0))
			return Color(255, 0, 255);
		return Color();
		*/
	};
	rt.get_scene().addObject(uv_sphere);

	
	// Allocate space for frames
	unsigned width = WIDTH, height = HEIGHT;
	unsigned int* frame = (unsigned int*) malloc(width * height * 4);
	
	double step = 3.14159265358979323846 / 100.0;
	
	for (int i = 0; i < 200; ++i) {
		std::cout << "Frame: " << i << std::endl;
		
		/* Scene 1
		white_sphere->center.z = (100 + 30 * std::sin(angle)) * SCALE;
		white_sphere->center.x = (10 * std::cos(angle)) * SCALE;
		*/
		
		/* Scene 2
		red_sphere->center.x = (10 * std::cos(angle)) * SCALE;
		*/
		
		uv_sphere->center.y = (20 + 20 * std::sin(angle)) * SCALE;
		
		for (int x = 0; x < rt.get_width(); ++x)
			for (int y = 0; y < rt.get_height(); ++y) {
				Color hit_color = rt.hitColorAt(x, y);
				hit_color.a = 255;
				frame[x + y * WIDTH] = hit_color.abgr();
			}
		
		std::string outname = OUTPUT_FOLDER;
		outname += "/frame_" + std::to_string(i);
		outname += ".png";
		
		struct stat st = {0};
		if (stat(OUTPUT_FOLDER, &st) == -1) 
			mkdir(OUTPUT_FOLDER, 0700);
		
		encodeOneStep(outname.c_str(), (unsigned char*)frame, width, height);
		angle += step;
	}
	
	free(frame);
};