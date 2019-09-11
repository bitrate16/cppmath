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
			for o in objects
				ray r(closest_hit.location, (o.center() - closest_hit.location))
				Manifold hitlight = o.hit(r)
				Color addition = Color.min(o.color * o.luminosity, closest_hit.color * reflect)
				lighting += addition
			
			// Calculate light
			// lighten color = max(texture_color, light_color)
			
			
			// Calculate refletions, refractions & lights
			return hitm;
		};
	};
}