#ifndef _IO_H
#define _IO_H

#include "data.h"
#include "globalRouting.h"
#include <fstream>
#include <iostream>
#include <vector>

void read_input(char *argv[], AStarRouting &routing, GridMap &grid_map);
void write_result(char *argc[], std::vector<std::vector<GCell>> &routing_paths);

#endif // _IO_H