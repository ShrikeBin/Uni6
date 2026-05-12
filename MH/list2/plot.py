import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass
import matplotlib.pyplot as plt
import numpy as np
import os
from dataclasses import dataclass
from typing import List

@dataclass
class Solution:
    '''
        [ASV]   Average solution value:                 float
        [BS]    Best solution:                          int
        [TOUR]  Best tour:                              List[int]
    '''
    
    city = ""
    n = 0
    # =============
    asvT = 0
    bsT = 0
    tourT = []
    # =============
    asvA = 0
    bsA = 0
    tourA = []
    
    def __str__(self):
        return ("===================================================\n" +  
        f"[City]:           {self.city}\n" + \
        f"[Vertex cout]:    {self.n}\n" + \
        "===================================================\n" + \
        f"[TABU]\n\
        Average solution value:                 {self.asvT}\n\
        Best solution value:                    {self.bsT}\n" + \
        f"[ANNEALING]\n\
        Average solution value:                 {self.asvA}\n\
        Best solution value:                    {self.bsA}\n")


BLOCK = "------------------------------------------"
TOUR_START = "===================[TOUR]==================="
TOUR_END =   "===================[END]===================="

def read_output_data(FILE_NAME: str = "run/outbest.dat"):
    with open(FILE_NAME, 'r') as file:
        full = file.read().split(BLOCK)
        data = []
        for block in full:
            if block == "\n":
                continue
            sol = Solution()
            for line in block.split("\n"):
                if line.startswith("Loaded file:"):
                    name = line.split("/")[-1].split("|")[0].strip()
                    vertex_n = line.split("/")[-1].split(":")[1].strip()
                    sol.city = name
                    sol.n = vertex_n
            for line in block.split("====================["):
                if line.startswith("TABU"):
                    sol.asvT = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.bsT = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tourT = [int(x) + 1 for x in line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">") if x]
                elif line.startswith("ANNEALING"):
                    sol.asvA = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.bsA = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tourA = [int(x) + 1 for x in line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">") if x]
            data.append(sol)
        return data

def read_tsp_file(FILE_NAME : str) -> np.ndarray:
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

def plot_results(sol: Solution, coords: np.ndarray) -> None:
    tasks = [
        {
            "num": 1, "label": "TABU", "tour": sol.tourT, "bs": sol.bsT,
            "stats": f"Avg Val: {sol.asvT}\nBest: {sol.bsT}"
        },
        {
            "num": 2, "label": "ANNEALING", "tour": sol.tourA, "bs": sol.bsA,
            "stats": f"Avg Val: {sol.asvA}\nBest: {sol.bsA}"
        }
    ]
    os.makedirs("./results", exist_ok=True)
    clean_city = sol.city.replace(".tsp", "")

    # 1. GENERATE INDIVIDUAL HIGH-RES PLOTS
    for task in tasks:
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.scatter(coords[:, 0], coords[:, 1], c='black', s=0.5, zorder=1)
        if task["tour"]:
            indices = np.array(task["tour"]) - 1
            ordered = coords[indices]
            ordered = np.vstack([ordered, ordered[0]])
            ax.plot(ordered[:, 0], ordered[:, 1], color='red', linewidth=0.4, alpha=0.8)
        plt.title(f"{sol.city} - Task {task['num']} ({task['label']})", fontsize=15, fontweight='bold')
        ax.text(0.02, 0.98, f"[{task['label']}]\n{task['stats']}", transform=ax.transAxes,
                fontsize=8, family='monospace', va='top', bbox=dict(facecolor='white', alpha=0.7))
        ax.set_aspect('equal')
        plt.savefig(f"./results/{clean_city}_T{task['num']}_{task['label']}.png", dpi=600, bbox_inches='tight')
        plt.close(fig)

    # 2. GENERATE SUMMARY PLOT
    plt.style.use('seaborn-v0_8-muted')
    fig, axes = plt.subplots(1, 2, figsize=(16, 8), facecolor='#f8f9fa')
    colors = ['#3498db', '#e74c3c']

    for i, task in enumerate(tasks):
        ax = axes[i]
        ax.set_facecolor('white')
        ax.scatter(coords[:, 0], coords[:, 1], c='#2c3e50', s=0.8, alpha=0.5)
        if task["tour"]:
            indices = np.array(task["tour"]) - 1
            ordered = coords[indices]
            ordered = np.vstack([ordered, ordered[0]])
            ax.plot(ordered[:, 0], ordered[:, 1], color=colors[i], linewidth=0.6, alpha=0.9)
        ax.set_title(f"{sol.city.upper()} - {task['label']}", fontsize=14, fontweight='bold', color='#2c3e50')
        ax.text(0.05, 0.05, task['stats'], transform=ax.transAxes, fontsize=9,
                family='monospace', bbox=dict(boxstyle='round', facecolor='white', edgecolor='#bdc3c7'))
        ax.set_aspect('equal')
        ax.set_xticks([]); ax.set_yticks([])

    plt.suptitle(f"{sol.city.upper()} — Tabu vs Annealing", fontsize=16, fontweight='bold', color='#2c3e50', y=1.01)
    plt.tight_layout(rect=[0, 0.03, 1, 0.97])
    plt.savefig(f"./results/{clean_city}_SUMMARY.png", dpi=250, bbox_inches='tight')
    plt.show()
    plt.close(fig)
    plt.style.use('default')

def visualize_all(solutions: List[Solution]) -> None:
    for sol in solutions:
        filename = sol.city if sol.city.endswith(".tsp") else f"{sol.city}.tsp"
        path = os.path.join("data", filename)
        
        if os.path.exists(path):
            coords = read_tsp_file(path)
            plot_results(sol, coords)
        else:
            print(f"Skipping {sol.city}: File not found at {path}")
    
visualize_all(read_output_data())

print(read_output_data())
