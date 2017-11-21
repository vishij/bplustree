#include <string>
#include <queue>
#include <iostream>

#include "bplustree.hpp"

/**
 * Initialise the tree with order = m.
 * @param order
 */
void BPlusTree::initialise(int order) {
    /* an internal node can have at most "order" no. of children; order = m
     * i.e., InternalNodes and DataNodes can have at most (order - 1), or m-1 no. of keys */
    this->order = order;

    // initialise root as an empty data node (with FLT_MAX as the only key for tracking)
    root = new DataNode();
    head = tail = static_cast<DataNode *>(root);
}

/**
 * Insert K-V pair
 * @param insert_key
 * @param value
 */
void BPlusTree::insert(float insert_key, std::string value) {
    Node *current_node = root;

    // loop till a DataNode is reached
    while (current_node->get_type().compare("DATA") != 0) {
        InternalNode *curr_internal_node = static_cast<InternalNode *>(current_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            if (insert_key < *i) {
                current_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                break;
            }
        }
    }
    static_cast<DataNode *>(current_node)->insert(insert_key, value);

    if (current_node->get_no_of_keys() == order) {
        auto split_result = static_cast<DataNode *>(current_node)->split(order);

        InternalNode *current_parent = static_cast<InternalNode *>(current_node->parent);
        bool get_new_root = true;
        while (nullptr != current_parent) {
            current_parent->combine(split_result);
            // split the parent if combine caused it to have "order" no. of children
            if (current_parent->get_no_of_keys() == order) {
                split_result = current_parent->split(order);
                // curr_node updated as it is being used in the if-condition below when needs_new_root is true
                current_node = current_parent;
                current_parent = static_cast<InternalNode *>(current_node->get_parent());
            } else {
                get_new_root = false;
                break;
            }
        }

        if (get_new_root) {
            InternalNode *new_root = split_result.first;
            auto new_root_right_child = split_result.second;
            // Make any child/parent associations here (not in DataNode.split())
            new_root->insert_child(0, current_node); // this is why current_node updated in the while loop above
            new_root->insert_child(1, new_root_right_child);
            current_node->set_parent(new_root);
            new_root_right_child->set_parent(new_root);
            root = new_root;
        }
    }
}

/**
 * Exact search
 * @param key
 * @return vector of string values corresponding to the search key
 * NOTE: Multiple pairs can have same key
 */
std::vector<std::string> BPlusTree::search(float key) {
    Node *current_node = root;
    std::vector<std::string> search_output_values;
    // loop till a DataNode is reached
    while (current_node->get_type().compare("DATA") != 0) {
        // cast Node to InternalNode
        InternalNode *current_internal_node = static_cast<InternalNode *>(current_node);
        for (std::vector<float>::iterator i = current_internal_node->keys.begin();
             i < current_internal_node->keys.end(); ++i) {
            // search key < key from internal node
            if (key < *i) {
                current_node = current_internal_node->child_ptrs.at(i - current_internal_node->keys.begin());
                break;
            }
        }
    }

    //after reaching data node, convert the curr_node to type - DataNode; to trace nodes from current data node to right of it
    DataNode *current_data_node = static_cast<DataNode *>(current_node);
    // to trace nodes left to current node, as they may have same key as current node
    DataNode *curr_data_node_left = current_data_node->left;
    bool continue_search_left = true;
    while (curr_data_node_left != nullptr && continue_search_left) {
        for (std::vector<float>::const_iterator i = curr_data_node_left->keys.begin();
             i < curr_data_node_left->keys.end(); ++i) {
            if (key == *i) {
                search_output_values.push_back(curr_data_node_left->values.at(i - curr_data_node_left->keys.begin()));
            } else if(key > *i) {
                continue_search_left = false;
                break;
            }
        }
        curr_data_node_left = curr_data_node_left->left;
    }

    bool continue_search_right = true;
    while (current_data_node != nullptr && continue_search_right) {
        for (std::vector<float>::const_iterator i = current_data_node->keys.begin(); i < current_data_node->keys.end(); ++i) {
            if (key == *i) {
                search_output_values.push_back(current_data_node->values.at(i - current_data_node->keys.begin()));
            } else if(key < *i){
                continue_search_right = false;
                break;
            }
        }
        current_data_node = current_data_node->right;
    }

    return search_output_values;
}

/**
 * Range search - Search for all pairs such that key_start <= key <= key_end
 * @param start_key
 * @param end_key
 * @return vector of pairs<float, string>
 */
std::vector<std::pair<float, std::string>> BPlusTree::search(float start_key, float end_key) {
    Node *current_node = root;
    std::pair<float, std::string> range_data;
    std::vector<std::pair<float, std::string>> range_search_output;
    // loop till a DataNode is reached
    while (current_node->get_type().compare("DATA") != 0) {
        // cast Node to InternalNode
        InternalNode *curr_internal_node = static_cast<InternalNode *>(current_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            // search key < key from internal node
            if (start_key < *i) {
                current_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                break;
            }
        }
    }

    //after reaching data node, convert the curr_node to type - DataNode
    DataNode *current_data_node = static_cast<DataNode *>(current_node);
    // to trace nodes left to current node, as they may have same key as current node
    DataNode *curr_data_node_left = current_data_node->left;
    bool continue_search_left = true;
    //trace nodes left to current node till a key less than start key is reached
    while (curr_data_node_left != nullptr && continue_search_left) {
        for (std::vector<float>::const_iterator i = curr_data_node_left->keys.begin();
             i < curr_data_node_left->keys.end(); ++i) {
            if (start_key == *i) {
                range_search_output.push_back(std::make_pair(*i, curr_data_node_left->values.at(i - curr_data_node_left->keys.begin())));
            } else if(start_key > *i){
                continue_search_left = false;
                break;
            }
        }
        curr_data_node_left = curr_data_node_left->left;
    }

    while (current_data_node != nullptr) {
        for (std::vector<float>::const_iterator i = current_data_node->keys.begin(); i < current_data_node->keys.end(); ++i) {
            if ((*i >= start_key) && (*i <= end_key)) {
                range_search_output.push_back(
                        std::make_pair(*i, current_data_node->values.at(i - current_data_node->keys.begin())));
            }
        }
        current_data_node = current_data_node->right;
    }

    return range_search_output;
}
