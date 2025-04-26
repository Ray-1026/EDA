#include "fm.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    FiducciaMattheyses fm(argv[1]);
    fm.solve();

    std::vector<int> best_record = fm.get_best_record();
    std::ofstream output_file("output.txt");
    for (unsigned int i = 0; i < best_record.size(); i++) {
        output_file << best_record[i] << "\n";
    }
    output_file.close();

    return 0;
}