#include "BDD.h"

void BDDSolver::addUniqueToBDDTable(const BDDKey &key, BDDNode *node)
{
    auto it = bdd_table.find(key);
    if (it == bdd_table.end())
        bdd_table[key] = node;
}

BDDNode *BDDSolver::makeNode(int var, BDDNode *low, BDDNode *high)
{
    if (low == high)
        return low;

    // search in bdd_table, if found, return the node; otherwise, create a new node
    BDDKey key(var, low, high);
    auto it = bdd_table.find(key);
    if (it != bdd_table.end())
        return it->second;
    else {
        BDDNode *node = new BDDNode(var, low, high);
        bdd_table[key] = node;
        return node;
    }
}

std::string BDDSolver::equationSimplify(std::string boolean_eq, char var, int value)
{
    std::string transformed_eq;
    transformed_eq.reserve(boolean_eq.size());
    char upper_var = var - 32;

    for (auto &c : boolean_eq) {
        if (c == var)
            transformed_eq += (value + '0');
        else if (c == upper_var)
            transformed_eq += ((1 - value) + '0');
        else
            transformed_eq += c;
    }

    // string tokenize
    std::vector<std::string> tokens;
    size_t start = 0, pos;
    while ((pos = transformed_eq.find("+", start)) != std::string::npos) {
        tokens.emplace_back(transformed_eq.substr(start, pos - start));
        start = pos + 1;
    }
    tokens.emplace_back(transformed_eq.substr(start));

    // construct new boolean equation
    std::string new_boolean_eq = "";
    for (std::string &token : tokens) {
        if (token.size() == 1 && token[0] == '1')
            return "1";

        bool discard = false;
        std::string new_token_without_one = "";
        for (char c : token) {
            if (c == '0') { // discard if token contains 0
                discard = true;
                break;
            }

            if (c != '1') // remove 1 in token
                new_token_without_one += c;
        }

        if (discard || new_token_without_one.empty())
            continue;

        new_boolean_eq += (new_boolean_eq.empty() ? (new_token_without_one) : ("+" + new_token_without_one));
    }

    return (new_boolean_eq.empty() ? "0" : new_boolean_eq);
}

BDDNode *BDDSolver::buildBDD(std::string &boolean_equation, unsigned int idx)
{
    if (idx > current_order.size())
        return nullptr;

    if (boolean_equation == "0") { // return 0 node
        addUniqueToBDDTable(BDDKey(0), zero);
        return zero;
    }

    if (boolean_equation == "1") { // return 1 node
        addUniqueToBDDTable(BDDKey(1), one);
        return one;
    }

    char var = current_order[idx];
    std::string low_eq = equationSimplify(boolean_equation, var, 0),
                high_eq = equationSimplify(boolean_equation, var, 1);

    // std::cout << "eq: " << boolean_equation << "\nvar: " << var << "\nlow: " << low_eq << "\nhigh: " << high_eq
    //           << "\n\n";

    BDDNode *low = buildBDD(low_eq, idx + 1), *high = buildBDD(high_eq, idx + 1);
    return makeNode(var, low, high);
}
