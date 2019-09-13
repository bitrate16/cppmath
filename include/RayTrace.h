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
		
		double power = 0.0;
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
		double reflect = 0.0;
		// 0 : no refraction
		double refract = 0.0;
		// refraction coefficient
		double refract_val = 0.0;
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
				if (tman.distance < 10e-8) {
					tman.distance = (-b + std::sqrt(discriminant)) / (2.0 * a);
					if (tman.distance < 10e-8)
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
	
	class Triangle : public SceneObject {
		
	public:
		cppmath::vec3 center;
		cppmath::vec3 A, B, C;
		ObjectMaterial material;
		
		Triangle(const cppmath::vec3& a, const cppmath::vec3& b, const cppmath::vec3& c) {
			set(a, b, c);
		};
		
		void set(const cppmath::vec3& a, const cppmath::vec3& b, const cppmath::vec3& c) {
			A = a;
			B = b;
			C = c;
			center = (A + B + C) / 3.0;
		};
		
		void setMaterial(const ObjectMaterial& material) {
			this->material = material;
		};
		
		TraceManifold hit(const ray& r) {
			cppmath::vec3 ab = B - A;
			cppmath::vec3 ac = C - A;
			
			// Caculate normal to plane
			TraceManifold tm;
			tm.normal = cppmath::vec3::cross(ab, ac).norm();
			
			double ndd = cppmath::vec3::dot(tm.normal, r.direction());
			
			// No hit, parallel
			if (std::abs(ndd) <= std::numeric_limits<double>::epsilon())
				return tm;
			
			double d = cppmath::vec3::dot(tm.normal, A);
			tm.distance = (cppmath::vec3::dot(tm.normal, r.origin()) + d) / ndd;
			
			if (tm.distance < 0)
				return tm;
			
			tm.location = r.point_at_parameter(tm.distance);
			
			cppmath::vec3 Ct;
			//std::cout << "PRE FRAG " << tm.location << std::endl;
			
			cppmath::vec3 edge0 = B - A;
			cppmath::vec3 vp0 = tm.location - A;
			Ct = cppmath::vec3::cross(edge0, vp0);
			
			// On the right side
			if (cppmath::vec3::dot(tm.normal, Ct) < 0)
				return tm;
			//std::cout << "PRE FRAG1 " << tm.location << std::endl;
			
			cppmath::vec3 edge1 = C - B;
			cppmath::vec3 vp1 = tm.location - B;
			Ct = cppmath::vec3::cross(edge0, vp1);
			
			// On the right side
			if (cppmath::vec3::dot(tm.normal, Ct) < 0)
				return tm;
			//std::cout << "PRE FRAG2 " << tm.location << std::endl;
			
			cppmath::vec3 edge2 = A - C;
			cppmath::vec3 vp2 = tm.location - C;
			Ct = cppmath::vec3::cross(edge0, vp2);
			
			// On the right side
			if (cppmath::vec3::dot(tm.normal, Ct) < 0)
				return tm;
			//std::cout << "PRE FRAG3 " << tm.location << std::endl;
			//std::cout << "FRAG" << std::endl;
			tm.hit = 1;
			return tm;
		};
		
		ObjectMaterial get_material(const cppmath::vec3& point) {
			return material;
		};
		
		cppmath::vec3 get_center() {
			return center;
		};
	};
	
	
	class Plane : public SceneObject {
		
	public:
		cppmath::vec3 location;
		cppmath::vec3 normal;
		ObjectMaterial material;
		
		Plane(const cppmath::vec3& location, const cppmath::vec3& normal) {
			set(location, normal);
		};
		
		void set(const cppmath::vec3& location, const cppmath::vec3& normal) {
			this->location = location;
			this->normal = normal;
			this->normal.norm();
		};
		
		void setMaterial(const ObjectMaterial& material) {
			this->material = material;
		};
		
		TraceManifold hit(const ray& r) {
			// Caculate normal to plane
			TraceManifold tm;
			tm.normal = normal;
			
			double ndd = cppmath::vec3::dot(tm.normal, r.direction());
			
			// No hit, parallel
			if (std::abs(ndd) <= 10e-8)
				return tm;
			
			tm.distance = cppmath::vec3::dot(tm.normal, location - r.origin()) / ndd;
			
			if (tm.distance < 10e-8)
				return tm;
			
			tm.location = r.point_at_parameter(tm.distance);
			tm.hit = 1;
			
			return tm;
		};
		
		ObjectMaterial get_material(const cppmath::vec3& point) {
			return material;
		};
		
		cppmath::vec3 get_center() {
			return location;
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

		// Diffuse scale for light in shadow of an object
		double shadow_diffuse = 0.5;
		// Minimal ray power to keep running
		double MIN_RAY_POWER = 10e-3;
	
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
		HitManifold shoot(const ray& r, int ignored_id = -1) {
			// ignored_id is used during reflection calculations to 
			//  avoid object trace itself forever till stack overflow.
			
			if (r.power < MIN_RAY_POWER)
				return HitManifold();
			
			// Find closest hit
			TraceManifold closest_hit;
			SceneObject* closest_object;
			int closest = -1;
			// Collect hits on all objects
			std::vector<TraceManifold> hits(objects.size());
			for (int i = 0; i < objects.size(); ++i)
				if (i == ignored_id)
					continue;
				else
					hits[i] = objects[i]->hit(r);
				
		
			// XXX: Move to 1st loop
			
			int distance_closest = std::numeric_limits<double>::max();
			for (int i = 0; i < hits.size(); ++i) {
				if (i == ignored_id)
					continue;
				else if (hits[i].hit && hits[i].distance >= 10e-8 && hits[i].distance - distance_closest < 10e-8) {
					closest = i;
					distance_closest = hits[i].distance;
				}
			}
			
			if (closest == -1)
				return { 0, spaint::Color::BLACK };
			
			closest_hit = hits[closest];
			closest_object = objects[closest];
			
			HitManifold hitm;
			hitm.hit = 1;
			
			// If object is emitting light in this point, apply light color with luminosity scale
			ObjectMaterial closest_material = closest_object->get_material(closest_hit.location);
			if (closest_material.luminosity) {
				hitm.color = closest_material.color;
				hitm.color.scale(closest_material.luminosity);
				hitm.color.scale(-cppmath::vec3::cos_between(closest_hit.normal, r.direction()));
			}
			
			// Calculate ambient lighting from light emitting objects
			spaint::Color lighting;
			// Iterate over all objects and hit then a ray to get summary resulting color lighting on this object
			spaint::Color surface_color = closest_material.color;
			surface_color.scale(closest_material.diffuse);
			if (closest_material.diffuse > 0) {
				for (int i = 0; i < objects.size(); ++i) {
					if (i == closest)
						continue;
					
					ray l(closest_hit.location, (objects[i]->get_center() - closest_hit.location).norm());
					TraceManifold trm = objects[i]->hit(l);
					
					if (!trm.hit)
						continue;
					
					/*
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
					*/
					
					ObjectMaterial tmat = objects[i]->get_material(trm.location);
					
					// If object emmit light
					if (tmat.luminosity > 0) {
						spaint::Color lumine = tmat.color;
						lumine.scale(tmat.luminosity);
						lumine.scale(-cppmath::vec3::cos_between(closest_hit.normal, trm.normal));
						spaint::Color minimapply = spaint::Color::min(lumine, surface_color);
					
						// Check for all overlapping objects & scale light 
						//  by cos between ray to object and ray to light
						// i.e.: Iterate over all objects and calculate total shadow value in shadow.
						double shadow_cos = 1.0;
						for (int j = 0; j < objects.size(); ++j) {
							if (i == j || j == closest)
								continue;
							
							TraceManifold trmo = objects[j]->hit(l);
							if (trmo.hit && trmo.distance >= 0 && trmo.distance < trm.distance) {
								ObjectMaterial mato = objects[i]->get_material(trmo.location);
								double cs = abs(cppmath::vec3::cos_between(l.direction(), (trmo.normal).norm()) * (1.0 - mato.refract) * shadow_diffuse);
								shadow_cos -= cs;
							}
						}
						shadow_cos = shadow_cos < 0 ? 0.0 : (shadow_cos > 1.0 ? 1.0 : shadow_cos);
						minimapply.scale(shadow_cos);
						
						// Apply color affect on surface
						lighting += minimapply;
					}
				}
			}
			
			hitm.color += lighting;
			hitm.color.scale(r.power);
			
			// Calculate reflections
			if (closest_material.reflect > 0) {
				ray l(closest_hit.location, cppmath::vec3::reflect(r.direction(), closest_hit.normal));
				l.power = r.power * closest_material.reflect;
				
				HitManifold hitr = shoot(l, closest);
				if (hitr.hit) {
					hitr.color.scale(closest_material.reflect);
					hitm.color += hitr.color;
				}
			}
			
			// Calculate refractions
			if (closest_material.refract > 0) {
				ray l(closest_hit.location, cppmath::vec3::refract(r.direction(), closest_hit.normal, closest_material.refract_val));
				l.power = r.power * closest_material.refract;
				
				HitManifold hitr = shoot(l, closest);
				if (hitr.hit) {
					hitr.color.scale(closest_material.refract);
					hitm.color += hitr.color;
				}
			}
			
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
	
		cppmath::vec3 location;
	
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
		
			cppmath::vec3 ray_direction;
			
			if (camera.flatProjection) {
				ray_direction = cppmath::vec3((camera.width / 2 - x) / (double) camera.width, (camera.height / 2 - y) / (double) camera.height, 1.0).norm();
				ray_direction.norm();
			} else {
				
				// Deprecated
				
				/*double angle_step = fov / (double) width;
				ray_direction = (std::tan(sx + (double) x * angle_step), std::tan(sy + (double) y * angle_step), 1.0);
				ray_direction.z = std::sqrt(1.0 - ray_direction.x * ray_direction.x - ray_direction.y * ray_direction.y);*/
			}
			
			ray r(camera.location, ray_direction);
			r.power = 1.0;
			HitManifold hit = scene.shoot(r);
			
			if (hit.hit)
				return hit.color;
			return get_background();
		};
	};
}