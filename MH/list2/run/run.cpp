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

void run_for_file(const std::vector<City>& cities, const std::string& filename) {
    bool PRINT_TOURS = false;
    bool PRINT_BEST_ONLY = true;
    bool DEBUG_ITER = false;
    unsigned int n = cities.size();

    std::cout << "Loaded file: " << filename << " | Number of vertices (n): " << n << "\n\n";

    ///TODO REMOVE IT AFTER FIGURING OUT THE BESST
    if(n >= 2000) return;

    std::vector<std::vector<uint_fast32_t>> dist_matrix(n, std::vector<uint_fast32_t>(n));
    for (uint_fast32_t i = 0; i < n; ++i) {
        for (uint_fast32_t j = 0; j < n; ++j) {
            dist_matrix[i][j] = discrete_distance(cities[i], cities[j]);
        }
    }

    // Taboo experiments
    uint_fast32_t TABU_REPETITIONS = 30;
    std::vector<uint_fast32_t> cooldowns = {5, 10, static_cast<uint_fast32_t>(std::log(n)), static_cast<uint_fast32_t>(std::sqrt(n))};
    std::vector<uint_fast32_t> max_iters = {static_cast<uint_fast32_t>(std::sqrt(n)), 100, 1000, 4000};
    std::vector<uint_fast32_t> sample_sizes = {static_cast<uint_fast32_t>(std::sqrt(n)), n};

    uint_fast32_t bestAVGTabu = -1;
    TabuResult bestTabuConfig;

    for(auto cooldown: cooldowns) {
        for(auto max_iter : max_iters) {
            for(auto sample_size : sample_sizes) {
                std::vector<uint_fast32_t> best_tour_taboo = random_tour(n, GEN);
                uint_fast32_t best_distance = -1;
                uint_fast64_t sum_distance = 0;
                uint_fast64_t sum_iters = 0;

                if(!PRINT_BEST_ONLY) {
                    std::cout << "\n====================[TABU]=====================\n" <<
                        "City: "                            << filename     << 
                        " (" << n << ")"                    << "\n"
                        "Max Iterations w/no improvement: " << max_iter     << "\n" 
                        "Tabu list expire cooldown: "       << cooldown     << "\n"
                        "Sample size for 2-opt: "           << sample_size  << "\n\n";
                }

                for(uint_fast32_t i = 1; i <= TABU_REPETITIONS; ++i){
                    if(DEBUG_ITER) {
                        std::cout<< "Iteration " << i << "/" << TABU_REPETITIONS << "\r" << std::flush;
                    }
                    std::cout << std::flush;

                    TabuResult result = tabu_search(random_tour(n, GEN), dist_matrix, GEN, cooldown, max_iter, sample_size);

                    sum_distance += result.distance;
                    sum_iters += result.iters;

                    if(result.distance < best_distance) {
                        best_tour_taboo = result.tour;
                        best_distance = result.distance;
                    }
                }
                double AVG = static_cast<double>(sum_distance) / TABU_REPETITIONS;
                if(AVG < bestAVGTabu){
                    bestAVGTabu = AVG;
                    bestTabuConfig.max_iters_no_improve = max_iter;
                    bestTabuConfig.sample_size = sample_size;
                    bestTabuConfig.tabu_cooldown = cooldown;
                }

                if(!PRINT_BEST_ONLY) {
                    std::cout << "Average solution value: "     << AVG                                                  << "\n";
                    std::cout << "Average improvement steps: "  << static_cast<double>(sum_iters)    / TABU_REPETITIONS << "\n";
                    std::cout << "Best solution: "              << best_distance                                        << "\n";
                    if(PRINT_TOURS) {
                        std::cout << "\n====================[TOUR]=====================\n";
                        for(uint_fast32_t city_id : best_tour_taboo) {
                            std::cout << city_id << ">";
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n====================[BEST TABU]=====================\n" <<
    "City: "                            << filename     << 
    " (" << n << ")"                    << "\n"         <<
    "Average Solution value: "          << bestAVGTabu                          << "\n" 
    "Max Iterations w/no improvement: " << bestTabuConfig.max_iters_no_improve  << "\n" 
    "Tabu list expire cooldown: "       << bestTabuConfig.tabu_cooldown         << "\n"
    "Sample size for 2-opt: "           << bestTabuConfig.sample_size           << "\n\n";
    

    // Annealing experiments
    uint_fast32_t ANNEALING_REPETITIONS = 30;
    std::vector<double> init_temps = {1000, static_cast<double>(n), static_cast<double>(2*n), static_cast<double>(10*n)};
    std::vector<double> cooling_rates = {0.8, 0.85, 0.9, 0.95, 0.99};
    std::vector<uint_fast32_t> epoch_lengths = {5, 10, static_cast<uint_fast32_t>(std::log(n)), static_cast<uint_fast32_t>(std::sqrt(n))};
    std::vector<uint_fast32_t> max_epochs = {static_cast<uint_fast32_t>(std::sqrt(n)), 100, n, 5*n, 10*n, 30*n};

    uint_fast32_t bestAVGAnnealing = -1;
    SAResult bestAnnealingConfig;

    for(auto init_temp: init_temps) {
        for(auto cooling_rate : cooling_rates) {
            for(auto epoch_lenght : epoch_lengths){
                for(auto max_epoch : max_epochs) {
                    std::vector<uint_fast32_t> best_tour_annealing = random_tour(n, GEN);
                    uint_fast32_t best_distance = -1;
                    uint_fast64_t sum_distance = 0;

                    if(!PRINT_BEST_ONLY) {
                        std::cout << "\n====================[ANNEALING]=====================\n" <<
                            "City: "                            << filename     << 
                            " (" << n << ")"                    << "\n"
                            "Max epochs w/no improvement: "     << max_epoch    << "\n" 
                            "Tries per epoch: "                 << epoch_lenght << "\n"
                            "Cooling rate: "                    << cooling_rate << "\n"
                            "Init Temp: "                       << init_temp    << "\n\n";
                    }

                    for(uint_fast32_t i = 1; i <= ANNEALING_REPETITIONS; ++i) {
                        if(DEBUG_ITER) {
                            std::cout<< "Iteration " << i << "/" << ANNEALING_REPETITIONS << "\r" << std::flush;
                        }
                        std::cout << std::flush;

                        SAResult result = simulated_annealing(random_tour(n, GEN), dist_matrix, GEN, init_temp, cooling_rate, epoch_lenght, max_epoch);

                        sum_distance += result.distance;

                        if(result.distance < best_distance) {
                            best_tour_annealing = result.tour;
                            best_distance = result.distance;
                        }
                    }
                    double AVG = static_cast<double>(sum_distance) / ANNEALING_REPETITIONS;
                    if(AVG < bestAVGAnnealing){
                        bestAVGAnnealing = AVG;
                        bestAnnealingConfig.cooling_rate = cooling_rate;
                        bestAnnealingConfig.epoch_length = epoch_lenght;
                        bestAnnealingConfig.max_epochs_no_improve = max_epoch;
                        bestAnnealingConfig.init_temp = init_temp;
                    }

                    if(!PRINT_BEST_ONLY) {
                        std::cout << "Average solution value: "     << AVG              << "\n";
                        std::cout << "Best solution: "              << best_distance    << "\n";
                        if(PRINT_TOURS) {
                            std::cout << "\n====================[TOUR]=====================\n";
                            for(uint_fast32_t city_id : best_tour_annealing) {
                                std::cout << city_id << ">";
                            }
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n====================[BEST ANNEALING]=====================\n" <<
    "City: "                            << filename     << 
    " (" << n << ")"                    << "\n"         <<
    "Average Solution value: "          << bestAVGAnnealing                             << "\n"    
    "Max epochs w/no improvement: "     << bestAnnealingConfig.max_epochs_no_improve    << "\n" 
    "Tries per epoch: "                 << bestAnnealingConfig.epoch_length             << "\n"
    "Cooling rate: "                    << bestAnnealingConfig.cooling_rate             << "\n"
    "Init Temp: "                       << bestAnnealingConfig.init_temp                << "\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file/directory>\n";
        return 1;
    }
    std::cout << std::fixed << std::setprecision(24);

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