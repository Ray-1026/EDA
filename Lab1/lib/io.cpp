#include "io.h"

void read_file(const char *filename, std::string &boolean_equation, std::vector<std::string> &orders)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::getline(file, boolean_equation);
    boolean_equation.pop_back(); // remove '.' at the end

    while (!file.eof()) {
        std::string order;
        std::getline(file, order);

        // skip empty lines
        if (order.empty())
            continue;

        orders.emplace_back(order);
    }

    file.close();
}

void write_file(const char *filename, int min_nodes)
{
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    file << min_nodes;
    file.close();
}