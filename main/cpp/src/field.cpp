#include "field.h"
#include "mesh.h"

extern mesh soccar_corner;
extern mesh soccar_goal;
extern mesh soccar_ramps_1;
extern mesh soccar_ramps_2;

extern mesh hoops_corner;
extern mesh hoops_ramps_1;
extern mesh hoops_ramps_2;
extern mesh hoops_net;
extern mesh hoops_rim;

extern mesh dropshot;

mat3 flip_x = {
  {-1.0f, 0.0f, 0.0f},
  { 0.0f, 1.0f, 0.0f},
  { 0.0f, 0.0f, 1.0f}
};

mat3 flip_y = {
  {1.0f,  0.0f, 0.0f},
  {0.0f, -1.0f, 0.0f},
  {0.0f,  0.0f, 1.0f}
};

bvh< tri > Field::collision_mesh;
std::vector < Field::wall > Field::walls;
std::vector < tri > Field::triangles;
std::vector < aabb > Field::obstacles;
float Field::r;
std::string Field::mode = std::string("Uninitialized");

void Field::initialize_soccar() {
	mesh soccar = {
	  soccar_corner,
	  soccar_corner.transform(flip_x),
	  soccar_corner.transform(flip_y),
	  soccar_corner.transform(dot(flip_x, flip_y)),
	  soccar_goal.translate(vec3{0.0f, -5120.0f, 0.0f}),
	  soccar_goal.translate(vec3{0.0f, -5120.0f, 0.0f}).transform(flip_y),
	  soccar_ramps_1,
	  soccar_ramps_1.transform(flip_x),
	  soccar_ramps_2,
	  soccar_ramps_2.transform(flip_x)
	};
	triangles = soccar.to_triangles();
	collision_mesh = bvh<tri>(triangles);

	walls = std::vector<wall>(2 + 8);

	// floor
	walls[0] = wall{ vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, true };

	// ceiling
	walls[1] = wall{ vec3{0.0f, 0.0f, 2048.0f}, vec3{0.0f, 0.0f, -1.0f}, true };

	// X-facing walls
	walls[2] = wall{ vec3{-4096.0f, 0.0f, 1024.0f}, vec3{ 1.0f, 0.0f, 0.0f}, true };
	walls[3] = wall{ vec3{ 4096.0f, 0.0f, 1024.0f}, vec3{-1.0f, 0.0f, 0.0f}, true };

	// Y-facing walls
	walls[4] = wall{ vec3{0.0f, -5120.0f, 1024.0f}, vec3{0.0f,  1.0f, 0.0f}, false };
	walls[5] = wall{ vec3{0.0f,  5120.0f, 1024.0f}, vec3{0.0f, -1.0f, 0.0f}, false };

	// diagonally-facing walls
	float s = sqrt(2.0f) / 2.0f;
	walls[6] = wall{ vec3{-3520.0f, -4543.76f, 1024.0f}, vec3{ s,  s, 0.0f}, false };
	walls[7] = wall{ vec3{ 3520.0f, -4543.76f, 1024.0f}, vec3{-s,  s, 0.0f}, false };
	walls[8] = wall{ vec3{-3520.0f,  4543.76f, 1024.0f}, vec3{ s, -s, 0.0f}, false };
	walls[9] = wall{ vec3{ 3520.0f,  4543.76f, 1024.0f}, vec3{-s, -s, 0.0f}, false };

	obstacles = std::vector<aabb>(2);

	obstacles[0] = aabb(-893.0f, 5100.0f, 25.0f, 893.0f, 5400.0f, 642.0f);
	obstacles[1] = aabb(-893.0f, -5400.0f, 25.0f, 893.0f, -5100.0f, 642.0f);

	r = 250.0f;

	mode = std::string("soccar");

}

void Field::initialize_hoops() {

	float scale = 0.9f;
	float y_offset = 431.664f;

	mat3 S = {
	  {scale, 0.0f, 0.0f},
	  {0.0f, scale, 0.0f},
	  {0.0f, 0.0f, scale},
	};

	vec3 dy = vec3{ 0.0f, y_offset, 0.0f };

	mesh transformed_hoops_net = hoops_net.transform(S).translate(dy);
	mesh transformed_hoops_rim = hoops_rim.transform(S).translate(dy);

	mesh hoops = {
	  hoops_corner,
	  hoops_corner.transform(flip_x),
	  hoops_corner.transform(flip_y),
	  hoops_corner.transform(dot(flip_x, flip_y)),
	  transformed_hoops_net,
	  transformed_hoops_net.transform(flip_y),
	  transformed_hoops_rim,
	  transformed_hoops_rim.transform(flip_y),
	  hoops_ramps_1,
	  hoops_ramps_1.transform(flip_x),
	  hoops_ramps_2,
	  hoops_ramps_2.transform(flip_y)
	};

	triangles = hoops.to_triangles();
	collision_mesh = bvh<tri>(triangles);

	// TODO
	walls = std::vector<wall>(2 + 8);

	// floor
	walls[0] = wall{ vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 0.0f, 1.0f}, true };

	// ceiling
	walls[1] = wall{ vec3{0.0f, 0.0f, 1820.0f}, vec3{0.0f, 0.0f, -1.0f}, true };

	// X-facing walls
	walls[2] = wall{ vec3{-2966.6f, 0.0f, 910.0f}, vec3{ 1.0f, 0.0f, 0.0f}, true };
	walls[3] = wall{ vec3{ 2966.6f, 0.0f, 910.0f}, vec3{-1.0f, 0.0f, 0.0f}, true };

	// Y-facing walls
	walls[4] = wall{ vec3{0.0f, -3581.0f, 910.0f}, vec3{0.0f,  1.0f, 0.0f}, true };
	walls[5] = wall{ vec3{0.0f,  3581.0f, 910.0f}, vec3{0.0f, -1.0f, 0.0f}, true };

	// diagonally-facing walls
	float s = sqrt(2.0f) / 2.0f;
	walls[6] = wall{ vec3{-2582.35f, -3200.0f, 910.0f}, vec3{ s,  s, 0.0f}, false };
	walls[7] = wall{ vec3{ 2582.35f, -3200.0f, 910.0f}, vec3{-s,  s, 0.0f}, false };
	walls[8] = wall{ vec3{-2582.35f,  3200.0f, 910.0f}, vec3{ s, -s, 0.0f}, false };
	walls[9] = wall{ vec3{ 2582.35f,  3200.0f, 910.0f}, vec3{-s, -s, 0.0f}, false };


	obstacles = std::vector<aabb>(4);

	// places where the rim meets the backboard 
	// TODO
	obstacles[0] = aabb{ 650.0f, -3600.0f, 300.0f,  800.0f, -3500.0f, 400.0f };
	obstacles[1] = aabb{ -800.0f, -3600.0f, 300.0f, -650.0f, -3500.0f, 400.0f };
	obstacles[2] = aabb{ 650.0f,  3500.0f, 300.0f,  800.0f,  3600.0f, 400.0f };
	obstacles[3] = aabb{ -800.0f,  3500.0f, 300.0f, -650.0f,  3600.0f, 400.0f };

	r = 250.0f;

	mode = std::string("hoops");

}

void Field::initialize_dropshot() {

	float scale = 0.393f;
	float z_offset = -207.565f;

	mat3 R = axis_rotation(vec3{ 0.0f, 0.0f, 0.52359877559f });

	mat3 S = {
	  {scale, 0.0f, 0.0f},
	  {0.0f, scale, 0.0f},
	  {0.0f, 0.0f, scale},
	};

	vec3 dz = vec3{ 0.0f, 0.0f, z_offset };

	mesh transformed_dropshot = dropshot.transform(dot(R, S)).translate(dz);

	triangles = transformed_dropshot.to_triangles();
	collision_mesh = bvh<tri>(triangles);

	walls = std::vector<wall>(2 + 6);

	// floor
	walls[0] = wall{ vec3{0.0f, 0.0f, 2.0f}, vec3{0.0f, 0.0f, 1.0f}, true };

	// ceiling
	walls[1] = wall{ vec3{0.0f, 0.0f, 2020.0f}, vec3{0.0f, 0.0f, -1.0f}, true };

	// Walls
	vec3 p = vec3{ 0.0f, 11683.6f * scale, 2768.64f * scale - z_offset };
	vec3 n = vec3{ 0.0f, -1.0f, 0.0f };
	R = axis_rotation(vec3{ 0.0f, 0.0f, 1.047197551196598f });
	for (int i = 2; i < 8; i++) {
		walls[i] = wall{ p, n, true };
		p = dot(R, p);
		n = dot(R, n);
	}

	// dropshot doesn't have any obstacles
	obstacles = std::vector<aabb>(0);

	r = 350.0f;

	mode = std::string("dropshot");

}

ray Field::raycast_any(const ray & r) {
	return collision_mesh.raycast_any(r);
}

ray Field::snap(vec3 p) {

	int min_id = -1;
	float min_distance = 1.0f;
	std::vector < int > violations;
	std::vector < float > distances(walls.size());

	// iterate over the walls and find constraints
	// that are being violated, and keep track of
	// the info related to the largest violation
	for (int i = 2; i < walls.size(); i++) {

		vec3 v = walls[i].p;
		vec3 n = walls[i].n;

		distances[i] = dot(p - v, n) - r;

		if (distances[i] < 0.0f) {
			violations.push_back(i);
		}

		if (distances[i] < min_distance) {
			min_distance = distances[i];
			min_id = i;
		}

	}

	// q is a position in world space that helps us define 
	// the snapped point normal, and move the snapped point
	// onto the cylindrical sections where planes meet
	vec3 q = p;

	switch (violations.size()) {

		// if there are no violations, then the point is on the interior
		// of the field and won't be snapped to any part of the boundary
	case 0:
		// do nothing
		break;

		// if the point is only out of bounds with respect to one halfspace,
		// then we can just push it back along that halfspace normal 
	case 1:
		q = p - min_distance * walls[min_id].n;
		break;

		// if the point is only out of bounds with respect to exactly
		// two halfspaces, things are more interesting. 
	case 2: {
		float d1 = distances[violations[0]];
		vec2 v1 = vec2(walls[violations[0]].p);
		vec2 n1 = vec2(walls[violations[0]].n);

		float d2 = distances[violations[1]];
		vec2 v2 = vec2(walls[violations[1]].p);
		vec2 n2 = vec2(walls[violations[1]].n);

		mat2 invA = inv(mat2{
		  {n1[0], n2[0]},
		  {n1[1], n2[1]}
			});

		vec2 o = dot(vec2{ dot(n1, v1) + r, dot(n2, v2) + r }, invA);

		vec2 g = dot(invA, vec2(p) - o);

		if (g[0] < 0 && g[1] > 0) q = p - d1 * n1;
		if (g[0] > 0 && g[1] < 0) q = p - d2 * n2;
		if (g[0] < 0 && g[1] < 0) q = vec3(o);
	} break;

		// if the point is only out of bounds with respect to exactly
		// three halfspaces, then only the largest violation matters,
		// and enforcement is similar to the single violation case.
	case 3:
		q = p - min_distance * walls[min_id].n;
		break;

		// for the given shapes of soccar, hoops, and dropshop maps,
		// no point should be able to violate more than 3 halfspace
		// constraints. 
	default:
		std::cout << "this should never happen" << std::endl;
		break;
	}

	// handling the floor and ceiling is much easier, since the map is prismatic
	// wall[0] is the floor
	// wall[1] is the ceiling
	q[2] = clip(p[2], walls[0].p[2] + r, walls[1].p[2] - r);

	return ray{
	  q + r * normalize(p - q), // the closest point on the boundary
	  normalize(q - p)          // the idealized surface normal for that point
	};

}

ray Field::collide(const obb & o) {
	return ray{ vec3{0.0f, 0.0f, 0.0f}, vec3{0.0f, 0.0f, 0.0f} };
}

ray Field::collide(const sphere & s) {

	auto contact_point = ray{ vec3{0.0, 0.0, 0.0}, vec3{0.0f, 0.0f, 0.0f} };

	if (mode == std::string("Uninitialized")) {
		std::cout << "Attempting to query an uninitialized Field." << std::endl;
	}
	else {
		float count = 0.0f;

		for (const auto w : walls) {
			float separation = dot(s.center - w.p, w.n);
			if (separation <= s.radius && w.collidable) {
				count++;
				contact_point.start += s.center - separation * w.n;
				contact_point.direction += w.n * (s.radius - separation);
			}
		}

		auto tris_hit = collision_mesh.intersect(s);

		for (const auto id : tris_hit) {

			vec3 p = triangles[id].center();
			vec3 n = triangles[id].unit_normal();

			float separation = dot(s.center - p, n);
			if (separation <= s.radius) {
				count++;
				contact_point.start += s.center - separation * n;
				contact_point.direction += n * (s.radius - separation);
			}
		}

		if (count > 0) {
			contact_point.start /= count;
			contact_point.direction = normalize(contact_point.direction);
		}
	}

	return contact_point;
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
void init_field(pybind11::module & m) {
	pybind11::class_<Field>(m, "Field")
		.def_readonly_static("mode", &Field::mode)
		.def_readonly_static("walls", &Field::walls)
		.def_readonly_static("triangles", &Field::triangles)
		.def_static("snap", &Field::snap)
		.def_static("collide", static_cast<ray(*)(const obb &)>(&Field::collide))
		.def_static("collide", static_cast<ray(*)(const sphere &)>(&Field::collide))
		.def_static("raycast_any", &Field::raycast_any);
}
#endif

