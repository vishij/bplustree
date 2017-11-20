#ifndef BPLUS_HPP_
#define BPLUS_HPP_

#include <iostream>
#include <string>
#include "node.hpp"

class BPlusTree
{
private:
    Node *root;
    int order;

    // useful only for internal debugging purposes
    // TODO remove before submission
    DataNode *head;
    DataNode *tail;

public:
    void initialise(int order);
    int get_order();
    void insert(float key, std::string value);
    std::vector<std::string> search(float key);

    /*
     * Ranged search
     *
     * Search for key such that key_start <= key <= key_end
     */
    std::string search(float key_start, float key_end);

    // useful only for internal debugging purposes
    // TODO remove before submission
    void print_all_keys();

    // useful only for internal debugging purposes
    // TODO remove before submission
    void print_tree_bfs();
};

#endif
