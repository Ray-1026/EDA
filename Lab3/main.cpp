#include "data.h"
#include "globalRouting.h"
#include "io.h"

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <*.gmp> <*.gcl> <*.cst> <*.lg>\n";
        return 1;
    }

    AStarRouting solver;
    GridMap grid_map;
    read_input(argv, solver, grid_map);

    solver.solve(grid_map);

    auto routing_paths = solver.getRoutingPaths();
    write_result(argv, routing_paths);

    return 0;
}