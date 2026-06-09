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

std::vector<City> read_data(const std::string& filename);

std::vector<uint_fast32_t> random_tour(int num_cities, std::mt19937& GEN);

uint_fast32_t discrete_distance(const City& a, const City& b);

uint_fast32_t tour_distance(
  const std::vector<uint_fast32_t>& tour, 
  const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

int_fast32_t diff_after_invert(
  const std::vector<uint_fast32_t>& tour, 
  uint_fast32_t i, 
  uint_fast32_t j, 
  const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

struct TabuResult {
  std::vector<uint_fast32_t> tour;
  uint_fast32_t distance;
  uint_fast32_t iters;
  uint_fast32_t max_iters_no_improve;
  uint_fast32_t tabu_cooldown;
  uint_fast32_t sample_size;
};

TabuResult tabu_search(
  std::vector<uint_fast32_t> initial_tour,
  const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
  std::mt19937& GEN,
  uint_fast32_t tabu_cooldown,
  uint_fast32_t max_iter_no_improve,
  uint_fast32_t sample_size
);

struct LocalSearchResult {
  std::vector<uint_fast32_t> tour;
  uint_fast32_t distance;
  uint_fast32_t iters;
  uint_fast32_t sample_size;
};

LocalSearchResult local_search(
  std::vector<uint_fast32_t> initial_tour, 
  const std::vector<std::vector<uint_fast32_t>>& dist_matrix, 
  std::mt19937& GEN,
  uint_fast32_t sample_size
);

#endif // UTILS_HPP