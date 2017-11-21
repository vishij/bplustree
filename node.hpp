#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <vector>
#include <cmath>
#include <utility>
#include <cfloat>
#include <iostream>

// Superclass to enable InternalNode to point to either kind of Node
class Node {
protected:
    // keys should always have the last value of FLT_MAX as GUARD VALUE
    // because we will be inserting elements by checking left
    // to right and this covers the corner case where the element
    // being inserted is the largest one
    std::vector<float> keys = {FLT_MAX};
    Node *parent = nullptr;

public:
    virtual std::string get_type() = 0;

    virtual Node *get_parent() { return parent; }

    virtual void set_parent(Node *node) { parent = node; }

    // IMPORTANT! We are subtracting 1 because of the FLT_MAX guard value
    int get_n_keys() { return keys.size() - 1; }

    // TODO remove this temporary method
    virtual void print_all_keys() {
        for (std::vector<float>::const_iterator i = keys.begin(), j = --keys.end(); i < j; ++i) {
            std::cout << *i << " ";
        }
        std::cout << std::endl;

    }

    friend class BPlusTree;
};

class InternalNode : public Node {
protected:
    std::vector<Node *> child_ptrs;
    InternalNode *parent = nullptr;

public:
    virtual std::string get_type() { return "INTERNAL"; }

    // returns the index where the key was inserted
    // this index is used to add a child pointer
    int insert_key(float key) {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i) {
            if (key < *i) {
                // DON'T FORGET TO CAPTURE NEW VALUE OF i!
                i = keys.insert(i, key);
                return i - keys.begin();
            }
        }
    }

    // combine this node with an InternalNode which was formed as a result
    // of a split
    void combine(std::pair<InternalNode *, Node *> split_result) {
        auto to_merge_with = split_result.first;
        auto its_child = split_result.second;

        int keypos = insert_key(*to_merge_with->keys.begin());
        insert_child(keypos + 1, its_child);
        its_child->set_parent(this);
    }

    std::pair<InternalNode *, Node *> split(int order) {
        // This code might be misleading. Keep in mind that we are subtracting keys.begin() from an iterator returned from the first part.
        int n_eles_in_left = (keys.begin() + (ceil(float(order) / 2) - 1)) - keys.begin();

        // new InternalNode with the "right" subset of elements
        InternalNode *new_right = new InternalNode();
        new_right->keys = std::vector<float>(keys.begin() + n_eles_in_left + 1, keys.end());
        new_right->child_ptrs = std::vector<Node *>(child_ptrs.begin() + n_eles_in_left + 1, child_ptrs.end());

        // change the parent pointer for each child moved into the new right node
        for (auto child: new_right->child_ptrs) {
            child->set_parent(new_right);
        }

        // new InternalNode with the "middle" (so to speak) element
        InternalNode *new_middle = new InternalNode();
        new_middle->insert_key(*(keys.begin() + n_eles_in_left));

        // truncate this node's information
        // IMPORTANT! Be sure to convert n_eles to indices considering the fact
        // that vectors are 0-indexed
        keys.resize(n_eles_in_left);
        keys.push_back(FLT_MAX);
        child_ptrs.resize(n_eles_in_left + 1);

        // new "right" becomes the right child of the new "middle"
        new_right->parent = new_middle;

        return std::pair<InternalNode *, InternalNode *>(new_middle, new_right);
    }

    /*
    // split on the basis of given order
    // TODO test if this needs to be made virtual
    InternalNode* split(int order)
    {
        // This code might be misleading. Keep in mind that we are subtracting keys.begin() from an iterator returned from the first part.
        int old_dn_keys_end = (keys.begin() + (ceil(order / 2) - 1)) - keys.begin();

        // new InternalNode with the "right" subset of elements
        InternalNode *new_right = new InternalNode();
        new_right->keys = std::vector<float>(keys.begin() + old_dn_keys_end + 2, keys.end());
        new_right->child_ptrs = std::vector<Node*>(child_ptrs.begin() + old_dn_keys_end + 2, child_ptrs.end());

        // new InternalNode with the "middle" (so to speak) element
        InternalNode *new_middle = new InternalNode();
        new_middle->insert_key(*(keys.begin() + old_dn_keys_end + 1));

        // truncate this node's information
        // IMPORTANT! Be sure to add 1 in resize because vectors are 0-indexed
        keys.resize(old_dn_keys_end + 1);
        keys.push_back(FLT_MAX);
        child_ptrs.resize(old_dn_keys_end + 2);

        // new "right" becomes the right child of the new "middle"
        new_middle->insert_child(0, nullptr);
        new_middle->insert_child(1, new_right);
        new_right->parent = new_middle;

        return new_middle;
    }
    */

    // maybe just a temp method
    // TODO remove if not required after testing
    void insert_child(int pos, Node *child_ptr) {
        child_ptrs.insert(child_ptrs.begin() + pos, child_ptr);
    }

    friend class BPlusTree;
};

class DataNode : public Node {
private:
    std::vector<std::string> values = {"END_MARKER"};
    DataNode *left = nullptr;
    DataNode *right = nullptr;

public:
    virtual std::string get_type() { return "DATA"; }

    virtual void print_all_keys() override {
        for (std::vector<float>::const_iterator i = keys.begin(), j = --keys.end(); i < j; ++i) {
            std::cout << "KEY VAL PAIR: " << *i << ",  " << values.at(i - keys.begin()) << std::endl;
        }
        std::cout << std::endl;

    }

    void insert(float key, std::string value) {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i) {
            if (key < *i) {
                // IMPORTANT!! Don't forget to capture the new value of the
                // iterator after insertion
                i = keys.insert(i, key);
                std::cout << "Key insertion successful in DataNode" << std::endl;
                // insert value at ith position
                values.insert(values.begin() + (i - keys.begin()), value);
                std::cout << "Value insertion successful in DataNode" << std::endl;
                break;
            }
        }
    }

    // split on the basis of the given order
    std::pair<InternalNode *, Node *> split(int order) {
        // IMPORTANT! we are doing ceil(m/2) - 2 instead of ceil(m/2) - 1
        // because our nodes are 0-indexed (so first 2 children means index
        // of last child should be 1)
        std::cout << "order for split is: " << order << std::endl;
        std::vector<float>::iterator old_dn_keys_end = keys.begin() + ceil(float(order) / 2) - 2;
        std::vector<std::string>::iterator old_dn_values_end = values.begin() + ceil(float(order) / 2) - 2;

        // create new DataNode with subset of larger keys and values
        DataNode *new_dn = new DataNode();
        new_dn->keys = std::vector<float>(old_dn_keys_end + 1, keys.end());
        std::cout << "new_dn keys made successfully" << std::endl;
        new_dn->values = std::vector<std::string>(old_dn_values_end + 1, values.end());

        // truncate the smaller subset in the existing node
        // IMPORTANT! Be sure to add 1 in resize because vectors are 0-indexed
        keys.resize(old_dn_keys_end - keys.begin() + 1);
        keys.push_back(FLT_MAX);
        values.resize(old_dn_keys_end - keys.begin() + 1);
        values.push_back("END_MARKER");

        // add new node into the doubly linked list
        new_dn->right = right;
        new_dn->left = this;
        right = new_dn;

        // new parent for the split DataNode
        // note that it is not necessary to set up the child and parent
        // pointers here (BPlusTree will do it) since we are returning a pair
        // which implicitly means that right DataNode in the pair is the child
        InternalNode *new_in = new InternalNode();
        new_in->insert_key(*new_dn->keys.begin());

        return std::pair<InternalNode *, DataNode *>(new_in, new_dn);
    }

    /*
    // split on the basis of given order
    // TODO test if this needs to be made virtual
    DataNode* split(int order)
    {
        // IMPORTANT! we are doing ceil(m/2) - 2 instead of ceil(m/2) - 1
        // because our nodes are 0-indexed (so first 2 children means index
        // of last child should be 1)
        std::cout << "order for split is: " << order << std::endl;
        std::vector<float>::const_iterator old_dn_keys_end = keys.begin() + ceil(order / 2) - 2;
        std::vector<std::string>::const_iterator old_dn_values_end = values.begin() + ceil(order / 2) - 2;

        // create new DataNode with subset of larger keys and values
        DataNode *new_dn = new DataNode();
        new_dn->keys = std::vector<float>(old_dn_keys_end + 1, keys.end());
        std::cout << "new_dn keys made successfully" << std::endl;
        new_dn->values = std::vector<std::string>(old_dn_values_end + 1, values.end());

        // truncate the smaller subset in the existing node
        // IMPORTANT! Be sure to add 1 in resize because vectors are 0-indexed
        keys.resize(old_dn_keys_end - keys.begin() + 1);
        keys.push_back(FLT_MAX);
        values.resize(old_dn_keys_end - keys.begin() + 1);
        values.push_back("END_MARKER");

        // add new node into the doubly linked list
        new_dn->right = right;
        new_dn->left = this;
        right = new_dn;

        return new_dn;
    }
     */

    friend class BPlusTree;

    friend class InternalNode;
};

#endif
