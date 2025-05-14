#ifndef ROUTING_H
#define ROUTING_H

#include "data.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

class AStarRouting {
  private:
    int dir[5] = {1, 0, -1, 0, 1};
    float epsilon; // heuristic weight
    float alpha, beta, gamma, delta, via_cost, max_cell_cost;
    std::vector<std::vector<GCell>> all_routing_paths;

    float getGCost(const GCell &gcell)
    {
        return alpha * gcell.wire_length + beta * gcell.overflow_cost + gamma * gcell.gcell_cost +
               delta * gcell.via_cost;
    }
    float getGCost(float wire_length, float overflow_cost, float gcell_cost, float via_cost)
    {
        return alpha * wire_length + beta * overflow_cost + gamma * gcell_cost + delta * via_cost;
    }

    float costFunc(const GCell &gcell) { return getGCost(gcell) + epsilon * gcell.h_cost; }

    bool check_bounds(int x, int y, int grid_map_width, int grid_map_height)
    {
        // return true if (x, y) is within the grid map bounds
        return (x >= 0 && y >= 0 && x < grid_map_width && y < grid_map_height);
    }

    void a_star(GCell &start, GCell &end, GridMap &grid_map, std::vector<GCell> &routing_path);

  public:
    AStarRouting(float epsilon = 0.5) : epsilon(epsilon) {}

    void setAlpha(float a) { alpha = a; }
    void setBeta(float b) { beta = b; }
    void setGamma(float g) { gamma = g; }
    void setDelta(float d) { delta = d; }
    void setViaCost(float v) { via_cost = v; }
    void setMaxCellCost(float m) { max_cell_cost = m; }
    std::vector<std::vector<GCell>> getRoutingPaths() { return all_routing_paths; }

    void solve(GridMap &grid_map);
};

#endif // ROUTING_H