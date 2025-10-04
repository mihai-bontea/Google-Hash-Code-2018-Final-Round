# Google Hash Code 2018 Final Round (City Plan)

>The population of the world is growing and becoming increasingly concentrated in cities. According to the World Bank, global urbanization (the percentage of the world’s population that lives in cities) crossed 50% in 2008 and reached 54% in 2016¹.

>The growth of urban areas creates interesting architectural challenges. How can city planners make efficient use of urban space? How should residential needs be balanced with access to public utilities, such as schools and parks?

>**Given a set of building projects, your task is to decide which of the available projects to build and where, in
order to maximize residential capacity and availability of utilities to residents.**

## Solution 1

### Strategy

Go over the city map in order, and at every step, choose the best scoring building that fits into the given space. A collision map ensures that no buildings overlap. The process of choosing the best building at every step is computationally intensive, so it's split on 12 parallel threads.

### Scoring

| File Name              |      Score | Avg Utility Access | Coverage % |
| ---------------------- | ---------: | -----------------: | ---------: |
| a_example              |        100 |                2.0 |       71.4 |
| b_short_walk           |  3,825,406 |                6.9 |       97.3 |
| c_going_green          |  8,770,728 |               12.3 |      100.0 |
| d_wide_selection       |  7,587,032 |               16.3 |       93.6 |
| e_precise_fit          |  4,795,290 |                1.0 |       99.5 |
| f_different_footprints |  4,940,263 |                6.4 |      100.0 |
| **Final**              | **29,918,819** |                    |            |


## Solution 2

### Strategy

Builds on the first solution; a greedy solution is obtained(although now buildings are penalized proportionally with size), and then a hill climbing algorithm is performed (tied to a 10 minute timer). Random buildings are removed and replaced with a better scoring building (if it's not already the best for that position).

### Scoring

| File Name              |          Score | Avg Utility Access | Coverage % |
| ---------------------- | -------------: | -----------------: | ---------: |
| a_example              |            100 |                2.0 |       71.4 |
| b_short_walk           |      4,895,099 |                8.3 |       98.3 |
| c_going_green          |      8,440,605 |               11.7 |       99.7 |
| d_wide_selection       |      7,897,054 |               16.4 |       95.3 |
| e_precise_fit          |      4,789,890 |                1.0 |       99.4 |
| f_different_footprints |      7,010,895 |                5.9 |       99.9 |
| **Final**              | **33,033,643** |                    |            |

## Visualizer

The visualizer script reads the input and output files, and creates a visual representation of the solutions. The validator script checks that the solutions satisfy the requirements, and computes the score, average utility access, and coverage percentage(density).

<img width="773" height="446" alt="Image" src="https://github.com/user-attachments/assets/efc4f968-8071-4ee6-8cfd-36ddc0098604" />
