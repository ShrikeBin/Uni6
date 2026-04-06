#include "utils.hpp"

#include <filesystem>
#include <chrono>

// Global random generator
std::random_device RD;
std::mt19937 GEN(RD());

void run_for_file(const std::vector<City>& cities, const std::string& filename) {
    int n = cities.size();

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

    for (uint_fast32_t k = 0; k < n; ++k) {
        std::cout<< "Iteration " << k + 1 << "/" << n << "\r" << std::flush;
        std::vector<uint_fast32_t> initial_tour = random_tour(n, GEN);
        LocalSearchResult result = local_search_naive(initial_tour, dist_matrix);
        
        sum_dist_1 += result.distance;
        sum_steps_1 += result.improvement_steps;
        if (result.distance < best_dist_1) best_dist_1 = result.distance;
    }

    std::cout << "TASK 1:\n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_1) / n << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_1) / n << "\n";
    std::cout << "Best solution: " << best_dist_1 << "\n\n";

    long long sum_dist_2 = 0;
    long long sum_steps_2 = 0;
    int best_dist_2 = std::numeric_limits<int>::max();

    for (uint_fast32_t k = 0; k < n; ++k) {
        std::vector<uint_fast32_t> initial_tour = random_tour(n, GEN);
        LocalSearchResult result = local_search_n_random(initial_tour, dist_matrix, GEN);
        
        sum_dist_2 += result.distance;
        sum_steps_2 += result.improvement_steps;
        if (result.distance < best_dist_2) best_dist_2 = result.distance;
    }

    std::cout << "TASK 2:\n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_2) / n << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_2) / n << "\n";
    std::cout << "Best solution: " << best_dist_2 << "\n\n";

    long long sum_dist_3 = 0;
    long long sum_steps_3 = 0;
    int best_dist_3 = std::numeric_limits<int>::max();

    std::uniform_int_distribution<> start_vertex_dist(1, n);
    int iterations = std::ceil(std::sqrt(n));
    MSTresult mst_result = prim_mst(cities, dist_matrix);

    for (int k = 0; k < iterations; ++k) {
        // Losowy wierzchołek startowy dla DFS
        int start_vertex = start_vertex_dist(GEN);

        // cykl TSP z drzewa MST
        std::vector<uint_fast32_t> initial_tour = mst_to_tsp(mst_result.edges, n, start_vertex);

        // klasyczne lokalne przeszukiwanie
        LocalSearchResult result = local_search_naive(initial_tour, dist_matrix);
        
        sum_dist_3 += result.distance;
        sum_steps_3 += result.improvement_steps;
        if (result.distance < best_dist_3) best_dist_3 = result.distance;
    }

    std::cout << "TASK 3: \n";
    std::cout << "Weight of the Minimum Spanning Tree (MST): " << mst_result.total_weight << "\n";
    std::cout << "Average solution value: " << static_cast<double>(sum_dist_3) / iterations << "\n";
    std::cout << "Average improvement steps: " << static_cast<double>(sum_steps_3) / iterations << "\n";
    std::cout << "Best solution: " << best_dist_3 << "\n";
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