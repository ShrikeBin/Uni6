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
        } else {
            std::cerr << "Error parsing line: " << line << "\n";
        }
    }
    return cities;
}

std::vector<uint_fast32_t> random_tour(int num_cities, std::mt19937& GEN){
    std::vector<uint_fast32_t> tour(num_cities);
    std::iota(tour.begin(), tour.end(), 1);
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

    if (i == 0 && j == n - 1 || i == j || i == n - 1 && j == 0) return 0;

    uint_fast32_t prev_a_idx = (i == 0) ? n - 1 : i - 1;
    uint_fast32_t next_b_idx = (j == n - 1) ? 0 : j + 1;

    uint_fast32_t a = tour[i] - 1;
    uint_fast32_t b = tour[j] - 1;
    uint_fast32_t prev_a = tour[prev_a_idx] - 1;
    uint_fast32_t next_b = tour[next_b_idx] - 1;

    uint_fast32_t old_distance = dist_matrix[prev_a][a] + dist_matrix[b][next_b];
    uint_fast32_t new_distance = dist_matrix[prev_a][b] + dist_matrix[a][next_b];

    return new_distance - old_distance;
}

LocalSearchResult local_search_naive(std::vector<uint_fast32_t> initial_tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix) {
    uint_fast32_t improvement_steps = 0;
    uint_fast32_t initial_distance = tour_distance(initial_tour, dist_matrix);
    uint_fast32_t current_distance = initial_distance;

    bool improvement = true;

    while(improvement) {
        improvement = false;

        int_fast32_t best_difference = 0;
        uint_fast32_t best_i = -1;
        uint_fast32_t best_j = -1;

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
        if(best_difference < 0) {
            std::reverse(initial_tour.begin() + best_i, initial_tour.begin() + best_j + 1); 
            current_distance += best_difference;
            ++improvement_steps;
            improvement = true;
        }
    }
    return LocalSearchResult{initial_tour, current_distance, improvement_steps};
}

LocalSearchResult local_search_n_random(std::vector<uint_fast32_t> initial_tour, const std::vector<std::vector<uint_fast32_t>>& dist_matrix, std::mt19937& GEN) {
  uint_fast32_t improvement_steps = 0;
  uint_fast32_t initial_distance = tour_distance(initial_tour, dist_matrix);
  uint_fast32_t current_distance = initial_distance;
  uint_fast32_t n = initial_tour.size();

  bool improvement = true;
  std::uniform_int_distribution<> dist(0, n - 1);

  while(improvement) {
    improvement = false;

    int_fast32_t best_difference = 0;
    uint_fast32_t best_i = -1;
    uint_fast32_t best_j = -1;

    for(uint_fast32_t k = 0; k < n; ++k) {
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

    if(best_difference < 0) {
        std::reverse(initial_tour.begin() + best_i, initial_tour.begin() + best_j + 1); 
        current_distance += best_difference;
        improvement_steps++;
        improvement = true;
    }
  }
  return LocalSearchResult{initial_tour, current_distance, improvement_steps};
}






///TODO: FIX THOSE
MSTresult prim_mst(const std::vector<City>& cities, const std::vector<std::vector<uint_fast32_t>>& dist_matrix) {
    int n = cities.size();
    
    // pomocnicza
    std::vector<bool> visited(n, false);
    
    // min_dist[i] będzie przechowywać minimalną odległość od drzewa MST do wierzchołka i
    std::vector<int> min_dist(n, std::numeric_limits<int>::max());
    
    // -1 brak rodzica
    std::vector<int> parent(n, -1); 

    std::vector<std::pair<uint_fast32_t, uint_fast32_t>> edges;
    uint_fast32_t total_weight = 0;

    min_dist[0] = 0;

    for (int step = 0; step < n; ++step) {
        // Znajdź nieodwiedzony wierzchołek z najmniejszym dystansem do MST
        int u = -1;
        int current_min = std::numeric_limits<int>::max();
        
        for (int i = 0; i < n; ++i) {
            if (!visited[i] && min_dist[i] < current_min) {
                current_min = min_dist[i];
                u = i;
            }
        }

        visited[u] = true;    // już odwiedzony
        
        if (parent[u] != -1) {
          // zapisanie id miast zamiast indeksów
            edges.push_back({cities[parent[u]].id, cities[u].id});
            total_weight += current_min;
        }

        // aktualizacja min_dist dla sąsiadów u
        for (int v = 0; v < n; ++v) {
            if (!visited[v]) {
                int dist = dist_matrix[u][v];
                if (dist < min_dist[v]) {
                    min_dist[v] = dist;
                    parent[v] = u; // wierzchołek u jest najlepszym rodzicem dla v
                }
            }
        }
    }
    return {edges, total_weight};
}

void dfs(int u, const std::vector<std::vector<uint_fast32_t>>& adj, std::vector<bool>& visited, std::vector<uint_fast32_t>& tour) {
    visited[u] = true;
    tour.push_back(u);
    
    for (int v : adj[u]) {
        if (!visited[v]) {
            dfs(v, adj, visited, tour);
        }
    }
}

std::vector<int> mst_to_tsp(const std::vector<std::pair<int, int>>& mst_edges, int num_vertices, int start_vertex) {

    std::vector<std::vector<int>> adj(num_vertices + 1);
    
    // lista sąsiedztwa dla drzewa MST
    for (const auto& edge : mst_edges) {
        adj[edge.first].push_back(edge.second);
        adj[edge.second].push_back(edge.first);
    }

    std::vector<bool> visited(num_vertices + 1, false);
    std::vector<int> initial_tour;
    
    // konstruowanie TSP poprzez DFS po drzewie MST poczynając od start_vertex
    dfs(start_vertex, adj, visited, initial_tour);
    
    return initial_tour;
}