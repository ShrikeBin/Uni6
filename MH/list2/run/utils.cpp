#include "utils.hpp"

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

uint_fast32_t tour_distance(const std::vector<uint_fast32_t>& tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix) {
    uint_fast32_t total_distance = 0;
    for (uint_fast32_t i = 0; i < tour.size() - 1; ++i) {
        total_distance += dist_matrix[tour[i]][tour[i + 1]];
    }
    total_distance += dist_matrix[tour.back()][tour.front()];
    return total_distance;
}

int_fast32_t diff_after_invert(const std::vector<uint_fast32_t>& tour, uint_fast32_t i, uint_fast32_t j, const std::vector<std::vector<uint_fast32_t>>& dist_matrix) {

    // We invert around indexes(i, j) corresponding to cities a and b.
    uint_fast32_t n = tour.size();

    if ((i == 0 && j == n - 1) || i == j || (i == n - 1 && j == 0)) return 0;

    uint_fast32_t prev_a_idx = (i == 0) ? n - 1 : i - 1;
    uint_fast32_t next_b_idx = (j == n - 1) ? 0 : j + 1;

    uint_fast32_t a = tour[i];
    uint_fast32_t b = tour[j];
    uint_fast32_t prev_a = tour[prev_a_idx];
    uint_fast32_t next_b = tour[next_b_idx];

    uint_fast32_t old_distance = dist_matrix[prev_a][a] + dist_matrix[b][next_b];
    uint_fast32_t new_distance = dist_matrix[prev_a][b] + dist_matrix[a][next_b];

    return new_distance - old_distance;
}

SAResult simulated_annealing(
    std::vector<uint_fast32_t> initial_tour,
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix,
    std::mt19937& GEN,
    double initial_temp,
    double cooling_rate,
    uint_fast32_t epoch_length,
    uint_fast32_t max_epochs_no_improve)
{
    uint_fast32_t n = initial_tour.size();
    std::vector<uint_fast32_t> current_tour = initial_tour;
    uint_fast32_t current_dist = tour_distance(current_tour, dist_matrix);

    std::vector<uint_fast32_t> best_tour = current_tour;
    uint_fast32_t best_dist = current_dist;

    std::uniform_int_distribution<uint_fast32_t> idx_dist(0, n - 1);
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    double temp = initial_temp;
    uint_fast32_t epochs_no_improve = 0;

    while (epochs_no_improve < max_epochs_no_improve) {
        for (uint_fast32_t step = 0; step < epoch_length; ++step) {
            // Pick random 2-opt move
            uint_fast32_t i = idx_dist(GEN);
            uint_fast32_t j = idx_dist(GEN);
            while (j == i) j = idx_dist(GEN);
            if (i > j) std::swap(i, j);

            int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);

            if (delta < 0) {
                std::reverse(current_tour.begin() + i, current_tour.begin() + j + 1);
                current_dist += delta;
            } else {
                // Accept worse solution with Boltzmann probability
                double probability = std::exp(-static_cast<double>(delta) / temp);
                if (prob_dist(GEN) < probability) {
                    std::reverse(current_tour.begin() + i, current_tour.begin() + j + 1);
                    current_dist += delta;
                }
            }

            if (current_dist < best_dist) {
                best_dist = current_dist;
                best_tour = current_tour;
            }
        }

        temp *= cooling_rate;

        if (current_dist < best_dist) {
            epochs_no_improve = 0;
        } else {
            ++epochs_no_improve;
        }
    }

    return {best_tour, best_dist, initial_temp, cooling_rate, epoch_length, max_epochs_no_improve};
}

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

    // Tabu list: tabu_list[i][j] = iteration until which move (i,j) is forbidden
    // Using a flat tenure counter per (i,j) pair (upper triangle only)
    // For large n a full n×n matrix is expensive but straightforward
    std::vector<std::vector<uint_fast32_t>> tabu_list(n, std::vector<uint_fast32_t>(n, 0));

    std::uniform_int_distribution<uint_fast32_t> idx_dist(0, n - 1);

    uint_fast32_t iter = 0;
    uint_fast32_t iter_no_improve = 0;

    while (iter_no_improve < max_iter_no_improve) {
        int_fast32_t best_delta = std::numeric_limits<int_fast32_t>::max();
        uint_fast32_t best_i = 0, best_j = 1;
        bool found = false;

        // Sample random 2-opt neighbours
        for (uint_fast32_t k = 0; k < sample_size; ++k) {
            uint_fast32_t i = idx_dist(GEN);
            uint_fast32_t j = idx_dist(GEN);
            while (j == i) j = idx_dist(GEN);
            if (i > j) std::swap(i, j);

            bool is_tabu = (tabu_list[i][j] > iter);

            int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);

            // Aspiration: accept tabu move if it yields a new global best
            bool aspiration = is_tabu &&
                (static_cast<int_fast64_t>(current_dist) + delta < static_cast<int_fast64_t>(best_dist));

            if (!is_tabu || aspiration) {
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                    found = true;
                }
            }
        }

        if (!found) {
            // All sampled moves tabu and none pass aspiration — just pick least-bad
            for (uint_fast32_t k = 0; k < sample_size; ++k) {
                uint_fast32_t i = idx_dist(GEN);
                uint_fast32_t j = idx_dist(GEN);
                while (j == i) j = idx_dist(GEN);
                if (i > j) std::swap(i, j);
                int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        // Apply the move
        std::reverse(current_tour.begin() + best_i, current_tour.begin() + best_j + 1);
        current_dist = static_cast<uint_fast32_t>(
            static_cast<int_fast64_t>(current_dist) + best_delta);

        // Mark move as tabu
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