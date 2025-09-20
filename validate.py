from dataclasses import dataclass
from typing import List, Tuple

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


solutions = ["sol1"]
input_files = ["a_example", "b_short_walk", "c_going_green", "d_wide_selection", "e_precise_fit", "f_different_footprints"]

for input_file in input_files:
    with open(f"input_files/{input_file}.in", 'r') as f:
        city_height, city_width, max_walking_dist, nr_building_projects = list(map(int, f.readline().split()))
        buildings = [read_building(f) for _ in range(nr_building_projects)]