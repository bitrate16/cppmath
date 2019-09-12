
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
#define WIDTH 250
#define HEIGHT 250
#define SCALE 1

using namespace spaint;
using namespace cppmath;
using namespace raytrace;
using namespace std;

// This example renders N steps of simple ray tracing 
//  scene and outputs to images in "output" directory

// bash c.sh "-lX11" example/raytrace_images
// ffmpeg -r 60 -f image2 -s 250x250 -start_number 0 -i frame_%01d.png -vframes 1000 -vcodec libx264 -crf 0  -pix_fmt yuv420p test.mp4

RayTrace rt;

void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
	/*Encode the image*/
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	/*if there's an error, display it*/
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

int main() {
	// 
	rt.camera = Camera(WIDTH, HEIGHT);
	rt.set_background(Color::BLACK);
	
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
	rt.get_scene().addObject(light_sphere);
	
	Sphere* white_sphere = new Sphere(vec3(0, 0, 100) * SCALE, 10 * SCALE);
	white_sphere->material.color = Color::WHITE;
	white_sphere->material.reflect = 1.0;
	rt.get_scene().addObject(white_sphere);
	
	// Allocate space for frames
	unsigned width = WIDTH, height = HEIGHT;
	unsigned int* frame = (unsigned int*) malloc(width * height * 4);
	
	double step = 3.14159265358979323846 / 100.0;
	double angle = 0;
	
	for (int i = 0; i < 200; ++i) {
		std::cout << "Frame: " << i << std::endl;
		
		white_sphere->center.z = (100 + 30 * std::sin(angle)) * SCALE;
		white_sphere->center.x = (10 * std::cos(angle)) * SCALE;
		
		for (int x = 0; x < WIDTH; ++x)
			for (int y = 0; y < HEIGHT; ++y) {
				Color hit_color = rt.hitColorAt(x, y);
				hit_color.a = 255;
				frame[x + y * WIDTH] = hit_color.abgr();
			}
		
		string outname = OUTPUT_FOLDER;
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