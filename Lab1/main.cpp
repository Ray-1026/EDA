#include "BDD.h"
#include "io.h"

int main(int argc, char *argv[])
{
    // check arguments number
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    // read file
    std::string boolean_equation;
    std::vector<std::string> orders;

    read_file(argv[1], boolean_equation, orders);
    // std::cout << "Boolean equation: " << boolean_equation << "\n";
    // std::cout << "Orders: ";
    // for (const auto &order : orders) {
    //     std::cout << order << " ";
    // }
    // std::cout << "\n";

    BDDSolver bdd_solver;
    for (std::string &order : orders) {
        bdd_solver.setOrder(order);
        bdd_solver.buildBDD(boolean_equation, 0);
        // std::cout << "Number of nodes: " << bdd_solver.countNodes() << "\n";
        bdd_solver.updateMinNodes();
        bdd_solver.clearBDDTable();
    }

    // write to file
    write_file(argv[2], bdd_solver.getMinNodes());

    return 0;
}