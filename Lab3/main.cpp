#include "data.h"
#include "globalRouting.h"
#include "io.h"

int main(int argc, char *argv[])
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <*.gmp> <*.gcl> <*.cst> <*.lg>\n";
        return 1;
    }

    // std::cout << "Hello, World!" << std::endl;

    AStarRouting routing;
    GridMap grid_map;
    read_input(argv, routing, grid_map);

    routing.solve(grid_map);

    auto routing_paths = routing.getRoutingPaths();
    write_result(argv, routing_paths);

    return 0;
}