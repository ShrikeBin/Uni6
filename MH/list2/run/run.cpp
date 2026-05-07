#include "utils.hpp"

#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cmath>

// Global random generator
std::random_device RD;
std::mt19937 GEN(RD());

void run_for_file(const std::vector<City>& cities, const std::string& filename) {
    unsigned int n = cities.size();

    std::cout << "Loaded file: " << filename << " | Number of vertices (n): " << n << "\n\n";

    std::vector<std::vector<uint_fast32_t>> dist_matrix(n, std::vector<uint_fast32_t>(n));
    for (uint_fast32_t i = 0; i < n; ++i) {
        for (uint_fast32_t j = 0; j < n; ++j) {
            dist_matrix[i][j] = discrete_distance(cities[i], cities[j]);
        }
    }

    // Taboo experiments
    std::vector<uint_fast32_t> tenures = {5, 10, static_cast<uint_fast32_t>(std::log(n)), static_cast<uint_fast32_t>(std::sqrt(n))};
    std::vector<uint_fast32_t> max_iters = {static_cast<uint_fast32_t>(std::sqrt(n)), 100, 1000};
    std::vector<uint_fast32_t> sample_sizes = {static_cast<uint_fast32_t>(std::sqrt(n)), n};

    std::vector<uint_fast32_t> best_tour_taboo = random_tour(n, GEN);
    for(auto tenure : tenures) {
        for(auto max_iter : max_iters) {
            for (auto sample_size : sample_sizes) {
                TabuResult result = tabu_search(best_tour_taboo, dist_matrix, GEN, tenure, max_iter, sample_size);
            }
        }
    }
    // or smth idk today
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