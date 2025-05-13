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
    float alpha, beta, gamma, delta, via_cost, max_cell_cost;
    float epsilon;
    std::vector<std::vector<GCell>> all_routing_paths;

    double getGCost(const GCell &gcell)
    {
        return alpha * gcell.wire_length + beta * gcell.overflow_cost + gamma * gcell.gcell_cost +
               delta * gcell.via_cost;
    }
    double getGCost(float wire_length, float overflow_cost, float gcell_cost, float via_cost)
    {
        return alpha * wire_length + beta * overflow_cost + gamma * gcell_cost + delta * via_cost;
    }
    double costFunc(const GCell &gcell) { return getGCost(gcell) + epsilon * gcell.h_cost; }
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