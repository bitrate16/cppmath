#pragma once

#include <cmath>
#include <vector>
#include <limits>
#include <iostream>

#include "vec3.h"
#include "Color.h"

namespace raytrace {
	
	class ray {
	
	public:
		
		// Ray origin
		cppmath::vec3 A;
		// Ray direction
		cppmath::vec3 B;
		
		ray(const cppmath::vec3& origin, const cppmath::vec3& direction) : A(origin), B(direction) {};
		
		ray(const cppmath::vec3& direction) : B(direction) {};
		
		cppmath::vec3 point_at_parameter(double t) const { return A + t * B; };
		
		cppmath::vec3 origin() const {
			return A;
		};
		
		cppmath::vec3 direction() const {
			return B;
		};
	};
		
	std::ostream& operator<<(std::ostream& os, const ray& r) {
		os << '[' << r.A << " --> " << r.B << ']';
		return os;
	}
	
	// Keeps information about ray hitting an object
	struct TraceManifold {
		// Has hit an object
		bool hit = 0.0;
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
		
		virtual TraceManifold hit(const ray& r) { return TraceManifold(); };
		
		virtual ObjectMaterial get_material(const cppmath::vec3& point) { return ObjectMaterial(); };
		
		virtual cppmath::vec3 get_center() { return cppmath::vec3(); };
	};
	
	class Sphere : public SceneObject {
		
		public:
		
		double radius;
		cppmath::vec3 center;
		ObjectMaterial material;
		
		Sphere(const cppmath::vec3& center, double radius) {
			set(center, radius);
		};
		
		void set(const cppmath::vec3& center, double radius) {
			this->center = center;
			this->radius = radius;
		};
		
		void setMaterial(const ObjectMaterial& material) {
			this->material = material;
		};
		
		TraceManifold hit(const ray& r) {
			//std::cout << r << std::endl;
			cppmath::vec3 oc = r.origin() - center;
			double a = cppmath::vec3::dot(r.direction(), r.direction());
			double b = 2.0 * cppmath::vec3::dot(oc, r.direction());
			double c = cppmath::vec3::dot(oc, oc) - radius * radius;
			double discriminant = b * b - 4.0 * a * c;
			if (discriminant < 0) {
				return TraceManifold();
			} else {
				TraceManifold tman;
				tman.distance = (-b - std::sqrt(discriminant)) / (2.0 * a);
				if (tman.distance < 0) {
					tman.distance = (-b + std::sqrt(discriminant)) / (2.0 * a);
					if (tman.distance < 0)
						return tman;
				}
				
				tman.hit = 1;
				tman.location = r.point_at_parameter(tman.distance);
				tman.normal = (tman.location - center).norm();
				return tman;
			}
		};
		
		ObjectMaterial get_material(const cppmath::vec3& point) {
			return material;
		};
		
		cppmath::vec3 get_center() {
			return center;
		};
	};
	
	struct HitManifold {
		// Set to 1 if something was hit, 0 else
		bool hit = 0;
		// Summary hit color
		spaint::Color color;
	};
	
	class RayTraceScene {
		
	private:
		
		// Set of object on the scene
		// Deleted automatically after destroy
		std::vector<SceneObject*> objects;
		
	public:
	
		~RayTraceScene() {
			for (int i = 0; i < objects.size(); ++i)
				delete objects[i];
		};
		
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
			int closest = -1;
			{ 
				// Collect hits on all objects
				std::vector<TraceManifold> hits(objects.size());
				for (int i = 0; i < objects.size(); ++i)
					hits[i] = objects[i]->hit(r);
			
				// XXX: Move to 1st loop
				
				int distance_closest = std::numeric_limits<double>::max();
				for (int i = 0; i < hits.size(); ++i) {
					if (hits[i].hit && hits[i].distance >= 0.0 && hits[i].distance < distance_closest) {
						closest = i;
						distance_closest = hits[i].distance;
					}
				}
				
				if (closest == -1)
					return { 0, spaint::Color::BLACK };
				
				closest_hit = hits[closest];
				closest_object = objects[closest];
			}
			
			HitManifold hitm;
			hitm.hit = 1;
			
			// If object is emitting light in this point, apply light color with luminosity scale
			ObjectMaterial closest_material = closest_object->get_material(closest_hit.location);
			if (closest_material.luminosity) {
				hitm.color = closest_material.color;
				hitm.color.scale(closest_material.luminosity);
				hitm.color.scale(-cppmath::vec3::cos_between(closest_hit.normal, r.direction()));
			}
			
			spaint::Color lighting;
			// Iterate over all objects and hit then a ray to get summary resulting color lighting on this object
			spaint::Color surface_color = closest_material.color;
			surface_color.scale(closest_material.diffuse);
			for (int i = 0; i < objects.size(); ++i) {
				if (i == closest)
					continue;
				
				ray l(closest_hit.location, (objects[i]->get_center() - closest_hit.location).norm());
				TraceManifold trm = objects[i]->hit(l);
				
				if (!trm.hit)
					continue;
				
				// Check if there is no object that will overlap light source
				bool overlap = 0;
				for (int j = 0; j < objects.size(); ++j) {
					if (i == j || j == closest)
						continue;
					
					TraceManifold trmo = objects[j]->hit(l);
					if (trmo.hit && trmo.distance >= 0 && trmo.distance < trm.distance) {
						overlap = 1;
						break;
					}
				}
				
				if (overlap)
					continue;
				
				ObjectMaterial tmat = objects[i]->get_material(trm.location);
				
				// If object emmit light
				if (tmat.luminosity > 0) {
					spaint::Color lumine = tmat.color;
					lumine.scale(tmat.luminosity);
					lumine.scale(-cppmath::vec3::cos_between(closest_hit.normal, trm.normal));
					spaint::Color minimapply = spaint::Color::min(lumine, surface_color);
					
					// Apply color affect on surface
					lighting += minimapply;
				}
			}
			
			hitm.color += lighting;
			
			// Calculate refletions, refractions
			
			return hitm;
		};
	};
	
	struct Camera {
	
	public:
		
		// Viewport resolution
		int width, height;
		// Fov in sphere screen projection
		double fov;
		// Z value in flat screen projection
		double zDistance;
		// Used for constructing ray coords for flat screen.
		bool flatProjection;
	
		Camera() {};
		
		Camera(int width, int height) {
			this->width          = width;
			this->height         = height;
			this->fov            = 0;
			this->zDistance      = 1.0;
			this->flatProjection = 1;
		};
		
		Camera(int width, int height, double fov) {
			this->width          = width;
			this->height         = height;
			this->fov            = fov * (3.14159265358979323846 / 180.0);
			this->zDistance      = 1.0;
			this->flatProjection = 0;
		};
		
		void set_fov(double fov) {
			this->fov = fov * (3.14159265358979323846 / 180.0);
			this->flatProjection = 0;
		};
		
		void set_zdistance(double zDistance) {
			this->zDistance = zDistance;
			this->flatProjection = 1;
		};
	};
	
	class RayTrace {
		RayTraceScene scene;
		
		// Color returned when no hit on any object
		spaint::Color background;
		
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
	
		Camera camera;
	
		RayTrace() {};
	
		RayTrace(const Camera& camera) {
			this->camera = camera;
		};
		
		inline spaint::Color& get_background() {
			return background;
		};
		
		inline void set_background(const spaint::Color& c) {
			background = c;
		};
		
		inline int get_width() {
			return camera.width;
		};
		
		inline int get_height() {
			return camera.height;
		};
		
		inline Camera& get_camera() {
			return camera;
		};
		
		inline RayTraceScene& get_scene() {
			return scene;
		};
		
		// Returns hit color on projection to camera view.
		// Automatically transforms x, y to coordinates & hits object
		spaint::Color hitColorAt(int x, int y) {			
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
			
			//double d = 1.0 / std::tan(fov / 2.0);
		
			cppmath::vec3 ray_direction;
			
			if (camera.flatProjection) {
				ray_direction = cppmath::vec3((camera.width / 2 - x) / (double) camera.width, (camera.height / 2 - y) / (double) camera.height, 1.0).norm();
				ray_direction.norm();
			}
			/*ray_direction.x += 0.5;
			ray_direction.y += 0.5;
			double aspect = (double) width / (double) height;
			ray_direction.x = aspect * (2.0 * ray_direction.x / (double) width) - 1.0;
			ray_direction.y = (2.0 * ray_direction.y / (double) height) - 1.0;
			ray_direction.z = 1.0;*/
			//ray_direction.norm();
//			cppmath::vec3::Z
//					+ cppmath::vec3::X * std::tan(fov/2.0) * (((x + 0.5)/width)*2.0 - 1.0)
//					+ cppmath::vec3::Y * std::tan(fov/2.0) * (1.0 - ((y + 0.5f)/height)*2.0);
//;
			
			//double angle_step = fov / (double) width;
			//ray_direction=(std::tan(sx + (double) x * angle_step), std::tan(sy + (double) y * angle_step), 1.0);
			// Normalize
			//ray_direction.z = std::sqrt(1.0 - ray_direction.x * ray_direction.x - ray_direction.y * ray_direction.y);
			
			ray r(cppmath::vec3::Zero, ray_direction);
			HitManifold hit = scene.shoot(r);
			
			if (hit.hit)
				return hit.color;
			return get_background();
		};
	};
}