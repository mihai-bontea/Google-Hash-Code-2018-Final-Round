from dataclasses import dataclass
from typing import List, Tuple

import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
import numpy as np

@dataclass
class Building:
    kind: str
    height: int
    width: int
    res_or_type: int
    walls: List[Tuple[int, int]]

def read_building(f) -> Building:
    kind, height, width, res_or_type = f.readline().strip().split()
    height, width, res_or_type = int(height), int(width), int(res_or_type)

    walls = [(i, j)
             for i in range(height)
             for j, ch in enumerate(f.readline().strip())
             if ch == "#"]

    return Building(kind, height, width, res_or_type, walls)


@dataclass
class Solution:
    buildings_placed: int
    id_at_coord: List[Tuple[int, Tuple[int, int]]]

def read_solution(filename) -> Solution:
    with open(filename, 'r') as f:
        buildings_placed = int(f.readline().strip())
        id_at_coord = [(project_id, (i, j)) 
                    for _ in range(buildings_placed)
                    for project_id, i, j in [map(int, f.readline().strip().split())]]
    
    return Solution(buildings_placed, id_at_coord)


def visualize_map(height, width, buildings, placements, output_path):
    grid = np.full((height, width), -1, dtype=int)
    color_map = np.full((height, width, 3), 1.0)

    for b_id, (px, py) in placements:
        building = buildings[b_id]
        for wi, wj in building.walls:
            i, j = px + wi, py + wj
            if 0 <= i < height and 0 <= j < width:
                grid[i, j] = b_id
                if building.kind == "R":
                    color_map[i, j] = mcolors.to_rgb("turquoise")
                else:
                    color_map[i, j] = mcolors.to_rgb("slateblue")

    fig, ax = plt.subplots(figsize=(6, 6))
    ax.imshow(color_map, origin="upper")

    # Hide ticks and grid lines
    ax.set_xticks([])
    ax.set_yticks([])

    # Save to file
    plt.savefig(output_path, dpi=300, bbox_inches="tight")
    plt.close(fig)
    print(f"Map saved to {output_path}")


solutions = ["sol1", "sol2"]
input_files = ["a_example", "b_short_walk", "c_going_green", "d_wide_selection", "e_precise_fit", "f_different_footprints"]

for input_file in input_files:
    with open(f"input_files/{input_file}.in", 'r') as f:
        city_height, city_width, max_walking_dist, nr_building_projects = list(map(int, f.readline().split()))
        buildings = [read_building(f) for _ in range(nr_building_projects)]

        for sol in solutions:
            output_file = f"output_files/{sol}/{input_file}.out"
            try:
                solution = read_solution(output_file)

                image_output_path = f"visualizers/{sol}/{input_file}.png"
                visualize_map(city_height, city_width, buildings, solution.id_at_coord, image_output_path)

            except FileNotFoundError:
                print(f"Error: File '{output_file}' not found.")