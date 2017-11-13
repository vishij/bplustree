#include <string>
#include "bplus.hpp"
#include "node.hpp"

void BPlusTree::initialise(int order)
{
    this->order = order;

    // initialise empty data node (with FLT_MAX as the only key)
    first_data_node  = new DataNode();
    head = tail = first_data_node;

    // initialise empty internal node (root) with FLT_MAX as the only key
    // and the first, empty data node as its child
    root = new InternalNode();
    root->create_new_child(0, first_data_node);
}

void BPlusTree::insert(float key_to_insert, std::string value)
{
    Node *curr_node = root;

    // reach a DataNode first
    while (curr_node->get_type().compare("DATA") != 0)
    {
        for (std::vector<float>::iterator i = curr_node->keys.begin(); i != curr_node->keys.end(); ++i)
        {
            if (key_to_insert < *i)
            {
                curr_node = curr_node->child_ptrs.at(i - curr_node->keys.begin());
                break;
            }
        }
    }
    curr_node.insert(key_to_insert, value);
}

std::string BPlusTree::search(float key)
{

}

/*
 * Ranged search
 *
 * Search for key such that key_start <= key <= key_end
 */
std::string BPlusTree::search(float key_start, float key_end)
{
}

void BPlusTree::print_all_keys()
{
    for (DataNode *dn=head; dn=dn->right; dn==nullptr)
    {
        std::cout << "%f  " << dn->key;
    }
    std::cout << std::endl;
}
