#include <string>
#include "bplus.hpp"
#include "node.hpp"

void BPlusTree::initialise(int order)
{
    // an InternalNode can have at most "order" no. of children
    // i.e., InternalNodes and DataNodes can have at most (order - 1) no. of keys
    this->order = order;

    // initialise root as an empty (with FLT_MAX as the only key) data node
    root = new DataNode();
    head = tail = static_cast<DataNode*>(root);
}

int BPlusTree::get_order()
{
    return order;
}

void BPlusTree::insert(float key_to_insert, std::string value)
{
    Node *curr_node = root;

    // reach a DataNode first
    while (curr_node->get_type().compare("DATA") != 0)
    {
        std::cout << "Entered an internal node" << std::endl;
        InternalNode *curr_internal_node = static_cast<InternalNode*>(curr_node);
        for (std::vector<float>::iterator i = curr_internal_node->keys.begin(); i < curr_internal_node->keys.end(); ++i)
        {
            if (key_to_insert < *i)
            {
                curr_node = curr_internal_node->child_ptrs.at(i - curr_internal_node->keys.begin());
                std::cout << "Shifted to the child at " << i - curr_internal_node->keys.begin() << std::endl;
                break;
            }
        }
    }
    DataNode *curr_data_node = static_cast<DataNode*>(curr_node);
    std::cout << "Trying to insert in DataNode..." << std::endl;
    curr_data_node->insert(key_to_insert, value);
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

// TODO remove this temporary method
void BPlusTree::print_all_keys()
{
    for (DataNode *dn = head; dn != nullptr; dn=dn->get_right())
    {
        dn->print_all_keys();
    }
}
