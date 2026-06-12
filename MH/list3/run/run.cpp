#include "utils.hpp"

#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

// Global random generator
std::random_device RD;
std::mt19937 GEN(RD());

void run_exp_for_file(const std::vector<City>& cities, const std::string& filename) {
    bool PRINT_TOURS    = false;
    bool DEBUG_ITER     = false;
    uint_fast32_t n     = cities.size();

    std::cout << "Loaded file: " << filename << " | Number of vertices (n): " << n << "\n\n";

    if (n >= 2000) return;

    // Build distance matrix
    std::vector<std::vector<uint_fast32_t>> dist_matrix(n, std::vector<uint_fast32_t>(n));
    for (uint_fast32_t i = 0; i < n; ++i)
        for (uint_fast32_t j = 0; j < n; ++j)
            dist_matrix[i][j] = discrete_distance(cities[i], cities[j]);

    constexpr uint_fast32_t REPETITIONS = 10;

    // ── Shared base params ────────────────────────────────────────────────────
    GAParams base;
    base.pop_size         = 100;
    base.max_generations  = 1000;
    base.no_improve_limit = 150;
    base.cross_prob       = 0.85;
    base.mut_prob         = 0.05;
    base.tournament_k     = 5;
    base.memetic          = false;
    base.memetic_2opt_iters = 0;   // 0 = use sqrt(n) internally

    // Helper: print common GA param block
    auto print_params = [&](const GAParams& p) {
        std::cout << "Population size:    " << p.pop_size         << "\n"
                  << "Max generations:    " << p.max_generations  << "\n"
                  << "No-improve limit:   " << p.no_improve_limit << "\n"
                  << "Crossover prob:     " << p.cross_prob       << "\n"
                  << "Mutation prob:      " << p.mut_prob         << "\n"
                  << "Tournament k:       " << p.tournament_k     << "\n";
    };

    // Helper: print result summary
    auto print_results = [&](uint_fast32_t best, double avg, double avg_gen,
                             const std::vector<uint_fast32_t>& tour) {
        std::cout << "Best distance:      " << best    << "\n"
                  << "Average distance:   " << avg     << "\n"
                  << "Avg generations:    " << avg_gen << "\n";
        if (PRINT_TOURS) {
            std::cout << "Best tour: ";
            for (auto c : tour) std::cout << c << ">";
            std::cout << "\n";
        }
    };

    // ══════════════════════════════════════════════════════════════════════════
    // Experiment 1: GA — OX crossover
    // ══════════════════════════════════════════════════════════════════════════
    {
        GAParams params      = base;
        params.crossover_type = CrossoverType::OX;

        std::vector<uint_fast32_t> best_tour;
        uint_fast32_t best_dist = std::numeric_limits<uint_fast32_t>::max();
        uint_fast64_t sum_dist  = 0;
        uint_fast64_t sum_gens  = 0;

        for (uint_fast32_t rep = 1; rep <= REPETITIONS; ++rep) {
            if (DEBUG_ITER)
                std::cout << "OX rep " << rep << "/" << REPETITIONS << "\r" << std::flush;

            GAResult r = genetic_algorithm(n, dist_matrix, GEN, params);
            sum_dist  += r.best_distance;
            sum_gens  += r.generations_run;
            if (r.best_distance < best_dist) {
                best_dist = r.best_distance;
                best_tour = r.best_tour;
            }
        }

        std::cout << "\n====================[GA — OX crossover]=====================\n"
                  << "File: " << filename << " (" << n << ")\n";
        print_params(params);
        print_results(best_dist,
                      static_cast<double>(sum_dist) / REPETITIONS,
                      static_cast<double>(sum_gens)  / REPETITIONS,
                      best_tour);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // Experiment 2: GA — PMX crossover
    // ══════════════════════════════════════════════════════════════════════════
    {
        GAParams params       = base;
        params.crossover_type = CrossoverType::PMX;

        std::vector<uint_fast32_t> best_tour;
        uint_fast32_t best_dist = std::numeric_limits<uint_fast32_t>::max();
        uint_fast64_t sum_dist  = 0;
        uint_fast64_t sum_gens  = 0;

        for (uint_fast32_t rep = 1; rep <= REPETITIONS; ++rep) {
            if (DEBUG_ITER)
                std::cout << "PMX rep " << rep << "/" << REPETITIONS << "\r" << std::flush;

            GAResult r = genetic_algorithm(n, dist_matrix, GEN, params);
            sum_dist  += r.best_distance;
            sum_gens  += r.generations_run;
            if (r.best_distance < best_dist) {
                best_dist = r.best_distance;
                best_tour = r.best_tour;
            }
        }

        std::cout << "\n====================[GA — PMX crossover]=====================\n"
                  << "File: " << filename << " (" << n << ")\n";
        print_params(params);
        print_results(best_dist,
                      static_cast<double>(sum_dist) / REPETITIONS,
                      static_cast<double>(sum_gens)  / REPETITIONS,
                      best_tour);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // Experiment 3: Memetic GA — OX + 2-opt polish after every mutation
    // ══════════════════════════════════════════════════════════════════════════
    {
        GAParams params         = base;
        params.crossover_type   = CrossoverType::OX;
        params.memetic          = true;
        params.memetic_2opt_iters = static_cast<uint_fast32_t>(std::sqrt(n));
        params.no_improve_limit = 80;   // 2-opt does heavy lifting, converges faster

        std::vector<uint_fast32_t> best_tour;
        uint_fast32_t best_dist = std::numeric_limits<uint_fast32_t>::max();
        uint_fast64_t sum_dist  = 0;
        uint_fast64_t sum_gens  = 0;

        for (uint_fast32_t rep = 1; rep <= REPETITIONS; ++rep) {
            if (DEBUG_ITER)
                std::cout << "Memetic rep " << rep << "/" << REPETITIONS << "\r" << std::flush;

            GAResult r = genetic_algorithm(n, dist_matrix, GEN, params);
            sum_dist  += r.best_distance;
            sum_gens  += r.generations_run;
            if (r.best_distance < best_dist) {
                best_dist = r.best_distance;
                best_tour = r.best_tour;
            }
        }

        std::cout << "\n====================[Memetic GA — OX + 2-opt]=====================\n"
                  << "File: " << filename << " (" << n << ")\n";
        print_params(params);
        std::cout << "2-opt samples:      " << params.memetic_2opt_iters << "\n";
        print_results(best_dist,
                      static_cast<double>(sum_dist) / REPETITIONS,
                      static_cast<double>(sum_gens)  / REPETITIONS,
                      best_tour);
    }

    // ══════════════════════════════════════════════════════════════════════════
    // Experiment 4: Island GA — 4 islands, ring migration, OpenMP parallel
    // ══════════════════════════════════════════════════════════════════════════
    {
        GAParams island_ga      = base;
        island_ga.crossover_type = CrossoverType::OX;
        island_ga.pop_size       = 50;    // 4 * 50 = 200 total individuals
        island_ga.no_improve_limit = 150;

        IslandParams iparams;
        iparams.num_islands        = 4;
        iparams.migration_interval = 20;
        iparams.migration_size     = 2;
        iparams.island_ga_params   = island_ga;

        std::vector<uint_fast32_t> best_tour;
        uint_fast32_t best_dist = std::numeric_limits<uint_fast32_t>::max();
        uint_fast64_t sum_dist  = 0;

        for (uint_fast32_t rep = 1; rep <= REPETITIONS; ++rep) {
            if (DEBUG_ITER)
                std::cout << "Island rep " << rep << "/" << REPETITIONS << "\r" << std::flush;

            IslandResult r = island_genetic_algorithm(n, dist_matrix, GEN, iparams);
            sum_dist += r.best_distance;
            if (r.best_distance < best_dist) {
                best_dist = r.best_distance;
                best_tour = r.best_tour;
            }
        }

        std::cout << "\n====================[Island GA — 4 islands, OX, OpenMP]=====================\n"
                  << "File: " << filename << " (" << n << ")\n";
        print_params(island_ga);
        std::cout << "Islands:            " << iparams.num_islands        << "\n"
                  << "Pop per island:     " << island_ga.pop_size         << "\n"
                  << "Migration interval: " << iparams.migration_interval << " generations\n"
                  << "Migration size:     " << iparams.migration_size     << "\n";
        print_results(best_dist,
                      static_cast<double>(sum_dist) / REPETITIONS,
                      0.0,   // generations_run not tracked per-island in IslandResult
                      best_tour);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file/directory>\n";
        return 1;
    }
    std::cout << std::fixed << std::setprecision(4);

    std::vector<std::string> file_queue;

    for (int i = 1; i < argc; ++i) {
        std::filesystem::path p(argv[i]);
        if (std::filesystem::is_directory(p)) {
            for (const auto& entry : std::filesystem::directory_iterator(p)) {
                if (entry.path().extension() == ".tsp")
                    file_queue.push_back(entry.path().string());
            }
        } else if (std::filesystem::exists(p)) {
            file_queue.push_back(p.string());
        } else {
            std::cerr << "Warning: Path not found: " << argv[i] << "\n";
        }
    }

    for (const std::string& filename : file_queue) {
        std::vector<City> cities = read_data(filename);

        if (cities.empty()) {
            std::cerr << "Failed to load: " << filename << "\n";
            continue;
        }

        std::cout << "------------------------------------------\n";
        auto start = std::chrono::high_resolution_clock::now();

        run_exp_for_file(cities, filename);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "\nFinished " << filename << " in " << duration.count() << " seconds.\n";
        std::cout << "------------------------------------------\n";
    }

    return 0;
}