
#include <iostream>
#include <string>
#include <cstdio>

#include "rawb.h"
#include "lodepng.h"

// Simple commang-line tool to convert rawb files to png files using lodepng
// Usage: rawb2png input_file.rawb output_file.png

// g++ -Iinclude -O3 src/lodepng.cpp example/rawb2png.cpp -o bin/rawb2png
	
bool encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	if(error) {
		fprintf(stderr, "lodepng error %u: %s\n", error, lodepng_error_text(error));
		return 0;
	}
	
	return 1;
};

int main(int argc, char** argv) {
	
	if (argc < 2) {
		std::cout << "Usage: rawb2png infile.rawb [outfile.png] [-silent/-s]" << std::endl;
		return 0;
	}
	
	bool silent = 0;
	std::string in_file;
	std::string out_file;
	
	if (argc == 2) {
		in_file = argv[1];
		out_file = argv[1] + std::string(".png");
	} else {
		in_file = argv[1];
		out_file = argv[2];
		
		if (argc >= 2 && strcmp(argv[2], "-silent") == 0)
			silent = 1;
		if (argc >= 2 && strcmp(argv[2], "-s") == 0)
			silent = 1;
	}
	
	try {
		rawb r(in_file);
		
		if (!silent) {
			std::cout << "Pixel type: ";
			switch (r.get_pixel_type()) {
				case rawb::pixel_type::RGBA: std::cout << "RGBA" << std::endl; break;
				case rawb::pixel_type::ARGB: std::cout << "ARGB" << std::endl; break;
				case rawb::pixel_type::BGRA: std::cout << "BGRA" << std::endl; break;
				case rawb::pixel_type::ABGR: std::cout << "ABGR" << std::endl; break;
			}
			
			std::cout << "Width: " << r.get_width() << std::endl;
			std::cout << "Height: " << r.get_height() << std::endl;
		}
		
		r.convert_pixel_type(rawb::pixel_type::ABGR);
		
		if (!encodeOneStep(out_file.c_str(), (unsigned char*) r.buffer, r.get_width(), r.get_height()))
			return 1;
		
	} catch (const std::exception & ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	
	return 0;
};