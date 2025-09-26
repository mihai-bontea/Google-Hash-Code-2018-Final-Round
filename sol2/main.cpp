#include <array>
#include <locale>
#include <iostream>

#include "Solver.h"

struct comma_numpunct : std::numpunct<char>
{
protected:
    char do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

int main()
{
    const std::string in_prefix = "../../input_files/";
    const std::string out_prefix = "../../output_files/sol2/";
    const std::array<std::string, 6> input_files = {"a_example.in", "b_short_walk.in", "c_going_green.in",
                                                    "d_wide_selection.in", "e_precise_fit.in", "f_different_footprints.in"};

    std::locale comma_locale(std::cout.getloc(), new comma_numpunct);
    std::cout.imbue(comma_locale);

    unsigned long long total_score = 0;
    for (const auto& input_file : input_files)
    {
        if (input_file == "a_example.in")
            continue;

        Data data(in_prefix + input_file);
        std::cout << "Successfully read input file.\n";
        Solver solver(data);

        const auto [score, result] = solver.solve();
        std::cout << "Score = " << score << ".\n\n";
        total_score += score;

        const auto out_filename = out_prefix + input_file.substr(0, (input_file.find('.'))) + ".out";
        Data::write_to_file(out_filename, result);
    }
    std::cout << "Total score = " << total_score << std::endl;
    return 0;
}