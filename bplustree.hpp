#ifndef BPLUS_HPP_
#define BPLUS_HPP_

#include <iostream>
#include <string>
#include "node.cpp"

/**
 * Header file for B+ Tree.
 */
class BPlusTree {
private:
    Node *root;
    int order;

    // useful only for internal debugging purposes
    // TODO remove before submission
    DataNode *head;
    DataNode *tail;

public:
    void initialise(int order);

    void insert(float insert_key, std::string value);

    std::vector<std::string> search(float key);

    /**
     * Range search - Search for all pairs such that key_start <= key <= key_end
     * @param start_key
     * @param end_key
     * @return
     */
    std::vector<std::pair<float, std::string>> search(float start_key, float end_key);
};

#endif
