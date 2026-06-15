#include "utils.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

// ─── TSP helpers ──────────────────────────────────────────────────────────────

std::vector<City> read_data(const std::string& filename) {
    std::vector<City> cities;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening:" << filename << "\n";
        return cities;
    }

    bool reading_nodes = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("EOF") != std::string::npos) break;
        if (line.find("NODE_COORD_SECTION") != std::string::npos) {
            reading_nodes = true;
            continue;
        }
        if (reading_nodes) {
            std::istringstream iss(line);
            City new_city;
            if (iss >> new_city.id >> new_city.x >> new_city.y) {
                cities.push_back(new_city);
            }
        }
    }
    return cities;
}

std::vector<uint_fast32_t> random_tour(int num_cities, std::mt19937& GEN) {
    std::vector<uint_fast32_t> tour(num_cities);
    std::iota(tour.begin(), tour.end(), 0);
    std::shuffle(tour.begin(), tour.end(), GEN);
    return tour;
}

uint_fast32_t discrete_distance(const City& a, const City& b) {
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return static_cast<uint_fast32_t>(std::sqrt(dx*dx + dy*dy) + 0.5);
}

uint_fast32_t tour_distance(
    const std::vector<uint_fast32_t>& tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix)
{
    uint_fast32_t total = 0;
    for (uint_fast32_t i = 0; i < tour.size() - 1; ++i)
        total += dist_matrix[tour[i]][tour[i+1]];
    total += dist_matrix[tour.back()][tour.front()];
    return total;
}

int_fast32_t diff_after_invert(
    const std::vector<uint_fast32_t>& tour,
    uint_fast32_t i, uint_fast32_t j,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix)
{
    uint_fast32_t n = tour.size();
    if ((i == 0 && j == n-1) || i == j || (i == n-1 && j == 0)) return 0;

    uint_fast32_t prev_a_idx = (i == 0) ? n-1 : i-1;
    uint_fast32_t next_b_idx = (j == n-1) ? 0   : j+1;

    uint_fast32_t a      = tour[i];
    uint_fast32_t b      = tour[j];
    uint_fast32_t prev_a = tour[prev_a_idx];
    uint_fast32_t next_b = tour[next_b_idx];

    uint_fast32_t old_d = dist_matrix[prev_a][a] + dist_matrix[b][next_b];
    uint_fast32_t new_d = dist_matrix[prev_a][b] + dist_matrix[a][next_b];
    return static_cast<int_fast32_t>(new_d) - static_cast<int_fast32_t>(old_d);
}

// ─── Tabu Search ──────────────────────────────────────────────────────────────

TabuResult tabu_search(
    std::vector<uint_fast32_t> initial_tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    uint_fast32_t tabu_cooldown,
    uint_fast32_t max_iter_no_improve,
    uint_fast32_t sample_size)
{
    uint_fast32_t n = initial_tour.size();
    std::vector<uint_fast32_t> current_tour = initial_tour;
    uint_fast32_t current_dist = tour_distance(current_tour, dist_matrix);

    std::vector<uint_fast32_t> best_tour = current_tour;
    uint_fast32_t best_dist = current_dist;

    std::vector<std::vector<uint_fast32_t>> tabu_list(n, std::vector<uint_fast32_t>(n, 0));
    std::uniform_int_distribution<uint_fast32_t> idx_dist(0, n-1);

    uint_fast32_t iter = 0;
    uint_fast32_t iter_no_improve = 0;

    while (iter_no_improve < max_iter_no_improve) {
        int_fast32_t best_delta = std::numeric_limits<int_fast32_t>::max();
        uint_fast32_t best_i = 0, best_j = 0;
        bool found = false;

        for (uint_fast32_t k = 0; k < sample_size; ++k) {
            uint_fast32_t i = idx_dist(GEN);
            uint_fast32_t j = idx_dist(GEN);
            while (j == i) j = idx_dist(GEN);
            if (i > j) std::swap(i, j);

            bool is_tabu = (tabu_list[i][j] > iter);
            int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);
            bool tabu_override = is_tabu &&
                (static_cast<int_fast64_t>(current_dist) + delta < static_cast<int_fast64_t>(best_dist));

            if (!is_tabu || tabu_override) {
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i; best_j = j;
                    found = true;
                }
            }
        }

        if (!found) {
            for (uint_fast32_t k = 0; k < sample_size; ++k) {
                uint_fast32_t i = idx_dist(GEN);
                uint_fast32_t j = idx_dist(GEN);
                while (j == i) j = idx_dist(GEN);
                if (i > j) std::swap(i, j);
                int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);
                if (delta < best_delta) {
                    best_delta = delta; best_i = i; best_j = j;
                }
            }
        }

        std::reverse(current_tour.begin() + best_i, current_tour.begin() + best_j + 1);
        current_dist = static_cast<uint_fast32_t>(
            static_cast<int_fast64_t>(current_dist) + best_delta);
        tabu_list[best_i][best_j] = iter + tabu_cooldown;

        if (current_dist < best_dist) {
            best_dist = current_dist;
            best_tour = current_tour;
            iter_no_improve = 0;
        } else {
            ++iter_no_improve;
        }
        ++iter;
    }

    return {best_tour, best_dist, iter, max_iter_no_improve, tabu_cooldown, sample_size};
}

// ─── Local Search / 2-opt ─────────────────────────────────────────────────────
LocalSearchResult local_search_mp(
    std::vector<uint_fast32_t> initial_tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    uint_fast32_t sample_size,
    uint_fast32_t max_iters)
{
    uint_fast32_t current_distance = tour_distance(initial_tour, dist_matrix);
    uint_fast32_t improvement_steps = 0;
    bool full_search = (sample_size == 0);
    std::uniform_int_distribution<uint_fast32_t> dist(0, initial_tour.size()-1);

    bool improvement = true;
    while (improvement && (improvement_steps <= max_iters)) {
        improvement = false;
        int_fast32_t best_diff = 0;
        uint_fast32_t best_i = 0, best_j = 0;

        if (full_search) {
            int_fast32_t  global_best_diff = 0;
            uint_fast32_t global_best_i    = 0;
            uint_fast32_t global_best_j    = 0;

            #pragma omp parallel
            {
                int_fast32_t  local_diff = 0;
                uint_fast32_t local_i    = 0;
                uint_fast32_t local_j    = 0;

                #pragma omp for schedule(dynamic, 8) nowait
                for (int i = 0; i < static_cast<int>(initial_tour.size()) - 1; ++i) {
                    for (uint_fast32_t j = i + 1; j < initial_tour.size(); ++j) {
                        int_fast32_t d = diff_after_invert(initial_tour, i, j, dist_matrix);
                        if (d < local_diff) {
                            local_diff = d;
                            local_i    = static_cast<uint_fast32_t>(i);
                            local_j    = j;
                        }
                    }
                }

                // One thread at a time merge local best into global best
                #pragma omp critical
                {
                    if (local_diff < global_best_diff) {
                        global_best_diff = local_diff;
                        global_best_i    = local_i;
                        global_best_j    = local_j;
                    }
                }
            }

            best_diff = global_best_diff;
            best_i    = global_best_i;
            best_j    = global_best_j;

        } else {
            // GEN is not thread-safe so keep it
            for (uint_fast32_t k = 0; k < sample_size; ++k) {
                uint_fast32_t i = dist(GEN);
                uint_fast32_t j = dist(GEN);
                while (j == i) j = dist(GEN);
                if (i > j) std::swap(i, j);
                int_fast32_t d = diff_after_invert(initial_tour, i, j, dist_matrix);
                if (d < best_diff) { best_diff = d; best_i = i; best_j = j; }
            }
        }

        if (best_diff < 0) {
            std::reverse(initial_tour.begin() + best_i, initial_tour.begin() + best_j + 1);
            current_distance = static_cast<uint_fast32_t>(
                static_cast<int_fast64_t>(current_distance) + best_diff);
            ++improvement_steps;
            improvement = true;
        }
    }
    return {initial_tour, current_distance, improvement_steps, sample_size};
}

LocalSearchResult local_search(
    std::vector<uint_fast32_t> initial_tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    uint_fast32_t sample_size,
    uint_fast32_t max_iters)
{
    uint_fast32_t current_distance = tour_distance(initial_tour, dist_matrix);
    uint_fast32_t improvement_steps = 0;
    const bool full_search = (sample_size == 0);
    const size_t tour_size = initial_tour.size();
    
    std::uniform_int_distribution<uint_fast32_t> dist(0, tour_size - 1);

    bool improvement = true;
    while (improvement && (improvement_steps <= max_iters)) {
        improvement = false;
        int_fast32_t best_diff = 0;
        uint_fast32_t best_i = 0;
        uint_fast32_t best_j = 0;

        if (full_search) {
            // Pure sequential 2-opt full neighborhood scan
            for (size_t i = 0; i < tour_size - 1; ++i) {
                for (size_t j = i + 1; j < tour_size; ++j) {
                    int_fast32_t d = diff_after_invert(initial_tour, i, j, dist_matrix);
                    if (d < best_diff) {
                        best_diff = d;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
        } else {
            for (uint_fast32_t k = 0; k < sample_size; ++k) {
                uint_fast32_t i = dist(GEN);
                uint_fast32_t j = dist(GEN);
                while (j == i) j = dist(GEN);
                
                if (i > j) std::swap(i, j);
                
                int_fast32_t d = diff_after_invert(initial_tour, i, j, dist_matrix);
                if (d < best_diff) { 
                    best_diff = d; 
                    best_i = i; 
                    best_j = j; 
                }
            }
        }

        if (best_diff < 0) {
            std::reverse(initial_tour.begin() + best_i, initial_tour.begin() + best_j + 1);
            current_distance = static_cast<uint_fast32_t>(
                static_cast<int_fast64_t>(current_distance) + best_diff);
            ++improvement_steps;
            improvement = true;
        }
    }
    
    return {initial_tour, current_distance, improvement_steps, sample_size};
}

// ─── GA: Tournament selection ─────────────────────────────────────────────────

uint_fast32_t tournament_select(
    const std::vector<uint_fast32_t>& fitnesses,
    uint_fast32_t k,
    std::mt19937& GEN)
{
    std::uniform_int_distribution<uint_fast32_t> dist(0, fitnesses.size()-1);
    uint_fast32_t best_idx = dist(GEN);
    for (uint_fast32_t i = 1; i < k; ++i) {
        uint_fast32_t idx = dist(GEN);
        if (fitnesses[idx] < fitnesses[best_idx])
            best_idx = idx;
    }
    return best_idx;
}

// ─── GA: OX crossover ─────────────────────────────────────────────────────────

std::pair<std::vector<uint_fast32_t>, std::vector<uint_fast32_t>>
crossover_OX(
    const std::vector<uint_fast32_t>& p1,
    const std::vector<uint_fast32_t>& p2,
    std::mt19937& GEN)
{
    uint_fast32_t n = p1.size();
    std::uniform_int_distribution<uint_fast32_t> dist(0, n-1);
    uint_fast32_t a = dist(GEN);
    uint_fast32_t b = dist(GEN);
    if (a > b) std::swap(a, b);

    auto make_child = [&](const std::vector<uint_fast32_t>& donor,
                          const std::vector<uint_fast32_t>& filler) {
        std::vector<uint_fast32_t> child(n, std::numeric_limits<uint_fast32_t>::max());
        std::vector<bool> used(n, false);

        for (uint_fast32_t i = a; i <= b; ++i) {
            child[i] = donor[i];
            used[donor[i]] = true;
        }

        uint_fast32_t fill_pos = (b + 1) % n;
        uint_fast32_t fill_src = (b + 1) % n;
        uint_fast32_t filled   = 0;
        uint_fast32_t total_fill = n - (b - a + 1);

        while (filled < total_fill) {
            if (!used[filler[fill_src]]) {
                child[fill_pos] = filler[fill_src];
                fill_pos = (fill_pos + 1) % n;
                ++filled;
            }
            fill_src = (fill_src + 1) % n;
        }
        return child;
    };

    return { make_child(p1, p2), make_child(p2, p1) };
}

// ─── GA: PMX crossover ────────────────────────────────────────────────────────

std::pair<std::vector<uint_fast32_t>, std::vector<uint_fast32_t>>
crossover_PMX(
    const std::vector<uint_fast32_t>& p1,
    const std::vector<uint_fast32_t>& p2,
    std::mt19937& GEN)
{
    uint_fast32_t n = p1.size();
    std::uniform_int_distribution<uint_fast32_t> dist(0, n-1);
    uint_fast32_t a = dist(GEN);
    uint_fast32_t b = dist(GEN);
    if (a > b) std::swap(a, b);

    auto make_child = [&](const std::vector<uint_fast32_t>& donor,
                          const std::vector<uint_fast32_t>& other) {
        std::vector<uint_fast32_t> child(n, std::numeric_limits<uint_fast32_t>::max());
        std::vector<uint_fast32_t> pos_in_donor(n);
        for (uint_fast32_t i = 0; i < n; ++i)
            pos_in_donor[donor[i]] = i;

        for (uint_fast32_t i = a; i <= b; ++i)
            child[i] = donor[i];

        for (uint_fast32_t i = 0; i < n; ++i) {
            if (i >= a && i <= b) continue;
            uint_fast32_t gene = other[i];
            while (true) {
                uint_fast32_t p = pos_in_donor[gene];
                if (p < a || p > b) break;
                gene = other[p];
            }
            child[i] = gene;
        }
        return child;
    };

    return { make_child(p1, p2), make_child(p2, p1) };
}

// ─── GA: Mutation — invert random ─────────────────────────────────────────────

void mutate_invert(std::vector<uint_fast32_t>& tour, std::mt19937& GEN) {
    uint_fast32_t n = tour.size();
    std::uniform_int_distribution<uint_fast32_t> dist(0, n-1);
    uint_fast32_t i = dist(GEN);
    uint_fast32_t j = dist(GEN);
    while (j == i) j = dist(GEN);
    if (i > j) std::swap(i, j);
    std::reverse(tour.begin() + i, tour.begin() + j + 1);
}

// ─── GA: one generation step ──────────────────────────────────────────────────

static std::vector<std::vector<uint_fast32_t>> next_generation(
    const std::vector<std::vector<uint_fast32_t>>& pop,
    const std::vector<uint_fast32_t>&              fitness,
    const std::vector<uint_fast32_t>&              elite_tour,
    uint_fast32_t                                  elite_dist,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937&                                  rng,
    const GAParams&                                gp,
    uint_fast32_t                                  memetic_iters)
{
    const uint_fast32_t POP = pop.size();
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    std::vector<std::vector<uint_fast32_t>> new_pop;
    new_pop.reserve(POP);

    while (new_pop.size() < POP) {
        uint_fast32_t idx1 = tournament_select(fitness, gp.tournament_k, rng);
        uint_fast32_t idx2 = tournament_select(fitness, gp.tournament_k, rng);
        for (uint_fast32_t t = 0; idx2 == idx1 && t < 10; ++t)
            idx2 = tournament_select(fitness, gp.tournament_k, rng);

        std::vector<uint_fast32_t> child1, child2;

        if (prob_dist(rng) < gp.cross_prob) {
            if (gp.crossover_type == CrossoverType::OX) {
                auto [c1, c2] = crossover_OX(pop[idx1], pop[idx2], rng);
                child1 = std::move(c1); child2 = std::move(c2);
            } else {
                auto [c1, c2] = crossover_PMX(pop[idx1], pop[idx2], rng);
                child1 = std::move(c1); child2 = std::move(c2);
            }
        } else {
            child1 = pop[idx1];
            child2 = pop[idx2];
        }

        if (prob_dist(rng) < gp.mut_prob) mutate_invert(child1, rng);
        if (prob_dist(rng) < gp.mut_prob) mutate_invert(child2, rng);

        if (gp.memetic) {
            child1 = local_search_mp(child1, dist_matrix, rng, 0, memetic_iters).tour;
            child2 = local_search_mp(child2, dist_matrix, rng, 0, memetic_iters).tour;
        }

        new_pop.push_back(std::move(child1));
        if (new_pop.size() < POP)
            new_pop.push_back(std::move(child2));
    }

    // Elitism — replace worst with all-time best
    std::vector<uint_fast32_t> new_fitness(POP);

    // OpenMP parallel fitness evaluation
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(POP); ++i)
        new_fitness[i] = tour_distance(new_pop[i], dist_matrix);

    uint_fast32_t worst = static_cast<uint_fast32_t>(
        std::max_element(new_fitness.begin(), new_fitness.end()) - new_fitness.begin());
    new_pop[worst] = elite_tour;

    return new_pop;
}

// ─── Standard GA ──────────────────────────────────────────────────────────────

GAResult genetic_algorithm(
    uint_fast32_t n,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    const GAParams& params)
{
    const uint_fast32_t POP = params.pop_size;
    const uint_fast32_t memetic_iters = params.memetic_2opt_iters;

    // ── Initial population ────────────────────────────────────────────────────
    std::vector<std::vector<uint_fast32_t>> pop(POP);
    for (uint_fast32_t i = 0; i < POP; ++i)
        pop[i] = random_tour(n, GEN);

    std::vector<uint_fast32_t> fitness(POP);

    // OpenMP parallel initial evaluation
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < static_cast<int>(POP); ++i)
        fitness[i] = tour_distance(pop[i], dist_matrix);

    uint_fast32_t best_idx = static_cast<uint_fast32_t>(
        std::min_element(fitness.begin(), fitness.end()) - fitness.begin());
    std::vector<uint_fast32_t> best_tour = pop[best_idx];
    uint_fast32_t              best_dist = fitness[best_idx];

    uint_fast32_t no_improve = 0;

    for (uint_fast32_t gen = 0; gen < params.max_generations; ++gen) {
        if(params.memetic) std::cout<< "GEN: " << gen << "/" << params.max_generations << "\n";
        pop = next_generation(pop, fitness, best_tour, best_dist,
                              dist_matrix, GEN, params, memetic_iters);

        // Re-evaluate
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < static_cast<int>(POP); ++i)
            fitness[i] = tour_distance(pop[i], dist_matrix);

        uint_fast32_t gen_best = static_cast<uint_fast32_t>(
            std::min_element(fitness.begin(), fitness.end()) - fitness.begin());

        if (fitness[gen_best] < best_dist) {
            best_dist = fitness[gen_best];
            best_tour = pop[gen_best];
            no_improve = 0;
        } else {
            ++no_improve;
        }

        if (no_improve >= params.no_improve_limit) break;
    }

    return {best_tour, best_dist, 0};
}

// ─── Island GA ────────────────────────────────────────────────────────────────
// Each island is run sequentially in its own OpenMP thread.
// Migration happens every migration_interval generations via a shared buffer
// protected by a mutex — no complex barrier needed since islands are
// independent between migration windows.

IslandResult island_genetic_algorithm(
    uint_fast32_t n,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    const IslandParams& params)
{
    const uint_fast32_t NUM_ISLANDS  = params.num_islands;
    const GAParams&     gp           = params.island_ga_params;
    const uint_fast32_t POP          = gp.pop_size;
    const uint_fast32_t MIG_INTERVAL = params.migration_interval;
    const uint_fast32_t MIG_SIZE     = params.migration_size;
    const uint_fast32_t MAX_GEN      = gp.max_generations;
    const uint_fast32_t memetic_iters = gp.memetic_2opt_iters;

    // Per-island RNGs seeded from master GEN
    std::vector<std::mt19937> rngs(NUM_ISLANDS);
    std::uniform_int_distribution<uint_fast64_t> seed_dist;
    for (auto& g : rngs) g.seed(seed_dist(GEN));

    // Migration buffers — island i writes its best individuals here after each
    // migration window; the next island reads them. Protected by one mutex each.
    std::vector<std::vector<std::vector<uint_fast32_t>>> mig_buf(NUM_ISLANDS);
    std::vector<std::mutex>                              mig_mtx(NUM_ISLANDS);

    // Results per island
    std::vector<std::vector<uint_fast32_t>> island_best_tour(NUM_ISLANDS);
    std::vector<uint_fast32_t>              island_best_dist(NUM_ISLANDS,
        std::numeric_limits<uint_fast32_t>::max());

    // ── Run islands in parallel via OpenMP ────────────────────────────────────
    #pragma omp parallel for num_threads(NUM_ISLANDS) schedule(static)
    for (int id = 0; id < static_cast<int>(NUM_ISLANDS); ++id) {
        std::mt19937& rng = rngs[id];

        // Initialise island population
        std::vector<std::vector<uint_fast32_t>> pop(POP);
        for (uint_fast32_t i = 0; i < POP; ++i)
            pop[i] = random_tour(n, rng);

        std::vector<uint_fast32_t> fitness(POP);
        for (uint_fast32_t i = 0; i < POP; ++i)
            fitness[i] = tour_distance(pop[i], dist_matrix);

        uint_fast32_t bi = static_cast<uint_fast32_t>(
            std::min_element(fitness.begin(), fitness.end()) - fitness.begin());
        std::vector<uint_fast32_t> best_tour = pop[bi];
        uint_fast32_t              best_dist = fitness[bi];

        uint_fast32_t no_improve = 0;

        for (uint_fast32_t gen = 1; gen <= MAX_GEN; ++gen) {
            // ── Migration: export ─────────────────────────────────────────────
            if (gen % MIG_INTERVAL == 0) {
                std::vector<uint_fast32_t> order(POP);
                std::iota(order.begin(), order.end(), 0);
                std::sort(order.begin(), order.end(),
                    [&](uint_fast32_t a, uint_fast32_t b){ return fitness[a] < fitness[b]; });

                std::vector<std::vector<uint_fast32_t>> emigrants;
                for (uint_fast32_t k = 0; k < MIG_SIZE && k < POP; ++k)
                    emigrants.push_back(pop[order[k]]);

                {
                    std::lock_guard<std::mutex> lock(mig_mtx[id]);
                    mig_buf[id] = std::move(emigrants);
                }

                // ── Migration: import from left neighbour (ring) ──────────────
                uint_fast32_t src = (id + NUM_ISLANDS - 1) % NUM_ISLANDS;
                std::vector<std::vector<uint_fast32_t>> immigrants;
                {
                    std::lock_guard<std::mutex> lock(mig_mtx[src]);
                    immigrants = mig_buf[src];   // copy — src may update next window
                }

                if (!immigrants.empty()) {
                    // Replace worst individuals
                    std::vector<uint_fast32_t> worst_order(POP);
                    std::iota(worst_order.begin(), worst_order.end(), 0);
                    std::sort(worst_order.begin(), worst_order.end(),
                        [&](uint_fast32_t a, uint_fast32_t b){ return fitness[a] > fitness[b]; });

                    for (uint_fast32_t k = 0; k < immigrants.size() && k < POP; ++k) {
                        uint_fast32_t ri = worst_order[k];
                        pop[ri]     = immigrants[k];
                        fitness[ri] = tour_distance(pop[ri], dist_matrix);
                    }
                }
            }

            // ── Next generation ───────────────────────────────────────────────
            pop = next_generation(pop, fitness, best_tour, best_dist,
                                  dist_matrix, rng, gp, memetic_iters);

            for (uint_fast32_t i = 0; i < POP; ++i)
                fitness[i] = tour_distance(pop[i], dist_matrix);

            uint_fast32_t gb = static_cast<uint_fast32_t>(
                std::min_element(fitness.begin(), fitness.end()) - fitness.begin());

            if (fitness[gb] < best_dist) {
                best_dist = fitness[gb];
                best_tour = pop[gb];
                no_improve = 0;
            } else {
                ++no_improve;
            }

            if (no_improve >= gp.no_improve_limit) break;
        }

        island_best_dist[id] = best_dist;
        island_best_tour[id] = best_tour;
    }

    // ── Collect global best ───────────────────────────────────────────────────
    uint_fast32_t global_best_dist = std::numeric_limits<uint_fast32_t>::max();
    std::vector<uint_fast32_t> global_best_tour;
    for (uint_fast32_t i = 0; i < NUM_ISLANDS; ++i) {
        if (island_best_dist[i] < global_best_dist) {
            global_best_dist = island_best_dist[i];
            global_best_tour = island_best_tour[i];
        }
    }

    return {global_best_tour, global_best_dist};
}