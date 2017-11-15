#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <vector>
#include <float.h>

// Superclass to enable InternalNode to point to either kind of Node
class Node
{
public:
    virtual std::string get_type() = 0;
};

class InternalNode: public Node
{
//private:
public:
    // keys should always have the last value of FLT_MAX as GUARD VALUE
    // because we will be inserting elements by checking left
    // to right and this covers the corner case where the element
    // being inserted is the largest one
    std::vector<float> keys = { FLT_MAX };

    std::vector<Node*> child_ptrs;

//public:
    virtual std::string get_type() { return "INTERNAL"; }
    int get_n_children() { return child_ptrs.size() - 1; } // -1 to remove FLT_MAX

    // returns the index where the key was inserted
    // this index is used to add a child pointer
    void insert_key(float key)
    {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i)
        {
            if (key < *i)
            {
                keys.insert(i, key);
                //return i - keys.begin();
            }
        }
    }

    // maybe just a temp method
    // TODO remove if not required after testing
    void create_new_child(int pos, Node *child_ptr)
    {
        child_ptrs.insert(child_ptrs.begin() + pos, child_ptr);
    }
};

class DataNode: public Node
{
//private:
public:
    // read the notes on the keys field of InternalNode
    std::vector<float> keys = { FLT_MAX };
    std::vector<std::string> values = { "end_marker" };
    DataNode *left;
    DataNode *right;

//public:
    DataNode()
    {
        left = right = nullptr;
    }

    virtual std::string get_type() { return "DATA"; }

    void insert(float key, std::string value)
    {
        for (std::vector<float>::iterator i = keys.begin(); i < keys.end(); ++i)
        {
            if (key < *i)
            {
                std::cout << "Inside DataNode's insert at pos " << i - keys.begin() << std::endl;
                keys.insert(i, key);
                std::cout << "Insertion successful in keys" << std::endl;
                // insert value at ith position
                auto pos = i - keys.begin();
                std::cout << "Pos calculated successfully!" << std::endl;
                values.insert(values.begin() + pos, value);
                std::cout << "Insertion successful in values" << std::endl;
                break;
            }
        }
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
