#include "BDD.h"
#include "io.h"
#include <atomic>
#include <limits>
#include <omp.h>

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

    // // serial version
    // BDDSolver bdd_solver;
    // for (std::string &order : orders) {
    //     bdd_solver.setOrder(order);
    //     bdd_solver.buildBDD(boolean_equation, 0);
    //     // std::cout << "Number of nodes: " << bdd_solver.countNodes() << "\n";
    //     bdd_solver.updateMinNodes();
    //     bdd_solver.clearBDDTable();
    // }

    // // write to file
    // write_file(argv[2], bdd_solver.getMinNodes());

    // Parallel version
    int num_threads = std::min(omp_get_max_threads(), static_cast<int>(orders.size()));
    omp_set_num_threads(num_threads);

    std::atomic<int> globalMinNodes(std::numeric_limits<int>::max());

#pragma omp parallel
    {
        int localMinNodes = std::numeric_limits<int>::max();

#pragma omp for schedule(guided)
        for (size_t i = 0; i < orders.size(); i++) {
            BDDSolver localSolver;
            localSolver.setOrder(orders[i]);
            localSolver.buildBDD(boolean_equation, 0);
            int localNodes = localSolver.countNodes();

            // Local minimum tracking
            localMinNodes = std::min(localMinNodes, localNodes);
        }

#pragma omp critical
        {
            globalMinNodes.store(std::min(globalMinNodes.load(std::memory_order_relaxed), localMinNodes),
                                 std::memory_order_relaxed);
        }
    }

    // write to file
    write_file(argv[2], globalMinNodes.load());

    return 0;
}