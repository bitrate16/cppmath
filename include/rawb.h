#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <fstream>
#include <stdexcept>

// Store images in RAWb format
// RAWb := HEADER + BODY
// HEADER := WIDTH[4B] + HEIGHT[4B] + PIXEL_TYPE[1B]
// BODY := [pixel[4B]][WIDTH*HEIGHT]
class rawb {
	
public:
	
	// Type of pixels order sequence
	enum pixel_type {
		RGBA, ARGB, BGRA, ABGR
	};
	
	union pixel {
		char     rgba[4];
		uint32_t       p;
	};
	
private:
	
	uint32_t width, height;
	pixel_type p_type;
	
	// Used when input file byte order does not match current machine byte order.
	void reverseInt32Order(uint32_t& i) {
		int reversed;

		char *n1, *n2;
		n1 = (char *)&i;
		n2 = (char *)&reversed;

		*(n2 + 0) =  *(n1 + 3);
		*(n2 + 1) =  *(n1 + 2);
		*(n2 + 2) =  *(n1 + 1);
		*(n2 + 3) =  *(n1 + 0);
		
		i = reversed;
	};
	
public:

	pixel* buffer = nullptr;
	
	rawb(uint64_t width, uint64_t height, pixel_type type = pixel_type::RGBA) : width(width), height(height), p_type(type) {
		buffer = (pixel*) calloc((size_t) width * (size_t) height, sizeof(pixel));
		
		if (!buffer) 
			throw std::runtime_error("Unable to allocate buffer");
	};
	
	rawb(const std::string& filename) {
		std::ifstream file(filename.c_str(), std::ios::binary);
		if (file.fail())
			throw std::runtime_error("File open failed");
		
		// Checking for endian to match endian of this machine 
		uint32_t endian_test;
		file.read((char*) &endian_test, 4);
		
		union {
			uint32_t i;
			char c[4];
		} test = { 0x01020304 };

		bool endian_match = (test.c[0] == (endian_test & 0xFF)); 
		
		file.read((char*) &width, 4);
		file.read((char*) &height, 4);
		
		if (!endian_match) {
			reverseInt32Order(width);
			reverseInt32Order(height);
		}
		
		uint8_t pix_type;
		file.read((char*) &pix_type, 1);
		p_type = (pixel_type) pix_type;
		
		// Check if pixel format is valid
		switch (p_type) {
			case pixel_type::RGBA:
			case pixel_type::ARGB:
			case pixel_type::BGRA:
			case pixel_type::ABGR:
				break;
				
			default:
				throw std::runtime_error("Pixel format error");
		};
		
		if (file.fail()){
			file.close();
			throw std::runtime_error("File format error");
		}
		
		buffer = (pixel*) malloc((size_t) width * (size_t) height * sizeof(pixel));
		if (!buffer) {
			file.close();
			throw std::runtime_error("Unable to allocate buffer");
		}
		
		// Read file to da buffer
		file.read((char*) buffer, (size_t) width * (size_t) height * sizeof(pixel));
		
		if (!endian_match) {	
			// Iterate over buffer to revert uint32_t byte order
			uint64_t isize = (size_t) width * (size_t) height;
			uint32_t* ibuffer = (uint32_t*) buffer;
			
			for (uint64_t i = 0; i < isize; ++i) 
				reverseInt32Order(ibuffer[i]);
		}
		
		if (file.fail()){
			file.close();
			throw std::runtime_error("File format error");
		}
		
		file.close();
	};
	
	~rawb() {
		if (buffer)
			free(buffer);
	};
	
	// Unsafe way to get pixel at location (x, y).
	// Does not check bounds and negative indices.
	pixel& get(uint32_t x, uint32_t y) {
		return buffer[x + width * y];
	};
	
	uint32_t get_width() {
		return width;
	};
	
	uint32_t get_height() {
		return height;
	};
	
	pixel_type get_pixel_type() {
		return p_type;
	};
	
	void write(const std::string& filename) {
		std::ofstream file;
		if (file.fail())
			throw std::runtime_error("File open failed");
		
		file.write((char*) &width, 4);
		file.write((char*) &width, 4);
		
		uint8_t pix_type = p_type;
		file.write((char*) &pix_type, 1);
		
		file.write((char*) buffer, (size_t) width * (size_t) height * sizeof(pixel));
		
		file.close();
	};
	
	// Chenge pixel storage type from existing to newly passed
	//  Current version is a bit slow
	void convert_pixel_type(pixel_type new_type) {
		if (p_type == new_type)
			return;
		
		uint64_t length = (uint64_t) width * (uint64_t) height * 4;
		
		if (p_type == RGBA) {
			if (new_type == BGRA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[0];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[2];
					a = buffer[i].rgba[3];
					buffer[i].rgba[2] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[0] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == ARGB) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[0];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[2];
					a = buffer[i].rgba[3];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[0] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[2] = a;
				}
			} else if (new_type == ABGR) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[0];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[2];
					a = buffer[i].rgba[3];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[2] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[0] = a;
				}
			}
		} else if (new_type == BGRA) {
			if (new_type == RGBA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[2];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[0];
					a = buffer[i].rgba[3];
					buffer[i].rgba[0] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[2] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == ARGB) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[2];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[0];
					a = buffer[i].rgba[3];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[0] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[2] = a;
				}
			} else if (new_type == ABGR) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[2];
					g = buffer[i].rgba[1];
					b = buffer[i].rgba[0];
					a = buffer[i].rgba[3];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[2] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[0] = a;
				}
			}
		} else if (new_type == ARGB) {
			if (new_type == RGBA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[0];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[2];
					buffer[i].rgba[0] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[2] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == BGRA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[0];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[2];
					buffer[i].rgba[2] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[0] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == ABGR) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[0];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[2];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[2] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[0] = a;
				}
			}
		} else if (new_type == ABGR) {
			if (new_type == RGBA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[2];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[0];
					buffer[i].rgba[0] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[2] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == BGRA) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[2];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[0];
					buffer[i].rgba[2] = r;
					buffer[i].rgba[1] = g;
					buffer[i].rgba[0] = b;
					buffer[i].rgba[3] = a;
				}
			} else if (new_type == ARGB) {
				for (uint64_t i = 0; i < length; ++i) {
					unsigned char r, g, b, a;
					r = buffer[i].rgba[3];
					g = buffer[i].rgba[2];
					b = buffer[i].rgba[1];
					a = buffer[i].rgba[0];
					buffer[i].rgba[3] = r;
					buffer[i].rgba[0] = g;
					buffer[i].rgba[1] = b;
					buffer[i].rgba[2] = a;
				}
			}
		}
	};
};