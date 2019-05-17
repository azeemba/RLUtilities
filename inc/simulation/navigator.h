#pragma once

#include "simulation/car.h"
#include "simulation/input.h"
#include "simulation/curve.h"

#include "linear_algebra/math.h"

#include "misc/graph.h"

#include <array>
#include <tuple>
#include <string>
#include <vector>

class Navigator {

 public:
  Car & car;

  Navigator(Car & c);

  void analyze_surroundings(float time_budget);
  Curve path_to(vec3, vec3, float);

  static void init_statics(
    std::vector< Graph::edge > nav_edges,
    std::vector< vec3 > nav_nodes,
    std::vector< vec3 > nav_normals,
    std::vector< int > parameters,
    std::vector< float > times,
    std::vector< uint32_t > paths);

  static std::vector < vec3 > navigation_nodes;
  static std::vector < vec3 > navigation_tangents;
  static std::vector < vec3 > navigation_normals;

 private:

  Curve lut_path_to(vec3, vec3, float);
  Curve navmesh_path_to(vec3, vec3, float);

  static float scale;
  static int nx, ntheta, nv;

  static uint32_t to_id(int, int, int, int);
  static std::tuple < int, int, int, int > from_id(uint32_t);

  static std::array < int, 3 > strides;
  static std::vector < float > LUT_times;
  static std::vector < uint32_t > LUT_paths;

  static Graph navigation_graph;
  static std::vector < vec3 > directions;

  int which_node;
  int which_direction;

  std::vector < int > navigation_frontier;
  std::vector < int > navigation_paths;
  std::vector < float > navigation_times;

};
