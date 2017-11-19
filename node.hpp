#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <vector>
#include <cmath>
#include <float.h>

// Superclass to enable InternalNode to point to either kind of Node
class Node
{
protected:
    // keys should always have the last value of FLT_MAX as GUARD VALUE
    // because we will be inserting elements by checking left
    // to right and this covers the corner case where the element
    // being inserted is the largest one
    std::vector<float> keys = { FLT_MAX };

public:
    virtual std::string get_type() = 0;
};

class InternalNode: public Node
{
protected:
    std::vector<Node*> child_ptrs;
    InternalNode *parent = nullptr;

public:
    virtual std::string get_type() { return "INTERNAL"; }

    // IMPORTANT! Remember that we don't need to add 1 because of the
    // FLT_MAX guard value
    int get_n_children() { return keys.size(); }

    InternalNode* get_parent() { return parent; }
    void set_parent(InternalNode *node) { parent = node; }

    // returns the index where the key was inserted
    // this index is used to add a child pointer
    int insert_key(float key)
    {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i)
        {
            if (key < *i)
            {
                // DON'T FORGET TO CAPTURE NEW VALUE OF i!
                i = keys.insert(i, key);
                return i - keys.begin();
            }
        }
    }

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
        keys.resize(old_dn_keys_end + 1);
        keys.push_back(FLT_MAX);
        child_ptrs.resize(old_dn_keys_end + 2);

        // new "right" becomes the right child of the new "middle"
        new_middle->insert_child(0, nullptr);
        new_middle->insert_child(1, new_right);
        new_right->parent = new_middle;

        return new_middle;
    }

    // maybe just a temp method
    // TODO remove if not required after testing
    void insert_child(int pos, Node *child_ptr)
    {
        child_ptrs.insert(child_ptrs.begin() + pos, child_ptr);
    }

    friend class BPlusTree;
    friend class DataNode;
};

class DataNode: public Node
{
private:
    std::vector<std::string> values = { "END_MARKER" };
    InternalNode *parent = nullptr;
    DataNode *left = nullptr;
    DataNode *right = nullptr;

public:
    virtual std::string get_type() { return "DATA"; }

    // IMPORTANT! We are subtracting 1 because of the FLT_MAX guard value
    int get_n_keys() { return keys.size() - 1; }

    InternalNode* get_parent() { return parent; }
    void set_parent(InternalNode *node) { parent = node; }

    void insert(float key, std::string value)
    {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i)
        {
            if (key < *i)
            {
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
        keys.resize(old_dn_keys_end - keys.begin());
        keys.push_back(FLT_MAX);
        values.resize(old_dn_keys_end - keys.begin());
        values.push_back("END_MARKER");

        // add new node into the doubly linked list
        new_dn->right = right;
        new_dn->left = this;
        right = new_dn;

        return new_dn;
    }

    friend class BPlusTree;

    // TODO remove this temporary method
    void print_all_keys()
    {
        for (std::vector<float>::const_iterator i = keys.begin(), j = --keys.end(); i < j; ++i)
        {
            std::cout << *i << " ";
        }
        std::cout << std::endl;

    }

    // TODO remove this temporary method
    DataNode* get_right()
    {
        return right;
    }

    // I think it's a bad idea to have this side effect in the destructor
    // TODO Will remove it if I find it makes the main program surprising
    ~DataNode()
    {
        if (left != nullptr)
        {
            left->right = right;
        }

        if (right != nullptr)
        {
            right->left = left;
        }
    }
};

#endif
