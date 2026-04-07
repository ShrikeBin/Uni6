#include "utils.hpp"

#include <filesystem>
#include <chrono>
#include <algorithm>

// Global random generator
std::random_device RD;
std::mt19937 GEN(RD());

void run_for_file(const std::vector<City>& cities, const std::string& filename) {
    unsigned int n = cities.size();
    const unsigned int LIMIT =  (n < 200)  ? 100 :
                                (n < 500)  ? 50 :
                                (n < 1000) ? 35  :
                                (n < 2000) ? 15  : 
                                (n < 4000) ? 5  : 2;

    std::cout << "Loaded file: " << filename << " | Number of vertices (n): " << n << "\n\n";

    std::vector<std::vector<uint_fast32_t>> dist_matrix(n, std::vector<uint_fast32_t>(n));
    for (uint_fast32_t i = 0; i < n; ++i) {
        for (uint_fast32_t j = 0; j < n; ++j) {
            dist_matrix[i][j] = discrete_distance(cities[i], cities[j]);
        }
    }

    long long sum_dist_1 = 0;
    long long sum_steps_1 = 0;
    int best_dist_1 = std::numeric_limits<int>::max();
    std::vector<uint_fast32_t> best_tour_naive = random_tour(n, GEN);

    for (uint_fast32_t k = 0; k < LIMIT; ++k) {
        std::cout<< "Iteration " << k + 1 << "/" << LIMIT << "\r" << std::flush;
        std::vector<uint_fast32_t> initial_tour = random_tour(n, GEN);
        LocalSearchResult result = local_search_naive(initial_tour, dist_matrix);
        
        sum_dist_1 += result.distance;
        sum_steps_1 += result.improvement_steps;
        if (result.distance < best_dist_1){
            best_tour_naive = result.tour;
            best_dist_1 = result.distance;
        }
    }

    std::cout << "TASK 1:   \n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_1) / LIMIT << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_1) / LIMIT << "\n";
    std::cout << "Best solution: " << best_dist_1 << "\n";
    std::cout << "\n====================[TOUR]=====================\n";
    for(uint_fast32_t city_id : best_tour_naive) {
        std::cout << city_id << ">";
    }
    std::cout << "\n===============================================\n\n";

    long long sum_dist_2 = 0;
    long long sum_steps_2 = 0;
    int best_dist_2 = std::numeric_limits<int>::max();
    std::vector<uint_fast32_t> best_tour_n_random = random_tour(n, GEN);

    for (uint_fast32_t k = 0; k < LIMIT*10; ++k) {
        std::cout<< "Iteration " << k + 1 << "/" << LIMIT*10 << "\r" << std::flush;
        std::vector<uint_fast32_t> initial_tour = random_tour(n, GEN);
        LocalSearchResult result = local_search_n_random(initial_tour, dist_matrix, GEN);
        
        sum_dist_2 += result.distance;
        sum_steps_2 += result.improvement_steps;
        if (result.distance < best_dist_2) {
            best_tour_n_random = result.tour;
            best_dist_2 = result.distance;
        }
    }

    std::cout << "TASK 2:   \n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_2) / LIMIT << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_2) / LIMIT << "\n";
    std::cout << "Best solution: " << best_dist_2 << "\n";
    std::cout << "\n====================[TOUR]=====================\n";
    for(uint_fast32_t city_id : best_tour_n_random) {
        std::cout << city_id << ">";
    }
    std::cout << "\n===============================================\n\n";

    long long sum_dist_3 = 0;
    long long sum_steps_3 = 0;
    int best_dist_3 = std::numeric_limits<int>::max();
    std::vector<uint_fast32_t> best_tour_MST = random_tour(n, GEN);

    std::uniform_int_distribution<> start_vertex_dist(0, n - 1);
    MSTresult mst_result = prim_mst(cities, dist_matrix);

    for (uint_fast32_t k = 0; k < LIMIT; ++k) {
        std::cout<< "Iteration " << k + 1 << "/" << LIMIT << "\r" << std::flush;

        uint_fast32_t random_start_vertex = start_vertex_dist(GEN);
        std::vector<uint_fast32_t> initial_tour = mst_to_tsp(mst_result.edges, n, random_start_vertex);
        LocalSearchResult result = local_search_naive(initial_tour, dist_matrix);
        
        sum_dist_3 += result.distance;
        sum_steps_3 += result.improvement_steps;
        if (result.distance < best_dist_3) {
            best_tour_MST = result.tour;
            best_dist_3 = result.distance;
        }
    }

    std::cout << "TASK 3:   \n";
    std::cout << "Weight of the Minimum Spanning Tree (MST): " << mst_result.total_weight << "\n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_3) / LIMIT << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_3) / LIMIT << "\n";
    std::cout << "Best solution: " << best_dist_3 << "\n";
    std::cout << "\n====================[TOUR]=====================\n";
    for(uint_fast32_t city_id : best_tour_MST) {
        std::cout << city_id << ">";
    }
    std::cout << "\n===============================================\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file/directory>\n";
        return 1;
    }

    std::vector<std::string> file_queue;

    for (int i = 1; i < argc; ++i) {
        std::filesystem::path p(argv[i]);

        if (std::filesystem::is_directory(p)) {
            for (const auto& entry : std::filesystem::directory_iterator(p)) {
                if (entry.path().extension() == ".tsp") {
                    file_queue.push_back(entry.path().string());
                }
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

        run_for_file(cities, filename);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "\n";
        std::cout << "Finished " << filename << " in " << duration.count() << " seconds.\n";
        std::cout << "------------------------------------------\n";
    }

    return 0;
}