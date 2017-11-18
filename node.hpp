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

    // IMPORTANT! Remember that we don't need to add 1 because of the
    // FLT_MAX guard value
    int get_n_children() { return keys.size(); }
};

class InternalNode: public Node
{
protected:
    std::vector<Node*> child_ptrs;
    InternalNode *parent = nullptr;

public:
    virtual std::string get_type() { return "INTERNAL"; }

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
                keys.insert(i, key);
                return i - keys.begin();
            }
        }
    }

    // split on the basis of given order
    // TODO test if this needs to be made virtual
    InternalNode* split(int order)
    {
       std::vector<float>::const_iterator old_dn_keys_end = keys.begin() + ceil((order / 2) - 1);
       std::vector<string>::const_iterator old_dn_values_end = values.begin() + ceil((order / 2) - 1);

       // create new DataNode with subset of larger keys and values
       DataNode *new_dn = new DataNode();
       new_dn->keys = new std::vector<float>(old_dn_keys_end + 1, keys.end());
       new_dn->values = new std::vector<std::string>(old_dn_values_end + 1, values.end());

       // create the new parent for this new DataNode
       InternalNode *new_dn_parent = new InternalNode();
       new_dn_parent->insert_key(*new_dn->keys.begin());
       // no left child
       new_dn_parent->child_ptrs.push_back(nullptr);
       // new node goes to the right of its new parent
       new_dn_parent->child_ptrs.push_back(new_dn);
       new_dn->parent = new_dn_parent;

       // truncate the smaller subset in the existing node
       keys = keys.resize(old_dn_keys_end - keys.begin());
       keys.push_back(FLT_MAX);
       values = values.resize(old_dn_keys_end - keys.begin());
       values.push_back("END_MARKER");
    }

    // maybe just a temp method
    // TODO remove if not required after testing
    void create_new_child(int pos, Node *child_ptr)
    {
        child_ptrs.insert(child_ptrs.begin() + pos, child_ptr);
    }

    friend class BPlusTree;
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
                // insert value at ith position
                values.insert(values.begin() + (i - keys.begin()), value);
                break;
            }
        }
    }

    // split on the basis of given order
    // TODO test if this needs to be made virtual
    InternalNode* split(int order)
    {
        
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
