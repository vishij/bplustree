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
 * @param key_to_insert
 * @param value
 */
void BPlusTree::insert(float key_to_insert, std::string value) {
    Node *curr_node = root;

    // loop till a DataNode is reached
    while (curr_node->get_type().compare("DATA") != 0) {
        InternalNode *curr_internal_node = static_cast<InternalNode *>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            if (key_to_insert < *i) {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                break;
            }
        }
    }
    static_cast<DataNode *>(curr_node)->insert(key_to_insert, value);

    if (curr_node->get_n_keys() == order) {
        auto split_result = static_cast<DataNode *>(curr_node)->split(order);

        InternalNode *curr_parent = static_cast<InternalNode *>(curr_node->parent);
        bool needs_new_root = true;
        while (nullptr != curr_parent) {
            curr_parent->combine(split_result);
            // split the parent if combine caused it to have "order" no. of children
            if (curr_parent->get_n_keys() == order) {
                split_result = curr_parent->split(order);
                // curr_node updated as it is being used in the if-condition below when needs_new_root is true
                curr_node = curr_parent;
                curr_parent = static_cast<InternalNode *>(curr_node->get_parent());
            } else {
                needs_new_root = false;
                break;
            }
        }

        if (needs_new_root) {
            InternalNode *new_root = split_result.first;
            auto new_root_right_child = split_result.second;
            // Make any child/parent associations here (not in DataNode.split())
            new_root->insert_child(0, curr_node); // this is why current_node updated in the while loop above
            new_root->insert_child(1, new_root_right_child);
            curr_node->set_parent(new_root);
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
    Node *curr_node = root;
    std::vector<std::string> search_output_arr;
    // loop till a DataNode is reached
    while (curr_node->get_type().compare("DATA") != 0) {
        // cast Node to InternalNode
        InternalNode *curr_internal_node = static_cast<InternalNode *>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            // search key < key from internal node
            if (key < *i) {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                break;
            }
        }
    }

    //after reaching data node, convert the curr_node to type - DataNode
    DataNode *curr_data_node = static_cast<DataNode *>(curr_node);
    DataNode *curr_data_node_left = curr_data_node->left;
    while (curr_data_node_left != nullptr) {
        for (std::vector<float>::const_iterator i = curr_data_node_left->keys.begin();
             i < curr_data_node_left->keys.end(); ++i) {
            if (key == *i) {
                search_output_arr.push_back(curr_data_node_left->values.at(i - curr_data_node_left->keys.begin()));
            }
        }
        curr_data_node_left = curr_data_node_left->left;
    }

    while (curr_data_node != nullptr) {
        for (std::vector<float>::const_iterator i = curr_data_node->keys.begin(); i < curr_data_node->keys.end(); ++i) {
            if (key == *i) {
                search_output_arr.push_back(curr_data_node->values.at(i - curr_data_node->keys.begin()));
            }
        }
        curr_data_node = curr_data_node->right;
    }

    return search_output_arr;
}

/**
 * Range search - Search for all pairs such that key_start <= key <= key_end
 * @param key_start
 * @param key_end
 * @return vector of pairs<float, string>
 */
std::vector<std::pair<float, std::string>> BPlusTree::search(float key_start, float key_end) {
    Node *curr_node = root;
    std::pair<float, std::string> range_data;
    std::vector<std::pair<float, std::string>> search_output_arr;
    // reach a DataNode first
    while (curr_node->get_type().compare("DATA") != 0) {
        // cast Node to internal node
        InternalNode *curr_internal_node = static_cast<InternalNode *>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin();
             i < curr_internal_node->keys.end(); ++i) {
            // search key < key from internal node
            if (key_start < *i) {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                break;
            }
        }
    }

    //after reaching data node, convert the curr_node to type - DataNode
    DataNode *curr_data_node = static_cast<DataNode *>(curr_node);

    //values from
    while (curr_data_node != nullptr) {
        for (std::vector<float>::const_iterator i = curr_data_node->keys.begin(); i < curr_data_node->keys.end(); ++i) {
            if ((*i >= key_start) && (*i <= key_end)) {
                search_output_arr.push_back(
                        std::make_pair(*i, curr_data_node->values.at(i - curr_data_node->keys.begin())));
            }
        }
        curr_data_node = curr_data_node->right;
    }
    return search_output_arr;
}
