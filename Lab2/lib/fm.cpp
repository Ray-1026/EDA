#include "fm.h"

void FiducciaMattheyses::read_file(const std::string &filename)
{
    std::string s;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    file >> num_nets >> num_cells;
    std::getline(file, s);

    // init cells
    for (int i = 0; i < num_cells; i++) {
        Cell c(i, num_cells);
        cells.emplace_back(c);

        // calculate partition A size
        partition_A_size += (1 - c.part);

        // record
        best_record.emplace_back(c.part);
    }

    // init nets
    for (int i = 0; i < num_nets; i++) {
        std::getline(file, s);
        std::istringstream iss(s);
        std::string token;

        nets.emplace_back(i);

        while (iss >> token) {
            int cid = std::stoi(token) - 1; // convert to 0-based index

            // build relationship between nets and cells
            nets[i].connected_cells.emplace_back(&cells[cid]);
            cells[cid].connected_nets.emplace_back(i);
        }
    }

    file.close();
}

void FiducciaMattheyses::init_balance_ratio()
{
    lower_bound = 0.45 * num_cells;
    upper_bound = 0.55 * num_cells;

    if (upper_bound - lower_bound < 1.0) {
        lower_bound -= 0.5;
        upper_bound += 0.5;
    }
}

void FiducciaMattheyses::init_gains()
{
    // calculate each net's partition size
    for (auto &net : nets)
        net.calculate_partition();

    // calculate each cell's gain
    for (auto &cell : cells) {
        cell.gain = 0; // reset gain

        for (int nid : cell.connected_nets) {
            int f = nets[nid].part_size[cell.part], t = nets[nid].part_size[1 - cell.part];

            if (f == 1)
                cell.gain++;
            if (t == 0)
                cell.gain--;
        }

        // std::cout << "Cell : " << cell.id + 1 << " gain: " << cell.gain << endl;
    }
}

void FiducciaMattheyses::init_buckets()
{
    // clear buckets
    buckets[0].clear();
    buckets[1].clear();
    buckets[0].resize(2 * num_nets + 1);
    buckets[1].resize(2 * num_nets + 1);
    max_partition_gains[0] = INT_MIN;
    max_partition_gains[1] = INT_MIN;

    // fill buckets
    for (auto &cell : cells) {
        buckets[cell.part][cell.gain + num_nets].emplace(cell.id);
        max_partition_gains[cell.part] = std::max(max_partition_gains[cell.part], cell.gain + num_nets);
    }

    // for (auto &b : buckets) {
    //     for (unsigned int i = 0; i < b.size(); i++) {
    //         std::cout << i - num_nets << ": ";
    //         for (auto &c : b[i]) {
    //             std::cout << c << " ";
    //         }
    //         std::cout << endl;
    //     }
    //     std::cout << endl;
    // }
}

void FiducciaMattheyses::find_max_gains(std::pair<int, int> &p, int partition)
{
    // int max_gain = INT_MIN, cid = -1;
    double partition_A_size_prime = (partition == A) ? (partition_A_size - 1) : (partition_A_size + 1);

    // break if the partition size is out of bounds
    if (partition_A_size_prime > upper_bound || partition_A_size_prime < lower_bound)
        return;

    // find max gain
    // for (int i = 2 * num_nets; i >= 0; i--) {
    for (int i = max_partition_gains[partition]; i >= 0; i--) {
        if (buckets[partition][i].size() > 0) {
            // max_gain = i - num_nets;
            // cid = *buckets[partition][i].begin();
            // p.first = *buckets[partition][i].begin();

            p.first = *buckets[partition][i].begin();
            p.second = i - num_nets;
            return;
        }
    }
}

void FiducciaMattheyses::move_cell(int cid, int to_partition)
{
    // update partition size
    partition_A_size += (to_partition == A) ? 1 : -1;

    // erase from the bucket
    buckets[cells[cid].part][cells[cid].gain + num_nets].erase(cid);

    // update cell's state
    cells[cid].move();
    cells[cid].lock();

    // update gain
    gain_sum += cells[cid].gain;
    update_gains(cid);
}

void FiducciaMattheyses::update_gains(int cid)
{
    int from_part = 1 - cells[cid].part, to_part = cells[cid].part;

    for (int nid : cells[cid].connected_nets) {
        // NOTE: cell's partition has changed, `from` = 1 - cell.part, `to` = cell.part
        int f = nets[nid].part_size[from_part], t = nets[nid].part_size[to_part];

        if (t == 0) {
            for (auto &c : nets[nid].connected_cells) {
                if (c->is_locked)
                    continue;

                // 1. erase from the bucket
                // 2. update cell's gain
                // 3. update bucket
                // 4. update max_partition_gains
                buckets[c->part][c->gain + num_nets].erase(c->id);
                c->gain++;
                buckets[c->part][c->gain + num_nets].emplace(c->id);
                max_partition_gains[c->part] = std::max(max_partition_gains[c->part], c->gain + num_nets);
            }
        }
        else if (t == 1) {
            for (auto &c : nets[nid].connected_cells) {
                if (c->is_locked || c->part == from_part)
                    continue;

                buckets[c->part][c->gain + num_nets].erase(c->id);
                c->gain--;
                buckets[c->part][c->gain + num_nets].emplace(c->id);
                max_partition_gains[c->part] = std::max(max_partition_gains[c->part], c->gain + num_nets);
            }
        }

        // update net's partition size
        nets[nid].part_size[from_part]--;
        nets[nid].part_size[to_part]++;
        f--;
        t++;

        if (f == 0) {
            for (auto &c : nets[nid].connected_cells) {
                if (c->is_locked)
                    continue;

                buckets[c->part][c->gain + num_nets].erase(c->id);
                c->gain--;
                buckets[c->part][c->gain + num_nets].emplace(c->id);
                max_partition_gains[c->part] = std::max(max_partition_gains[c->part], c->gain + num_nets);
            }
        }
        else if (f == 1) {
            for (auto &c : nets[nid].connected_cells) {
                if (c->is_locked || c->part == to_part)
                    continue;

                buckets[c->part][c->gain + num_nets].erase(c->id);
                c->gain++;
                buckets[c->part][c->gain + num_nets].emplace(c->id);
                max_partition_gains[c->part] = std::max(max_partition_gains[c->part], c->gain + num_nets);
            }
        }
    }
}

void FiducciaMattheyses::unlock()
{
    gain_sum = gain_max;
    partition_A_size = 0;
    for (int i = 0; i < num_cells; i++) {
        cells[i].unlock();
        cells[i].part = best_record[i];
        partition_A_size += (1 - cells[i].part);
    }

    init_gains();
    init_buckets();
}

void FiducciaMattheyses::solve()
{
    int unlock_time = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                                 start_time) < std::chrono::milliseconds(29800)) {
        // auto timer1 = std::chrono::high_resolution_clock::now();
        std::pair<int, int> max_A = {-1, INT_MIN}, max_B = {-1, INT_MIN};
        find_max_gains(max_A, A);
        find_max_gains(max_B, B);
        // auto timer2 = std::chrono::high_resolution_clock::now();
        // std::cout << "Time taken to find max gains: "
        //           << std::chrono::duration_cast<std::chrono::microseconds>(timer2 - timer1).count() / 1000000.0
        //           << " seconds" << endl;

        // auto timer3 = std::chrono::high_resolution_clock::now();
        if (max_A.first == -1 && max_B.first == -1) {
            // break;

            unlock();
            unlock_time++;
            continue;
        }
        else {
            int move_target = (max_A.second > max_B.second) ? max_A.first : max_B.first;
            int move_to_partition = (max_A.second > max_B.second) ? B : A;
            move_cell(move_target, move_to_partition);
        }
        // auto timer4 = std::chrono::high_resolution_clock::now();
        // std::cout << "Time taken to move cell: "
        //           << std::chrono::duration_cast<std::chrono::microseconds>(timer4 - timer3).count() / 1000000.0
        //           << " seconds" << endl;

        // auto timer5 = std::chrono::high_resolution_clock::now();
        if (gain_sum > gain_max) { // update the best record
            gain_max = gain_sum;

#pragma omp parallel for
            for (int i = 0; i < num_cells; i++)
                best_record[i] = cells[i].part;
        }
        // auto timer6 = std::chrono::high_resolution_clock::now();
        // std::cout << "Time taken to update best record: "
        //           << std::chrono::duration_cast<std::chrono::microseconds>(timer6 - timer5).count() / 1000000.0
        //           << " seconds" << endl;

        // break;
    }

#if DEBUG
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << unlock_time + 1 << " iters taken: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000000.0
              << " seconds" << endl;
#endif
}