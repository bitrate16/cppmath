#pragma once

#inlude <vector>
#include <limits>

#inlude "vec3.h"
#inlude "Color.h"

namespace RayTrace {
	
	class ray {
	
	public:
		
		// Ray origin
		cppmath::vec3 A;
		// Ray direction
		cppmath::vec3 B;
		
		ray(const cppmath::vec3& origin, const cppmath::vec3& direction) A(origin), B(direction) {};
		
		ray(const cppmath::vec3& direction) B(direction) {};
		
		cppmath::vec3 point_at_parameter(double t) { return A + t * B; };
		
		cppmath::vec3 origin() {
			return A;
		};
		
		cppmath::vec3 direction() {
			return B;
		};
	};
	
	// Keeps information about ray hitting an object
	struct TraceManifold {
		// Has hit an object
		bool hit;
		// Distance to a hit point from ray.origin()
		double distance;
		// Hit point location
		cppmath::vec3 location;
		// Notmal to an object in hit point
		cppmath::vec3 normal;
	};
	
	struct ObjectMaterial {
		spaint::Color color;
		// 0 : transparent
		double opacity = 1.0;
		// 0 : no reflection
		double reflect = 1.0;
		// 0 : no refration
		double refract = 0.0;
		// 0 : no diffuse color on light
		double diffuse = 1.0;
		// 0 : no light emission
		double luminosity = 0.0;
	};
	
	class SceneObject {
		
	public:
		
		virtual TraceManifold hit(const ray& r);
		
		virtual ObjectMaterial material(const cppmath::vec3& point);
		
		virtual cppmath::vec3 center();
	};
	
	struct HitManifold {
		// Set to 1 if something was hit, 0 else
		bool reached = 0;
		// Summary hit color
		spaint::Color color;
	};
	
	class RayTraceScene {
		
	private:
		
		// Set of object on the scene
		std::vector<SceneObject*> objects;
		
	public:
		
		// Add object to list of scene objects, check for NULL
		void addObject(SceneObject* o) {
			if (o)
				objects.push_back(o);
		};
		
		// Shoot ray into scene to probe color
		HitManifold shoot(const ray& r) {
			
			// Find closest hit
			TraceManifold closest_hit;
			SceneObject* closest_object;
			{ 
				// Collect hits on all objects
				std::vector<TraceManifold> hits(objects.size());
				for (int i = 0; i < objects.size(); ++i)
					hits[i] = objects[i].hit(r);
			
				// XXX: Move to 1st loop
				
				int closest = -1;
				int distance_closest = std::numeric_limits<double>::max();
				for (int i = 0; i < hits.size(); ++i)
					if (hits[i].distance >= 0.0 && hits[i].distance < distance_closest) {
						closest = i;
						closest_distance = hits[i].distance;
					}
				
				if (closest == -1)
					return { 0, spaint::Color::BLACK };
				
				closest_hit = hits[closest];
				closest_object = objects[closest];
			}
			
			HitManifold hitm;
			hitm.reached = 1;
			
			// If object is emitting light in this point, apply light color with luminosity scale
			Material closest_material = closest_object->material(closest_hit.location);
			if (closest_material.luminosity) {
				hitm.color = closest_material.color;
				hitm.color.scale(closest_material.luminosity);
			}
			
			Color lightning;
			// Iterate over all objects and hit then a ray to get summary resulting color lighting on this object
			Color surface_color = closest_material.color;
			surface_color.scale(closest_material.diffuse);
			for (int i = 0; i < objects.size(); ++i) {
				ray l(closest_hit.location, (objects[i].center() - closest_hit.location).norm());
				TraceManifold trm = objects[i].hit(l);
				
				if (!trm.hit)
					continue;
				
				// Check if there is no object that will overlap light source
				bool overlap = 0;
				for (int j = 0; j < objects.size() && i != j; ++j) {
					TraceManifold trmo = objects[j].hit(l);
					if (trmo.distance >= 0 && trmo.distance < trm.distance) {
						overlap = 1;
						break;
					}
				}
				
				if (overlap)
					continue;
				
				ObjectMateral tmat = objects[i].material(trm.location);
				
				// If object emmit light
				if (tmat.luminosity > 0) {
					Color lumine = tmat.color;
					lumne.scale(tmat.luminosity);
					Color minimapply = Color::min(lumine, surface_color);
					
					// Apply color affect on surface
					lighting += minimapply;
				}
			}
			
			// Calculate refletions, refractions
			
			return hitm;
		};
	};
	
	class RayTrace {
		RayTraceScene scene;
		
		int width, height;
		
		Color background;
		
		//         depth
		//    < - - - - - - >                                
		//    |            /| X height                         
		//    |          /  |                          
		//    |        /    |                          
		//    |      /      | ^                         
		//    |    /        | |                          
		//    |  /          | width                         
		//    |/ \          | |                         
		//  --*   |fov      | v                         
		//     \ /          |                          
		//       \          |                          
		//         \        |                          
		//           \      |                          
		//             \    |                          
		//               \  |                          
		//                 \|                          
		//                                           
		
	public:
	
		RayTrace(int width, int height, double fov) {
			this->width = width;
			this->height = height;
			this->fov = fov;
		};
		
		inline Color& get_background() {
			return background;
		};
		
		inline int get_width() {
			return width;
		};
		
		inline int get_height() {
			return height;
		};
		
		inline RayTraceScene& get_scene() {
			return scene;
		};
		
		// Returns hit color on projection to camera view.
		// Automatically transforms x, y to coordinates & hits object
		Color hitColorAt(int x, int y) {
			double depth = (double) width / 2.0 * std::ctan(fov / 2.0);
			// Step on x/y angles
			double angle_step = fov / (double) width;
			// Starting/ending angle coords
			double ax = -fov / 2.0;
			double ay = -angle_step * (double) height / 2.0;
			
			/*for (int x = 0; x < width; ++x) {
				for (int y = 0; y < height; ++y) {
					
					vec3 ray_direction(std::cos(ax), std::cos(ay), 1.0);
					ray_direction.norm();
					
					ray r(vec3::Zero, ray_direction);
					HitManifold hit = scene.hit(r);
					
					ay += angle_step;
				}
				ax += angle_step;
			}*/
			
			ax += (double) x * angle_step;
			ay += (double) y * angle_step;
			
			vec3 ray_direction(std::cos(ax), std::cos(ay), 1.0);
			ray_direction.norm();
			
			ray r(vec3::Zero, ray_direction);
			HitManifold hit = scene.hit(r);
			
			if (hit.reached)
				return hit.color;
			return get_background();
		};
	};
}