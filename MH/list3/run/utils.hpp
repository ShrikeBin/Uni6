#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <limits>

// ─── Basic types ──────────────────────────────────────────────────────────────

struct City {
    uint32_t id;
    double x;
    double y;
};

// ─── TSP helpers ──────────────────────────────────────────────────────────────

std::vector<City>           read_data(const std::string& filename);
std::vector<uint_fast32_t>  random_tour(int num_cities, std::mt19937& GEN);
uint_fast32_t               discrete_distance(const City& a, const City& b);

uint_fast32_t tour_distance(
    const std::vector<uint_fast32_t>& tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

int_fast32_t diff_after_invert(
    const std::vector<uint_fast32_t>& tour,
    uint_fast32_t i,
    uint_fast32_t j,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix);

// ─── Previous algorithm results (kept for memetics reuse) ─────────────────────

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
    uint_fast32_t sample_size);

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
    uint_fast32_t sample_size,
    uint_fast32_t max_iters
);

// ─── GA types ─────────────────────────────────────────────────────────────────

enum class CrossoverType {
    OX,   // Order Crossover
    PMX   // Partially Mapped Crossover
};

struct GAParams {
    uint_fast32_t pop_size          = 100;
    uint_fast32_t max_generations   = 1000;
    uint_fast32_t no_improve_limit  = 150;
    double        cross_prob        = 0.85;
    double        mut_prob          = 0.05;
    uint_fast32_t tournament_k      = 5;
    CrossoverType crossover_type    = CrossoverType::OX;
    bool          memetic           = false;   // apply 2-opt after mutation
    uint_fast32_t memetic_2opt_iters = 0;      // 0 = use sqrt(n) samples
};

struct GAResult {
    std::vector<uint_fast32_t> best_tour;
    uint_fast32_t              best_distance;
    uint_fast32_t              generations_run;
};

// ─── Island GA ────────────────────────────────────────────────────────────────

struct IslandParams {
    uint_fast32_t num_islands        = 8;
    uint_fast32_t migration_interval = 20;   // migrate every N generations
    uint_fast32_t migration_size     = 4;    // how many individuals to send
    GAParams      island_ga_params;          // params for each island's GA
};

struct IslandResult {
    std::vector<uint_fast32_t> best_tour;
    uint_fast32_t              best_distance;
};

IslandResult island_genetic_algorithm(
    uint_fast32_t n,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    const IslandParams& params);

// ─── GA crossover operators ───────────────────────────────────────────────────

// Order Crossover (OX)
std::pair<std::vector<uint_fast32_t>, std::vector<uint_fast32_t>>
crossover_OX(
    const std::vector<uint_fast32_t>& p1,
    const std::vector<uint_fast32_t>& p2,
    std::mt19937& GEN);

// Partially Mapped Crossover (PMX)
std::pair<std::vector<uint_fast32_t>, std::vector<uint_fast32_t>>
crossover_PMX(
    const std::vector<uint_fast32_t>& p1,
    const std::vector<uint_fast32_t>& p2,
    std::mt19937& GEN);

// ─── GA mutation ──────────────────────────────────────────────────────────────

// Reverse a random sub-segment (2-opt move)
void mutate_invert(std::vector<uint_fast32_t>& tour, std::mt19937& GEN);

// ─── GA selection ─────────────────────────────────────────────────────────────

// Tournament selection — returns index into population
uint_fast32_t tournament_select(
    const std::vector<uint_fast32_t>& fitnesses,
    uint_fast32_t k,
    std::mt19937& GEN);

// ─── Main GA entry point ──────────────────────────────────────────────────────

GAResult genetic_algorithm(
    uint_fast32_t n,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    const GAParams& params);

#endif // UTILS_HPP