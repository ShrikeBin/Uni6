import numpy as np
import matplotlib.pyplot as plt
import glob
import os
from tqdm import tqdm

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
        Optimized distance calculation using numpy vectorized operations.
    '''
    ordered_coords = coordinates[permutation]
    next_coords = np.roll(ordered_coords, -1, axis=0)
    distances = np.linalg.norm(ordered_coords - next_coords, axis=1)
    
    return np.sum(distances)

def get_best_neighbor_naive(permutation: np.ndarray, coordinates: np.ndarray) -> (np.ndarray, float):
    '''
        Browse all 2-opt neighbors and return the best one.
    '''
    # TODO: Implement this to actually work and work efficiently.
    pass

def naive_local_search(coordinates: np.ndarray, init: np.ndarray) -> np.ndarray:
    '''
        Naive local search exploring all 2-opt neighbors.
    '''
    best_perm = init.copy()
    best_dist = calculate_distance(best_perm, coordinates)
    improved = True

    while improved:
        improved = False
        new_perm, new_distance = get_best_neighbor_naive(best_perm, coordinates)
        if new_distance < best_dist:
            best_perm = new_perm
            best_dist = new_distance
            improved = True
    return best_perm

for tsp_file in glob.glob('./data/*.tsp'):
    print(f"Processing {tsp_file}...")
    coordinates = read_tsp_file(tsp_file)
    n = len(coordinates)
    
    best_overall_perm = None
    best_overall_dist = float('inf')
    
    for _ in tqdm(range(TRIALS), desc="Trials"):
        init_perm = np.random.permutation(n)
        best_perm = naive_local_search(coordinates, init_perm)
        best_dist = calculate_distance(best_perm, coordinates)
        
        if best_dist < best_overall_dist:
            best_overall_dist = best_dist
            best_overall_perm = best_perm
    
    print(f"Best distance for {tsp_file}: {best_overall_dist}")
