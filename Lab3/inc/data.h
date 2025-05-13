#ifndef DATA_H
#define DATA_H

#include <vector>

#define X first
#define Y second
#define M1 1
#define M2 0

typedef unsigned int uint;

// class GCell {
//   public:
//     int x = 0, y = 0;
//     unsigned int h_capacity = 0, v_capacity = 0, h_occupied = 0, v_occupied = 0;
//     double g_cost = 0, h_cost = 0, of_cost = 0;
//     GCell *parent = nullptr;
// };

struct GCell {
    int x, y;                   // coordinates of the cell
    bool visited = false;       // visited flag
    int h_capacity, h_occupied; // left edge
    int v_capacity, v_occupied; // bottom edge
    float h_cost;               // heuristic cost to target (end)
    float wire_length;          // wire length
    float overflow_cost;        // overflow cost
    float gcell_cost;           // cost of the cells
    float via_cost;             // number of vias
    int layer;                  // layer (M1(1) or M2(0))
    std::pair<int, int> parent; // parent cell's grid index
    // GCell *parent = nullptr;
};

class GridMap {
  private:
    struct Rect {
        int x = 0, y = 0, w = 0, h = 0;
    };

  public:
    // .ra
    Rect routing_area;

    // .g
    int grid_wh[2] = {0, 0};

    // .c
    Rect chip1, chip2;

    // .b
    std::vector<std::pair<int, int>> bumps_chip1, bumps_chip2;

    // .l
    std::vector<std::vector<float>> cost_chip1, cost_chip2;

    // .ec
    std::vector<std::vector<GCell>> gcells;

    // utils functions
    std::pair<int, int> coord2GridIdx(int x, int y)
    {
        return {(x - routing_area.x) / grid_wh[0], (y - routing_area.y) / grid_wh[1]};
    }
};

#endif // DATA_H