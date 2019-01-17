#pragma once

#include <vector>
#include <string>

#include "bvh.h"
#include "geometry.h"

class Field {

	struct wall {
		vec3 p;
		vec3 n;
		bool collidable;
	};

public:
	static bvh<tri> collision_mesh;
	static std::vector<wall> walls;
	static std::vector<tri> triangles;
	static std::vector<aabb> obstacles;
	static float r;
	static std::string mode;

	static void initialize_soccar();
	static void initialize_hoops();
	static void initialize_dropshot();

	static ray snap(vec3 p);
	static ray collide(const obb & o);
	static ray collide(const sphere & o);

	static ray raycast_any(const ray &);

};
