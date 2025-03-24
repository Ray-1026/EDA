#ifndef IO_H
#define IO_H

#include <fstream>
#include <iostream>
#include <vector>

void read_file(const char *filename, std::string &boolean_equation, std::vector<std::string> &orders);
void write_file(const char *filename, int min_nodes);

#endif // IO_H