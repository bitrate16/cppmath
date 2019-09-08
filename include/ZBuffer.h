#pragma once

#include <cmath>

#include "vec3.h"
#include "vec2.h"
#include "ivec2.h"
#include "Color.h"

namespace spaint {
	// ZBuffer technics allows rendering 3D graphics using orthogonal ray length casting.
	class ZBuffer {
	
	public:
	
		// instance of contained point
		struct zmatch {
			// Distance to this matched point or -1 of point does not exist.
			double z = -1;
			// RGB colors
			char r, g, b;
		};
		
	private:
	
		// Sign function
		template <typename T> int sgn(T val) {
			return (T(0) < val) - (val < T(0));
		}

		// Map of pixels on the screen
		std::vector<std::vector<zmatch>> map;
		
		// Resolution of map
		int width;
		int height;
		
	public:	
	
		ZBuffer(int _width, int _height) {
			width = _width < 0 ? -_width : _width;
			height = _height < 0 ? -_height : _height;
			
			map.resize(width);
			for (int i = 0; i < width; ++i)
				map[i].resize(height);
		};
		
		inline zmatch& get(int x, int y) {
			return map[x][y];
		};
		
		inline int get_width() {
			return width;
		};
		
		inline int get_height() {
			return height;
		};
		
		inline void clear() {
			for (int x = 0; x < get_width(); ++x)
				for (int y = 0; y < get_height(); ++y) {
					map[x][y].z = -1;
					map[x][y].r = 0;
					map[x][y].g = 0;
					map[x][y].b = 0;
				}
		};
		
		void resize(int _width, int _height) { 
			width = _width < 0 ? -_width : _width;
			height = _height < 0 ? -_height : _height;
			
			map.resize(width);
			for (int i = 0; i < width; ++i)
				map[i].resize(height);
		};
		
		void point(const cppmath::vec3& v, const Color& c) {
			if (v.x < 0 || v.y < 0 || v.x > get_width() || v.y > get_height())
				return;
			
			if (v.z < 0)
				return;
			
			int x = v.x;
			int y = v.y;
			
			if (v.z >= 0 && (map[x][y].z < 0 || v.z <= map[x][y].z)) {
				map[x][y].z = v.z;
				map[x][y].r = c.r;
				map[x][y].g = c.g;
				map[x][y].b = c.b;
			}
		};
		
		void fastPoint(const cppmath::vec3& v, const Color& c) {
			int x = v.x;
			int y = v.y;
			
			if (v.z >= 0 && (map[x][y].z < 0 || v.z <= map[x][y].z)) {
				map[x][y].z = v.z;
				map[x][y].r = c.r;
				map[x][y].g = c.g;
				map[x][y].b = c.b;
			}
		};
		
		void fastVLine(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c1, const Color& c2) {
			if (v1.y == v2.y) 
				point(v1, Color::halfmix(c1, c2));
			else if (v1.y < v2.y) {
				double mix = 0.0;
				double mixstep = 1.0 / (v2.y - v1.y);
				cppmath::vec3 point = v1;
				double dz = (v2.z - v1.z) / (v2.y - v1.y);
				for (; point.y <= v2.y; point.y += 1) {
					point.z += dz;
					this->point(point, Color::interpolate(c1, c2, mix));
					mix += mixstep;
				}
			} else {
				double mix = 0.0;
				double mixstep = 1.0 / (v1.y - v2.y);
				cppmath::vec3 point = v2;
				double dz = (v1.z - v2.z) / (v1.y - v2.y);
				for (; point.y <= v1.y; point.y += 1) {
					point.z += dz;
					this->point(point, Color::interpolate(c2, c1, mix));
					mix += mixstep;
				}
			}
		};
		
		void fastVLine(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c) {
			if (v1.y == v2.y) 
				point(v1, c);
			else if (v1.y < v2.y) {
				cppmath::vec3 point = v1;
				double dz = (v2.z - v1.z) / (v2.y - v1.y);
				for (; point.y <= v2.y; point.y += 1) {
					point.z += dz;
					this->point(point, c);
				}
			} else {
				cppmath::vec3 point = v2;
				double dz = (v1.z - v2.z) / (v1.y - v2.y);
				for (; point.y <= v1.y; point.y += 1) {
					point.z += dz;
					this->point(point, c);
				}
			}
		};
		
		void fastHLine(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c1, const Color& c2) {
			if (v1.x == v2.x) 
				point(v1, Color::halfmix(c1, c2));
			else if (v1.x < v2.x) {
				double mix = 0.0;
				double mixstep = 1.0 / (v2.x - v1.x);
				cppmath::vec3 point = v1;
				double dz = (v2.z - v1.z) / (v2.x - v1.x);
				for (; point.x <= v2.x; point.x += 1) {
					point.z += dz;
					this->point(point, Color::interpolate(c1, c2, mix));
					mix += mixstep;
				}
			} else {
				double mix = 0.0;
				double mixstep = 1.0 / (v1.x - v2.x);
				cppmath::vec3 point = v2;
				double dz = (v1.z - v2.z) / (v1.x - v2.x);
				for (; point.x <= v1.x; point.x += 1) {
					point.z += dz;
					this->point(point, Color::interpolate(c2, c1, mix));
					mix += mixstep;
				}
			}
		};
		
		void fastHLine(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c) {
			if (v1.x == v2.x) 
				point(v1, c);
			else if (v1.x < v2.y) {
				cppmath::vec3 point = v1;
				double dz = (v2.z - v1.z) / (v2.x - v1.x);
				for (; point.x <= v2.x; point.x += 1) {
					point.z += dz;
					this->point(point, c);
				}
			} else {
				cppmath::vec3 point = v2;
				double dz = (v1.z - v2.z) / (v1.x - v2.x);
				for (; point.x <= v1.x; point.x += 1) {
					point.z += dz;
					this->point(point, c);
				}
			}
		};
		
		void bresenham_line(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c1, const Color& c2) {
			double dx = v2.x - v1.x;
			
			if (dx == 0)
				fastVLine(v1, v2, c1, c2);
			
			double dy = v2.y - v1.y;
			
			if (dy == 0)
				fastVLine(v1, v2, c1, c2);
			
			if (dx >= dy) {
				double derr = std::abs(dy / dx);
				double err = 0.0;
				// Step for color
				double sx = 1.0 / std::abs(dx);
				double ct = 0.0;
				// Step for z
				double sz = (v2.z - v1.z) * sx;
				cppmath::vec3 point = v1;
				
				if (dx > 0)
					for (; point.x <= v2.x; point.x += 1) {
						this->point(point, Color::interpolate(c1, c2, ct));
						ct += sx;
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.y += sgn(dy);
							err -= 1.0;
						}
					}
				else
					for (; point.x >= v2.x; point.x -= 1) {
						this->point(point, Color::interpolate(c1, c2, ct));
						ct += sx;
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.y += sgn(dy);
							err -= 1.0;
						}
					}
			} else {
				double derr = std::abs(dx / dy);
				double err = 0.0;
				// Step for color
				double sy = 1.0 / std::abs(dy);
				double ct = 0.0;
				// Step for z
				double sz = (v2.z - v1.z) * sy;
				cppmath::vec3 point = v1;
				
				if (dx > 0)
					for (; point.y <= v2.y; point.y += 1) {
						this->point(point, Color::interpolate(c1, c2, ct));
						ct += sy;
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.x += sgn(dx);
							err -= 1.0;
						}
					}
				else
					for (; point.y >= v2.y; point.y -= 1) {
						this->point(point, Color::interpolate(c1, c2, ct));
						ct += sy;
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.x += sgn(dx);
							err -= 1.0;
						}
					}
			}
		};
		
		void bresenham_line(const cppmath::vec3& v1, const cppmath::vec3& v2, const Color& c) {
			double dx = v2.x - v1.x;
			
			if (dx == 0)
				fastVLine(v1, v2, c);
			
			double dy = v2.y - v1.y;
			
			if (dy == 0)
				fastVLine(v1, v2, c);
			
			if (dx >= dy) {
				double derr = std::abs(dy / dx);
				double err = 0.0;
				// Step for z
				double sz = (v2.z - v1.z) / std::abs(dx);
				cppmath::vec3 point = v1;
				
				if (dx > 0)
					for (; point.x <= v2.x; point.x += 1) {
						this->point(point, c);
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.y += sgn(dy);
							err -= 1.0;
						}
					}
				else
					for (; point.x >= v2.x; point.x -= 1) {
						this->point(point, c);
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.y += sgn(dy);
							err -= 1.0;
						}
					}
			} else {
				double derr = std::abs(dx / dy);
				double err = 0.0;
				// Step for z
				double sz = (v2.z - v1.z) / std::abs(dy);
				cppmath::vec3 point = v1;
				
				if (dx > 0)
					for (; point.y <= v2.y; point.y += 1) {
						this->point(point, c);
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.x += sgn(dx);
							err -= 1.0;
						}
					}
				else
					for (; point.y >= v2.y; point.y -= 1) {
						this->point(point, c);
						point.z += sz;
						
						err += derr;
						if (err >= 0.5) {
							point.x += sgn(dx);
							err -= 1.0;
						}
					}
			}
		};
		
		void line(const cppmath::vec3& v1_, const cppmath::vec3& v2_, const Color& c) {
			cppmath::vec3 v1 = v1_;
			cppmath::vec3 v2 = v2_;
			
			cppmath::vec3 dv = v2 - v1;
			cppmath::vec2 ds(dv.x >= 0 ? 1 : -1, dv.y >= 0 ? 1 : -1);
			
			if (abs(dv.x) > abs(dv.y)) {
				cppmath::vec2 tan = cppmath::vec2(dv.y / dv.x, dv.z / dv.x);
				
				if (ds.x > 0)
					for (int mx = 0; mx <= v2.x - v1.x; ++mx) {
						int x = v1.x + mx;
						int y = v1.y + tan.x * mx;
						double t = (double) mx / (double) (v2.x - v1.x);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
								map[x][y].r = c.r;
								map[x][y].g = c.g;
								map[x][y].b = c.b;
							}
					}
				else
					for (int mx = 0; mx >= v2.x - v1.x; --mx) {
						int x = v1.x + mx;
						int y = v1.y + tan.x * mx;
						double t = (double) mx / (double) (v2.x - v1.x);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
								map[x][y].r = c.r;
								map[x][y].g = c.g;
								map[x][y].b = c.b;
							}
					}
			} else {
				cppmath::vec2 tan = cppmath::vec2(dv.x / dv.y, dv.z / dv.y);
				
				if (ds.y > 0)
					for (int my = 0; my <= v2.y - v1.y; ++my) {
						int y = v1.y + my;
						int x = v1.x + tan.x * my;
						double z = v1.z + tan.y * my;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
								map[x][y].r = c.r;
								map[x][y].g = c.g;
								map[x][y].b = c.b;
							}
					}
				else
					for (int my = 0; my >= v2.y - v1.y; --my) {
						int y = v1.y + my;
						int x = v1.x + tan.x * my;
						double z = v1.z + tan.y * my;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
								map[x][y].r = c.r;
								map[x][y].g = c.g;
								map[x][y].b = c.b;
							}
					}

			}
		};
		
		void line(const cppmath::vec3& v1_, const cppmath::vec3& v2_, const Color& a, const Color& b) {
			cppmath::vec3 v1 = v1_;
			cppmath::vec3 v2 = v2_;
			
			cppmath::vec3 dv = v2 - v1;
			cppmath::vec2 ds(dv.x >= 0 ? 1 : -1, dv.y >= 0 ? 1 : -1);
			
			if (abs(dv.x) > abs(dv.y)) {
				cppmath::vec2 tan = cppmath::vec2(dv.y / dv.x, dv.z / dv.x);
				
				if (ds.x > 0)
					for (int mx = 0; mx <= v2.x - v1.x; ++mx) {
						int x = v1.x + mx;
						int y = v1.y + tan.x * mx;
						double t = (double) mx / (double) (v2.x - v1.x);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
							
								Color mix = Color::interpolate(a, b, t);
								map[x][y].r = mix.r;
								map[x][y].g = mix.g;
								map[x][y].b = mix.b;
							}
					}
				else
					for (int mx = 0; mx >= v2.x - v1.x; --mx) {
						int x = v1.x + mx;
						int y = v1.y + tan.x * mx;
						double t = (double) mx / (double) (v2.x - v1.x);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
							
								Color mix = Color::interpolate(a, b, t);
								map[x][y].r = mix.r;
								map[x][y].g = mix.g;
								map[x][y].b = mix.b;
							}
					}
			} else {
				cppmath::vec2 tan = cppmath::vec2(dv.x / dv.y, dv.z / dv.y);
				
				if (ds.y > 0)
					for (int my = 0; my <= v2.y - v1.y; ++my) {
						int y = v1.y + my;
						int x = v1.x + tan.x * my;
						double t = (double) my / (double) (v2.y - v1.y);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
							
								Color mix = Color::interpolate(a, b, t);
								map[x][y].r = mix.r;
								map[x][y].g = mix.g;
								map[x][y].b = mix.b;
							}
					}
				else
					for (int my = 0; my >= v2.y - v1.y; --my) {
						int y = v1.y + my;
						int x = v1.x + tan.x * my;
						double t = (double) my / (double) (v2.y - v1.y);
						double z = v1.z * (1 - t) + v2.z * t;
						
						if ((x >= 0 && x < get_width()) && (y >= 0 && y < get_height()))
							if (z >= 0 && (map[x][y].z < 0 || z <= map[x][y].z)) {
								map[x][y].z = z;
							
								Color mix = Color::interpolate(a, b, t);
								map[x][y].r = mix.r;
								map[x][y].g = mix.g;
								map[x][y].b = mix.b;
							}
					}
			}
		};
		
		void triangle(const cppmath::vec3& v1_, const cppmath::vec3& v2_, const cppmath::vec3& v3_, const Color& c1, const Color& c2, const Color& c3) {
			cppmath::vec3 v1 = v1_;
			cppmath::vec3 v2 = v2_;
			cppmath::vec3 v3 = v3_;
			
			// 2 variants:
			//                        
			// A-----|---------B      <-- Case 1
			//  \ P  |  Q     /        
			//   \   |      /          --X> 
			//    \  |    /           |
			//     \ |  /             Y
			//      \|/               v
			//       C           
			//                  
			//      C            
			//     /|\            
			//    / |  \          
			//   /  |    \        <-- Case 2
			//  / P |  Q   \      
			// A----|-------B       
            //                          
            //                          
            //                          
            //  Rendering as:                        
            //         --> <--                
            //  A---------|-----B                        
            //   \        |    /          
            //     \      |   /            
            //       \    |  /             
            //         \  | /              
            //           \|/               
            //            C              
            
			cppmath::vec3 A = v1, B = v2, C = v3;			
			Color cA, cB, cC;			
			
			// Check vertices match
			if (v1 == v2 && v2 == v3) {
				point(v1, Color::halfmix3(c1, c2, c3));
				return;
			}
			
			if (v1 == v2) {
				line(v1, v3, Color::halfmix(c1, c2), c3);
				return;
			}
			
			if (v1 == v3) {
				line(v1, v2, Color::halfmix(c1, c3), c2);
				return;
			}
			
			if (v3 == v2) {
				line(v1, v2, Color::halfmix(c2, c3), c2);
				return;
			}
			
			// Check for inline
			if (v1.y == v2.y && v2.y == v3.y) {
				if (v1.x > v2.x && v2.x > v3.x || v1.x < v2.x && v2.x > v3.x) {
					fastVLine(v1, v2, c1, c2);
					fastVLine(v2, v3, c2, c3);
					return;
				}
				if (v2.x > v1.x && v1.x > v3.x || v2.x < v1.x && v1.x < v3.x) {
					fastVLine(v2, v1, c2, c1);
					fastVLine(v1, v3, c1, c3);
					return;
				}
				if (v1.x > v3.x && v3.x > v2.x || v1.x < v3.x && v3.x > v2.x) {
					fastVLine(v1, v3, c1, c3);
					fastVLine(v3, v2, c3, c2);
					return;
				}
			}
			
			if (v1.x == v2.x && v2.x == v3.x) {
				if (v1.y > v2.y && v2.y > v3.y || v1.y < v2.y && v2.y > v3.y) {
					fastHLine(v1, v2, c1, c2);
					fastHLine(v2, v3, c2, c3);
					return;
				}
				if (v2.y > v1.y && v1.y > v3.y || v2.y < v1.y && v1.y < v3.y) {
					fastHLine(v2, v1, c2, c1);
					fastHLine(v1, v3, c1, c3);
					return;
				}
				if (v1.y > v3.y && v3.y > v2.y || v1.y < v3.y && v3.y > v2.y) {
					fastHLine(v1, v3, c1, c3);
					fastHLine(v3, v2, c3, c2);
					return;
				}
			}
			
			// Sort vertices as A, B, C
			if (v1.x <= v2.x && v1.x <= v3.x) {
				A = v1;
				cA = c1;
				if (v2.x > v3.x) {
					B = v2;
					cB = c2;
					C = v3;
					cC = c3;
				} else {
					B = v3;
					cB = c3;
					C = v2;
					cC = c2;
				}
			} else if (v2.x <= v1.x && v2.x <= v3.x) {
				A = v2;
				cA = c2;
				if (v1.x > v3.x) {
					B = v1;
					cB = c1;
					C = v3;
					cC = c3;
				} else {
					B = v3;
					cB = c3;
					C = v1;
					cC = c1;
				}
			} else if (v3.x <= v1.x && v3.x <= v2.x) {
				A = v3;
				cA = c3;
				if (v1.x > v2.x) {
					B = v1;
					cB = c1;
					C = v2;
					cC = c2;
				} else {
					B = v2;
					cB = c2;
					C = v1;
					cC = c1;
				}
			}
			
			// Calculate C vertice origin
			double p = (B.x - A.x) * (C.y - A.y);
			double q = (C.x - A.x) * (B.y - A.y);
			
			double det = p - q;
			
			cppmath::ivec3 iA = A;
			cppmath::ivec3 iB = B;
			cppmath::ivec3 iC = C;
			
			// Draw vertices & bounds
			//point(v1, c1);
			//point(v2, c2);
			//point(v3, c3);
			//
			line(v1, v2, c1, c2);
			line(v2, v3, c2, c3);
			line(v3, v1, c3, c1);
			
			cppmath::vec2 tanAB((B.y - A.y) / (B.x - A.x), (B.z - A.z) / (B.x - A.x));
			cppmath::vec2 tanAC((C.y - A.y) / (C.x - A.x), (C.z - A.z) / (C.x - A.x));
			
			for (int x = 0; x <= (C.x - A.x); ++x) { // P
				double ABy = A.y + tanAB.x * x;
				double ABz = A.z + tanAB.y * x;
				double ABt = x / (B.x - A.x);
				double ACy = A.y + tanAC.x * x;
				double ACz = A.z + tanAC.y * x;
				double ACt = x / (C.x - A.x);
				
				fastVLine(cppmath::vec3(A.x + x, ACy, ACz), cppmath::vec3(A.x + x, ABy, ABz), Color::interpolate(cA, cC, ACt), Color::interpolate(cA, cB, ABt));
				point(cppmath::vec3(A.x + x, ACy, ACz), Color::interpolate(cA, cC, ACt));
				point(cppmath::vec3(A.x + x, ABy, ABz), Color::interpolate(cA, cB, ABt));
			}
			
			cppmath::vec2 tanBA((A.y - B.y) / (A.x - B.x), (A.z - B.z) / (A.x - B.x));
			cppmath::vec2 tanBC((C.y - B.y) / (C.x - B.x), (C.z - B.z) / (C.x - B.x));
			
			for (int x = 0; x < (B.x - C.x); ++x) { // Q
				double BAy = B.y - tanBA.x * x;
				double BAz = B.z - tanBA.y * x;
				double BAt = x / (B.x - A.x);
				double BCy = B.y - tanBC.x * x;
				double BCz = B.z - tanBC.y * x;
				double BCt = x / (B.x - C.x);
				
				fastVLine(cppmath::vec3(B.x - x, BAy, BAz), cppmath::vec3(B.x - x, BCy, BCz), Color::interpolate(cB, cA, BAt), Color::interpolate(cB, cC, BCt));
				point(cppmath::vec3(B.x - x, BAy, BAz), Color::interpolate(cB, cA, BAt));
				point(cppmath::vec3(B.x - x, BCy, BCz), Color::interpolate(cB, cC, BCt));
			}
		};
		
		void triangle(const cppmath::vec3& v1_, const cppmath::vec3& v2_, const cppmath::vec3& v3_, const Color& c) {
			cppmath::vec3 v1 = v1_;
			cppmath::vec3 v2 = v2_;
			cppmath::vec3 v3 = v3_;
			
			// 2 variants:
			//                        
			// A-----|---------B      <-- Case 1
			//  \ P  |  Q     /        
			//   \   |      /          --X> 
			//    \  |    /           |
			//     \ |  /             Y
			//      \|/               v
			//       C           
			//                  
			//      C            
			//     /|\            
			//    / |  \          
			//   /  |    \        <-- Case 2
			//  / P |  Q   \      
			// A----|-------B       
            //                          
            //                          
            //                          
            //  Rendering as:                        
            //         --> <--                
            //  A---------|-----B                        
            //   \        |    /          
            //     \      |   /            
            //       \    |  /             
            //         \  | /              
            //           \|/               
            //            C              
            
			cppmath::vec3 A = v1, B = v2, C = v3;	
			
			// Check vertices match
			if (v1 == v2 && v2 == v3) {
				point(v1, c);
				return;
			}
			
			if (v1 == v2) {
				line(v1, v3, c);
				return;
			}
			
			if (v1 == v3) {
				line(v1, v2, c);
				return;
			}
			
			if (v3 == v2) {
				line(v1, v2, c);
				return;
			}
			
			// Check for inline
			if (v1.y == v2.y && v2.y == v3.y) {
				if (v1.x > v2.x && v2.x > v3.x || v1.x < v2.x && v2.x > v3.x) {
					fastVLine(v1, v2, c);
					fastVLine(v2, v3, c);
					return;
				}
				if (v2.x > v1.x && v1.x > v3.x || v2.x < v1.x && v1.x < v3.x) {
					fastVLine(v2, v1, c);
					fastVLine(v1, v3, c);
					return;
				}
				if (v1.x > v3.x && v3.x > v2.x || v1.x < v3.x && v3.x > v2.x) {
					fastVLine(v1, v3, c);
					fastVLine(v3, v2, c);
					return;
				}
			}
			
			if (v1.x == v2.x && v2.x == v3.x) {
				if (v1.y > v2.y && v2.y > v3.y || v1.y < v2.y && v2.y > v3.y) {
					fastHLine(v1, v2, c);
					fastHLine(v2, v3, c);
					return;
				}
				if (v2.y > v1.y && v1.y > v3.y || v2.y < v1.y && v1.y < v3.y) {
					fastHLine(v2, v1, c);
					fastHLine(v1, v3, c);
					return;
				}
				if (v1.y > v3.y && v3.y > v2.y || v1.y < v3.y && v3.y > v2.y) {
					fastHLine(v1, v3, c);
					fastHLine(v3, v2, c);
					return;
				}
			}
			
			// Sort vertices as A, B, C
			if (v1.x <= v2.x && v1.x <= v3.x) {
				A = v1;
				if (v2.x > v3.x) {
					B = v2;
					C = v3;
				} else {
					B = v3;
					C = v2;
				}
			} else if (v2.x <= v1.x && v2.x <= v3.x) {
				A = v2;
				if (v1.x > v3.x) {
					B = v1;
					C = v3;
				} else {
					B = v3;
					C = v1;
				}
			} else if (v3.x <= v1.x && v3.x <= v2.x) {
				A = v3;
				if (v1.x > v2.x) {
					B = v1;
					C = v2;
				} else {
					B = v2;
					C = v1;
				}
			}
			
			// Calculate C vertice origin
			double p = (B.x - A.x) * (C.y - A.y);
			double q = (C.x - A.x) * (B.y - A.y);
			
			double det = p - q;
			
			cppmath::ivec3 iA = A;
			cppmath::ivec3 iB = B;
			cppmath::ivec3 iC = C;
			
			// Draw vertices & bounds
			//point(v1, c1);
			//point(v2, c2);
			//point(v3, c3);
			//
			line(v1, v2, c);
			line(v2, v3, c);
			line(v3, v1, c);
			
			cppmath::vec2 tanAB((B.y - A.y) / (B.x - A.x), (B.z - A.z) / (B.x - A.x));
			cppmath::vec2 tanAC((C.y - A.y) / (C.x - A.x), (C.z - A.z) / (C.x - A.x));
			
			for (int x = 0; x <= (C.x - A.x); ++x) { // P
				double ABy = A.y + tanAB.x * x;
				double ABz = A.z + tanAB.y * x;
				double ABt = x / (B.x - A.x);
				double ACy = A.y + tanAC.x * x;
				double ACz = A.z + tanAC.y * x;
				double ACt = x / (C.x - A.x);
				
				fastVLine(cppmath::vec3(A.x + x, ACy, ACz), cppmath::vec3(A.x + x, ABy, ABz), c);
				point(cppmath::vec3(A.x + x, ACy, ACz), c);
				point(cppmath::vec3(A.x + x, ABy, ABz), c);
			}
			
			cppmath::vec2 tanBA((A.y - B.y) / (A.x - B.x), (A.z - B.z) / (A.x - B.x));
			cppmath::vec2 tanBC((C.y - B.y) / (C.x - B.x), (C.z - B.z) / (C.x - B.x));
			
			for (int x = 0; x < (B.x - C.x); ++x) { // Q
				double BAy = B.y - tanBA.x * x;
				double BAz = B.z - tanBA.y * x;
				double BAt = x / (B.x - A.x);
				double BCy = B.y - tanBC.x * x;
				double BCz = B.z - tanBC.y * x;
				double BCt = x / (B.x - C.x);
				
				fastVLine(cppmath::vec3(B.x - x, BAy, BAz), cppmath::vec3(B.x - x, BCy, BCz), c);
				point(cppmath::vec3(B.x - x, BAy, BAz), c);
				point(cppmath::vec3(B.x - x, BCy, BCz), c);
			}
		};
	};
};