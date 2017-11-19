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
    std::cout << "Key inserted in DataNode sucessfully" << std::endl;

    DataNode *new_data_node;
    if (curr_data_node->get_n_keys() == order)
    {
        std::cout << "Trying to split DataNode" << std::endl;
        new_data_node = curr_data_node->split(order);
        new_data_node->set_parent(curr_data_node->get_parent());
        std::cout << "DataNode split completed successfully" << std::endl;

        // if this is not the root node, we need to handle insert and (possibly)
        // split on its ancestors, up to the root
        if (nullptr != curr_data_node->parent)
        {
            // first key from new DataNode gets inserted into its parent
            // and split is done if required
            int key_pos = curr_data_node->parent->insert_key(*(new_data_node->keys.begin()));
            curr_data_node->parent->insert_child(key_pos + 1, new_data_node);
            if (curr_data_node->parent->get_n_children() == order)
            {
                curr_data_node->parent->split(order);
            }
        }
        // if this is the root node, we need a new InternalNode to become the parent
        else
        {
            InternalNode *new_root = new InternalNode();
            new_root->insert_key(*new_data_node->keys.begin());
            new_root->insert_child(0, curr_data_node);
            new_root->insert_child(1, new_data_node);
            curr_data_node->set_parent(new_root);
            new_data_node->set_parent(new_root);
        }
    }
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
