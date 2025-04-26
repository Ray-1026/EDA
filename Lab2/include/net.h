#ifndef NET_H
#define NET_H

#include "cell.h"

class Net {
  public:
    int id;
    int part_size[2] = {0, 0};           // Number of cells in each partition
    std::vector<Cell *> connected_cells; // List of cells connected to this net

    Net(int id) : id(id) {}

    void calculate_partition()
    {
        part_size[0] = 0;
        part_size[1] = 0;

        for (auto &cell : connected_cells)
            part_size[cell->part]++;
    }
};

#endif