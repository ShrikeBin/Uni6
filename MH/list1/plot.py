import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass


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
                    sol.tour1 = line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">")
                elif line.startswith(" 2"):
                    sol.asv2 = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.ais2 = line.split("Average improvement steps: ")[1].split("\n")[0]
                    sol.bs2 = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tour2 = line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">")
                elif line.startswith(" 3"):
                    sol.wmsp3 = line.split("Weight of the Minimum Spanning Tree (MST): ")[1].split("\n")[0]
                    sol.asv3 = line.split("Average solution value: ")[1].split("\n")[0]
                    sol.ais3 = line.split("Average improvement steps: ")[1].split("\n")[0]
                    sol.bs3 = line.split("Best solution: ")[1].split("\n")[0]
                    sol.tour3 = line.split(TOUR_START)[1].split(TOUR_END)[0].strip().split(">")
            data.append(sol)
        return data
    
def plot_results(sol: Solution) -> None:
    fig = plt.figure(figsize=(15, 5))
    
    # 1. Plot the best tour found
    ax1 = fig.add_subplot(1, 3, 1)
    ordered = coords[best_p]
    ordered = np.vstack([ordered, ordered[0]])
    ax1.plot(ordered[:, 0], ordered[:, 1], 'r-o', markersize=2, linewidth=1)
    ax1.set_title(f"Best Tour: {sol.bs1}")
    
    # 2. Plot the best tour found
    ax1 = fig.add_subplot(1, 3, 1)
    ordered = coords[best_p]
    ordered = np.vstack([ordered, ordered[0]])
    ax1.plot(ordered[:, 0], ordered[:, 1], 'r-o', markersize=2, linewidth=1)
    ax1.set_title(f"Best Tour: {sol.bs2}")
    
    # 3. Plot the best tour found
    ax1 = fig.add_subplot(1, 3, 1)
    ordered = coords[best_p]
    ordered = np.vstack([ordered, ordered[0]])
    ax1.plot(ordered[:, 0], ordered[:, 1], 'r-o', markersize=2, linewidth=1)
    ax1.set_title(f"Best Tour: {sol.bs3}")

    plt.suptitle(f"Dataset: {sol.city}")
    plt.tight_layout()
    plt.savefig(f"./results/{sol.city.split(".tsp")[0]}_results.png")
    plt.show()
    
for solution in read_output_data():
    print(solution)