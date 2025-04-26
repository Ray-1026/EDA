#ifndef FM_H
#define FM_H

#include "cell.h"
#include "net.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <omp.h>
#include <random>
#include <sstream>
#include <unordered_set>

#define endl '\n'
#define DEBUG 0

class FiducciaMattheyses {
  private:
    // timer
    std::chrono::high_resolution_clock::time_point start_time;

    // early stopping
    const int early_stop = 10;
    int no_improvement_round = 0;
    bool improvement = false;

    // perturbation
    bool do_perturb;
    std::mt19937 rng;

    // balance ratio: 0.45 ~ 0.55
    double lower_bound, upper_bound;

    int num_nets, num_cells;                         // Number of nets and cells
    int partition_A_size = 0;                        // Size of partition A
    int max_partition_gains[2] = {INT_MIN, INT_MIN}; // Max gain for each partition
    int gain_sum = 0, gain_max = INT_MIN;            // Current sum of gains and max gain
    std::vector<Cell> cells;                         // List of cells
    std::vector<Net> nets;                           // List of nets
    std::vector<std::unordered_set<int>> buckets[2]; // Buckets for each partition
    std::vector<int> best_record;                    // Best record of the partitioning

    // utils
    void read_file(const std::string &filename);
    void init_balance_ratio();
    void init_gains();
    void init_buckets();
    void find_max_gains(std::pair<int, int> &p, int partition);
    void move_cell(int cid, int to_partition);
    void update_gains(int cid);
    void unlock();
    void perturb();

  public:
    FiducciaMattheyses(const std::string &filename, int seed, bool do_perturb) : do_perturb(do_perturb)
    {
        // Start the timer
        start_time = std::chrono::high_resolution_clock::now();

        // Read the input file
        read_file(filename);

        // init
        rng.seed(seed);
        init_balance_ratio();
        init_gains();
        init_buckets();
    }

    void solve();

    std::vector<int> get_best_record() { return best_record; }
};

#endif // FM_H