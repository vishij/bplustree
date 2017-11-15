#include <string>
#include "bplus.hpp"
#include "node.hpp"

void BPlusTree::initialise(int order)
{
    this->order = order;

    // initialise empty data node (with FLT_MAX as the only key)
    auto first_data_node  = new DataNode();
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

void BPlusTree::print_all_keys()
{
    for (DataNode *dn=head; dn!=nullptr; dn=dn->right)
    {
        for (auto const &key: dn->keys)
        {
            std::cout << key << " ";
        }

        //std::cout << "%f  " << dn->key;
    }
    std::cout << std::endl;
}
