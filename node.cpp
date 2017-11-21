#include <string>
#include <vector>
#include <cmath>
#include <cfloat>
#include <iostream>

/**
 * Superclass Node to enable InternalNode to point to either kind of Node - InternalNode or DataNode
 */

class Node {

protected:
    /*  Last value for keys assigned as FLT_MAX as max value,
     * to cover the corner case where the element being inserted is the largest one
     * as the insert works by checking elements left to right */

    std::vector<float> keys = {FLT_MAX};
    Node *parent = nullptr;

public:
    virtual std::string get_type() = 0;

    virtual Node *get_parent() { return parent; }

    virtual void set_parent(Node *node) { parent = node; }

    // Subtract 1 because of the FLT_MAX as max/guard value
    int get_no_of_keys() { return keys.size() - 1; }

    friend class BPlusTree;
};

class InternalNode : public Node {

protected:
    std::vector<Node *> child_ptrs;
//    InternalNode *parent = nullptr;

public:
    virtual std::string get_type() override { return "INTERNAL"; }

    /**
     * Returns the index where the key was inserted which is used to add a child pointer
     * @param key
     * @return
     */
    int insert_key(float key) {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i) {
            if (key < *i) {
                // capture value of i
                i = keys.insert(i, key);
                return i - keys.begin();
            }
        }
    }

    /**
     * Combine the parent internal node with a given InternalNode which was formed as a result of a split
     * @param split_result
     */
    void combine(std::pair<InternalNode *, Node *> split_result) {
        auto nodes_to_merge = split_result.first;
        auto child_node = split_result.second;

        int key_position = insert_key(*nodes_to_merge->keys.begin());
        insert_child(key_position + 1, child_node);
        child_node->set_parent(this);
    }

    /**
     * Split a node on the basis of given order
     * @param order
     * @return pair of Nodes - InternalNode-InternalNode
     */
    std::pair<InternalNode *, Node *> split(int order) {
        // NOTE: Subtracted keys.begin() from an iterator returned from the first part
        int elements_in_left = (keys.begin() + (ceil(float(order) / 2) - 1)) - keys.begin();

        // new InternalNode with the "right" subset of elements
        InternalNode *new_right_node = new InternalNode();
        new_right_node->keys = std::vector<float>(keys.begin() + elements_in_left + 1, keys.end());
        new_right_node->child_ptrs = std::vector<Node *>(child_ptrs.begin() + elements_in_left + 1, child_ptrs.end());

        // change the parent pointer for each child moved into the new right node
        for (auto child: new_right_node->child_ptrs) {
            child->set_parent(new_right_node);
        }

        // new InternalNode with the "middle" element
        InternalNode *new_middle_node = new InternalNode();
        new_middle_node->insert_key(*(keys.begin() + elements_in_left));

        // Truncate/resize the node's information. NOTE: convert elements_in_left to indices considering the fact that vectors are 0-indexed
        keys.resize(elements_in_left);
        keys.push_back(FLT_MAX);
        child_ptrs.resize(elements_in_left + 1);

        // new "right" node becomes the right child of the new "middle" node
        new_right_node->parent = new_middle_node;

        return std::pair<InternalNode *, InternalNode *>(new_middle_node, new_right_node);
    }

    // temp method
    void insert_child(int position, Node *child_ptr) {
        child_ptrs.insert(child_ptrs.begin() + position, child_ptr);
    }

    friend class BPlusTree;
};

/**
 * Class to represent Leaf Nodes - contain nodes doubly linked list. Nodes internally contain array of data pairs stored.
 * NOTE: vector of keys is inherited from Node class. Vector of values containing value at index i corresponding to a key at index i
 */
class DataNode : public Node {
private:
    std::vector<std::string> values = {"END_MARKER"};
    DataNode *left = nullptr;
    DataNode *right = nullptr;

public:
    virtual std::string get_type() override { return "DATA"; }

    /**
     * Insert K-V pair in a given node
     * @param key
     * @param value
     */
    void insert(float key, std::string value) {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i) {
            if (key < *i) {
                // capture the new value of i (iterator) after insertion for value insertion at that same position
                i = keys.insert(i, key);
                // insert value at ith position
                values.insert(values.begin() + (i - keys.begin()), value);
                break;
            }
        }
    }

    /**
     * Splits a DataNode on the basis of the given order - when no. of pairs > m-1 for any node
     * @param order
     * @return pair of Nodes - InternalNode-DataNode
     */
    std::pair<InternalNode *, Node *> split(int order) {
        /* ceil(m/2) - 2 instead of ceil(m/2) - 1 because nodes are 0-indexed
         * (In case of m = 3 and split at 3, split at 0, (1,2)) */
        std::vector<float>::iterator old_data_node_end_keys = keys.begin() + ceil(float(order) / 2) - 2;
        std::vector<std::string>::iterator old_data_node_end_vals = values.begin() + ceil(float(order) / 2) - 2;

        // create new DataNode with subset of larger keys and values
        DataNode *new_data_node = new DataNode();
        new_data_node->keys = std::vector<float>(old_data_node_end_keys + 1, keys.end());
        new_data_node->values = std::vector<std::string>(old_data_node_end_vals + 1, values.end());

        /* Truncate/resize the smaller subset in the existing node
         * NOTE: added 1 in resize because vectors are 0-indexed */
        keys.resize(old_data_node_end_keys - keys.begin() + 1);
        keys.push_back(FLT_MAX);
        values.resize(old_data_node_end_keys - keys.begin() + 1);
        values.push_back("END_MARKER");

        // add new node into doubly linked list
        new_data_node->right = right;
        new_data_node->left = this;
        right = new_data_node;

        /* New parent for the split DataNode.
         * NOTE: No need to set the child and parent pointers here (BPlusTree does it)
         * as the pair returned implicitly means that right DataNode in the pair is the child */
        InternalNode *new_internal_node = new InternalNode();
        new_internal_node->insert_key(*new_data_node->keys.begin());

        return std::pair<InternalNode *, DataNode *>(new_internal_node, new_data_node);
    }

    friend class BPlusTree;
    friend class InternalNode;
};