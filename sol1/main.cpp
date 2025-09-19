#include <iostream>
#include <array>

#include "Solver.h"

int main()
{
    const std::string in_prefix = "../../input_files/";
    const std::string out_prefix = "../../output_files/sol1/";
    const std::array<std::string, 6> input_files = {"a_example.in", "b_short_walk.in", "c_going_green.in",
                                                    "d_wide_selection.in", "e_precise_fit.in", "f_different_footprints.in"};

    for (const auto& input_file : input_files)
    {
        Data data(in_prefix + input_file);
        std::cout << "Successfully read\n";
        Solver solver(data);

        std::cout << data.buildings.size();

        solver.solve();

//        const auto out_filename = out_prefix + input_file.substr(0, (input_file.find('.'))) + ".out";
//        Data::write_to_file(out_filename);

//        break;
    }

    return 0;
}
