#ifndef CELL_H
#define CELL_H

#include <vector>

#define A 0
#define B 1

class Cell {
  public:
    int id;
    int part;
    int gain;
    bool is_locked;
    std::vector<int> connected_nets; // List of nets connected to this cell

    Cell(int id, int max_cells) : id(id), gain(0), is_locked(false) { part = (id < max_cells / 2) ? A : B; }

    void lock() { is_locked = true; }    // Lock the cell
    void unlock() { is_locked = false; } // Unlock the cell
    void move() { part = 1 - part; }     // Move the cell to the other partition
};

#endif