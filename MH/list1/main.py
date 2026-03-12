import numpy as np
import matplotlib.pyplot as plt
import glob
import os
import networkx as nx

def read_tsp_file(FILE_NAME : str) -> np.ndarray:
    '''
        Read TSP data from a file.
    Args:
        FILE_NAME (str): self-explanatory
    Returns:
        np.ndarray: An array of shape (n, 2) containing the x and y coordinates of each city, ordered by their city IDs.
    '''
    UNWANTED_TEXT = {"NODE_COORD_SECTION", "EOF", "COMMENT", "NAME", "TYPE", "DIMENSION", "EDGE_WEIGHT_TYPE"}

    with open(FILE_NAME, 'r') as file:
        lines = file.readlines()
        data = []
        for line in lines:
            if line.strip() and not any(line.startswith(text) for text in UNWANTED_TEXT):
                parts = line.split()
                if len(parts) >= 3:
                    x = float(parts[1])
                    y = float(parts[2])
                    data.append((x, y))
        return np.array(data)


def calculate_distance(permutation: np.ndarray, coordinates: np.ndarray) -> float:
    '''
        Calculate the total distance of TSP.
    Args:
        permutation (np.ndarray): An array representing the order of cities.
        coordinates (np.ndarray): An array of shape (n, 2) containing 
                                  the x and y coordinates of each city,
                                  ordered by their city IDs.
    Returns:
        float: The total distance.
    '''
    dist = 0.0
    for i in range(len(permutation)):
        current_city = permutation[i]
        next_city = permutation[(i + 1) % len(permutation)]
        dist += np.linalg.norm(coordinates[current_city] - coordinates[next_city])
    return dist

def plot_results(name: str, m10: np.ndarray, m50: np.ndarray, coords: np.ndarray, best_p: np.ndarray, global_min: float) -> None:
    fig = plt.figure(figsize=(15, 5))
    
    # 1. Plot the best tour found
    ax1 = fig.add_subplot(1, 3, 1)
    ordered = coords[best_p]
    ordered = np.vstack([ordered, ordered[0]])
    ax1.plot(ordered[:, 0], ordered[:, 1], 'r-o', markersize=2, linewidth=1)
    ax1.set_title(f"Best Tour: {global_min:.2f}")
    
    # 2. Plot Mins of groups of 10 
    ax2 = fig.add_subplot(1, 3, 2)
    ax2.bar(range(len(m10)), m10, color='skyblue')
    ax2.axhline(np.mean(m10), color='blue', linestyle='--', label=f'Avg: {np.mean(m10):.0f}')
    ax2.set_title("Mins of 10-draw groups")
    ax2.legend()

    # 3. Plot Mins of groups of 50
    ax3 = fig.add_subplot(1, 3, 3)
    ax3.bar(range(len(m50)), m50, color='salmon')
    ax3.axhline(np.mean(m50), color='red', linestyle='--', label=f'Avg: {np.mean(m50):.0f}')
    ax3.set_title("Mins of 50-draw groups")
    ax3.legend()

    plt.suptitle(f"Dataset: {name}")
    plt.tight_layout()
    plt.savefig(f"./results/{name}_results.png")
    plt.show()

def run_experiment_random(file_path: str, TRIALS: int = 1000) -> None:
    DATA = read_tsp_file(file_path)
    file_name = os.path.basename(file_path)
    
    distances = []
    best_dist = float('inf')
    best_perm = None

    # Perform {TRIALS} trials
    for _ in range(TRIALS):
        perm = np.random.permutation(len(DATA))
        dist = calculate_distance(perm, DATA)
        distances.append(dist)
        
        if dist < best_dist:
            best_dist = dist
            best_perm = perm

    distances = np.array(distances)

    mins_10 = np.min(distances.reshape(100, 10), axis=1)
    avg_min_10 = np.mean(mins_10)

    mins_50 = np.min(distances.reshape(20, 50), axis=1)
    avg_min_50 = np.mean(mins_50)

    global_min = np.min(distances)

    print(f"\nRESULTS FOR {file_name}:")
    print(f"Average of mins (groups of 10): {avg_min_10:.2f}")
    print(f"Average of mins (groups of 50): {avg_min_50:.2f}")
    print(f"Global Minimum found: {global_min:.2f}")

    plot_results(f"{file_name}_random", mins_10, mins_50, DATA, best_perm, global_min)

def run_experiment_MST(file_path: str, TRIALS: int = 1000) -> None:
    DATA = read_tsp_file(file_path)
    file_name = os.path.basename(file_path)
    num_nodes = len(DATA)
    
    G = nx.Graph()
    for i in range(num_nodes):
        for j in range(i + 1, num_nodes):
            d = np.linalg.norm(DATA[i] - DATA[j])
            G.add_edge(i, j, weight=d)
    
    mst = nx.minimum_spanning_tree(G)
    mst_weight = mst.size(weight='weight')
    
    distances = []
    best_dist = float('inf')
    best_perm = None

    for _ in range(TRIALS):
        start_node = np.random.randint(0, num_nodes)

        perm = list(nx.dfs_preorder_nodes(mst, source=start_node))
        perm = np.array(perm)
        
        dist = calculate_distance(perm, DATA)
        distances.append(dist)
        
        if dist < best_dist:
            best_dist = dist
            best_perm = perm

    distances = np.array(distances)

    mins_10 = np.min(distances.reshape(100, 10), axis=1)
    avg_min_10 = np.mean(mins_10)

    mins_50 = np.min(distances.reshape(20, 50), axis=1)
    avg_min_50 = np.mean(mins_50)

    global_min = np.min(distances)

    print(f"\n--- MST TRIALS RESULTS FOR {file_name} ---")
    print(f"Fixed MST Weight: {mst_weight:.2f}")
    print(f"Best MST-based Tour found: {global_min:.2f}")
    print(f"Average of mins (groups of 10): {avg_min_10:.2f}")
    print(f"Average of mins (groups of 50): {avg_min_50:.2f}")

    plot_results(f"{file_name}_MST_trials", mins_10, mins_50, DATA, best_perm, global_min)

TRIALS = 1000
for tsp_file in glob.glob('./data/*.tsp'):
    run_experiment_random(tsp_file)
    run_experiment_MST(tsp_file)
