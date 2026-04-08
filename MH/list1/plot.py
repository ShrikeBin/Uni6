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
        [WMSP]  Weight of the Minimum Spanning Tree:    int
        [ASV]   Average solution value:                 float
        [AIS]   Average improvement steps:              float
        [BS]    Best solution:                          int
        [TOUR]  Best tour:                              List[int]
    '''
    
    city = ""
    n = 0
    # =============
    asv1 = 0
    ais1 = 0
    bs1 = 0
    tour1 = []
    # =============
    asv2 = 0
    ais2 = 0
    bs2 = 0
    tour2 = []
    # =============
    wmsp3 = 0
    asv3 = 0
    ais3 = 0
    bs3 = 0
    tour3 = []
    
    def __str__(self):
        return ("===================================================\n" +  
        f"[City]:           {self.city}\n" + \
        f"[Vertex cout]:    {self.n}\n" + \
        "===================================================\n" + \
        f"[TASK 1]\n\
        Average solution value:                 {self.asv1}\n\
        Average improvement steps:              {self.ais1}\n\
        Best solution value:                    {self.bs1}\n" + \
        f"[TASK 2]\n\
        Average solution value:                 {self.asv2}\n\
        Average improvement steps:              {self.ais2}\n\
        Best solution value:                    {self.bs2}\n" + \
        f"[TASK 3]\n\
        Weight of the Minimum Spanning Tree:    {self.wmsp3}\n\
        Average solution value:                 {self.asv3}\n\
        Average improvement steps:              {self.ais3}\n\
        Best solution value:                    {self.bs3}\n")
    

BLOCK = "------------------------------------------"
TOUR_START = "====================[TOUR]====================="
TOUR_END = "==============================================="

def read_output_data(FILE_NAME: str = "out.dat"):
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
            for line in block.split("TASK"):
                if line.startswith(" 1"):
                    sol.asv1 = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.ais1 = line.split("Average improvement steps: ")[1].split("\n")[0]
                    sol.bs1 = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tour1 = [int(x) + 1 for x in line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">") if x]
                elif line.startswith(" 2"):
                    sol.asv2 = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.ais2 = line.split("Average improvement steps: ")[1].split("\n")[0]
                    sol.bs2 = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tour2 = [int(x) + 1 for x in line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">") if x]
                elif line.startswith(" 3"):
                    sol.wmsp3 = line.split("Weight of the Minimum Spanning Tree (MST): ")[1].split("\n")[0]
                    sol.asv3 = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.ais3 = line.split("Average improvement steps: ")[1].split("\n")[0]
                    sol.bs3 = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tour3 = [int(x) + 1 for x in line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">") if x]
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
            "num": 1, "tour": sol.tour1, "bs": sol.bs1,
            "stats": f"Avg Val: {sol.asv1}\nAvg Steps: {sol.ais1}\nBest: {sol.bs1}"
        },
        {
            "num": 2, "tour": sol.tour2, "bs": sol.bs2,
            "stats": f"Avg Val: {sol.asv2}\nAvg Steps: {sol.ais2}\nBest: {sol.bs2}"
        },
        {
            "num": 3, "tour": sol.tour3, "bs": sol.bs3,
            "stats": f"MST: {sol.wmsp3}\nAvg Val: {sol.asv3}\nAvg Steps: {sol.ais3}\nBest: {sol.bs3}"
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
        
        plt.title(f"{sol.city} - Task {task['num']}", fontsize=15, fontweight='bold')
        ax.text(0.02, 0.98, f"[TASK {task['num']}]\n{task['stats']}", transform=ax.transAxes, 
                fontsize=8, family='monospace', va='top', bbox=dict(facecolor='white', alpha=0.7))
        ax.set_aspect('equal')
        plt.savefig(f"./results/{clean_city}_T{task['num']}.png", dpi=600, bbox_inches='tight')
        plt.close(fig)

    # 2. GENERATE SUMMRAY PLOT
    plt.style.use('seaborn-v0_8-muted')
    fig, axes = plt.subplots(1, 3, figsize=(20, 8), facecolor='#f8f9fa')
    
    for i, task in enumerate(tasks):
        ax = axes[i]
        ax.set_facecolor('white')
        
        ax.scatter(coords[:, 0], coords[:, 1], c='#2c3e50', s=0.8, alpha=0.5)
        
        if task["tour"]:
            indices = np.array(task["tour"]) - 1
            ordered = coords[indices]
            ordered = np.vstack([ordered, ordered[0]])
            colors = ['#3498db', '#e74c3c', '#2ecc71']
            ax.plot(ordered[:, 0], ordered[:, 1], color=colors[i], linewidth=0.6, alpha=0.9)

        ax.set_title(f"{sol.city.upper()} - TASK {task['num']}", fontsize=14, fontweight='bold', color='#2c3e50')
        ax.text(0.05, 0.05, task['stats'], transform=ax.transAxes, fontsize=9, 
                family='monospace', bbox=dict(boxstyle='round', facecolor='white', edgecolor='#bdc3c7'))
        
        ax.set_aspect('equal')
        ax.set_xticks([]); ax.set_yticks([])

    plt.tight_layout(rect=[0, 0.03, 1, 0.90])
    
    plt.savefig(f"./results/{clean_city}_SUMMARY.png", dpi=600, bbox_inches='tight')
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
