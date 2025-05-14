#include "globalRouting.h"

void AStarRouting::solve(GridMap &grid_map)
{
    // std::cout << "Alpha: " << alpha << "\n";
    // std::cout << "Beta: " << beta << "\n";
    // std::cout << "Gamma: " << gamma << "\n";
    // std::cout << "Delta: " << delta << "\n";
    // std::cout << "Via Cost: " << via_cost << "\n";
    // std::cout << "Max Cell Cost: " << max_cell_cost << "\n";
    // std::cout << "Routing...\n";

    for (uint i = 0; i < grid_map.bumps_chip1.size(); i++) {
        std::vector<GCell> routing_path;
        std::pair<int, int> start_grid = grid_map.coord2GridIdx(grid_map.bumps_chip1[i].X, grid_map.bumps_chip1[i].Y),
                            end_grid = grid_map.coord2GridIdx(grid_map.bumps_chip2[i].X, grid_map.bumps_chip2[i].Y);
        // route
        a_star(grid_map.gcells[start_grid.Y][start_grid.X], grid_map.gcells[end_grid.Y][end_grid.X], grid_map,
               routing_path);

        all_routing_paths.push_back(routing_path);
    }
}

void AStarRouting::a_star(GCell &start, GCell &end, GridMap &grid_map, std::vector<GCell> &routing_path)
{
    bool path_found = false;

    // init gcells
    for (auto &gcell_row : grid_map.gcells) {
        for (GCell &gcell : gcell_row) {
            gcell.visited = false;
            gcell.h_cost = std::abs(gcell.x - end.x) + std::abs(gcell.y - end.y);
            gcell.wire_length = 0;
            gcell.overflow_cost = 0;
            gcell.gcell_cost = -1;
            gcell.via_cost = 0;
            gcell.parent = {-1, -1};
        }
    }

    // start and end layers are set to M1
    start.layer = M1;
    end.layer = M1;

    // A* open list
    auto cmp = [&](const GCell *a, const GCell *b) { return costFunc(*a) > costFunc(*b); };
    std::priority_queue<GCell *, std::vector<GCell *>, decltype(cmp)> open_list(cmp);

    std::pair<int, int> start_grid_idx = grid_map.coord2GridIdx(start.x, start.y);
    start.gcell_cost = grid_map.cost_chip1[start_grid_idx.Y][start_grid_idx.X];
    open_list.push(&start);

    while (!open_list.empty()) {
        GCell *current = open_list.top();
        open_list.pop();
        std::pair<int, int> current_grid_idx = grid_map.coord2GridIdx(current->x, current->y);
        current->visited = true;

        if (current->x == end.x && current->y == end.y) { // Found target
            path_found = true;
            continue;
        }

        for (int i = 0; i < 4; i++) {
            // i=0: [1, 0] ==> right
            // i=1: [0, -1] ==> up
            // i=2: [-1, 0] ==> left
            // i=3: [0, 1] ==> down
            std::pair<int, int> next_grid_idx = {current_grid_idx.X + dir[i], current_grid_idx.Y + dir[i + 1]};

            // check if next grid is out of bounds or visited
            if (!check_bounds(next_grid_idx.X, next_grid_idx.Y, grid_map.gcells[0].size(), grid_map.gcells.size()) ||
                grid_map.gcells[next_grid_idx.Y][next_grid_idx.X].visited) {
                continue;
            }

            GCell *next_gcell = &grid_map.gcells[next_grid_idx.Y][next_grid_idx.X];

            float tmp_wire_length = current->wire_length + grid_map.grid_wh[int(i & 1)];
            float tmp_overflow_cost = 0;
            float tmp_gcell_cost = current->gcell_cost;
            float tmp_via_cost = current->via_cost;
            if (i & 1) { // i==1 or i==3
                if (next_gcell->v_occupied + 1 > next_gcell->v_capacity)
                    tmp_overflow_cost = 5 * (next_gcell->v_occupied + 1 - next_gcell->v_capacity) * max_cell_cost;

                if (current->layer == M2) {
                    tmp_via_cost += via_cost;
                    tmp_gcell_cost -= grid_map.cost_chip2[current_grid_idx.Y][current_grid_idx.X];
                    tmp_gcell_cost += 0.5 * (grid_map.cost_chip1[current_grid_idx.Y][current_grid_idx.X] +
                                             grid_map.cost_chip2[current_grid_idx.Y][current_grid_idx.X]);
                }

                tmp_gcell_cost += grid_map.cost_chip1[next_grid_idx.Y][next_grid_idx.X];
            }
            else { // i==0 or i==2
                if (next_gcell->h_occupied + 1 > next_gcell->h_capacity)
                    tmp_overflow_cost = 5 * (next_gcell->h_occupied + 1 - next_gcell->h_capacity) * max_cell_cost;

                if (current->layer == M1) {
                    tmp_via_cost += via_cost;
                    tmp_gcell_cost -= grid_map.cost_chip1[current_grid_idx.Y][current_grid_idx.X];
                    tmp_gcell_cost += 0.5 * (grid_map.cost_chip1[current_grid_idx.Y][current_grid_idx.X] +
                                             grid_map.cost_chip2[current_grid_idx.Y][current_grid_idx.X]);
                }

                tmp_gcell_cost += grid_map.cost_chip2[next_grid_idx.Y][next_grid_idx.X];
            }

            float tentative_g_cost = getGCost(tmp_wire_length, tmp_overflow_cost, tmp_gcell_cost, tmp_via_cost);
            if (next_gcell->gcell_cost == -1 || tentative_g_cost < getGCost(*next_gcell)) {
                next_gcell->wire_length = tmp_wire_length;
                next_gcell->overflow_cost = tmp_overflow_cost;
                next_gcell->gcell_cost = tmp_gcell_cost;
                next_gcell->via_cost = tmp_via_cost;
                next_gcell->layer = (i & 1) ? M1 : M2;
                next_gcell->parent = {current_grid_idx.X, current_grid_idx.Y};

                open_list.push(next_gcell);
            }
        }
    }

    if (path_found) {
        GCell *current = &end, *parent;
        while (current->parent != std::pair<int, int>{-1, -1}) {
            parent = &grid_map.gcells[current->parent.Y][current->parent.X];

            if (current->y != parent->y)
                current->v_occupied++;
            else
                current->h_occupied++;

            routing_path.push_back(*current);
            current = parent;
        }
        routing_path.push_back(*current);
    }
    else {
        std::cout << "No path found!\n";
    }
}