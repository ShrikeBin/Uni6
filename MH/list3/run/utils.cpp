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
    // Tabu list holds inversions (i,j) as full tours would be very memory inefficient for big N
    std::vector<std::vector<uint_fast32_t>> tabu_list(n, std::vector<uint_fast32_t>(n, 0));

    std::uniform_int_distribution<uint_fast32_t> idx_dist(0, n - 1);

    uint_fast32_t iter = 0;
    uint_fast32_t iter_no_improve = 0;

    while (iter_no_improve < max_iter_no_improve) {
        int_fast32_t best_delta = std::numeric_limits<int_fast32_t>::max();
        uint_fast32_t best_i = 0;
        uint_fast32_t best_j = 0;
        bool found = false;

        for (uint_fast32_t k = 0; k < sample_size; ++k) {
            uint_fast32_t i = idx_dist(GEN);
            uint_fast32_t j = idx_dist(GEN);
            
            while (j == i) { 
                j = idx_dist(GEN);
            }

            if (i > j) {
                std::swap(i, j);
            }

            bool is_tabu = (tabu_list[i][j] > iter);

            int_fast32_t delta = diff_after_invert(current_tour, i, j, dist_matrix);

            // tabu_override: accept tabu move if it yields a new global best
            bool tabu_override = is_tabu &&
                (static_cast<int_fast64_t>(current_dist) + delta < static_cast<int_fast64_t>(best_dist));

            if (!is_tabu || tabu_override) {
                if (delta < best_delta) {
                    best_delta = delta;
                    best_i = i;
                    best_j = j;
                    found = true;
                }
            }
        }

        if (!found) {
            // All sampled moves tabu and none pass aspiration — pick least bad
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

LocalSearchResult local_search(
    std::vector<uint_fast32_t> initial_tour, 
    const std::vector<std::vector<uint_fast32_t>>& dist_matrix, 
    std::mt19937& GEN,
    uint_fast32_t sample_size) 
{
    uint_fast32_t improvement_steps = 0;
    uint_fast32_t initial_distance = tour_distance(initial_tour, dist_matrix);
    uint_fast32_t current_distance = initial_distance;

    bool improvement = true;
    bool full_search = (sample_size == initial_tour.size());
    std::uniform_int_distribution<> dist(0, sample_size - 1);

    while(improvement) {
        improvement = false;

        int_fast32_t best_difference = 0;
        uint_fast32_t best_i = -1;
        uint_fast32_t best_j = -1;

        if(full_search){
            for(uint_fast32_t i = 0; i < initial_tour.size() - 1; ++i) {
                for(uint_fast32_t j = i + 1; j < initial_tour.size(); ++j) {
                    int_fast32_t difference = diff_after_invert(initial_tour, i, j, dist_matrix);
                    if (difference < best_difference) {
                        best_difference = difference;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
        } else {
            for(uint_fast32_t k = 0; k < sample_size; ++k) {
                uint_fast32_t i = dist(GEN);
                uint_fast32_t j = dist(GEN);
                while (j == i) j = dist(GEN);
                if (i > j) std::swap(i, j);

                int_fast32_t difference = diff_after_invert(initial_tour, i, j, dist_matrix);
                if (difference < best_difference) {
                    best_difference = difference;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if(best_difference < 0) {
            std::reverse(initial_tour.begin() + best_i, initial_tour.begin() + best_j + 1); 
            current_distance += best_difference;
            improvement_steps++;
            improvement = true;
        }
    }
    return LocalSearchResult{initial_tour, current_distance, improvement_steps, sample_size};
}