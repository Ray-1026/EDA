#include "io.h"

void read_input(char *argv[], AStarRouting &routing, GridMap &grid_map)
{
    std::ifstream gmp_file(argv[1]), gcl_file(argv[2]), cst_file(argv[3]);
    std::string dump; // To skip the first line

    /* .gmp start */
    // .ra
    gmp_file >> dump;
    gmp_file >> grid_map.routing_area.x >> grid_map.routing_area.y >> grid_map.routing_area.w >>
        grid_map.routing_area.h;
    // .g
    gmp_file >> dump;
    gmp_file >> grid_map.grid_wh[0] >> grid_map.grid_wh[1];
    // .c
    gmp_file >> dump;
    gmp_file >> grid_map.chip1.x >> grid_map.chip1.y >> grid_map.chip1.w >> grid_map.chip1.h;
    grid_map.chip1.x += grid_map.routing_area.x;
    grid_map.chip1.y += grid_map.routing_area.y;
    // .b
    gmp_file >> dump;
    while (gmp_file >> dump, dump != ".c") {
        std::pair<int, int> tmp_bump;
        gmp_file >> tmp_bump.X >> tmp_bump.Y;
        tmp_bump.X += grid_map.chip1.x;
        tmp_bump.Y += grid_map.chip1.y;
        grid_map.bumps_chip1.push_back(tmp_bump);
    }

    // .c
    gmp_file >> grid_map.chip2.x >> grid_map.chip2.y >> grid_map.chip2.w >> grid_map.chip2.h;
    grid_map.chip2.x += grid_map.routing_area.x;
    grid_map.chip2.y += grid_map.routing_area.y;
    // .b
    gmp_file >> dump;
    while (gmp_file >> dump) {
        std::pair<int, int> tmp_bump;
        gmp_file >> tmp_bump.X >> tmp_bump.Y;
        tmp_bump.X += grid_map.chip2.x;
        tmp_bump.Y += grid_map.chip2.y;
        grid_map.bumps_chip2.push_back(tmp_bump);
    }
    gmp_file.close();
    /* .gmp end */

    /* .gcl start */
    // .ec
    gcl_file >> dump;
    int gcell_w_num = grid_map.routing_area.w / grid_map.grid_wh[0],
        gcell_h_num = grid_map.routing_area.h / grid_map.grid_wh[1];
    grid_map.gcells.resize(gcell_h_num, std::vector<GCell>(gcell_w_num));
    for (int i = 0; i < gcell_h_num; i++) {
        for (int j = 0; j < gcell_w_num; j++) {
            GCell *gcell_tmp = &grid_map.gcells[i][j];
            gcl_file >> gcell_tmp->h_capacity >> gcell_tmp->v_capacity;
            gcell_tmp->x = j * grid_map.grid_wh[0] + grid_map.routing_area.x;
            gcell_tmp->y = i * grid_map.grid_wh[1] + grid_map.routing_area.y;
            gcell_tmp->h_occupied = 0;
            gcell_tmp->v_occupied = 0;
            gcell_tmp->parent = {-1, -1}; // Initialize parent
        }
    }
    gcl_file.close();
    /* .gcl end */

    /* .cst start */
    float var;
    cst_file >> dump >> var;
    routing.setAlpha(var);
    cst_file >> dump >> var;
    routing.setBeta(var);
    cst_file >> dump >> var;
    routing.setGamma(var);
    cst_file >> dump >> var;
    routing.setDelta(var);
    // .v
    cst_file >> dump;
    cst_file >> var;
    routing.setViaCost(var);
    // .l
    cst_file >> dump;
    float max_cost = 0.0;
    grid_map.cost_chip1.resize(gcell_h_num, std::vector<float>(gcell_w_num));
    grid_map.cost_chip2.resize(gcell_h_num, std::vector<float>(gcell_w_num));
    for (int i = 0; i < gcell_h_num; i++) {
        for (int j = 0; j < gcell_w_num; j++) {
            float cost;
            cst_file >> cost;
            grid_map.cost_chip1[i][j] = cost;

            max_cost = std::max(max_cost, cost);
        }
    }
    // .l
    cst_file >> dump;
    for (int i = 0; i < gcell_h_num; i++) {
        for (int j = 0; j < gcell_w_num; j++) {
            float cost;
            cst_file >> cost;
            grid_map.cost_chip2[i][j] = cost;

            max_cost = std::max(max_cost, cost);
        }
    }
    routing.setMaxCellCost(max_cost);
    cst_file.close();
    /* .cst end */

    std::cout << "Input files read successfully.\n";
}

void write_result(char *argc[], std::vector<std::vector<GCell>> &routing_paths)
{
    std::ofstream lg_file(argc[4]);
    for (uint i = 0; i < routing_paths.size(); i++) {
        lg_file << "n" << i + 1 << "\n";

        std::string layer = "M1";
        std::pair<int, int> cur = {routing_paths[i][0].x, routing_paths[i][0].y};
        std::pair<int, int> next = {routing_paths[i][1].x, routing_paths[i][1].y};
        std::pair<int, int> layer_start = cur;

        if (next.X != cur.X) { // right or left
            lg_file << "via\n";
            layer = "M2";
        }
        lg_file << layer << " " << layer_start.X << " " << layer_start.Y;

        for (uint j = 1; j < routing_paths[i].size() - 1; j++) {
            cur = {routing_paths[i][j].x, routing_paths[i][j].y};
            next = {routing_paths[i][j + 1].x, routing_paths[i][j + 1].y};

            if (layer == "M1") {
                if (next.X != cur.X) { // right or left
                    lg_file << " " << cur.X << " " << cur.Y << "\n";
                    lg_file << "via\n";
                    layer_start = cur;
                    layer = "M2";
                    lg_file << layer << " " << layer_start.X << " " << layer_start.Y;
                }
            }
            else {
                if (next.Y != cur.Y) { // up or down
                    lg_file << " " << cur.X << " " << cur.Y << "\n";
                    lg_file << "via\n";
                    layer_start = cur;
                    layer = "M1";
                    lg_file << layer << " " << layer_start.X << " " << layer_start.Y;
                }
            }
        }
        lg_file << " " << routing_paths[i].back().x << " " << routing_paths[i].back().y << "\n";
        if (layer == "M2")
            lg_file << "via\n";
        lg_file << ".end\n";
    }

    lg_file.close();
}