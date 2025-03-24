#ifndef BDD_H
#define BDD_H

#include <iostream>
#include <limits.h>
#include <unordered_map>
#include <vector>

struct BDDNode {
    int var;
    BDDNode *low;
    BDDNode *high;

    BDDNode(int var, BDDNode *low = nullptr, BDDNode *high = nullptr) : var(var), low(low), high(high) {}
};

class BDDSolver {
  private:
    struct BDDKey {
        int var;
        BDDNode *low;
        BDDNode *high;

        BDDKey(int var, BDDNode *low = nullptr, BDDNode *high = nullptr) : var(var), low(low), high(high) {}

        bool operator==(const BDDKey &other) const
        {
            return var == other.var && low == other.low && high == other.high;
        }
    };

    struct BDDKeyHash {
        std::size_t operator()(const BDDKey &k) const
        {
            auto h1 = std::hash<int>()(k.var);
            auto h2 = std::hash<BDDNode *>()(k.low);
            auto h3 = std::hash<BDDNode *>()(k.high);

            return (h1 ^ (h2 << 1)) ^ (h3 << 2);
        }
    };

    // Add a unique node to the BDD table, if already exists, no action is taken
    void addUniqueToBDDTable(const BDDKey &key, BDDNode *node);

    // Create a new BDD node, if already exists, return the existing node
    BDDNode *makeNode(int var, BDDNode *low, BDDNode *high);

    // Simplify boolean equation by setting variable to value
    std::string equationSimplify(std::string boolean_eq, char var, int value);

    int min_nodes = INT_MAX;
    std::string current_order;
    BDDNode *one = new BDDNode(1), *zero = new BDDNode(0);
    std::unordered_map<BDDKey, BDDNode *, BDDKeyHash> bdd_table;

  public:
    BDDSolver() {}

    // Build BDD from boolean equation
    BDDNode *buildBDD(std::string &boolean_equation, unsigned int idx);

    // Set order of variables
    void setOrder(const std::string order) { current_order = order; }

    // Clear BDD table
    void clearBDDTable() { bdd_table.clear(); }

    // Count number of nodes in current BDD
    int countNodes() { return bdd_table.size(); }

    // Update minimum number of nodes
    void updateMinNodes() { min_nodes = std::min(min_nodes, countNodes()); }

    // Get minimum number of nodes
    int getMinNodes() { return min_nodes; }
};

#endif // BDD_H