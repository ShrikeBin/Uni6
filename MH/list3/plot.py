import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass, field
import os
from typing import List

@dataclass
class AlgorithmResult:
    label: str = ""
    best_distance: float = 0
    avg_distance: float = 0
    avg_generations: float = 0
    tour: List[int] = field(default_factory=list)
    params: dict = field(default_factory=dict)

@dataclass
class Solution:
    city: str = ""
    n: int = 0
    elapsed: float = 0.0
    results: List[AlgorithmResult] = field(default_factory=list)

    def __str__(self):
        lines = [
            "===================================================",
            f"[City]:        {self.city}",
            f"[Vertices]:    {self.n}",
            f"[Time]:        {self.elapsed:.4f}s",
            "===================================================",
        ]
        for r in self.results:
            lines.append(f"[{r.label}]")
            lines.append(f"  Best distance:  {r.best_distance}")
            lines.append(f"  Avg distance:   {r.avg_distance}")
        return "\n".join(lines)


BLOCK_SEP = "------------------------------------------"
TOUR_KEY = "Best tour:"


def parse_algorithm_block(header: str, body: str) -> AlgorithmResult:
    result = AlgorithmResult(label=header.strip())
    params = {}

    for line in body.splitlines():
        line = line.strip()
        if not line:
            continue
        if line.startswith(TOUR_KEY):
            tour_str = line[len(TOUR_KEY):].strip().strip("[]")
            result.tour = [int(x) + 1 for x in tour_str.split(">") if x.strip().lstrip('-').isdigit()]
        elif "Best distance:" in line:
            try:
                result.best_distance = float(line.split("Best distance:")[1].strip())
            except ValueError:
                pass
        elif "Average distance:" in line:
            try:
                result.avg_distance = float(line.split("Average distance:")[1].strip())
            except ValueError:
                pass
        elif "Avg generations:" in line:
            try:
                result.avg_generations = float(line.split("Avg generations:")[1].strip())
            except ValueError:
                pass
        elif ":" in line:
            key, _, val = line.partition(":")
            params[key.strip()] = val.strip()

    result.params = params
    return result


def read_output_data(FILE_NAME: str = "run/out.dat") -> List[Solution]:
    with open(FILE_NAME, 'r') as f:
        content = f.read()

    solutions = []
    blocks = content.split(BLOCK_SEP)

    for block in blocks:
        block = block.strip()
        if not block:
            continue

        sol = Solution()

        # Parse "Loaded file:" line
        for line in block.splitlines():
            line = line.strip()
            if line.startswith("Loaded file:"):
                parts = line.split("|")
                path_part = parts[0].replace("Loaded file:", "").strip()
                sol.city = os.path.basename(path_part)
                if len(parts) > 1 and "Number of vertices" in parts[1]:
                    try:
                        sol.n = int(parts[1].split(":")[1].strip())
                    except ValueError:
                        pass
            elif line.startswith("Finished") and "seconds" in line:
                try:
                    sol.elapsed = float(line.split("in")[1].replace("seconds.", "").strip())
                except (ValueError, IndexError):
                    pass

        # Split by algo section headers: ====================[...]=====================
        import re
        sections = re.split(r'={10,}\[(.+?)\]={10,}', block)
        # sections: [preamble, header1, body1, header2, body2, ...]
        it = iter(sections[1:])
        for header, body in zip(it, it):
            algo = parse_algorithm_block(header, body)
            sol.results.append(algo)

        if sol.city or sol.results:
            solutions.append(sol)

    return solutions


def read_tsp_file(FILE_NAME: str) -> np.ndarray:
    UNWANTED_TEXT = {"NODE_COORD_SECTION", "EOF", "COMMENT", "NAME", "TYPE", "DIMENSION", "EDGE_WEIGHT_TYPE"}
    with open(FILE_NAME, 'r') as file:
        lines = file.readlines()
    data = []
    for line in lines:
        if line.strip() and not any(line.startswith(text) for text in UNWANTED_TEXT):
            parts = line.split()
            if len(parts) >= 3:
                try:
                    x = float(parts[1])
                    y = float(parts[2])
                    data.append((x, y))
                except ValueError:
                    pass
    return np.array(data)


def plot_results(sol: Solution, coords: np.ndarray) -> None:
    os.makedirs("./results", exist_ok=True)
    clean_city = sol.city.replace(".tsp", "")
    n_algos = len(sol.results)

    colors = ['#3498db', '#e74c3c', '#2ecc71', '#f39c12', '#9b59b6', '#1abc9c']

    # Individual high-res plots
    for i, res in enumerate(sol.results):
        fig, ax = plt.subplots(figsize=(10, 10))
        ax.scatter(coords[:, 0], coords[:, 1], c='black', s=0.5, zorder=1)
        if res.tour:
            indices = np.array(res.tour) - 1
            ordered = coords[indices]
            ordered = np.vstack([ordered, ordered[0]])
            ax.plot(ordered[:, 0], ordered[:, 1], color=colors[i % len(colors)], linewidth=0.4, alpha=0.8)
        ax.set_title(f"{sol.city} — {res.label}", fontsize=13, fontweight='bold')
        stats = f"Best: {res.best_distance}\nAvg:  {res.avg_distance}"
        ax.text(0.02, 0.98, stats, transform=ax.transAxes, fontsize=8,
                family='monospace', va='top', bbox=dict(facecolor='white', alpha=0.7))
        ax.set_aspect('equal')
        safe_label = res.label.replace(" ", "_").replace("/", "-")
        plt.savefig(f"./results/{clean_city}_{safe_label}.png", dpi=300, bbox_inches='tight')
        plt.close(fig)

    # Summary plot
    plt.style.use('seaborn-v0_8-muted')
    cols = min(2, n_algos)
    rows = (n_algos + 1) // 2
    fig, axes = plt.subplots(rows, cols, figsize=(cols * 8, rows * 7), facecolor='#f8f9fa')
    axes_flat = np.array(axes).flatten() if n_algos > 1 else [axes]

    for i, res in enumerate(sol.results):
        ax = axes_flat[i]
        ax.set_facecolor('white')
        ax.scatter(coords[:, 0], coords[:, 1], c='#2c3e50', s=0.6, alpha=0.4)
        if res.tour:
            indices = np.array(res.tour) - 1
            ordered = coords[indices]
            ordered = np.vstack([ordered, ordered[0]])
            ax.plot(ordered[:, 0], ordered[:, 1], color=colors[i % len(colors)], linewidth=0.5, alpha=0.9)
        ax.set_title(res.label, fontsize=11, fontweight='bold', color='#2c3e50')
        stats = f"Best: {res.best_distance}\nAvg:  {res.avg_distance}"
        ax.text(0.05, 0.05, stats, transform=ax.transAxes, fontsize=8,
                family='monospace', bbox=dict(boxstyle='round', facecolor='white', edgecolor='#bdc3c7'))
        ax.set_aspect('equal')
        ax.set_xticks([])
        ax.set_yticks([])

    # Hide unused subplots
    for j in range(n_algos, len(axes_flat)):
        axes_flat[j].set_visible(False)

    plt.suptitle(f"{sol.city.upper()} — Algorithm Comparison", fontsize=15,
                 fontweight='bold', color='#2c3e50', y=1.01)
    plt.tight_layout()
    plt.savefig(f"./results/{clean_city}_SUMMARY.png", dpi=200, bbox_inches='tight')
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


solutions = read_output_data()
for s in solutions:
    print(s)
visualize_all(solutions)