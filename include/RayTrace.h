#pragma once

#include <cmath>
#include <vector>
#include <limits>
#include <iostream>
#include <functional>

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
		
		ray(const cppmath::vec3& origin, const cppmath::vec3& direction, double pwr) : A(origin), B(direction), power(pwr) {};
		
		ray(const cppmath::vec3& origin, const cppmath::vec3& direction) : A(origin), B(direction) {};
		
		ray(const cppmath::vec3& direction) : B(direction) {};
		
		ray(const ray& r) : A(r.A), B(r.B), power(r.power) {};
		
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
		// Set to 0 to avoid rendering surface for sphere lights
		bool surface_visible = 1;
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
	
	class UVSphere : public SceneObject {
		
	public:
		
		double radius;
		cppmath::vec3 center;
		ObjectMaterial material;
		// Maps uv texture
		std::function<spaint::Color(double, double)> uv_map = [](double u, double v) -> spaint::Color { return spaint::Color(0); };
		
		UVSphere(const cppmath::vec3& center, double radius) {
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
			cppmath::vec3 normal = point - center;
			
			double u = std::atan2(normal.x, normal.z);
			double v = std::atan2(std::sqrt(normal.x * normal.x + normal.z * normal.z), normal.y);
			
			ObjectMaterial mat = material;
			mat.color = uv_map(u, v);
			
			return mat;
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
			// TraceManifold tm;
			// cppmath::vec3 v0v1 = B - A; 
			// cppmath::vec3 v0v2 = C - A; 
			// cppmath::vec3 pvec = cppmath::vec3::cross(v0v1, v0v2); 
			// double det = cppmath::vec3::dot(v0v1, pvec); 
			// 
			// if (fabs(det) < 10e-8) return tm; 
			// double invDet = 1 / det; 
		    // 
			// cppmath::vec3 tvec = r.origin() - A; 
			// double u = cppmath::vec3::dot(tvec, pvec) * invDet; 
			// if (u < 0 || u > 1) return tm; 
		    // 
			// cppmath::vec3 qvec = cppmath::vec3::cross(tvec, v0v1); 
			// double v = cppmath::vec3::dot(r.direction(), qvec) * invDet; 
			// if (v < 0 || u + v > 1) return tm; 
		    // 
			// double t = cppmath::vec3::dot(v0v2, qvec) * invDet; 
		    // 
			// tm.hit = 1;
			// tm.normal = pvec;
			// tm.distance = t;
			// tm.location = r.point_at_parameter(t);
			// std::cout << pvec << std::endl;
			// return tm; 
			
			///
			
			// cppmath::vec3 ab = B - A;
			// cppmath::vec3 ac = C - A;
			// 
			// // Caculate normal to plane
			// TraceManifold tm;
			// tm.normal = cppmath::vec3::cross(ab, ac).norm();
			// 
			// cppmath::vec3 tvec = cppmath::vec3::scoss(tm.normal, A);
			// 
			// double ndd = cppmath::vec3::dot(ab, tm.normal);
			// 
			// // No hit, parallel
			// if (std::abs(ndd) <= 10e-8)
			// 	return tm;
			// 
			// ndd = 1.0 / ndd;
			// 
			// cppmath::vec3 tv = r.origin() - A;
			// double u = cppmath::vec3::dot(tv, tm.normal) * ndd;
			// if (u < 0 || u > 1)
			// 	return tm;
			// 
			// cppmath::vec3 qv = cppmath::vec3::cross(tv, ab);
			// double v = cppmath::vec3::dot(r.direction(), qv) * ndd;
			// if (v < 0 || u + v > 1)
			// 	return tm;
			// 
			// tm.distance = cppmath::vec3::dot(ac, qv);
			// tm.location = r.point_at_parameter(tm.distance);
			// tm.hit = 1;
			// 
			// return tm;
			
			///
			
			// tm.distance = cppmath::vec3::dot(tm.normal, A - r.origin()) / ndd;
			// 
			// if (tm.distance < 10e-8)
			// 	return tm;
			// 
			// tm.location = r.point_at_parameter(tm.distance);
			// 
			// cppmath::vec3 Ct;
			// 
			// cppmath::vec3 edge0 = B - A;
			// cppmath::vec3 vp0 = tm.location - A;
			// Ct = cppmath::vec3::cross(edge0, vp0);
			// 
			// // On the right side
			// if (cppmath::vec3::dot(tm.normal, Ct) < 0)
			// 	return tm;
			// 
			// cppmath::vec3 edge1 = C - B;
			// cppmath::vec3 vp1 = tm.location - B;
			// Ct = cppmath::vec3::cross(edge0, vp1);
			// 
			// // On the right side
			// if (cppmath::vec3::dot(tm.normal, Ct) < 0)
			// 	return tm;
			// 
			// cppmath::vec3 edge2 = A - C;
			// cppmath::vec3 vp2 = tm.location - C;
			// Ct = cppmath::vec3::cross(edge0, vp2);
			// 
			// // On the right side
			// if (cppmath::vec3::dot(tm.normal, Ct) < 0)
			// 	return tm;
			// 
			// std::cout << ab << ' ' << ac << ' ' << tm.normal << std::endl;
			// tm.hit = 1;
			// return tm;
			
			return TraceManifold();
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
			if ((ndd < 0 ? -ndd : ndd) <= 10e-8)
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
	
	class UVPlane : public SceneObject {
		
	public:
		cppmath::vec3 location;
		cppmath::vec3 normal;
		ObjectMaterial material;
		// Maps uv texture
		std::function<spaint::Color(double, double)> uv_map = [](double u, double v) -> spaint::Color { return spaint::Color(0); };
		
		UVPlane(const cppmath::vec3& location, const cppmath::vec3& normal) {
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
			if ((ndd < 0 ? -ndd : ndd) <= 10e-8)
				return tm;
			
			tm.distance = cppmath::vec3::dot(tm.normal, location - r.origin()) / ndd;
			
			if (tm.distance < 10e-8)
				return tm;
			
			tm.location = r.point_at_parameter(tm.distance);
			tm.hit = 1;
			
			return tm;
		};
		
		ObjectMaterial get_material(const cppmath::vec3& point) {
			cppmath::vec3 u = cppmath::vec3(normal.y, -normal.x, 0).norm();
			cppmath::vec3 v = cppmath::vec3::cross(normal, u);
			double u_coord = cppmath::vec3::dot(u, point);
			double v_coord = cppmath::vec3::dot(v, point);
			
			ObjectMaterial mat = material;
			mat.color = uv_map(u_coord, v_coord);
			
			return mat;
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

		// Minimal ray power to keep running
		double MIN_RAY_POWER = 10e-3;
		// Length of shift when relaunching ray & avoiding intersection with the same plane
		double RAY_SHIFT = 10e-6;
		// Set soft shadows enabled
		bool soft_shadows = 0;
		// Diffuse scale for light in shadow of an object
		double soft_shadows_scale = 0.5;
		// Enable diffuse light calculations
		bool diffuse_light = 0;
		// Diffuse light calculations scale
		double diffuse_light_scale = 0.5;
		// Enable random ray tracing in diffuse light
		bool random_diffuse_ray = 0;
		// Number of random diffuse rays
		int random_diffuse_count = 1;
		// Number of horisontal angle steps in non-random diffuse ray tracing
		int horisontal_diffuse_count = 16;
		// Number of vertical angle steps in non-random diffuse ray tracing
		int vertical_diffuse_count = 4;
		// Maximal ray recursion depth
		int MAX_RAY_DEPTH = -1;
	
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
		HitManifold shoot(const ray& r, int ignored_id = -1, int ray_depth = 1) {
			// ignored_id is used during reflection calculations to 
			//  avoid object trace itself forever till stack overflow.
			
			if (r.power < MIN_RAY_POWER)
				return HitManifold();
			
			// Result
			HitManifold hitm;
			
			if (MAX_RAY_DEPTH != -1 && ray_depth > MAX_RAY_DEPTH)
				return hitm;
			
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
				return hitm;
			
			closest_hit = hits[closest];
			closest_object = objects[closest];
			
			hitm.hit = 1;
			
			// If object is emitting light in this point, apply light color with luminosity scale
			ObjectMaterial closest_material = closest_object->get_material(closest_hit.location);
			
			// if object has no surface visible, shoot ray over it
			if (!closest_material.surface_visible) {
				ray l(closest_hit.location + RAY_SHIFT * r.direction(), r.direction(), r.power);
				// Throw new ray & ignore current traced object
				return shoot(l, ignored_id, ray_depth); 
			}
			
			// Calculate luminosity if object emmit light
			if (closest_material.luminosity) {
				hitm.color = closest_material.color;
				hitm.color.scale(closest_material.luminosity);
				hitm.color.scale(-cppmath::vec3::cos_between(closest_hit.normal, r.direction()));
			}
			
			// If traced object deffuses light from other light sources, than
			//  calculate amount of light produced by light sources and
			//  reflected from other light-diffusing objects.
			if (closest_material.diffuse > 0) {
				// Total light emitted by light emitting objects
				spaint::Color lighting;
			
				// Calculate ambient lighting from light emitting objects
				for (int i = 0; i < objects.size(); ++i) {
					if (i == closest)
						continue;
					
					ray l(closest_hit.location, (objects[i]->get_center() - closest_hit.location).norm());
					TraceManifold trm = objects[i]->hit(l);
					
					if (!trm.hit)
						continue;
					
					// Hard shadows or diffuse shadows
					if (diffuse_light && !soft_shadows) {
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
					}
					
					ObjectMaterial tmat = objects[i]->get_material(trm.location);
					
					// Calculate amount of light emitted by objects[i]
					if (tmat.luminosity > 0) {
						spaint::Color lumine = tmat.color;
						lumine.scale(closest_material.color);
						lumine.scale(tmat.luminosity);
						lumine.scale(closest_material.diffuse);
						lumine.scale(-cppmath::vec3::cos_between(closest_hit.normal, trm.normal));
						
						if (soft_shadows) {
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
									double cs = cppmath::vec3::cos_between(l.direction(), trmo.normal);
									cs = cs < 0 ? -cs : cs;
									cs *= cppmath::vec3::cos_between(l.direction(), (objects[i]->get_center() - l.origin()).norm());
									cs *= (1.0 - mato.refract);
									cs *= (1.0 - soft_shadows_scale);
									shadow_cos -= cs;
								}
							}
							
							// Apply light in shadow
							shadow_cos = shadow_cos < 0 ? 0.0 : (shadow_cos > 1.0 ? 1.0 : shadow_cos);
							lumine.scale(shadow_cos);
						}
						
						// Apply color affect on surface
						lighting += lumine;
					}
				}
				
				// Apply lighting in shadows and direct light
				hitm.color += lighting;
				
				// Cast rays in parametrized directions to collect summary diffuse
				if (diffuse_light) {					
					if (!random_diffuse_ray) {
						// Shoot rays in any directions to calculate diffuse light amount from reflections of other objects
						double u_step = 3.14159265358979323846 * 2.0 / (double) horisontal_diffuse_count;
						double v_step = 3.14159265358979323846 / (double) vertical_diffuse_count;
						
						// Move rotation
						double u = 0.0;
						double v = 0.0;
						
						// Create new temp ray with less power, a bit shifted off surface
						ray l(closest_hit.location + RAY_SHIFT * closest_hit.normal, closest_hit.normal, r.power * closest_material.diffuse * diffuse_light_scale);
						
						cppmath::vec3 E = cppmath::vec3(1.0, 1.0, 1.0);
						cppmath::vec3 T = cppmath::vec3::cross(E, closest_hit.normal);
						E = cppmath::vec3::cross(T, closest_hit.normal);
						E.norm();
						T.norm();
						
						spaint::Color summary_diffuse;
						int total_diffuse = 0;
						
						for (int vi = 0; vi < vertical_diffuse_count; ++vi)
							for (int ui = 0; ui < horisontal_diffuse_count; ++ui) {
								l.B = closest_hit.normal;
								l.B = cppmath::vec3::rotateAroundVector(l.B, T, v);
								l.B = cppmath::vec3::rotateAroundVector(l.B, closest_hit.normal, u);
								
								double cos_ln = cppmath::vec3::cos_between(l.direction(), closest_hit.normal);
								if (cos_ln <= 0)
									continue;
								
								HitManifold hitd = shoot(l, ignored_id, ray_depth + 1);
								if (!hitd.hit)
									continue;
								
								++total_diffuse;
								
								/*spaint::Color dif_res = hitd.color;
								dif_res.scale(closest_material.color);
								dif_res.scale(cppmath::vec3::cos_between(l.direction(), -r.direction()));
								dif_res.scale(cos_ln);
								summary_diffuse += dif_res;*/
								
								spaint::Color dif_res = hitd.color;
								dif_res.scale_off_range(closest_material.color);
								double cos_lmr = cppmath::vec3::cos_between(l.direction(), -r.direction());
								dif_res.scale_off_range(cos_lmr);
								dif_res.scale_off_range(cos_ln);
								summary_diffuse.add_off_range(dif_res);
							
								u += u_step;
								v += v_step;
							}
							
						// Scale color & apply	
						summary_diffuse.scale_off_range(closest_material.diffuse);
						summary_diffuse.scale_off_range(diffuse_light_scale);
						summary_diffuse.scale(1.0 / (double) total_diffuse);
						hitm.color += summary_diffuse;
					} else {
						// Shoot rays in any directions to calculate diffuse light amount from reflections of other objects
						double u_range = 3.14159265358979323846 * 2.0;
						double v_range = 3.14159265358979323846;
						
						// Move rotation
						double u = 0.0;
						double v = 0.0;
						
						// Create new temp ray with less power, a bit shifted off surface
						ray l(closest_hit.location + RAY_SHIFT * closest_hit.normal, closest_hit.normal, r.power * closest_material.diffuse * diffuse_light_scale);
						
						cppmath::vec3 E = cppmath::vec3(1.0, 1.0, 1.0);
						cppmath::vec3 T = cppmath::vec3::cross(E, closest_hit.normal);
						E = cppmath::vec3::cross(T, closest_hit.normal);
						E.norm();
						T.norm();
						
						spaint::Color summary_diffuse;
						int total_diffuse = 0;
						
						for (int i = 0; i < random_diffuse_count; ++i) {
							u = ((double) rand() / (RAND_MAX)) * u_range;
							v = ((double) rand() / (RAND_MAX)) * v_range;
							
							l.B = closest_hit.normal;
							l.B = cppmath::vec3::rotateAroundVector(l.B, T, v);
							l.B = cppmath::vec3::rotateAroundVector(l.B, closest_hit.normal, u);
							
							double cos_ln = cppmath::vec3::cos_between(l.direction(), closest_hit.normal);
							if (cos_ln <= 0)
								continue;
							
							HitManifold hitd = shoot(l, ignored_id, ray_depth + 1);
							if (!hitd.hit)
								continue;
							
							++total_diffuse;
							
							spaint::Color dif_res = hitd.color;
							dif_res.scale_off_range(closest_material.color);
							dif_res.scale_off_range(cppmath::vec3::cos_between(l.direction(), -r.direction()));
							dif_res.scale_off_range(cos_ln);
							summary_diffuse.add_off_range(dif_res);
						}
							
						// Scale color & apply	
						summary_diffuse.scale_off_range(closest_material.diffuse);
						summary_diffuse.scale_off_range(diffuse_light_scale);
						summary_diffuse.scale(1.0 / (double) total_diffuse);
						hitm.color += summary_diffuse;
					}
				}
			}
			
			// Calculate reflections
			if (closest_material.reflect > 0) {
				cppmath::vec3 direction = cppmath::vec3::reflect(r.direction(), closest_hit.normal);
				ray l(closest_hit.location + RAY_SHIFT * direction, direction);
				l.power = r.power * closest_material.reflect;
				
				HitManifold hitr = shoot(l, ignored_id, ray_depth + 1);
				if (hitr.hit) {
					hitr.color.scale(closest_material.reflect);
					hitm.color += hitr.color;
				}
			}
			
			// Calculate refractions
			if (closest_material.refract > 0) {
				cppmath::vec3 direction = cppmath::vec3::refract(r.direction(), closest_hit.normal, closest_material.refract_val);
				ray l(closest_hit.location + RAY_SHIFT * direction, direction);
				l.power = r.power * closest_material.refract;
				
				HitManifold hitr = shoot(l, ignored_id, ray_depth + 1);
				if (hitr.hit) {
					hitr.color.scale(closest_material.refract);
					hitm.color += hitr.color;
				}
			}
			
			hitm.color.scale(r.power);
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