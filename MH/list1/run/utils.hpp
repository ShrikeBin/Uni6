#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <numeric>   
#include <algorithm>
#include <random>
#include <limits>

struct City {
  uint32_t id;
  double x;
  double y;
};

struct LocalSearchResult {
  std::vector<uint_fast32_t> tour;
  uint_fast32_t distance;
  uint_fast32_t improvement_steps;
};

struct MSTresult {
  std::vector<std::pair<int, int>> edges;
  int total_weight;
};

std::vector<City> read_data(const std::string& filename);

std::vector<uint_fast32_t> random_tour(int num_cities, std::mt19937& GEN);

uint_fast32_t discrete_distance(const City& a, const City& b);
uint_fast32_t tour_distance(const std::vector<uint_fast32_t>& tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

int_fast32_t diff_after_invert(const std::vector<uint_fast32_t>& tour, uint_fast32_t i, uint_fast32_t j, const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

LocalSearchResult local_search_naive(std::vector<uint_fast32_t> initial_tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix);
LocalSearchResult local_search_n_random(std::vector<uint_fast32_t> initial_tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix, std::mt19937& GEN);

MSTresult prim_mst(const std::vector<City>& cities, const std::vector<std::vector<uint_fast32_t>>& dist_matrix);
void dfs(int u, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::vector<int>& tour);
std::vector<uint_fast32_t> mst_to_tsp(const std::vector<std::pair<int, int>>& mst_edges, int num_vertices, int start_vertex);

#endif // UTILS_HPP