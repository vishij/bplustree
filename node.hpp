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
private:
    // keys should always have the last value of FLT_MAX as GUARD VALUE
    // because we will be inserting elements by checking left
    // to right and this covers the corner case where the element
    // being inserted is the largest one
    std::vector<float> keys = { FLT_MAX };

    std::vector<Node*> child_ptrs;

public:
    virtual std::string get_type() { return "INTERNAL"; }
    int get_n_children() { return child_ptrs.size() - 1; } // -1 to remove FLT_MAX

    // returns the index where the key was inserted
    // this index is used to add a child pointer
    int insert_key(float key)
    {
        for (std::vector<float>::iterator i = v.begin(); i!= v.end(); ++i)
        {
            if (key < *i)
            {
                v.insert(i, key);
                return i - v.begin();
            }
        }
    }
};

class DataNode: public Node
{
private:
    float key;
    std::string value;
    DataNode *left;
    DataNode *right;

public:
    DataNode(float key, std::string value)
        : key(key), value(value)
    {
        left = right = nullptr;
    }

    virtual std::string get_type() { return "DATA"; }

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
